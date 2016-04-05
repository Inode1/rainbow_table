#include <iostream>
#include <random>
#include <string>
#include <chrono>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

#include "md5.h"

constexpr uint8_t asciBegin = 0x20;
constexpr uint8_t asciEnd   = 0x7e;
constexpr uint8_t mode   = asciEnd - asciBegin + 1;
constexpr uint8_t wordSize  = 5;
constexpr uint16_t reductionFunctionCount = 1000;
constexpr uint32_t wordCount = 10000;
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

std::string getWordBySeed(uint32_t seed, uint32_t hash)
{
    std::minstd_rand0 g1(seed ^ hash);

    std::string result;
    result.reserve(wordSize);
    for (int i = 0; i < wordSize; ++i)
    {
        uint32_t temp = g1() % mode + asciBegin;

        result += temp;
    }
    return result;
}

struct HashMd5
{
    std::string               Value;
    //mutable std::vector<std::string>  HashChain;
    size_t                    Hash;

    HashMd5(const std::string& passwordValue)
    {
        Value = passwordValue;
        //HashChain.reserve(2 * reductionFunctionCount - 1);
        Hash = md5_64bit(Value);
    }

    bool operator==(const HashMd5& that)
    {
        return Value == that.Value;
    }

/*    void AddElementHashChain(const std::string& lhs)
    {
        HashChain.push_back(lhs);
    }*/

};

bool operator==(const HashMd5& lhs, const HashMd5& rhs)
{
    return lhs.Value == rhs.Value;
}

namespace std {
  template <>
  struct hash<HashMd5>
  {
    size_t operator()(const HashMd5 & t) const
    {
        return t.Hash;
    }
  };
}

int main()
{
    std::vector<uint32_t> reducionFunction;       
    reducionFunction.reserve(reductionFunctionCount);
    for (int i = 0; i < reductionFunctionCount; ++i)
    {
        reducionFunction.push_back(i);
    }

    std::unordered_set<HashMd5>              collisionPasswordSet;
    collisionPasswordSet.reserve(wordCount);
    std::unordered_map<std::string, std::string>  reverseCollision;
    reverseCollision.reserve(wordCount);


    // generate all table
    std::cout << "Begin generate rainbow table:" << std::endl
              << "                 Word Length: " << static_cast<uint32_t>(wordSize) << std::endl
              << "    Reduction function count: " << static_cast<uint32_t>(reductionFunctionCount) << std::endl
              << "    Count word in dictionary: " << static_cast<uint32_t>(wordCount) << std::endl;
    auto start = std::chrono::system_clock::now();

    for (uint32_t i = 0; i < wordCount; ++i)
    {
        //std::cout << "Iteration: " << i << std::endl;
        HashMd5 value{generateWord()};
        if (collisionPasswordSet.find(value) != collisionPasswordSet.end())
        {
            --i;
            continue;
        }

        auto it = collisionPasswordSet.insert(value).first;
        bool isMd5Time = true;
        std::string middleResult = it->Value; 
        
        for (uint32_t j = 0; j < 2 * reductionFunctionCount; ++j)
        {
            middleResult = isMd5Time ? md5(middleResult) 
                                     : getWordBySeed(reducionFunction[j / 2], md5_64bit(middleResult));
            isMd5Time = !isMd5Time;
        }   

        reverseCollision.emplace(middleResult, value.Value);
    }

/*    for (const auto& a: reverseCollision)
    {
        std::cout << a.first << "  " << a.second << std::endl;
    }
*/
    std::cout << "Rainbow table build" << std::endl
              << "              Elapse time: " << std::chrono::duration_cast<std::chrono::microseconds>
                                           (std::chrono::system_clock::now() - start).count() << std::endl
              << "collisionPasswordSet size: " <<  collisionPasswordSet.size() << std::endl
              << "    reverseCollision size: " << reverseCollision.size() << std::endl; 

    // try to find pass by hash
    // H -> R1 -> H -> R2 -> H -> R3
    std::string hashNeedFind = "b781cbb29054db12f88f08c6e161c199";
    //std::cout << "Enter hash:";
    //std::cin >> hashNeedFind;
    std::cout << "Find result for hash: " << hashNeedFind << std::endl;

    
    std::string backReduction;

    for (int i = reductionFunctionCount - 1; i >= 0; --i)
    {
        auto hash = md5_64bit(hashNeedFind);
        for (int j = i; j < reductionFunctionCount - 1; ++j)
        {
            hash = md5_64bit(md5(getWordBySeed(reducionFunction[j], hash)));
        }
        backReduction = getWordBySeed(hash, reducionFunction.back());
        //std::cout << "back backReduction: " << backReduction << "  " << i << "  " << std::endl;
        auto isFind = reverseCollision.find(backReduction);
        if (isFind != reverseCollision.end())
        {
            auto positon = collisionPasswordSet.find(isFind->second);

            bool isMd5Time = true;
            std::string previousValue; 
            std::string middleResult = positon->Value;
            
            for (uint32_t j = 0; j < 2 * reductionFunctionCount; ++j)
            {
                if (middleResult == hashNeedFind)
                {
                    std::cout << "Element found in HashChain: " << positon->Value << '\n';
                    std::cout << "            Password found: " << previousValue << '\n';
                    return 0;
                }
                previousValue = middleResult;
                middleResult = isMd5Time ? md5(middleResult) 
                                         : getWordBySeed(reducionFunction[j / 2], md5_64bit(middleResult));
                //std::cout << "    " << previousValue << "      " << std::endl; 
                isMd5Time = !isMd5Time;
            }
        }
    }

    std::cout << "Password not found" << std::endl;
    return 0;
}