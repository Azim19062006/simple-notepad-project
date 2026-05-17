#ifndef FIND_REPLACE_DIALOG_H
#define FIND_REPLACE_DIALOG_H

#include <QDialog>
#include <QTextDocument>

#include "ui_find_replace_dialog.h"

class QTextEdit;

class FindReplaceDialog : public QDialog {
    Q_OBJECT
public:
    explicit FindReplaceDialog(QWidget* parent = nullptr);
    void setEditor(QTextEdit* editor);

private slots:
    void onFindNext();
    void onReplace();
    void onReplaceAll();

private:
    QTextDocument::FindFlags findFlags() const;
    Ui::find_replace_dialog ui_;
    QTextEdit* editor_;
};

#endif
