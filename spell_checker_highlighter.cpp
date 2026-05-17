#include "spell_checker_highlighter.h"

#include <QRegularExpression>

#include "spell_checker.h"

SpellCheckerHighlighter::SpellCheckerHighlighter(QTextDocument* parent,
    const SpellChecker* checker)
    : QSyntaxHighlighter(parent)
    , checker_(checker)
{
    misspelledFormat_.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
    misspelledFormat_.setUnderlineColor(Qt::red);
}

void SpellCheckerHighlighter::highlightBlock(const QString& text)
{
    if (!checker_) {
        return;
    }
    QRegularExpression regex("\\S+");
    auto it = regex.globalMatch(text);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString token = match.captured(0);
        if (checker_->isMisspelled(token)) {
            setFormat(match.capturedStart(0), match.capturedLength(0),
                misspelledFormat_);
        }
    }
}
