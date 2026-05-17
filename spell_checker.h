#ifndef SPELL_CHECKER_H
#define SPELL_CHECKER_H

#include <array>
#include <string>
#include <unordered_set>
#include <vector>

#include <QString>
#include <QStringList>

class SpellChecker {
public:
    bool loadWords(const QString& path);
    bool isMisspelled(const QString& word) const;
    QString normalize(const QString& word) const;
    QStringList suggestions(const QString& word, int maxSuggestions = 5) const;

private:
    std::unordered_set<std::string> words_;
    std::array<std::vector<std::string>, 26> buckets_;
};

#endif
