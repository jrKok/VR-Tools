#ifndef KEY_H
#define KEY_H

#include <button_vr.h>

using std::string;

class Key : public button_VR
{
public:
    Key(bool modal=false);
    void DefineKey(char c1, char c2, bool utf, bool spec, string showVal, string mainVal,string shiftVal, string altVal, string abbrName, string abbrVal, int ln, int cl);
    string GetKeyName();
    string GetKeyMainVal();
    string GetKeyShift();
    string GetKeyAlt();
    string GetKeyAbbrVal();
    string GetKeyAbbrName();
    void SetKeyShiftText(string sStr);
    void SetKeyAltText(string aStr);
    char GetChar1();
    char GetChar2();
    bool IsSpecialKey();
    bool IsUTFKey();
    void reposition (int dX,int dY);
    Key* GetMyPointer();
private:
    char ch1,ch2;
    bool isUTF8,isUTFShift,isUTFAlt,isSpecial;
    string keyShowString,keyMainString,keyShiftString,keyAltString,keyAbbrString,keyAbbrName;
    int line, column;

};

#endif // KEY_H
