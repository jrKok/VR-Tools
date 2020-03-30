#ifndef MANAGEMODALWINDOW_H
#define MANAGEMODALWINDOW_H

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
#include "drawlogic.h"

class ManageModalWindow
{
public:
    ManageModalWindow(DrawLogic * dp);
    ~ManageModalWindow();
    static XPLMWindowID CreateModalWindow( void drawCB(XPLMWindowID,void*),char myColor,int width, int height);
    static XPLMWindowID CreateMousedModalWindow(int mouseH(XPLMWindowID, int, int, int, void*), void drawCB(XPLMWindowID,void*), char myColor, int width, int height);
    static void         PickUpDrawPad();
    static void         ResizeModalWindow(int width, int height);
    static void         DestroyModalWindow();
    static void         SetScreenCoords(int left, int bottom,int right, int top);
    static void         PrintmyRectvisstatus();
    static XPLMWindowID GetCurrentWindowId();
    static void         MakeTopWindow();
    static void         ReactivateDrawPad();
    static void         ConstrainGeometry();
    static  int         GetLeft();
    static  int         GetBottom();
    static void         HideMyRect();
    static void         DebugRects();

private:
    static rectangles        * myRect;
    static XPLMDataRef       g_vr_dref;
    static XPLMWindowID      myModalWindow;
    static int               myWidth,myHeight,myLeft,myBottom;
    static ManageModalWindow *myself;
    static int               dummy_mouse_handler(XPLMWindowID, int, int, int, void *);
    static XPLMCursorStatus  dummy_cursor_status_handler(XPLMWindowID, int, int, void *);
    static int               dummy_wheel_handler(XPLMWindowID, int, int, int, int, void *);
    static void              dummy_key_handler(XPLMWindowID, char, XPLMKeyFlags, char, void*, int);
    static float             UpdateValue(float elpSc,float elpTime,int countr,void* refcon);
           DrawLogic         *myDrawPad;
};

#endif // MANAGEMODALWINDOW_H
