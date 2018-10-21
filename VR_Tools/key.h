#ifndef KEY_H
#define KEY_H

#include <button_vr.h>


class Key : public button_VR
{
public:
    Key();
    void DefineKey(char c1, char c2, bool utf, bool spec, std::string showVal, std::string mainVal,std::string shiftVal, std::string altVal, std::string abbrVal, int ln, int cl);
    std::string GetKeyName();
    std::string GetKeyMainVal();
    std::string GetKeyShift();
    std::string GetKeyAlt();
    std::string GetKeyAbbr();
    void SetKeyShiftText(std::string sStr);
    void SetKeyAltText(std::string aStr);
    char GetChar1();
    char GetChar2();
    bool IsSpecialKey();
    bool IsUTFKey();
    void reposition (int dX,int dY);
    Key* GetMyPointer();
private:
    char ch1,ch2;
    bool isUTF8,isUTFShift,isUTFAlt,isSpecial;
    std::string keyShowString,keyMainString,keyShiftString,keyAltString,keyAbbrString;
    int line, column;

};

#endif // KEY_H
