#ifndef DREFWINDOW_H
#define DREFWINDOW_H

// class for handling the "show FPS" and other datarefs
// defines static functions for X Planes callbacks

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
#include <layout.h>
#include "vrcreader.h"
#include "linedialog.h"
#include "temporarywindow.h"
#include "vrviews.h"
#include "advanced.h"

using std::string;
using std::vector;

class DRefWindow
{
public:
    DRefWindow();
    ~DRefWindow();
    void WriteDebug(string in_string);
    void ToggleShowMode();
    void Setup();
    void Unload();

    static void	 drawDRef(XPLMWindowID in_window_id, void * in_refcon);
    static int   MyDRefCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void * inRefcon);
    static float UpdateValue(float elpSc,float elpTime,int countr,void* refcon);


    //dummies because not needed for operation but needed for declaration
    static int	 dummy_mouse_handler(XPLMWindowID in_window_id, int x, int y, int is_down, void * in_refcon);
    static XPLMCursorStatus dummy_cursor_status_handler(XPLMWindowID in_window_id, int x, int y, void * in_refcon);
    static int	 dummy_wheel_handler(XPLMWindowID in_window_id, int x, int y, int wheel, int clicks, void * in_refcon);
    static void  dummy_key_handler(XPLMWindowID in_window_id,char in_key,XPLMKeyFlags in_flag,char in_VK,void* refcon,int is_losing_focus);



    static XPLMWindowID	idDrefWindow;
    static bool isInVRMode,isShowModeOnPress;
    static float cyan[3],gray[3];
    static int counter,whatToShow;
    static float currentDRefValue, currentmeasure,period;
    static vector<float> movingAverage;
    static string valueToShow;
    static DRefWindow * myself;

    static XPLMDataRef    g_vr_dref,g_FPS,g_IAS,g_TAS,g_GS,g_GFcD,g_AOA,g_GFcAcf,g_accX,g_accY,g_accZ,mousejoy;
    static XPLMCommandRef CommandFPS ,CommandIAS,CommandTAS,CommandGS;
    static XPLMCommandRef CommandGFD,CommandAOA,CommandGFA;

    bool GetShowModeOnPress();
    void DisposeWindow();


protected:



    void SetupWindow();
    void GetAOA();
    void GetGForceDown();
    void GetGForceCalculated();
    void GetFPS();
    void GetIAS();
    void GetGS();
    void GetTAS();


};

#endif // DREFWINDOW_H
