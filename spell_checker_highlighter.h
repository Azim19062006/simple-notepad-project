#ifndef SPELL_CHECKER_HIGHLIGHTER_H
#define SPELL_CHECKER_HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>

class SpellChecker;

class SpellCheckerHighlighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    SpellCheckerHighlighter(QTextDocument* parent, const SpellChecker* checker);

protected:
    void highlightBlock(const QString& text) override;

private:
    const SpellChecker* checker_;
    QTextCharFormat misspelledFormat_;
};

#endif
