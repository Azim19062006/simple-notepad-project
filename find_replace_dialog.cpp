#include "find_replace_dialog.h"

#include <QTextCursor>
#include <QTextEdit>

FindReplaceDialog::FindReplaceDialog(QWidget* parent)
    : QDialog(parent)
    , editor_(nullptr)
{
    ui_.setupUi(this);
    connect(ui_.find_next_button, &QPushButton::clicked, this,
        &FindReplaceDialog::onFindNext);
    connect(ui_.replace_button, &QPushButton::clicked, this,
        &FindReplaceDialog::onReplace);
    connect(ui_.replace_all_button, &QPushButton::clicked, this,
        &FindReplaceDialog::onReplaceAll);
    connect(ui_.close_button, &QPushButton::clicked, this, &QDialog::close);
}

void FindReplaceDialog::setEditor(QTextEdit* editor)
{
    editor_ = editor;
}

QTextDocument::FindFlags FindReplaceDialog::findFlags() const
{
    QTextDocument::FindFlags flags;
    if (ui_.case_sensitive_check->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    return flags;
}

void FindReplaceDialog::onFindNext()
{
    if (!editor_) {
        return;
    }
    QString findText = ui_.find_input->text();
    if (findText.isEmpty()) {
        return;
    }
    bool found = editor_->find(findText, findFlags());
    if (!found) {
        QTextCursor cursor = editor_->textCursor();
        cursor.movePosition(QTextCursor::Start);
        editor_->setTextCursor(cursor);
        editor_->find(findText, findFlags());
    }
}

void FindReplaceDialog::onReplace()
{
    if (!editor_) {
        return;
    }
    QString findText = ui_.find_input->text();
    if (findText.isEmpty()) {
        return;
    }
    QTextCursor cursor = editor_->textCursor();
    if (cursor.hasSelection()) {
        Qt::CaseSensitivity cs = ui_.case_sensitive_check->isChecked()
            ? Qt::CaseSensitive
            : Qt::CaseInsensitive;
        if (QString::compare(cursor.selectedText(), findText, cs) == 0) {
            cursor.insertText(ui_.replace_input->text());
            editor_->setTextCursor(cursor);
        }
    }
    onFindNext();
}

void FindReplaceDialog::onReplaceAll()
{
    if (!editor_) {
        return;
    }
    QString findText = ui_.find_input->text();
    if (findText.isEmpty()) {
        return;
    }
    QTextCursor cursor = editor_->textCursor();
    cursor.movePosition(QTextCursor::Start);
    editor_->setTextCursor(cursor);
    while (editor_->find(findText, findFlags())) {
        QTextCursor replaceCursor = editor_->textCursor();
        replaceCursor.insertText(ui_.replace_input->text());
        editor_->setTextCursor(replaceCursor);
    }
}
