#include "main_window.h"

#include <QAction>
#include <QColorDialog>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDialog>
#include <QKeySequence>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QRegularExpression>
#include <QSettings>
#include <QStatusBar>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextStream>
#include <QToolBar>

#include <algorithm>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "find_replace_dialog.h"
#include "notepad_exception.h"
#include "spell_check_text_edit.h"
#include "spell_checker_highlighter.h"
#include "text_transform.h"
#include "word_frequency_dialog.h"

namespace {
constexpr int kMaxRecentFiles = 5;

int countWordsWithNormalize(const QString& text, const SpellChecker& checker)
{
    QRegularExpression regex("\\S+");
    auto it = regex.globalMatch(text);
    int count = 0;
    while (it.hasNext()) {
        QString token = it.next().captured(0);
        if (!checker.normalize(token).isEmpty()) {
            ++count;
        }
    }
    return count;
}

std::vector<std::pair<QString, int>> buildWordFrequencyData(const QString& text,
    const SpellChecker& checker)
{
    QRegularExpression regex("\\S+");
    auto it = regex.globalMatch(text);
    std::map<std::string, int> counts;
    while (it.hasNext()) {
        QString token = it.next().captured(0);
        QString normalized = checker.normalize(token);
        if (!normalized.isEmpty()) {
            ++counts[normalized.toStdString()];
        }
    }

    std::vector<std::pair<QString, int>> data;
    data.reserve(counts.size());
    for (const auto& item : counts) {
        data.push_back({ QString::fromStdString(item.first), item.second });
    }

    std::sort(data.begin(), data.end(), [](const auto& a, const auto& b) {
        if (a.second != b.second) {
            return a.second > b.second;
        }
        return QString::compare(a.first, b.first, Qt::CaseInsensitive) < 0;
    });

    return data;
}
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , editor_(nullptr)
    , highlighter_(nullptr)
    , findDialog_(nullptr)
    , wordFreqDialog_(nullptr)
    , wordCountLabel_(nullptr)
    , lineCountLabel_(nullptr)
    , cursorPosLabel_(nullptr)
    , boldAction_(nullptr)
    , italicAction_(nullptr)
    , underlineAction_(nullptr)
    , recentFilesMenu_(nullptr)
    , zoomLevel_(0)
{
    setupEditor();
    setupMenus();
    setupToolBar();
    setupStatusBar();
    loadRecentFiles();
    updateRecentFilesMenu();
    updateTitle();

    bool loaded = spellChecker_.loadWords("data/words.txt");
    if (!loaded) {
        QMessageBox::critical(this, "Error",
            "Could not load word list from data/words.txt.");
    }

    highlighter_ = new SpellCheckerHighlighter(editor_->document(), &spellChecker_);
    editor_->setSpellChecker(&spellChecker_);

    connect(editor_, &QTextEdit::textChanged, this, &MainWindow::updateStatus);
    connect(editor_, &QTextEdit::cursorPositionChanged, this,
        &MainWindow::updateCursorPosition);
    connect(editor_, &QTextEdit::currentCharFormatChanged, this,
        &MainWindow::updateFormatActions);

    updateStatus();
    updateCursorPosition();
}

void MainWindow::setupEditor()
{
    editor_ = new SpellCheckTextEdit(this);
    setCentralWidget(editor_);
    findDialog_ = new FindReplaceDialog(this);
    findDialog_->setEditor(editor_);
    wordFreqDialog_ = new WordFrequencyDialog(this);
}

void MainWindow::setupMenus()
{
    QMenu* fileMenu = menuBar()->addMenu("File");
    QAction* newAction = fileMenu->addAction("New", this, &MainWindow::newFile);
    newAction->setShortcut(QKeySequence::New);

    QAction* openAction = fileMenu->addAction("Open...", this, &MainWindow::openFile);
    openAction->setShortcut(QKeySequence::Open);

    QAction* saveAction = fileMenu->addAction("Save", this, &MainWindow::saveFile);
    saveAction->setShortcut(QKeySequence::Save);

    QAction* saveAsAction = fileMenu->addAction("Save As...", this, &MainWindow::saveFileAs);
    saveAsAction->setShortcut(QKeySequence::SaveAs);

    recentFilesMenu_ = fileMenu->addMenu("Recent Files");

    fileMenu->addSeparator();
    QAction* exitAction = fileMenu->addAction("Exit", this, &QWidget::close);
    exitAction->setShortcut(QKeySequence::Quit);

    QMenu* editMenu = menuBar()->addMenu("Edit");
    QAction* undoAction = editMenu->addAction("Undo", editor_, &QTextEdit::undo);
    undoAction->setShortcut(QKeySequence::Undo);

    QAction* redoAction = editMenu->addAction("Redo", editor_, &QTextEdit::redo);
    redoAction->setShortcut(QKeySequence::Redo);

    editMenu->addSeparator();
    QAction* cutAction = editMenu->addAction("Cut", editor_, &QTextEdit::cut);
    cutAction->setShortcut(QKeySequence::Cut);

    QAction* copyAction = editMenu->addAction("Copy", editor_, &QTextEdit::copy);
    copyAction->setShortcut(QKeySequence::Copy);

    QAction* pasteAction = editMenu->addAction("Paste", editor_, &QTextEdit::paste);
    pasteAction->setShortcut(QKeySequence::Paste);

    QAction* selectAllAction = editMenu->addAction("Select All", editor_, &QTextEdit::selectAll);
    selectAllAction->setShortcut(QKeySequence::SelectAll);

    editMenu->addSeparator();
    QAction* findAction = editMenu->addAction("Find / Replace", this, &MainWindow::showFindReplace);
    findAction->setShortcut(QKeySequence::Find);

    QMenu* transformMenu = editMenu->addMenu("Transform");
    transformMenu->addAction("Uppercase", this, &MainWindow::transformUppercase);
    transformMenu->addAction("Lowercase", this, &MainWindow::transformLowercase);
    transformMenu->addAction("Capitalize", this, &MainWindow::transformCapitalize);
    transformMenu->addAction("Sentence Case", this,
        &MainWindow::transformSentenceCase);
    transformMenu->addAction("Swap Case", this, &MainWindow::transformSwapCase);

    QMenu* formatMenu = menuBar()->addMenu("Format");
    boldAction_ = formatMenu->addAction("Bold", this, &MainWindow::toggleBold);
    boldAction_->setCheckable(true);
    boldAction_->setShortcut(QKeySequence::Bold);
    boldAction_->setIcon(QIcon("data/images/bold.svg"));

    italicAction_ = formatMenu->addAction("Italic", this, &MainWindow::toggleItalic);
    italicAction_->setCheckable(true);
    italicAction_->setShortcut(QKeySequence::Italic);
    italicAction_->setIcon(QIcon("data/images/italic.svg"));

    underlineAction_ = formatMenu->addAction("Underline", this, &MainWindow::toggleUnderline);
    underlineAction_->setCheckable(true);
    underlineAction_->setShortcut(QKeySequence::Underline);
    underlineAction_->setIcon(QIcon("data/images/underline.svg"));

    formatMenu->addSeparator();
    formatMenu->addAction("Font...", this, &MainWindow::chooseFont);
    formatMenu->addAction("Text Color...", this, &MainWindow::chooseTextColor);

    QMenu* toolsMenu = menuBar()->addMenu("Tools");
    toolsMenu->addAction("Word Frequency", this, &MainWindow::showWordFrequency);
    toolsMenu->addAction("Check Spelling...", this, &MainWindow::checkSpelling);

    QMenu* viewMenu = menuBar()->addMenu("View");
    QAction* zoomInAction = viewMenu->addAction("Zoom In", this, &MainWindow::zoomIn);
    zoomInAction->setShortcut(QKeySequence::ZoomIn);
    QAction* zoomOutAction = viewMenu->addAction("Zoom Out", this, &MainWindow::zoomOut);
    zoomOutAction->setShortcut(QKeySequence::ZoomOut);
    QAction* resetZoomAction = viewMenu->addAction("Reset Zoom", this, &MainWindow::resetZoom);
    resetZoomAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
}

void MainWindow::setupToolBar()
{
    QToolBar* formatToolBar = addToolBar("Format");
    formatToolBar->addAction(boldAction_);
    formatToolBar->addAction(italicAction_);
    formatToolBar->addAction(underlineAction_);
}

void MainWindow::setupStatusBar()
{
    wordCountLabel_ = new QLabel(this);
    lineCountLabel_ = new QLabel(this);
    cursorPosLabel_ = new QLabel(this);
    statusBar()->addPermanentWidget(wordCountLabel_);
    statusBar()->addPermanentWidget(lineCountLabel_);
    statusBar()->addPermanentWidget(cursorPosLabel_);
}

void MainWindow::updateTitle()
{
    if (currentFilePath_.isEmpty()) {
        setWindowTitle("Notepad");
    } else {
        setWindowTitle(QString("Notepad: %1").arg(currentFilePath_));
    }
}

void MainWindow::openRecentFile()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (!action) {
        return;
    }
    QString filePath = action->data().toString();
    if (filePath.isEmpty()) {
        return;
    }
    openFilePath(filePath);
}

bool MainWindow::openFilePath(const QString& filePath)
{
    try {
        QFileInfo info(filePath);
        if (!info.exists()) {
            throw file_not_found_exception("File not found.");
        }
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw file_read_exception("Could not open file for reading.");
        }
        QTextStream in(&file);
        QString content = in.readAll();
        if (in.status() != QTextStream::Ok) {
            throw file_read_exception("Failed to read file.");
        }
        editor_->setPlainText(content);
        currentFilePath_ = filePath;
        updateTitle();
    } catch (const notepad_exception& ex) {
        QMessageBox::critical(this, "Error", ex.what());
        return false;
    }

    addRecentFile(filePath);
    return true;
}

void MainWindow::addRecentFile(const QString& filePath)
{
    if (filePath.isEmpty()) {
        return;
    }
    recentFiles_.removeAll(filePath);
    recentFiles_.prepend(filePath);
    while (recentFiles_.size() > kMaxRecentFiles) {
        recentFiles_.removeLast();
    }
    saveRecentFiles();
    updateRecentFilesMenu();
}

void MainWindow::loadRecentFiles()
{
    QSettings settings;
    QStringList stored = settings.value("recentFiles").toStringList();
    QStringList filtered;
    for (const QString& path : stored) {
        if (QFileInfo::exists(path)) {
            filtered.append(path);
        }
    }
    recentFiles_ = filtered;
}

void MainWindow::saveRecentFiles() const
{
    QSettings settings;
    settings.setValue("recentFiles", recentFiles_);
}

void MainWindow::updateRecentFilesMenu()
{
    if (!recentFilesMenu_) {
        return;
    }

    recentFilesMenu_->clear();
    if (recentFiles_.isEmpty()) {
        QAction* emptyAction = recentFilesMenu_->addAction("(Empty)");
        emptyAction->setEnabled(false);
        return;
    }

    for (const QString& path : recentFiles_) {
        QString label = QFileInfo(path).fileName();
        QAction* action = recentFilesMenu_->addAction(label);
        action->setData(path);
        connect(action, &QAction::triggered, this, &MainWindow::openRecentFile);
    }

    recentFilesMenu_->addSeparator();
    QAction* clearAction = recentFilesMenu_->addAction("Clear Recent Files");
    connect(clearAction, &QAction::triggered, this, [this]() {
        recentFiles_.clear();
        saveRecentFiles();
        updateRecentFilesMenu();
    });
}

void MainWindow::newFile()
{
    editor_->clear();
    currentFilePath_.clear();
    updateTitle();
}

void MainWindow::openFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open File");
    if (filePath.isEmpty()) {
        return;
    }

    openFilePath(filePath);
}

void MainWindow::saveFile()
{
    if (currentFilePath_.isEmpty()) {
        saveFileAs();
        return;
    }

    try {
        QFile file(currentFilePath_);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            throw file_write_exception("Could not open file for writing.");
        }
        QTextStream out(&file);
        out << editor_->toPlainText();
        if (out.status() != QTextStream::Ok) {
            throw file_write_exception("Failed to write file.");
        }
        addRecentFile(currentFilePath_);
    } catch (const notepad_exception& ex) {
        QMessageBox::critical(this, "Error", ex.what());
    }
}

void MainWindow::saveFileAs()
{
    QString filePath = QFileDialog::getSaveFileName(this, "Save File As");
    if (filePath.isEmpty()) {
        return;
    }
    currentFilePath_ = filePath;
    saveFile();
    updateTitle();
}

void MainWindow::showFindReplace()
{
    findDialog_->show();
    findDialog_->raise();
    findDialog_->activateWindow();
}

void MainWindow::showWordFrequency()
{
    auto data = buildWordFrequencyData(editor_->toPlainText(), spellChecker_);
    wordFreqDialog_->setData(data);
    wordFreqDialog_->show();
    wordFreqDialog_->raise();
    wordFreqDialog_->activateWindow();
}

void MainWindow::checkSpelling()
{
    if (highlighter_) {
        highlighter_->rehighlight();
    }
}

void MainWindow::applyTransform(std::string (*transform)(const std::string&))
{
    QTextCursor cursor = editor_->textCursor();
    if (cursor.hasSelection()) {
        QString selected = cursor.selectedText();
        selected.replace(QChar(0x2029), '\n');
        std::string transformed = transform(selected.toStdString());
        cursor.insertText(QString::fromStdString(transformed));
    } else {
        QString text = editor_->toPlainText();
        std::string transformed = transform(text.toStdString());
        editor_->setPlainText(QString::fromStdString(transformed));
    }
}

void MainWindow::transformUppercase()
{
    applyTransform(to_uppercase);
}

void MainWindow::transformLowercase()
{
    applyTransform(to_lowercase);
}

void MainWindow::transformCapitalize()
{
    applyTransform(capitalize_words);
}

void MainWindow::transformSentenceCase()
{
    applyTransform(sentence_case);
}

void MainWindow::transformSwapCase()
{
    applyTransform(swap_case);
}

void MainWindow::mergeFormatOnSelection(const QTextCharFormat& format)
{
    QTextCursor cursor = editor_->textCursor();
    if (!cursor.hasSelection()) {
        cursor.select(QTextCursor::WordUnderCursor);
    }
    cursor.mergeCharFormat(format);
    editor_->mergeCurrentCharFormat(format);
}

void MainWindow::applyFormatToSelectionOrDocument(const QTextCharFormat& format)
{
    QTextCursor cursor = editor_->textCursor();
    QTextCursor original = cursor;
    if (!cursor.hasSelection()) {
        cursor.select(QTextCursor::Document);
    }
    cursor.mergeCharFormat(format);
    editor_->setTextCursor(original);
    editor_->mergeCurrentCharFormat(format);
}

void MainWindow::toggleBold(bool checked)
{
    QTextCharFormat format;
    format.setFontWeight(checked ? QFont::Bold : QFont::Normal);
    mergeFormatOnSelection(format);
}

void MainWindow::toggleItalic(bool checked)
{
    QTextCharFormat format;
    format.setFontItalic(checked);
    mergeFormatOnSelection(format);
}

void MainWindow::toggleUnderline(bool checked)
{
    QTextCharFormat format;
    format.setFontUnderline(checked);
    mergeFormatOnSelection(format);
}

void MainWindow::chooseFont()
{
    bool ok = false;
    QFont font = QFontDialog::getFont(&ok, editor_->font(), this, "Font");
    if (!ok) {
        return;
    }
    QTextCharFormat format;
    format.setFont(font);
    applyFormatToSelectionOrDocument(format);
}

void MainWindow::chooseTextColor()
{
    QColor color = QColorDialog::getColor(editor_->textColor(), this, "Text Color");
    if (!color.isValid()) {
        return;
    }
    QTextCharFormat format;
    format.setForeground(color);
    applyFormatToSelectionOrDocument(format);
}

void MainWindow::zoomIn()
{
    editor_->zoomIn(1);
    ++zoomLevel_;
}

void MainWindow::zoomOut()
{
    editor_->zoomOut(1);
    --zoomLevel_;
}

void MainWindow::resetZoom()
{
    if (zoomLevel_ > 0) {
        editor_->zoomOut(zoomLevel_);
    } else if (zoomLevel_ < 0) {
        editor_->zoomIn(-zoomLevel_);
    }
    zoomLevel_ = 0;
}

void MainWindow::updateStatus()
{
    int wordCount = countWordsWithNormalize(editor_->toPlainText(), spellChecker_);
    int lineCount = editor_->document()->blockCount();
    wordCountLabel_->setText(QString("Words: %1").arg(wordCount));
    lineCountLabel_->setText(QString("Lines: %1").arg(lineCount));
}

void MainWindow::updateCursorPosition()
{
    QTextCursor cursor = editor_->textCursor();
    int line = cursor.blockNumber() + 1;
    int column = cursor.positionInBlock() + 1;
    cursorPosLabel_->setText(QString("Line: %1, Col: %2").arg(line).arg(column));
}

void MainWindow::updateFormatActions(const QTextCharFormat& format)
{
    boldAction_->setChecked(format.fontWeight() == QFont::Bold);
    italicAction_->setChecked(format.fontItalic());
    underlineAction_->setChecked(format.fontUnderline());
}
