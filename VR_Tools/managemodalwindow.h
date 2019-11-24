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

class ManageModalWindow
{
public:
    ManageModalWindow();
    ~ManageModalWindow();
    static XPLMWindowID CreateModalWindow( void drawCB(XPLMWindowID,void*),char myColor,int width, int height);
    static XPLMWindowID CreateMousedModalWindow(int mouseH(XPLMWindowID, int, int, int, void*), void drawCB(XPLMWindowID,void*), char myColor, int width, int height);
    static void ResizeModalWindow(int width, int height);
    static void DestroyModalWindow();
    static void Recalculate(int x, int y);
    static void PrintmyRectvisstatus();
    static XPLMWindowID GetCurrentWindowId();

private:
    static rectangles *myRect;
    static XPLMDataRef g_vr_dref;
    static XPLMWindowID myModalWindow;
    static int myWidth,myHeight;
    static int	dummy_mouse_handler(XPLMWindowID in_window_id, int x, int y, int is_down, void * in_refcon);
    static XPLMCursorStatus dummy_cursor_status_handler(XPLMWindowID in_window_id, int x, int y, void * in_refcon);
    static int	dummy_wheel_handler(XPLMWindowID in_window_id, int x, int y, int wheel, int clicks, void * in_refcon);
    static void dummy_key_handler(XPLMWindowID in_window_id,char in_key,XPLMKeyFlags in_flag,char in_VK,void* refcon,int is_losing_focus);
    static float UpdateValue(float elpSc,float elpTime,int countr,void* refcon);
};

#endif // MANAGEMODALWINDOW_H
