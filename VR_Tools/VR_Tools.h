#ifndef SHOW_FPS_TEST_H
/* future developments :
 *
 *version 1.3
 * emit beta
 *
 * make temporary files for storing edition when resizing, or quiting without save (define a bool to check if editions have been made)
 * make temporary file (bck) to store initial state of file for cancel command
 * improve abbreviations routines : add in keyboard's definition text file a value to be returned, make routines read this value
 *  - key shortcuts for text : RWY,ILS, CRS, HDG, BRG, VOR,NDB, ALT, qnh, hPa, WPT, FRQ, Comm, ... (2 chars or 1 spec Key)
 * enhance draw functions of all buttons in making them in a sole routine (or class)
 * implement physical keyboard callback
 * make a repeat feature for keyboards
 * correct bugs
 *
 * make cut/copy/paste with windows clipboard
 * define international keyboards, make a line in .ini to define which keyboard is to be used.
 *
 *  menus : new file when avaible for V2), )
 *
 * look again into comm frequencies (sometimes 0.005 doesn't seem to be taken)

  * add an ini option to select if toggle means close text window or hide window
 *
 * directory shortcuts (mostly my plane, then a more elaborate directory shortcut manager)
 * ini files in airplanes directory
 *
 * version 1.4 :
 * create dialogs : simple with OK; simple with 2 choices (Yes No return false or true), simple with 3 choices (return int), complex with return a string
 * make helper dialogs for control keys
 * headposition saver/take position to one of the config.txt positions
 * fuel manager
 * make datarefs (or static vars) for filename, directory name, positions in file
 * make const references for function calls and ranged for loops
 * modern C++ style implementation of windows
 * modern openGL
 * texture keyboard
 * multifiles
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
#include <layoutwithedit.h>
#include <drefwindow.h>
#include <vrcommandfilter.h>

using std::string;

#if IBM
    #include <Windows.h>
#endif
#if LIN
    #include <GL/gl.h>
#elif __GNUC__
    #include <OpenGL/gl.h>
#else
    #include <gl/GL.h>
#endif

#ifndef XPLM301
    #error This is made to be compiled against the XPLM301 SDK
#endif
#endif // SHOW_FPS_TEST_H
