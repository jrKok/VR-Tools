#ifndef STRINGOPS_H
#define STRINGOPS_H
#include <string>
#include <vector>
#include <fstream>
#include <XPLMDisplay.h>
#include <XPLMGraphics.h>
#include <XPLMUtilities.h>

typedef long long unsigned ulong;
class stringOps
{
public:
    stringOps();
    void WriteDebug(std::string message);
    bool contains(const std::string inString, const std::string inContains);
    std::string splitRight(const std::string inString, const std::string split);
    std::string cleanOut(const std::string inString,const std::string inToClean);
    std::string bestLeft(std::string &inString, int MaxL);
    std::string bestLeftSize(std::string &inString, int in_sz);
    int findLengthForSize (std::string inString,int in_sz);
    std::string splitAtSize(std::string &inString, int in_sz);
    int StringSize(std::string in_str);
    std::string ToUTF8(std::string ansiSTR);
    std::string DecodeInstruction(std::string in_instr,std::string &out_right,std::string &comment);
    std::string Trim(const std::string& str,
                     const std::string& whitespace = " \t");
    std::string splitRightAtSize(std::string &inString, int in_sz);
    static std::string RemoveLastUTFCharFromString(std::string in_string);
};

#endif // STRINGOPS_H
