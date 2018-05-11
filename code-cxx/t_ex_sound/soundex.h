/***
 * Excerpted from "Modern C++ Programming with Test-Driven Development",
 * published by The Pragmatic Bookshelf.  Copyrights apply to this code. It may
 * not be used to create training material, courses, books, articles, and the
 * like. Contact us if you are in doubt.  We make no guarantees that this code
 * is fit for any purpose.  Visit
 * http://www.pragmaticprogrammer.com/titles/lotdd for more book information.
***/

// Chapter	      2, Test-Driven Development: A First Example
// 
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
        // *TN* do not like to express case handling only on head.

        // return zeroPad(head(word) + encodeMultipleDigits(tail(word)));
        //
        // To support case:
        // return zeroPad(upperFront(head(word)) + encodeMultipleDigits(tail(word)));
        
        // To support when the first and the second are the same char,
        //
        // Our solution involves a little bit of change to the overall policy
        // embodied in encode. We pass `the entire word` to encodedDigits for
        // encoding so that we can compare the encoding of the second letter to
        // the first. 
        //
        // TN: this seems great rather than having state to keep the first
        // between head() and tail().
        //
        // This cause to change encodeMultipleDigits() to encode the first to
        // handle both when there should be one char when char is not found in
        // the map such as A so that tail() should works on not-null-string AND
        // when the first and the second map to the same such as "Bbxx"

        return zeroPad(upperFront(head(word)) + tail(encodeMultipleDigits(word)));
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

        auto it = encodings.find(std::tolower(letter));
        return it == encodings.end() ? DIGIT_NOT_FOUND : it->second;
    }

private:
    static const size_t MAX_CODE_LENGTH{4};
    static const std::string DIGIT_NOT_FOUND;
    
    std::string head(const std::string &word) const
    {
        return word.substr(0, 1);
    }

    std::string upperFront(const std::string &word) const
    {
        return std::string(1, std::toupper(word.front()));
    }

    std::string tail(const std::string &word) const
    {
        return word.substr(1);
    }

    std::string zeroPad(const std::string &word) const
    {
        auto zerosNeeded = MAX_CODE_LENGTH - word.size();

        return word + std::string(zerosNeeded, '0');

        // when use cxx-string-append
        // std::string padded(word);
        // return padded.append(zerosNeeded, '0');
    }

    // check overflow when word.size() > MAX_CODE_LENGTH and which can be done
    // at zeroPad or here.
    
    // std::string encodeMultipleDigits(const std::string &word) const
    // {
    //     // crash for the most since encodeDigit() will return null when not
    //     // found in the map.
    //     //
    //     // std::string encoded;
    //     // encoded += encodeDigit(word.front());

    //     // works for the most but not
    //     // CombinesDuplicateCodesWhenSecondDuplicatesFirst() since there will be
    //     // the first char.
    //     //
    //     // std::string encoded{word.front()};

    //     std::string encoded;
    //     encoded += encodeDigit(word.front());
        
    //     for (const auto letter : tail(word))
    //     {
    //         if (isComplete(encoded)) 
    //             break;

    //         auto digit = encodeDigit(letter);
    //         if (DIGIT_NOT_FOUND != digit && digit != lastDigit(encoded))
    //             encoded += encodeDigit(letter);
    //     }

    //     return encoded;
    // }

    std::string encodeMultipleDigits(const std::string &word) const
    {
        std::string encoded;

        encodeHead(encoded, word);
        encodeTail(encoded, word);

        return encoded;
    }

    void encodeHead(std::string &encoding, const std::string &word) const
    {
        encoding += encodeDigit(word.front());
    }    

    void encodeTail(std::string &encoding, const std::string &word) const 
    {
        // for (const auto letter : tail(word))
        // {
        //     if (!isComplete(encoding)) 
        //         encodeLetter(encoding, letter);
        // }

        // to use index
        for (auto i = 1u; i < word.length(); ++i)
        {
            if (!isComplete(encoding)) 
                encodeLetter(encoding, word[i], word[i-1]);
        }
    }

    void encodeLetter(std::string &encoding, char letter, char prevLetter) const 
    {
        auto digit = encodeDigit(letter);
        if (DIGIT_NOT_FOUND != digit && 
                (digit != lastDigit(encoding) || isVowel(prevLetter)))
            encoding += digit;
    }

    bool isVowel(char letter) const
    {
        return std::string("aeiouy").find(std::tolower(letter)) != std::string::npos;
    }

    bool isComplete(const std::string &encoding) const
    {
        // return encoding.length() == MAX_CODE_LENGTH-1;
        return encoding.length() == MAX_CODE_LENGTH;
    }

    std::string lastDigit(const std::string encoding) const
    {
        if (encoding.empty())
            return "";

        return std::string(1, encoding.back());
        // return std::string{encoding.back()};
    }
};

const std::string Soundex::DIGIT_NOT_FOUND{"*"};

#endif // SOUNDEX_H
