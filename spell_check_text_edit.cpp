#include "spell_check_text_edit.h"

#include <QAction>
#include <QContextMenuEvent>
#include <QMenu>
#include <QTextCursor>

#include "spell_checker.h"

SpellCheckTextEdit::SpellCheckTextEdit(QWidget* parent)
    : QTextEdit(parent)
    , checker_(nullptr)
{
}

void SpellCheckTextEdit::setSpellChecker(const SpellChecker* checker)
{
    checker_ = checker;
}

void SpellCheckTextEdit::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu* menu = createStandardContextMenu();

    if (checker_) {
        QTextCursor cursor = cursorForPosition(event->pos());
        cursor.select(QTextCursor::WordUnderCursor);
        QString word = cursor.selectedText();
        if (!word.isEmpty() && checker_->isMisspelled(word)) {
            QStringList suggestions = checker_->suggestions(word, 5);
            if (!suggestions.isEmpty()) {
                QAction* before = menu->actions().isEmpty() ? nullptr
                                                            : menu->actions().first();
                for (const QString& suggestion : suggestions) {
                    QAction* action = new QAction(suggestion, menu);
                    connect(action, &QAction::triggered, this,
                        [this, cursor, suggestion]() {
                            QTextCursor replaceCursor = cursor;
                            replaceCursor.select(QTextCursor::WordUnderCursor);
                            replaceCursor.insertText(suggestion);
                        });
                    menu->insertAction(before, action);
                }
                menu->insertSeparator(before);
            }
        }
    }

    menu->exec(event->globalPos());
    delete menu;
}
