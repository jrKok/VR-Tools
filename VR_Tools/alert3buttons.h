#ifndef ALERT3BUTTONS_H
#define ALERT3BUTTONS_H

#include "button_vr.h"
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

using std::string;
class Alert3Buttons
{
public:
    Alert3Buttons();
    void MakeAlert(const string &yesStr, const string &noStr, const string &cancelStr, const string alertStr, std::function<void()> cBck);
    int  GetAnswer();
    static void Recalculate ();
    static void DrawMyself(XPLMWindowID in_window_id, void *);
    static int MouseHandler(XPLMWindowID in_window_id, int ix, int iy, int is_down, void * unused);
    void EndAlert();//deletes what needs to be, calls a callback provided by the caller
private:

    std::function<void()> callBack;
    button_VR *yesButton,*noButton,*cancelButton;
    int width, height,answer,myStringNumber,textOffsetX,textOffsetY;
    static bool mouseLocated;
    static int left,bottom;
    static XPLMWindowID myXPWindow;
    static Alert3Buttons *myself;

};

#endif // ALERT3BUTTONS_H
