#ifndef SHOW_FPS_TEST_H
/* future developments :
 *
 * try setVrWindowsizes
 *
 * make custom commands for physical buttons and menu,
 *  commands : select first line -works also for dir window, looks up which picker is selected -
 *         ,   line up and
 *             down -also in dir window -,
 *             delete line,
 *             show all
 *  menus : fit to file
 *          keep file,
 *          keep size
 *          toggle FPS display, (new file when avaible for V2), )
 *
 * look again into comm frequencies (sometimes 0.005 doesn't seem to be taken)
  * then V2 with edit
  * add an ini option to select if toggle means close text window or hide window
 * text edit
 * multifiles
 * directory shortcuts
 * ini files in airplanes directory
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
