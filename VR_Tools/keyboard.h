#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <key.h>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "layout.h"

using std::string;

class Keyboard
{
public:
    Keyboard();
    void    WriteDebug(string message);

    void    DefineKeyboard (int type);//0 : american english, then for others waiting for development
    string  ConvString(const string &in_String);
    void    MakeKeyDefs(const string &in_Line,std::vector<string> &out_keys);
    void    FindKeySpecs(const string &in_Def,
                      bool &isSpec,
                      string &showString,
                      string &mainString,
                      string &shiftString,
                      string &altString,
                      string &abbrString,
                      int &spacing,
                      int &keyWidth);
    int     MakeLine(const int &offY, const string &keyDefs, int lineNumber, std::vector<std::shared_ptr<Key>> &keyLine);
    void    MakeKeyboard(int oX, int oY);

    void    SetOrigin (int x, int y);
    void    Recalculate(int l, int t);
    void    Relocate (int newX, int newY);
    void    DrawKb();

    bool    PressKey(int cx, int cy, bool &special, std::string &keyName, std::string &keyVal);
    string  ReadKey(std::shared_ptr<Key> key, bool &special, std::string &keyName);
    string  Physical_Key_Handler(char in_char,XPLMKeyFlags flag,char in_VK);
    bool    IsKeyPressed();
    void    ReleaseCurrentKey();
    void    ShiftPressed();
    void    ReleaseStates();
    void    ShiftCapsPressed();
    void    CTRLPressed();
    void    ALTPressed();
    void    AbbrPressed();
    void    ToShiftKeys();
    void    ToNormalKeys();
    void    ToAltKeys();
    void    ToAbbrKeys();
    int     MyWidth();
    int     MyHeight();
    bool    IsSpecialKey();
    bool    IsShiftKeyActive();
    bool    IsCapsKeyActive();
    bool    IsControlKeyActive();
    bool    IsAltKeyActive();
    bool    IsAbbreviationsKeyActive();

    rectangles allKeyboard,rL1,rL2,rL3,rL4,rL5;
private:
    std::vector<std::shared_ptr<Key>> kL1,kL2,kL3,kL4,kL5;
    string defL1,defL2,defL3,defL4,defL5;
    Key* activeKey;
    Key* shiftKeyLeft,*shiftKeyRight,*ctrlKey,*altKey,*abbrvKey,*capsKey;
    int offX, offY;
    int keyHeight,keyWidth;
    int currentKeyLine,currentKeyIdx;
    bool keyPressed,shiftToggle,capsToggle,ctrlToggle,altToggle,abbreviationsToggle,specialKey;

};

#endif // KEYBOARD_H
