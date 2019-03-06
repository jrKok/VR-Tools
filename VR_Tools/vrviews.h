#ifndef VRVIEWS_H
#define VRVIEWS_H

#include "list_box_with_scrb.h"
#include <functional>
#include <vector>
#include <filesystem>
#include <memory>
#include <string>
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMDataAccess.h"
#include "XPLMPlugin.h"
#include "XPLMProcessing.h"
#include "XPLMUtilities.h"
#include "XPLMMenus.h"
#include "VR_Tools_global.h"
#include <localclipboard.h>
#include "modalbutton.h"
#include <rectangles.h>
#include <point.h>
#include "cursor.h"
#include "textline.h"
#include "keyboard.h"
#include "vrcreader.h"


using std::string;
using std::vector;

class VrViews:List_Box_With_ScrB
{
public:
    VrViews();
    ~VrViews();
    virtual void ShowAll();
    void MakeDialog(const string &yesStr, const string &noStr, const string &cancelStr, const string alertStr,const string &in_String, std::function<void()> cBck,int in_width=400);
    int  GetAnswer();
    int  GetSelectedHotspot();
    string GetUserLine();
    string GetActionLaunched();
    static void RecalculateDialog ();
    static void DrawMyself(XPLMWindowID in_window_id, void * in_refcon);
    static int MouseHandler(XPLMWindowID in_window_id, int x, int y, int is_down, void * unused);
    void ProcessKeyPress(std::string keyName,std::string in_String);
    void LaunchAction(string in_action);
    void CheckButtonsVisibility();
    void ContinueKeyPress();
    bool IsLineNotTooWide();
    void MoveCursorRight();
    void MoveCursorLeft();
    void EraseSelection();
    void EraseFromLine(int begin, int end);
    void MessageLine3(const string &in_line);
    void Cut();
    void Copy();
    void Paste();
    void Backspace();
    void Supress();
    void InsertLetter(string fromKeyb);
    void EndAlert();
    void Relog(float inX,float inY,float inZ);
    void DisableEdit();
    bool IsEditDisabled();
    //functionality : move to, rename, create, up, down, delete, save
private:

    std::function<void()> callBack;
    ModalButton *yesButton,*noButton,*cancelButton;
    ModalButton *relogButton,*upButton,*downButton,*renameButton,*createButton,*deleteButton,*repositionButton;
    ModalButton *clicked;
    rectangles *textRect;
    int width, height,answer,myStringNumber,myStringN2,myStringN3,textOffsetX,textOffsetY,selectedHotsp;
    float targetX,targetY,targetZ;
    string userLine;
    Keyboard *keyb;
    TextLine editLine;
    cursor cursor;
    bool specialKey;
    bool actionLaunched,mightSave,disableEdit;
    string action;
    static bool mouseLocated;
    static int left,top;
    static XPLMWindowID myXPWindow;
    static VrViews *myself;
    vector<string> forCursor;
};

#endif // VRVIEWS_H
