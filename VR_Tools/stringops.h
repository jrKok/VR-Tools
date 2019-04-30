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
static       void WriteDebug(string message);
static       bool contains(const string inString, const string inContains);
static     string splitRight(const string inString, const string split);
static     string cleanOut(const string inString,const string inToClean);
static     string bestLeft(string &inString, int MaxL);
static     string bestLeftSize(string &inString, int in_sz);
static        int findLengthForSize (string inString,int in_sz);
static     string splitAtSize(string &inString, int in_sz);
static        int StringSize(string in_str);
static     string ToUTF8(string ansiSTR);
static     string DecodeInstruction(string in_instr,string &out_right,string &comment);
static     string Trim(const string& str, const string& whitespace = " \t");
static     string splitRightAtSize(string &inString, int in_sz);
static     string RemoveLastUTFCharFromString(string in_string);
static      bool  IsANumber(string to_test);
static     string ConvertFloatToString(float in_float, ulong prec=4);
static     float  ConvertStringToFloat(string in_string);

private:
static string digits,numSigns;
};

#endif // STRINGOPS_H
