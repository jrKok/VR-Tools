#ifndef DREFWINDOW_H
#define DREFWINDOW_H

// class for handling the "show FPS" and other datarefs
// defines static functions for X Planes callbacks
//weather report : when atis tuned, get qnh, T°, wind direction, wind speed, dew point, visibility, cloud lower limit and density, risk of thunderstorms
//when no ATIS, qnh, T°, vis under or over 1 nm, wind speed and direction

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

#if LIN
    #include <GL/gl.h>
#else
    #include <glew.h>
   // #include <gl/GL.h>
#endif

using std::string;
using std::vector;

class DRefWindow
{
public:
    DRefWindow();
    ~DRefWindow();
    void ToggleShowMode();
    void Setup();
    void Unload();

    static void	 drawDRef(XPLMWindowID in_window_id, void * in_refcon);
    static void  drawWReport(XPLMWindowID in_window_id, void * in_refcon);
    static int   MyDRefCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void * inRefcon);
    static float UpdateValue(float, float, int, void*);


    //dummies because not needed for operation but needed for declaration
    static int	 dummy_mouse_handler(XPLMWindowID, int, int, int, void *);
    static XPLMCursorStatus dummy_cursor_status_handler(XPLMWindowID, int, int, void *);
    static int	 dummy_wheel_handler(XPLMWindowID, int, int, int, int, void *);
    static void  dummy_key_handler(XPLMWindowID, char, XPLMKeyFlags, char, void*, int);



    static XPLMWindowID	idDrefWindow;
    static bool isInVRMode,isShowModeOnPress,alwaysWeather;
    static float cyan[3],gray[3];
    static int counter,whatToShow;
    static float currentDRefValue, currentmeasure,period;
    static vector<float> movingAverage;
    static string valueToShow;
    static string s_qnh,s_temps,s_winds,s_clouds,s_vis,s_thunderStorms;
    static DRefWindow * myself;

    static XPLMDataRef    g_vr_dref,g_FPS,g_IAS,g_TAS,g_GS,g_GFcD,g_AOA,g_GFcAcf,g_accX,g_accY,g_accZ,mousejoy;
    static XPLMDataRef w_cloudType0,w_cT1,w_cT2,w_CloudBase0,w_cB1,w_cB2,w_visibility,w_qnh,w_windSpeed, w_windDirection,w_temp, w_dewP, w_thunderS;
    static XPLMDataRef w_radio1Tuned,w_radio2Tuned,w_ATISon;
    static XPLMCommandRef CommandFPS ,CommandIAS,CommandTAS,CommandGS;
    static XPLMCommandRef CommandGFD,CommandAOA,CommandGFA;
    static XPLMCommandRef CommandWeatherReport;

    bool GetShowModeOnPress();
    void DisposeWindow();

protected:



    void SetupWindow(bool isReport=false);
    void GetAOA();
    void GetGForceDown();
    void GetGForceCalculated();
    void GetFPS();
    void GetIAS();
    void GetGS();
    void GetTAS();
    void GetWeatherReport();
    void GetSmallWeatherReport();
    void GetATISWeatherReport();


};

#endif // DREFWINDOW_H
