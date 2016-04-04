#include <iostream>
#include <random>
#include <string>

#include "md5.h"

constexpr uint8_t asciBegin = 0x20;
constexpr uint8_t asciEnd   = 0x7e;
constexpr uint8_t wordSize  = 5;
constexpr uint16_t reductionFunctionCount  = 1000;
std::random_device rd;
std::mt19937 gen(rd());

std::string generateWord()
{
    std::uniform_int_distribution<> dis(asciBegin, asciEnd);    
    std::string password;
    password.reserve(wordSize);
    for (int i = 0; i < wordSize; ++i) 
    {
        password += dis(gen);
    }
    return password;
}

int main()
{
    std::vector<uint32_t> reducionFunction;       
    reducionFunction.reserve(reductionFunctionCount);
    for (int i = 0; i < reductionFunctionCount; ++i)
    {
        reducionFunction.push_back(i);
    }

    struct HashMd5
    {
        std::string Value;
        bool operator==(const HashMd5& that)
        {
            return Value == that.Value;
        }

        size_t operator()()
        {
            return md5_64bit(Value); 
        }

    };


    std::cout << "md5 of 'grape': " << md5("grape") << std::endl;
    return 0;
}