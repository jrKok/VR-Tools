#ifndef TEMPORARYWINDOW_H
#define TEMPORARYWINDOW_H

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

using std::string;
using std::vector;

class temporaryWindow
{
public:
    temporaryWindow();
    static void DrawMyself(XPLMWindowID in_window_id, void *);
    static void ShowAlert(string in_String,float time_to_show);
    static void StopAlert();
private:
    static XPLMWindowID myXPWindow;
    static int myWindowNumber,myStringNumber;
    static float duration,timeStamp;
    static int width,height;
};

#endif // TEMPORARYWINDOW_H
