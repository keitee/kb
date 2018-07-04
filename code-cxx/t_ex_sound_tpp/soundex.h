/***
 * Excerpted from "Modern C++ Programming with Test-Driven Development",
 * published by The Pragmatic Bookshelf.
 * Copyrights apply to this code. It may not be used to create training material, 
 * courses, books, articles, and the like. Contact us if you are in doubt.
 * We make no guarantees that this code is fit for any purpose. 
 * Visit http://www.pragmaticprogrammer.com/titles/lotdd for more book information.
***/
#ifndef Soundex_h
#define Soundex_h

class Soundex {
public:
   Soundex() {
      initializeCodeMap();
   }

   void initializeCodeMap() {
      codes_['b'] = codes_['f'] = codes_['p'] = codes_ ['v'] = "1";
      codes_['c'] = codes_['g'] = codes_['j'] = codes_ ['k'] = 
         codes_['q'] = codes_['s'] = codes_['x'] = codes_['z'] = "2";
      codes_['d'] = codes_['t'] = "3";
      codes_['l'] = "4";
      codes_['m'] = codes_['n'] = "5";
      codes_['r'] = "6";
   }

   std::string encode(const std::string& word) const {
      std::string code("");
      code += head(word);
      encodeTail(word, code);
      return zeroPad(code);
   }
   // ...

   char head(const std::string& word) const {
      return word[0];
   }

   void encodeTail(const std::string& word, std::string& code) const {
      if (word[1] == 0) return;
      code += codeFor(word[1]);
      encodeTail(tail(word), code);
   }
   std::string tail(const std::string& word) const {
      return word.substr(1);
   }

   std::string codeFor(char c) const {
      return codes_[static_cast<size_t>(c)];
   }

   std::string zeroPad(const std::string& code) const {
      return code + (hasEncodedCharacters(code) ? "00" : "000");
   }

   bool hasEncodedCharacters(const std::string& code) const {
      return code[1] != 0;
   }

private:
   std::string codes_[128];
};

#endif
