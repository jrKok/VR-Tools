#ifndef HOTSPOTS_H
#define HOTSPOTS_H
/*
 * This class handles the custom commands for hotspot management.
 * It executes the move to commands, triggers UI based hotspot editing with vrviews.
 * The writing and reading of the vr_config.txt is performed by the Vrcreader class.
 */
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

class OpCenter;
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
    static float MoveMeToHotSpot(float, float, int, void*); //flight loop callback
    static int   Create_Hotspot_Handler(XPLMCommandRef, XPLMCommandPhase inPhase, void *);
    static int   Edit_Hotspot_Handler(XPLMCommandRef, XPLMCommandPhase in_phase, void *);
    static void  MakeMoveComplete();
    static void  MakeMove4();
    static void  Handle_End_Of_Create_Command();
    static void  Handle_End_Of_Edit();
    static void  Handle_Advanced();
    static void  HandleErrorHotspotList();
    static void  SetMyCenter(OpCenter *opc);

    static void  ReloadCurrentAircraft();

    void  LogPilotHead(float &phx,float &phy,float &phz);
    void DoJump();

    static XPLMCommandRef CmdTurnLeft,CmdTurnRight,CmdJumpNext,CmdJumpBack;
    static XPLMCommandRef MoveToHS1,MoveToHS2,MoveToHS3,MoveToHS4;
    static XPLMCommandRef CmdEditHotspot;

private:
        static OpCenter     * myCenter;
        static XPLMDataRef    pilotX,pilotY,pilotZ,pilotPsi,outside;
        static XPLMDataRef    g_vr_dref;
        static XPLMCommandRef CmdRight,CmdLeft,CmdUp,CmdDown,CmdForward,CmdBackward;
        static XPLMCommandRef CmdRightF,CmdLeftF,CmdUpF,CmdDownF,CmdForwardF,CmdBackwardF;

        static XPLMCommandRef CmdX,CmdY,CmdZ,CmdTurn;
        static XPLMCommandRef CmdCreateHotspot,ReloadAC,VRReset;
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
