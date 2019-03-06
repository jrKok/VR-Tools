#ifndef LINEDIALOG_H
#define LINEDIALOG_H

#include "modalbutton.h"
#include "VR_Tools_global.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMDataAccess.h"
#include "XPLMPlugin.h"
#include "XPLMProcessing.h"
#include "XPLMUtilities.h"
#include "XPLMMenus.h"
#include <memory>
#include <string>
#include <rectangles.h>
#include <point.h>
#include <functional>
#include "cursor.h"
#include "textline.h"
#include "keyboard.h"
#include <vector>
#include <localclipboard.h>

using std::string;
using std::vector;

class LineDialog
{
public:
    LineDialog();
    ~LineDialog();
    void MakeDialog(const string &yesStr, const string &noStr, const string &cancelStr, const string alertStr,const string &in_String, std::function<void()> cBck,int in_width=400);
    int  GetAnswer();
    string GetUserLine();
    static void Recalculate ();
    static void DrawMyself(XPLMWindowID in_window_id, void * in_refcon);
    static int MouseHandler(XPLMWindowID in_window_id, int x, int y, int is_down, void * unused);
    void ProcessKeyPress(std::string keyName,std::string in_String);
    void ContinueKeyPress();
    bool IsLineNotTooWide();
    void MoveCursorRight();
    void MoveCursorLeft();
    void EraseSelection();
    void EraseFromLine(int begin, int end);
    void Cut();
    void Copy();
    void Paste();
    void Backspace();
    void Supress();
    void InsertLetter(string fromKeyb);
    void EndAlert();//deletes what needs to be, calls a callback provided by the caller

private:

    std::function<void()> callBack;
    ModalButton *yesButton,*noButton,*cancelButton;
    rectangles *textRect;
    int width, height,answer,myStringNumber,textOffsetX,textOffsetY;
    string userLine;
    Keyboard *keyb;
    TextLine editLine;
    cursor cursor;
    bool specialKey;
    static bool mouseLocated;
    static int left,top;
    static XPLMWindowID myXPWindow;
    static LineDialog *myself;
    vector<string> forCursor;
};

#endif // LINEDIALOG_H
