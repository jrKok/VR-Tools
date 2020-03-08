#include "key.h"
#include "drawlogic.h"

Key::Key(bool modal) : button_VR(modal),
    ch1(0),
    ch2(0),
    isUTF8(false),
    isSpecial(false),
    keyShowString(""),
    keyMainString(""),
    keyShiftString(""),
    keyAltString(""),
    keyAbbrString(""),
    keyAbbrName(""),
    line(0),
    column(0)

{

}
void Key::DefineKey(char c1, char c2, bool utf, bool spec, string showVal, string mainVal,string shiftVal, string altVal, string abbrName, string abbrVal, int ln, int cl){
    ch1=c1;
    ch2=c2;
    isUTF8=utf;
    isSpecial=spec;
    keyShowString=showVal;
    keyMainString=mainVal;
    keyShiftString=shiftVal;
    keyAltString=altVal;
    keyAbbrString=abbrVal;
    keyAbbrName=abbrName;
    setText(showVal);
    DrawLogic::ChangeColorString(stringNumber,Clr_White);
    setColor(Clr_LightBlue);
    line=ln;
    column=cl;
    DrawLogic::SetVisibilityString(stringNumber,true);

}

Key* Key::GetMyPointer(){
    return this;
}

string Key::GetKeyName(){
    if (isSpecial) return keyMainString;
    else return keyShowString;
}
string Key::GetKeyMainVal(){
    return keyMainString;
}

string Key::GetKeyShift(){
    return keyShiftString;
}

string Key::GetKeyAlt(){
    return keyAltString;
}

string Key::GetKeyAbbrVal(){
    return keyAbbrString;
}

string Key::GetKeyAbbrName(){
    return keyAbbrName;
}

void Key::SetKeyShiftText(string sStr){
    keyShiftString=sStr;
}

void Key::SetKeyAltText(string aStr){
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

