#include "key.h"

Key::Key() : button_VR(),
    ch1(0),
    ch2(0),
    isUTF8(false),
    isSpecial(false),
    keyShowString(""),
    keyMainString(""),
    keyShiftString(""),
    keyAltString(""),
    keyAbbrString(""),
    line(0),
    column(0)

{

}
void Key::DefineKey(char c1, char c2, bool utf, bool spec, std::string showVal, std::string mainVal,std::string shiftVal, std::string altVal, std::string abbrVal, int ln, int cl){
    ch1=c1;
    ch2=c2;
    isUTF8=utf;
    isSpecial=spec;
    keyShowString=showVal;
    keyMainString=mainVal;
    keyShiftString=shiftVal;
    keyAltString=altVal;
    keyAbbrString=abbrVal;
    setText(showVal);
    line=ln;
    column=cl;
}

Key* Key::GetMyPointer(){
    return this;
}

std::string Key::GetKeyName(){
    if (isSpecial) return keyMainString;
    else return keyShowString;
}
std::string Key::GetKeyMainVal(){
    return keyMainString;
}

std::string Key::GetKeyShift(){
    return keyShiftString;
}

std::string Key::GetKeyAlt(){
    return keyAltString;
}

std::string Key::GetKeyAbbr(){
    return keyAbbrString;
}

void Key::SetKeyShiftText(std::string sStr){
    keyShiftString=sStr;
}

void Key::SetKeyAltText(std::string aStr){
    keyAltString=aStr;
}

char Key::GetChar1(){
    return ch1;
}

char Key::GetChar2(){
    return ch2;
}

bool Key::IsSpecialKey(){
    return isSpecial;
}

bool Key::IsUTFKey(){
    return isUTF8;
}

void Key::reposition (int dX,int dY){
    offsetX+=dX;
    offsetY+=dY;
    offsetTextX+=dX;
    offsetTextY+=dY;
    recalculate();
}
