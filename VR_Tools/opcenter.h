#ifndef OPCENTER_H
#define OPCENTER_H

/*OpCenter is the operational center for VR Tools.
 * It launches almost all the servers for the different parts
 * of the plugin. It creates the environment, the plugin menu, the windows.
 *  It might even serve the dataref viewer in the future.
 * It receives mouseclicks and redirects them towards their destinations.
 * */

//#include "VR_Tools_global.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMDataAccess.h"
#include "XPLMPlugin.h"
#include "XPLMProcessing.h"
#include "XPLMUtilities.h"
#include "XPLMMenus.h"
#include <string>
#include "layout.h"
#include "textreader.h"
#include "showdir.h"
#include "layoutwithedit.h"
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

using std::string;

class OpCenter
{
public:
    OpCenter();
   ~OpCenter();
    int      SetupCenter();
    void     LaunchOperations();
    void     HaltOperations();
    void     SuspendOperations();
    int      ResumeOperations();
static float DisplayLoop(float, float, int, void*); //Loop to signal all open window managers to trigger update of draw params

    /*Forward declarations */
 static   void  drawText(XPLMWindowID, void *);
 static   void  drawFileWindow(XPLMWindowID in_window_id, void *);
 static   int   handle_mouse_for_TextW (XPLMWindowID, int x, int y, XPLMMouseStatus mouse_status, void *);
 static   int   handle_mouse_for_FileS(XPLMWindowID in_window_id, int x, int y, XPLMMouseStatus mouse_status, void *);
 static   void  handle_physical_keyboard(XPLMWindowID, char, XPLMKeyFlags, char, void*, int);

 static  int	dummy_mouse_handler(XPLMWindowID, int, int, int, void *);
 static XPLMCursorStatus dummy_cursor_status_handler(XPLMWindowID, int, int, void *);
 static   int	dummy_wheel_handler(XPLMWindowID in_window_id, int x, int y, int wheel, int clicks, void * in_refcon);
 static   void  dummy_key_handler(XPLMWindowID, char, XPLMKeyFlags, char, void*, int);

 static   void  menuHandler(void*, void* inItemRef);

 static   int   MyTextReaderCommandHandler  (XPLMCommandRef,
                                             XPLMCommandPhase   inPhase,
                                             void               *inRefcon);//toggle the text window
          void  MakeTextWindow();
          void  MakeMenus();
          void  MakeFileWindow();
          void  ReadNewFile();
static    void  EndEditMode();
static    bool  HasModalWindow();
static    void  SetModalWindow(bool mw);
static    bool  IsLaunched;

private:
static OpCenter * pointerToMe;
static XPLMCreateFlightLoop_t DLoop;
static XPLMFlightLoopID DLoopId;
static XPLMWindowID	g_textWindow,g_FileWindow;
static bool is_In_Edit_Mode;
static Layout *wLayout;
static LayoutWithEdit *wELayout;
static Layout* ptrLayout;
static ShowDir *dispDir;
static ManageModalWindow *manageMW;
static DRefWindow drefW;
static Hotspots htsp;
static VRCommandFilter commandFilter;

static int menuIdx,idxOfModeMenuItem,itemAdjusted,itemFast,itemSlow,itemReload;
static XPLMMenuID  menuId,menuTextOpt,menuHotspots;
static bool g_in_vr,hasModalWindow;
       XPLMDataRef g_vr_dref;
       XPLMCommandRef CommandText,CmdFirstLine,CmdNextLine,CmdPrevLine,CmdDelLine,CmdReload;
       IniSettings ini;
       //DrawLogic drw;
       temporaryWindow tw;
       globals colordefs;
       fontMan fontmanager;
       FileStack files;


};

#endif // OPCENTER_H
