#ifndef SHOW_FPS_TEST_H
/* future developments :
 * beta2 resize
 * b3 ini
 * commands
 *
 * then V2 with edit
 * for debug :
 *
 * replace to last page after resize if lastpage is true
 * look in resize limits
 * try setVrWindowsizes
 *
 *
 *
 *
 * - implement undo
 *
 * make custom commands for physical buttons and menu, (select first line -works also for dir window, looks up which picker is selected -
 *         , line up and down -also in dir window -, resize, load file, new file when avaible, delete line,show all)
 * resize display by custom commands or drag drawn handlers or even test for size
 * create ini file and customisation of behavior (like begin with file launched,directory for startup,keep last file opened, toggle = hide or destroy sizes of display)
 *
 * ini options :
 *
 * Presets : dev console
 * start options
 *  open window at start : bool
 *  autoreload at open : bool
 *  change color briefly when text changed :bool
 *  move to last page at open : bool
 * display :
 *  width : int
 *  height : int
 *  fit to file : bool
 *  fixed dimensions : bool
 * file
 *  default directory : string
 *  default file : string
 *  always load last file opened : bool
 *  trim line option : truncate line, split at space, split at max larg
 * look into the nav/com/adf availability issue
 *function :
 *  del/all buttons : bool
 *  NAV,COM,ADF buttons : bool
 *Datarefs displayed
 *
 * version 2 :
 *
 * text edit
 * multifiles
 * directory shortcuts
 *
 * */

#include "VR_Tools_global.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMDataAccess.h"
#include "XPLMPlugin.h"
#include "XPLMProcessing.h"
#include "XPLMUtilities.h"
#include "XPLMMenus.h"
#include "string.h"
#include <string>
#include <layout.h>
#include <stringops.h>
#include <textreader.h>
#include <showdir.h>


#if IBM
    #include <windows.h>
#endif
#if LIN
    #include <GL/gl.h>
#elif __GNUC__
    #include <OpenGL/gl.h>
#else
    #include <GL/gl.h>
#endif

#ifndef XPLM301
    #error This is made to be compiled against the XPLM301 SDK
#endif

#endif // SHOW_FPS_TEST_H
