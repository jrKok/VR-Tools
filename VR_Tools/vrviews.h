#ifndef VRVIEWS_H
#define VRVIEWS_H

#include "list_box_with_scrb.h"
#include <functional>
#include <vector>
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
#include <rectangles.h>
#include <point.h>
#include "cursor.h"
#include "textline.h"
#include "keyboard.h"
#include "vrcreader.h"
#include "button_vr.h"
#include "drawlogic.h"


using std::string;
using std::vector;

enum{
    action_Relog=1,
    action_Up=2,
    action_Down=3,
    action_Create=4,
    action_Delete=5,
    action_Reposition=6,
    action_Rename=7,
    action_MoveUp=8,
    action_MoveDown=9,
    action_MoveAft=10,
    action_MoveFor=11,
    action_MoveRight=12,
    action_MoveLeft=13,
    action_TurnLeft=14,
    action_TurnRight=15,
    action_TiltUp=16,
    action_TiltDown=17,
    action_skewRight=16,
    action_skewLeft=17
};

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
    int GetActionLaunched();
    static void RecalculateDialog ();
    static void DrawMyself(XPLMWindowID in_window_id, void * in_refcon);
    static int MouseHandler(XPLMWindowID in_window_id, int x, int y, int is_down, void * unused);
    void ProcessKeyPress(string keyName,string in_String);
    void LaunchAction(int in_action);
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
    void MouseToUp();
    //functionality : move to, rename, create, up, down, delete, save
private:

    void LaunchMoveCommand();
    static XPLMCommandRef CmdRight,CmdLeft,CmdUp,CmdDown,CmdForward,CmdBackward;
    XPLMCommandRef CommandLaunched;
    std::function<void()> callBack;
    button_VR *yesButton,*noButton,*cancelButton,*advancedButton;
    button_VR *relogButton,*upButton,*downButton,*renameButton,*createButton,*deleteButton,*repositionButton;
    button_VR *vaftButton,*vforwButton,*vupButton,*vdownButton,*vleftButton,*vrightButton;
    button_VR *clicked;
    rectangles *textRect;
    int width, height,answer,myStringNumber,myStringN2,myStringN3,textOffsetX,textOffsetY,selectedHotsp;
    float targetX,targetY,targetZ;
    string userLine;
    Keyboard *keyb;
    TextLine editLine;
    cursor cursor;
    bool specialKey;
    bool actionLaunched,mightSave,disableEdit,filterblock;
    int action;
    static bool mouseLocated;
    static int left,top,bottom,right;
    static XPLMWindowID myXPWindow;
    static VrViews *myself;
    vector<string> forCursor;
    bool mouseDrag,mouseUp;
    float epochClick;
};

#endif // VRVIEWS_H
