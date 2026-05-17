#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QTextCharFormat>

#include <string>

#include "spell_checker.h"

class QAction;
class QLabel;
class QMenu;
class FindReplaceDialog;
class SpellCheckerHighlighter;
class SpellCheckTextEdit;
class WordFrequencyDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void newFile();
    void openFile();
    void openRecentFile();
    void saveFile();
    void saveFileAs();
    void showFindReplace();
    void showWordFrequency();
    void checkSpelling();
    void transformUppercase();
    void transformLowercase();
    void transformCapitalize();
    void transformSentenceCase();
    void transformSwapCase();
    void toggleBold(bool checked);
    void toggleItalic(bool checked);
    void toggleUnderline(bool checked);
    void chooseFont();
    void chooseTextColor();
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void updateStatus();
    void updateCursorPosition();
    void updateFormatActions(const QTextCharFormat& format);

private:
    void setupEditor();
    void setupMenus();
    void setupToolBar();
    void setupStatusBar();
    void updateTitle();
    bool openFilePath(const QString& filePath);
    void addRecentFile(const QString& filePath);
    void loadRecentFiles();
    void saveRecentFiles() const;
    void updateRecentFilesMenu();
    void mergeFormatOnSelection(const QTextCharFormat& format);
    void applyFormatToSelectionOrDocument(const QTextCharFormat& format);
    void applyTransform(std::string (*transform)(const std::string&));

    QString currentFilePath_;
    SpellCheckTextEdit* editor_;
    SpellChecker spellChecker_;
    SpellCheckerHighlighter* highlighter_;
    FindReplaceDialog* findDialog_;
    WordFrequencyDialog* wordFreqDialog_;
    QLabel* wordCountLabel_;
    QLabel* lineCountLabel_;
    QLabel* cursorPosLabel_;
    QAction* boldAction_;
    QAction* italicAction_;
    QAction* underlineAction_;
    QMenu* recentFilesMenu_;
    QStringList recentFiles_;
    int zoomLevel_;
};

#endif
