#ifndef SPELL_CHECK_TEXT_EDIT_H
#define SPELL_CHECK_TEXT_EDIT_H

#include <QTextEdit>

class SpellChecker;

class SpellCheckTextEdit : public QTextEdit {
    Q_OBJECT
public:
    explicit SpellCheckTextEdit(QWidget* parent = nullptr);
    void setSpellChecker(const SpellChecker* checker);

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    const SpellChecker* checker_;
};

#endif
