#include "spell_checker.h"

#include <QFile>
#include <QTextStream>

#include <algorithm>
#include <cmath>
#include <vector>

namespace {
int boundedEditDistance(const std::string& a, const std::string& b, int maxDistance)
{
    int n = static_cast<int>(a.size());
    int m = static_cast<int>(b.size());
    if (std::abs(n - m) > maxDistance) {
        return maxDistance + 1;
    }

    std::vector<int> prev(static_cast<size_t>(m + 1));
    std::vector<int> curr(static_cast<size_t>(m + 1));
    for (int j = 0; j <= m; ++j) {
        prev[static_cast<size_t>(j)] = j;
    }

    for (int i = 1; i <= n; ++i) {
        curr[0] = i;
        int rowMin = curr[0];
        for (int j = 1; j <= m; ++j) {
            int cost = a[static_cast<size_t>(i - 1)] == b[static_cast<size_t>(j - 1)]
                ? 0
                : 1;
            int deletion = prev[static_cast<size_t>(j)] + 1;
            int insertion = curr[static_cast<size_t>(j - 1)] + 1;
            int substitution = prev[static_cast<size_t>(j - 1)] + cost;
            int value = std::min({ deletion, insertion, substitution });
            curr[static_cast<size_t>(j)] = value;
            rowMin = std::min(rowMin, value);
        }
        if (rowMin > maxDistance) {
            return maxDistance + 1;
        }
        prev.swap(curr);
    }

    return prev[static_cast<size_t>(m)];
}
}

bool SpellChecker::loadWords(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    words_.clear();
    for (auto& bucket : buckets_) {
        bucket.clear();
    }

    QTextStream stream(&file);
    while (!stream.atEnd()) {
        QString line = stream.readLine().trimmed();
        if (line.isEmpty()) {
            continue;
        }
        QString normalized = normalize(line);
        if (normalized.isEmpty()) {
            continue;
        }
        std::string word = normalized.toStdString();
        if (words_.insert(word).second) {
            char c = word[0];
            if (c >= 'a' && c <= 'z') {
                buckets_[static_cast<size_t>(c - 'a')].push_back(word);
            }
        }
    }

    return true;
}

QString SpellChecker::normalize(const QString& word) const
{
    QString result;
    result.reserve(word.size());
    for (const QChar& ch : word) {
        if (ch.isLetter()) {
            result.append(ch.toLower());
        }
    }
    return result;
}

bool SpellChecker::isMisspelled(const QString& word) const
{
    QString normalized = normalize(word);
    if (normalized.isEmpty()) {
        return false;
    }
    return words_.find(normalized.toStdString()) == words_.end();
}

QStringList SpellChecker::suggestions(const QString& word, int maxSuggestions) const
{
    QString normalized = normalize(word);
    if (normalized.isEmpty()) {
        return {};
    }
    std::string target = normalized.toStdString();
    if (words_.find(target) != words_.end()) {
        return {};
    }

    char first = target[0];
    if (first < 'a' || first > 'z') {
        return {};
    }

    const auto& bucket = buckets_[static_cast<size_t>(first - 'a')];
    std::vector<std::pair<int, std::string>> candidates;
    for (const auto& candidate : bucket) {
        int lengthDiff = std::abs(
            static_cast<int>(candidate.size()) - static_cast<int>(target.size()));
        if (lengthDiff > 2) {
            continue;
        }
        int distance = boundedEditDistance(target, candidate, 2);
        if (distance <= 2) {
            candidates.push_back({ distance, candidate });
        }
    }

    std::sort(candidates.begin(), candidates.end(),
        [](const auto& a, const auto& b) {
            if (a.first != b.first) {
                return a.first < b.first;
            }
            return a.second < b.second;
        });

    QStringList results;
    for (int i = 0; i < static_cast<int>(candidates.size()) && i < maxSuggestions;
        ++i) {
        results << QString::fromStdString(candidates[static_cast<size_t>(i)].second);
    }

    return results;
}
