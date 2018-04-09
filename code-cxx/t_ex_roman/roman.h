//
// Convert Arabic number to Roman number
//

#include <string>
#include <map>

class RomanConvert
{
    public:

        // 1. tried to use input/output string arg like 
        //
        //      void convert(uint32_t, std::string &)
        //
        // and use public string member in the fixture class.
        // However, since there are many asserts in a test, have to reset
        // this string member before using the next assert. dropped.
        //
        // 2. uses one single string and add at the front whenever converted a
        // letter. By this, no need to reverse the result as itoa() does but
        // there will be a downside; relocation and performance. Howerver, since
        // it is not a long string, do not think they matter here. 

        std::string convert(uint32_t number) const 
        {
            std::string converted;
            uint32_t tens{0}, digit{0};

            // for fixed roman letters
            // the code works without this to cover fixed romans since the loop
            // below will find it eventually. However, it uses a map already and
            // may save some time.
            
            auto it = roman_table.find(number);
            if (it != roman_table.cend())
                return it->second;

            for (; number;)
            {
                digit = number % 10;

                if (digit < 4)
                {
                    padWithFoundRoman(digit, tens, 0, converted);
                }
                else if (digit > 5 && digit < 9)
                {
                    padWithFoundRoman(digit, tens, 5, converted);
                }
                else if (!(digit % 5))
                {
                    findFiveBaseRoman(digit, tens, converted);
                }
                else if (digit == 4 || digit == 9)
                {
                    findSubstractRoman(digit, tens, converted);
                }

                ++tens;

                number /= 10;
            }

            return converted;
        }

    private:
        const std::map<uint32_t, std::string> roman_table{
            {0, ""},
            {1, "I"}, {5, "V"},
            {10, "X"}, {50, "L"},
            {100, "C"}, {500, "D"},
            {1000, "M"}
        };

        void padWithFoundRoman(uint32_t number, uint32_t tens, uint32_t fixed, std::string &padded) const
        {
            std::string result;

            // find the letter to pad
            auto it = roman_table.find( pow(10, tens) );
            if ( it == roman_table.cend() )
                std::cout << "roman_table(" << pow(10, tens) << ") not found" << std::endl;

            std::string letter = it->second;

            // find the base
            it = roman_table.find( fixed*pow(10, tens) );
            if ( it == roman_table.cend() )
                std::cout << "roman_table(" << fixed*pow( 10, tens ) << ") not found" << std::endl;

            std::string base = it->second;

            // add the base once
            result += base;

            // add the pad
            for (uint32_t count = number - fixed; count; --count)
                result += letter;

            padded.insert(0, result);
        }

        void findSubstractRoman(uint32_t number, uint32_t tens, std::string &converted) const
        {
            std::string padded;

            // find the letter in substract form
            auto it = roman_table.find( pow(10, tens) );
            if ( it == roman_table.cend() )
                std::cout << "roman_table(" << pow(10, tens) << ") not found" << std::endl;

            std::string letter = it->second;

            // find the base
            it = roman_table.find( (number+1)*pow(10, tens) );
            if ( it == roman_table.cend() )
                std::cout << "roman_table(" << (number+1)*pow( 10, tens ) << ") not found" << std::endl;

            std::string base = it->second;

            converted.insert( 0, letter + base );
        }

        void findFiveBaseRoman(uint32_t number, uint32_t tens, std::string &converted) const
        {
            // find the letter in substract form
            auto it = roman_table.find( number * pow(10, tens) );
            if ( it == roman_table.cend() )
                std::cout << "roman_table(" << pow(10, tens) << ") not found" << std::endl;

            converted.insert( 0, it->second );
        }
};

