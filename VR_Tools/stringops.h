#ifndef STRINGOPS_H
#define STRINGOPS_H
#include <string>
#include <vector>
#include <fstream>
#include <XPLMDisplay.h>
#include <XPLMGraphics.h>
#include <XPLMUtilities.h>

using std::string;

typedef long long unsigned ulong;
class stringOps
{
public:
    stringOps();
    void WriteDebug(string message);
    bool contains(const string inString, const string inContains);
    string splitRight(const string inString, const string split);
    string cleanOut(const string inString,const string inToClean);
    string bestLeft(string &inString, int MaxL);
    string bestLeftSize(string &inString, int in_sz);
    int findLengthForSize (string inString,int in_sz);
    string splitAtSize(string &inString, int in_sz);
    int StringSize(string in_str);
    string ToUTF8(string ansiSTR);
    string DecodeInstruction(string in_instr,string &out_right,string &comment);
    string Trim(const string& str,
                     const string& whitespace = " \t");
    string splitRightAtSize(string &inString, int in_sz);
    static string RemoveLastUTFCharFromString(string in_string);
};

#endif // STRINGOPS_H
