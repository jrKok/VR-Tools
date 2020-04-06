#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <key.h>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "XPLMUtilities.h"
#include "VR_Tools_global.h"

using std::string;
//typedef std::vector<std::shared_ptr<Key>>  lineOfKeys;
typedef std::vector<Key*>  lineOfKeys;
class LayoutWithEdit;

class Keyboard
{
public:
    Keyboard(bool modal=false);
    ~Keyboard();
    void    WriteDebug(string message);
    string  ConvString(const string &in_String);
    void    MakeKeyDefs(const string &in_Line,std::vector<string> &out_keys);
    void    FindKeySpecs(const string &in_Def,
                      bool &isSpec,
                      string &showString,
                      string &mainString,
                      string &shiftString,
                      string &altString,
                      string &abbrName,
                      string &abbrString,
                      int &spacing,
                      int &keyWidth,
                      ulong &vk_tag);
    int     MakeLine(const int &offY, const string &keyDefs, int lineNumber, lineOfKeys &keyLine);
    void    MakeKeyboard(int oX, int oY,bool numpad=false);

    void    SetOrigin (int x, int y);
    void    Recalculate(int l, int t);
    void    Relocate (int newX, int newY);

    bool    ReadLine(int cx, int cy, lineOfKeys in_line, bool &special, string &keyName, string &keyVal);
    bool    PressKey(int cx, int cy, bool &special, string &keyName, string &keyVal);
    string  ReadKey(Key *key, bool &special, string &keyName);

    bool    IsKeyPressed();
    void    ReleaseCurrentKey();
    void    ShiftPressed();
    void    ReleaseStates();
    void    ShiftCapsPressed();
    void    CTRLPressed();
    void    SetCTRLToggle(bool val);
    void    ALTPressed();
    void    AbbrPressed();
    void    ToShiftKeys();
    void    ToNormalKeys();
    void    ToAltKeys();
    void    ToAbbrKeys();
    int     MyWidth();
    int     MyHeight();
    int     MyBottom();
    int     MyLeft();
    bool    IsSpecialKey();
    bool    IsShiftKeyActive();
    bool    IsCapsKeyActive();
    bool    IsControlKeyActive();
    bool    IsAltKeyActive();
    bool    IsAbbreviationsKeyActive();
    void    SetWarningMode(bool warning);
    void    SetVisibility(bool vis);
    void    ShowDebugKeys();

static int  Physical_Key_Handler(char in_char,XPLMKeyFlags flag,char in_VKs,void* inRefcon);

static  lineOfKeys physicalKeys;
static string activeKeyName,activeKeyString;
static bool activeIsSpecialKey;
static Keyboard* myKeyboard;
     void    EnablePhysicalKeyboard(void *caller);
     void    DisablePhysicalKeyboard(void *caller);

    rectangles allKeyboard,rL1,rL2,rL3,rL4,rL5;

private:
    bool isModal;
    lineOfKeys  kL1,kL2,kL3,kL4,kL5;
    Key* activeKey;
    Key* shiftKeyLeft,*shiftKeyRight,*ctrlKey,*altKey,*abbrvKey,*capsKey;
    int offX, offY;
    int keyHeight,keyWidth;
    int currentKeyLine,currentKeyIdx;
    float virtualKeyTimer,virtualKeyLatency;
    bool physicalKyBdEnabled;
    bool keyPressed,shiftToggle,capsToggle,ctrlToggle,altToggle,abbreviationsToggle,specialKey;
static bool physicalKeyPressed;
static int  activeVKCode;
static float physicalKeyTimer,latency;


};

#endif // KEYBOARD_H
