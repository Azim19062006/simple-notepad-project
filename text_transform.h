#ifndef TEXT_TRANSFORM_H
#define TEXT_TRANSFORM_H

#include <algorithm>
#include <cctype>
#include <string>

inline std::string to_uppercase(const std::string& input)
{
    std::string output = input;
    std::transform(output.begin(), output.end(), output.begin(),
        [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return output;
}

inline std::string to_lowercase(const std::string& input)
{
    std::string output = input;
    std::transform(output.begin(), output.end(), output.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return output;
}

inline std::string capitalize_words(const std::string& input)
{
    std::string output = input;
    bool newWord = true;
    for (char& c : output) {
        unsigned char uc = static_cast<unsigned char>(c);
        if (std::isalpha(uc)) {
            if (newWord) {
                c = static_cast<char>(std::toupper(uc));
                newWord = false;
            } else {
                c = static_cast<char>(std::tolower(uc));
            }
        } else {
            newWord = true;
        }
    }
    return output;
}

inline std::string sentence_case(const std::string& input)
{
    std::string output = to_lowercase(input);
    bool newSentence = true;
    for (char& c : output) {
        unsigned char uc = static_cast<unsigned char>(c);
        if (std::isalpha(uc)) {
            if (newSentence) {
                c = static_cast<char>(std::toupper(uc));
                newSentence = false;
            }
        } else if (c == '.' || c == '!' || c == '?') {
            newSentence = true;
        }
    }
    return output;
}

inline std::string swap_case(const std::string& input)
{
    std::string output = input;
    for (char& c : output) {
        unsigned char uc = static_cast<unsigned char>(c);
        if (std::islower(uc)) {
            c = static_cast<char>(std::toupper(uc));
        } else if (std::isupper(uc)) {
            c = static_cast<char>(std::tolower(uc));
        }
    }
    return output;
}

#endif
