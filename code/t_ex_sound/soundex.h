/***
 * Excerpted from "Modern C++ Programming with Test-Driven Development",
 * published by The Pragmatic Bookshelf.  Copyrights apply to this code. It may
 * not be used to create training material, courses, books, articles, and the
 * like. Contact us if you are in doubt.  We make no guarantees that this code
 * is fit for any purpose.  Visit
 * http://www.pragmaticprogrammer.com/titles/lotdd for more book information.
***/

// 1. Retain the first letter.
//
// 2. Drop all other occurrences of a, e, i, o, u, y, h, w.
//
// 3. Replace consonants with digits (after the first letter):
// b, f, p, v: 1
// c, g, j, k, q, s, x, z: 2
// d, t : 3
// l: 4
// m, n: 5
// r: 6
//
// 4. If two adjacent letters encode to the same number, encode them instead as
// a single number. Also, do so if two letters with the same number are
// separated by h or w (but code them twice if separated by a vowel).  This rule
// also applies to the first letter.
//
// 5. Stop when you have a letter and three digits. Zero-pad if needed.

#ifndef SOUNDEX_H
#define SOUNDEX_H

#include <string>
#include <unordered_map>

class Soundex
{
public:
    std::string encode(const std::string &word) const
    {
        return zeroPad(head(word) + encodeMultipleDigits(tail(word)));
    }

private:
    static const size_t MAX_CODE_LENGTH{4};

    std::string head(const std::string &word) const
    {
        // Q: this cause const converison error
        // return std::string(word[0]);
        return word.substr(0, 1);
    }

    std::string tail(const std::string &word) const
    {
        return word.substr(1);
    }

    // *TODO* do not check when word.size() > MAX_CODE_LENGTH
    std::string zeroPad(const std::string &word) const
    {
        auto zerosNeeded = MAX_CODE_LENGTH - word.size();
        std::cout << "zerosNeeded: " << zerosNeeded << std::endl;

        return word + std::string(zerosNeeded, '0');

        // when use cxx-string-append
        // std::string padded(word);
        // return padded.append(zerosNeeded, '0');
    }

    std::string encodeMultipleDigits(const std::string &word) const
    {
        std::string encoded;

        for (const auto letter : word)
        {
            if (encoded.length() = 
            encoded += encodeDigit(letter);
        }

        return encoded;
    }

    std::string encodeDigit(const char letter) const
    {
        const std::unordered_map<char, std::string> encodings {
            {'b', "1"}, {'f', "1"}, {'p', "1"}, {'v', "1"},
            {'c', "2"}, {'g', "2"}, {'j', "2"}, {'k', "2"}, {'q', "2"},
            {'s', "2"}, {'x', "2"}, {'z', "2"},
            {'d', "3"}, {'t', "3"},
            {'l', "4"},
            {'m', "5"}, {'n', "5"},
            {'r', "6"}
        };

        auto it = encodings.find(letter);
        return it == encodings.end() ? "" : it->second;
    }
};

#endif // SOUNDEX_H
