#ifndef ADVANCED_H
#define ADVANCED_H

#include "VR_Tools_global.h"
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
#include "button_vr.h"
#include <rectangles.h>
#include <point.h>
#include "cursor.h"
#include "stringops.h"
#include "textline.h"
#include "keyboard.h"
#include "vrcreader.h"

#if LIN
   #include <cmath>
   using std::max;
#endif

using std::string;
using std::vector;

struct Action{
    string ActionName;
    button_VR *button;
    ulong number;
};
class advanced
{
public:
    advanced();
    ~advanced();

    void MakeAdvancedDialog(Hotspot htsp, std::function<void()> cBck);
    void MakeModalButton(string name, string actionName, ulong myNumber, int width, int height, int offsetX, int offsetY);
    void MakeLine(TextLine &in_Line, int in_x, int in_y, int in_width, int in_height, string in_text);
    void PrintLines();
    static void DrawMyself(XPLMWindowID, void * in_refcon);
    static int MouseHandler(XPLMWindowID in_window_id, int in_x, int in_y, int is_down, void * unused);
    void SelectLine(TextLine *in_line,int x,float *in_param);
    void ProcessKeyPress(bool special,string keyName,string in_String);
    void Backspace();
    void MoveCursorLeft();
    void MoveCursorRight();
    void EraseFromLine(int begin, int end);
    void EraseSelection();
    void ButtonAction(string actionName, ulong activeIdx);
    void GetBBoxDescription();
    void CommitActiveLine();

    void EndEdit();
    void Commit();
    Hotspot GetHotspot();
    bool IsToCommit();
    float ConvertUnitToMeter(int in_unit,float in_value);
    float ConvertMeterToUnit(int in_unit,float in_value);
    void  ChangeCurrentUnit(int new_unit, ulong activeIdx);
private:
    bool endAlert;
    static ulong selectedB;
    std::function<void()> callBack;
    static advanced *myself;
    Hotspot currentHsp;
    vector<Action*> actionButtons;
    float *activeParameter;
    float bBoxOffset,bBoxLatShift,bBoxAxShift,bBoxheight,bBoxWidth;
    float rot,tilt,pitch;
    static int left,bottom;
    TextLine sittingL,offsetL,rightShiftL,aftShiftL,heightL,widthL,rotL,tiltL,pitchL;
    TextLine *activeLine;
    string sittingT,offsetT,rightShiftT,aftShiftT,heightT,widthT,rotT,tiltT,pitchT;
    int sittingN,offsetN,rightShiftN,aftShiftN,heightN,widthN,rotN,tiltN,pitchN,hsNameN,hsBoxN,hsAdditionalN;
    rectangles rectSit,rectBox,rectParam,rectButtons;
    point sittingP,offsetP,rightShiftP,aftShiftP,heightP,widthP,rotP,tiltP,pitchP,hsNameP,hsBoxP,hsAdditionalP;
    string actionSelected;
    cursor cursor;
    vector<string> forcursor;
    Keyboard *numpad;
    bool mouseDrag;
    float epochClick;
    string notallowed;
    ulong maxlength;
    bool hasInterval;
    float minval,maxval;
    int currentUnit;
    ulong numberOfDigits;
    float gapincr,widthincr,heightincr,latshiftincr,axshiftincr,rotincr,tiltincr,pitchincr;

};

#endif // ADVANCED_H
