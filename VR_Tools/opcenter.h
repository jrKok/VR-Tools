#ifndef OPCENTER_H
#define OPCENTER_H

/*OpCenter is the operational center for VR Tools.
 * It launches almost all the servers for the different parts
 * of the plugin. It creates the environment, the plugin menu, the windows.
 *  It might even serve the dataref viewer in the future.
 * It receives mouseclicks and redirects them towards their destinations.
 * */

//#include "VR_Tools_global.h"
#include <memory>
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMDataAccess.h"
#include "XPLMPlugin.h"
#include "XPLMProcessing.h"
#include "XPLMUtilities.h"
#include "XPLMMenus.h"
#include <string>
#include "textreader.h"
#include "showdir.h"
#include "drefwindow.h"
#include "vrcommandfilter.h"
#include "filepointer.h"
#include "drawlogic.h"
#include "temporarywindow.h"
#include "hotspots.h"
#include "globals.h"
#include "fontman.h"
#include "managemodalwindow.h"
#include "filestack.h"
#include "vrcreader.h"

using std::string;

class OpCenter
{
public:
             OpCenter();
            ~OpCenter();
    //General setup
       bool  SetupCenter();
       void  LaunchOperations();
       void  HaltOperations();
       void  LoadHotspots();
       void  SuspendOperations();
       int   ResumeOperations();
static void  CheckVRMirror();
static void  CheckMenu();

static float DisplayLoop(float, float, int, void*);
static void  SetHotspotCall(bool htc);
//Loop to signal all open window managers to trigger update of draw params

    /*Forward declarations for X-Plane interaction*/
static void  drawText(XPLMWindowID, void *);
static void  drawFileWindow(XPLMWindowID in_window_id, void *);
static  int  handle_mouse_for_TextW (XPLMWindowID, int x, int y, XPLMMouseStatus mouse_status, void *);
static  int  handle_mouse_for_FileS(XPLMWindowID in_window_id, int x, int y, XPLMMouseStatus mouse_status, void *);
static void  handle_physical_keyboard(XPLMWindowID, char, XPLMKeyFlags, char, void*, int);

static  int  dummy_mouse_handler(XPLMWindowID, int, int, int, void *);
static XPLMCursorStatus dummy_cursor_status_handler(XPLMWindowID, int, int, void *);
static  int	 dummy_wheel_handler(XPLMWindowID in_window_id, int x, int y, int wheel, int clicks, void * in_refcon);
static void  dummy_key_handler(XPLMWindowID, char, XPLMKeyFlags, char, void*, int);

   //Menu driver
static void  menuHandler(void*menuRef, void* inItemRef);
       void  SetEnableHSMoves(bool has_hotspots);
       void  SetEnableTextOptions();
       void  SetCheckDataShow();
       void  TriggerDRefCommand(XPLMCommandRef in_command);
    //Run-Time driver
static  int  MyTextReaderCommandHandler  (XPLMCommandRef,
                                             XPLMCommandPhase   inPhase,
                                             void               *inRefcon);//toggle the text window
       void  MakeTextWindow();
       void  MakeMenus();
       void  MakeFileWindow();
       void  ReadNewFile();
       bool  HasModalWindow();
       void  SetModalWindow(bool mw);
       bool  IsLaunched;
       void  ToggleEditMode();
       bool  IsInEditMode();

private:
 static OpCenter *myself;
 std::unique_ptr<Layout> wLayout;
 std::unique_ptr<LayoutWithEdit> wELayout;
 std::unique_ptr<ShowDir> dispDir;
 std::unique_ptr<ManageModalWindow> manageMW;
 std::unique_ptr<DrawLogic> dpd[4];
 Layout* ptrLayout;
 XPLMCreateFlightLoop_t DLoop;
 XPLMFlightLoopID DLoopId;
 XPLMWindowID	g_textWindow,g_FileWindow;
 bool is_In_Edit_Mode,callHtsp;
 DRefWindow drefW;
 std::unique_ptr<Hotspots> htsp;
 VRCommandFilter commandFilter;
 int menuIdx;//master menu
 int opt[5];//text menu option
 int idxOfModeMenuItem,itemAdjusted,itemFast,itemSlow,itemReload,moveNext,movePrev,loadMirror,loadMirrorReloadPlane;//hotspot menu items
 int iFPS,iAoA,iTAS,iIAS,iGS,iGF,iWeather;//Dref items
 XPLMMenuID  menuId,menuText,menuTextOpt,menuHotspots,menuData;
 bool g_in_vr,hasModalWindow;

       bool has_been_setup, has_been_launched;
       XPLMDataRef g_vr_dref;
       XPLMCommandRef CommandText,CmdFirstLine,CmdNextLine,CmdPrevLine,CmdDelLine,CmdReload;
       IniSettings ini;
       temporaryWindow tw;
       globals colordefs;
       fontMan fontmanager;
       VRCReader vrcR;
};

#endif // OPCENTER_H
