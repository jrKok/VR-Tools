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
    static int   MyJumpCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void * inRefcon);
    static float readValuesCallback(float elpSc,float elpTime,int countr,void* refcon);
    static void  PrepareToMove();
    static float MoveMeToHotSpot(float elpSc,float elpTime,int countr,void* refcon);

    //dummies because not needed for operation but needed for declaration
    static int	 dummy_mouse_handler(XPLMWindowID in_window_id, int x, int y, int is_down, void * in_refcon);
    static XPLMCursorStatus dummy_cursor_status_handler(XPLMWindowID in_window_id, int x, int y, void * in_refcon);
    static int	 dummy_wheel_handler(XPLMWindowID in_window_id, int x, int y, int wheel, int clicks, void * in_refcon);
    static void  dummy_key_handler(XPLMWindowID in_window_id,char in_key,XPLMKeyFlags in_flag,char in_VK,void* refcon,int is_losing_focus);
    static int   Create_Hotspot_Handler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void * inRefcon);
    static int   Edit_Hotspot_Handler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void * inRefcon);
    static void  Handle_End_Of_Create_Command();
    static void  Handle_End_Of_Edit();
    static float UpdateValue(float elpSc,float elpTime,int countr,void* refcon);
    static void  ReloadCurrentAircraft();


    static XPLMWindowID	idDrefWindow;
    static bool isInVRMode,isShowModeOnPress;
    static float cyan[3],gray[3];
    static int counter,whatToShow;
    static float currentDRefValue, currentmeasure,period;
    static vector<float> movingAverage;
    static string valueToShow;
    static DRefWindow * myself;
    static bool isLatched;

    static XPLMDataRef    g_vr_dref,g_FPS,g_IAS,g_TAS,g_GS,g_GFcD,g_AOA,g_GFcAcf,g_accX,g_accY,g_accZ,mousejoy;
    static XPLMDataRef    pilotX,pilotY,pilotZ,pilotPsi,outside;
    static XPLMCommandRef CommandFPS ,CommandIAS,CommandTAS,CommandGS;
    static XPLMCommandRef CommandGFD,CommandAOA,CommandGFA,CmdJumpNext,CmdJumpBack;
    static XPLMCommandRef CmdRight,CmdLeft,CmdUp,CmdDown,CmdForward,CmdBackward;
    static XPLMCommandRef CmdRightF,CmdLeftF,CmdUpF,CmdDownF,CmdForwardF,CmdBackwardF;
    static XPLMCommandRef CmdTurnLeft,CmdTurnRight;
    static XPLMCommandRef CmdX,CmdY,CmdZ,CmdTurn;
    static XPLMCommandRef CmdCreateHotspot,ReloadAC,CmdEditHotspot,VRReset;

    bool GetShowModeOnPress();
    void DisposeWindow();
    void  LogPilotHead(float &phx,float &phy,float &phz);

protected:
    static float targetX,targetY,targetZ,targetPsi;
    static int signX,signY,signZ,signRot;
    static int phaseMove;
    static bool fastX,fastY,fastZ,doneX,doneY,doneZ,dialog_ongoing,filterblock;
    static XPLMCreateFlightLoop_t moveLoop;
    static XPLMFlightLoopID moveLoopId;
    static LineDialog *dlg;
    static VrViews *vrconfigEditor;
    void SetupWindow();
    void GetAOA();
    void GetGForceDown();
    void GetGForceCalculated();
    void GetFPS();
    void GetIAS();
    void GetGS();
    void GetTAS();
    void GetLatched();
    void DoJump();

};

#endif // DREFWINDOW_H
