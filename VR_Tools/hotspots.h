#ifndef HOTSPOTS_H
#define HOTSPOTS_H
#include "VR_Tools_global.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMDataAccess.h"
#include "XPLMPlugin.h"
#include "XPLMProcessing.h"
#include "XPLMUtilities.h"
#include <memory>
#include <string>
#include "vrcreader.h"
#include "linedialog.h"
#include "temporarywindow.h"
#include "vrviews.h"
#include "advanced.h"

using std::string;
using std::vector;

class Hotspots
{
public:
      Hotspots();
     ~Hotspots();
void  Setup();

    static int   MyJumpCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void * inRefcon);
    static float readValuesCallback(float elpSc,float elpTime,int countr,void* refcon);
    static void  PrepareToMove();
    static float MoveMeToHotSpot(float elpSc,float elpTime,int countr,void* refcon);
    static int   Create_Hotspot_Handler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void * inRefcon);
    static int   Edit_Hotspot_Handler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void * inRefcon);
    static void  MakeMoveComplete();
    static void  MakeMove4();
    static void  Handle_End_Of_Create_Command();
    static void  Handle_End_Of_Edit();
    static void  Handle_Advanced();

    static void  ReloadCurrentAircraft();

    void  LogPilotHead(float &phx,float &phy,float &phz);
    void DoJump();

    static XPLMCommandRef CmdTurnLeft,CmdTurnRight,CmdJumpNext,CmdJumpBack;
    static XPLMCommandRef MoveToHS1,MoveToHS2,MoveToHS3,MoveToHS4;

private:
        static XPLMDataRef    pilotX,pilotY,pilotZ,pilotPsi,outside;
        static XPLMDataRef    g_vr_dref;
        static XPLMCommandRef CmdRight,CmdLeft,CmdUp,CmdDown,CmdForward,CmdBackward;
        static XPLMCommandRef CmdRightF,CmdLeftF,CmdUpF,CmdDownF,CmdForwardF,CmdBackwardF;

        static XPLMCommandRef CmdX,CmdY,CmdZ,CmdTurn;
        static XPLMCommandRef CmdCreateHotspot,ReloadAC,CmdEditHotspot,VRReset;
        static float targetX,targetY,targetZ,targetPsi;
        static int signX,signY,signZ,signRot;
        static int phaseMove;
        static bool fastX,fastY,fastZ,doneX,doneY,doneZ,dialog_ongoing,filterblock;
        static Hotspots * myself;
        static XPLMCreateFlightLoop_t moveLoop;
        static XPLMFlightLoopID moveLoopId;
        static LineDialog *dlg;
        static VrViews *vrconfigEditor;
        static advanced *advancedEditor;

};

#endif // HOTSPOTS_H
