#include "drefwindow.h"
#include "drawlogic.h"
#include "vrcommandfilter.h"

XPLMWindowID DRefWindow::idDrefWindow;

bool  DRefWindow::isInVRMode=true;
bool  DRefWindow::isShowModeOnPress=true;
bool  DRefWindow::alwaysWeather=true;
float DRefWindow::cyan[]={0,1.0,1.0};
float DRefWindow::gray[]={0.375f,0.375f,0.368f};
vector<float> DRefWindow::movingAverage(10,0);
int   DRefWindow::counter(0);
int   DRefWindow::whatToShow(0);
float DRefWindow::currentDRefValue(0);
float DRefWindow::currentmeasure(0);
float DRefWindow::period(0);

string DRefWindow::valueToShow("");
string DRefWindow::s_qnh("");
string DRefWindow::s_temps("");
string DRefWindow::s_winds("");
string DRefWindow::s_clouds("");
string DRefWindow::s_vis("");
string DRefWindow::s_thunderStorms("");

DRefWindow * DRefWindow::myself(nullptr);

XPLMDataRef DRefWindow::g_vr_dref(nullptr);
XPLMDataRef DRefWindow::g_FPS(nullptr);
XPLMDataRef DRefWindow::g_IAS(nullptr);
XPLMDataRef DRefWindow::g_TAS(nullptr);
XPLMDataRef DRefWindow::g_GS(nullptr);
XPLMDataRef DRefWindow::g_GFcD(nullptr);
XPLMDataRef DRefWindow::g_AOA(nullptr);
XPLMDataRef DRefWindow::g_GFcAcf(nullptr);
XPLMDataRef DRefWindow::g_accX(nullptr);
XPLMDataRef DRefWindow::g_accY(nullptr);
XPLMDataRef DRefWindow::g_accZ(nullptr);
XPLMDataRef DRefWindow::mousejoy(nullptr);

XPLMDataRef DRefWindow::w_radio1Tuned(nullptr);
XPLMDataRef DRefWindow::w_radio2Tuned(nullptr);
XPLMDataRef DRefWindow::w_ATISon(nullptr);
XPLMDataRef DRefWindow::w_cloudType0(nullptr);
XPLMDataRef DRefWindow::w_cT1(nullptr);
XPLMDataRef DRefWindow::w_cT2(nullptr);
XPLMDataRef DRefWindow::w_CloudBase0(nullptr);
XPLMDataRef DRefWindow::w_cB1(nullptr);
XPLMDataRef DRefWindow::w_cB2(nullptr);
XPLMDataRef DRefWindow::w_visibility(nullptr);
XPLMDataRef DRefWindow::w_qnh(nullptr);
XPLMDataRef DRefWindow::w_windSpeed(nullptr);
XPLMDataRef DRefWindow::w_windDirection(nullptr);
XPLMDataRef DRefWindow::w_temp(nullptr);
XPLMDataRef DRefWindow::w_dewP(nullptr);
XPLMDataRef DRefWindow::w_thunderS(nullptr);

XPLMCommandRef DRefWindow::CommandFPS(nullptr);
XPLMCommandRef DRefWindow::CommandIAS(nullptr);
XPLMCommandRef DRefWindow::CommandTAS(nullptr);
XPLMCommandRef DRefWindow::CommandGS(nullptr);
XPLMCommandRef DRefWindow::CommandGFD(nullptr);
XPLMCommandRef DRefWindow::CommandAOA(nullptr);
XPLMCommandRef DRefWindow::CommandGFA(nullptr);
XPLMCommandRef DRefWindow::CommandWeatherReport(nullptr);



DRefWindow::DRefWindow()

{

}

DRefWindow::~DRefWindow(){
myself=nullptr;
}

void DRefWindow::Setup(){
    g_vr_dref        = XPLMFindDataRef("sim/graphics/VR/enabled");
    g_FPS            = XPLMFindDataRef("sim/time/framerate_period");
    if (g_FPS==nullptr){
        g_FPS        = XPLMFindDataRef("sim/operation/misc/frame_rate_period");
        DrawLogic::WriteDebug("will use misc/frame_rate_period for FPS");}
    g_IAS            = XPLMFindDataRef("sim/flightmodel/position/indicated_airspeed");
    g_TAS            = XPLMFindDataRef("sim/flightmodel/position/true_airspeed");
    g_GS             = XPLMFindDataRef("sim/flightmodel/position/groundspeed");
    g_GFcD           = XPLMFindDataRef("sim/flightmodel/forces/g_nrml");
    g_AOA            = XPLMFindDataRef("sim/flightmodel/position/alpha");
    g_GFcAcf         = XPLMFindDataRef("sim/flightmodel/forces/g_axil");
    g_accX           = XPLMFindDataRef("sim/flightmodel/position/local_ax");
    g_accY           = XPLMFindDataRef("sim/flightmodel/position/local_ay");
    g_accZ           = XPLMFindDataRef("sim/flightmodel/position/local_az");
    mousejoy         = XPLMFindDataRef("sim/operation/override/override_joystick");
    w_radio1Tuned    = XPLMFindDataRef("sim/atc/com1_active");
    w_radio2Tuned    = XPLMFindDataRef("sim/atc/com2_tuned_facility");
    w_ATISon         = XPLMFindDataRef("sim/atc/atis_enabled");
    w_cloudType0     = XPLMFindDataRef("sim/weather/cloud_type[0]");
    w_cT1            = XPLMFindDataRef("sim/weather/cloud_type[1]");
    w_cT2            = XPLMFindDataRef("sim/weather/cloud_type[2]");
    w_CloudBase0     = XPLMFindDataRef("sim/weather/cloud_base_msl_m[0]");
    w_cB1            = XPLMFindDataRef("sim/weather/cloud_base_msl_m[1]");
    w_cB2            = XPLMFindDataRef("sim/weather/cloud_base_msl_m[2]");
    w_visibility     = XPLMFindDataRef("sim/weather/visibility_reported_m");
    w_qnh            = XPLMFindDataRef("sim/weather/barometer_sealevel_inhg");
    w_windSpeed      = XPLMFindDataRef("sim/weather/wind_speed_kt[0]");
    w_windDirection  = XPLMFindDataRef("sim/weather/wind_direction_degt[0]");
    w_temp           = XPLMFindDataRef("sim/weather/temperature_sealevel_c");
    w_dewP           = XPLMFindDataRef("sim/weather/dewpoi_sealevel_c");
    w_thunderS       = XPLMFindDataRef("sim/weather/thunderstorm_percent");

    CommandFPS       = XPLMCreateCommand("VR_Tools/Custom/Show_Data/ShowFPS", "Show FPS");
    CommandIAS       = XPLMCreateCommand("VR_Tools/Custom/Show_Data/ShowIAS", "Show IAS");
    CommandTAS       = XPLMCreateCommand("VR_Tools/Custom/Show_Data/ShowTAS", "Show TAS");
    CommandGS        = XPLMCreateCommand("VR_Tools/Custom/Show_Data/ShowGS", "Show GS");
    CommandGFD       = XPLMCreateCommand("VR_Tools/Custom/Show_Data/ShowGFDown", "Show G-Force Down");
    CommandAOA       = XPLMCreateCommand("VR_Tools/Custom/Show_Data/ShowAOA", "Show Angle of Attack");
    CommandGFA       = XPLMCreateCommand("VR_Tools/Custom/Show_Data/ShowGFComp", "Show G-Force Computed");
CommandWeatherReport = XPLMCreateCommand("VR_Tools/Custom/Show_Data/ShowWeather","Weather Report");


    void * nb=new(int*);
    (*(int*)nb)=0;
    XPLMRegisterCommandHandler(CommandFPS,MyDRefCommandHandler,1, nb);

    nb=new(int*);
    (*(int*)nb)=1;
    XPLMRegisterCommandHandler(CommandIAS,MyDRefCommandHandler,1,nb);
    nb=new(int*);(*(int*)nb)=2;
    XPLMRegisterCommandHandler(CommandTAS,MyDRefCommandHandler,1,nb);
    nb=new(int*);(*(int*)nb)=3;
    XPLMRegisterCommandHandler(CommandGS,MyDRefCommandHandler,1,nb);
    nb=new(int*);(*(int*)nb)=4;
    XPLMRegisterCommandHandler(CommandGFD,MyDRefCommandHandler,1,nb);
    nb=new(int*);(*(int*)nb)=5;
    XPLMRegisterCommandHandler(CommandAOA,MyDRefCommandHandler,1,nb);
    nb=new(int*);(*(int*)nb)=6;
    XPLMRegisterCommandHandler(CommandGFA,MyDRefCommandHandler,1,nb);
    nb=new(int*);(*(int*)nb)=7;
    XPLMRegisterCommandHandler(CommandWeatherReport,MyDRefCommandHandler,1,nb);

    myself=this;
    //rectangles rect(true);
    //DrawLogic::AddRectangle(rect,Clr_Black);
}

void DRefWindow::Unload(){
    if (idDrefWindow != nullptr){
        XPLMDebugString("Unloading VR window\r\n");
        XPLMSetWindowPositioningMode(idDrefWindow,xplm_WindowPositionFree,0);
        XPLMDestroyWindow(idDrefWindow);
        idDrefWindow = nullptr;
        counter=0;
        for (ulong I(0);I<10;I++){
            movingAverage[I]=0;
        }}
}

void DRefWindow::ToggleShowMode(){
    isShowModeOnPress=!isShowModeOnPress;
}

void DRefWindow::drawDRef(XPLMWindowID, void *) // draw FPS Window's content
{
    XPLMSetGraphicsState( 0,0,0,0,1,1,0);
    int l, t, r, b;
    XPLMGetWindowGeometry(idDrefWindow, &l, &t, &r, &b);

    glColor3fv(DRefWindow::gray);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glBegin(GL_TRIANGLE_FAN);
    {
        glVertex2i(l+1, t-1);
        glVertex2i(r-1, t-1);
        glVertex2i(r-1, b+1);
        glVertex2i(l+1, b+1);
    }
    glEnd();

        XPLMDrawString(DRefWindow::cyan, l+10, t-16, (char *)valueToShow.c_str(), nullptr, xplmFont_Proportional);

}

void DRefWindow::drawWReport(XPLMWindowID, void *){
    XPLMSetGraphicsState( 0,0,0,0,1,1,0);
    int l, t, r, b;
    XPLMGetWindowGeometry(idDrefWindow, &l, &t, &r, &b);

   /* glColor3fv(DRefWindow::gray);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glBegin(GL_TRIANGLE_FAN);
    {
        glVertex2i(l+1, t-1);
        glVertex2i(r-1, t-1);
        glVertex2i(r-1, b+1);
        glVertex2i(l+1, b+1);
    }
    glEnd();*/

        XPLMDrawString(DRefWindow::cyan, l+10, t-16, (char *)s_qnh.c_str(), nullptr, xplmFont_Proportional);
        XPLMDrawString(DRefWindow::cyan, l+10, t-28, (char *)s_winds.c_str(), nullptr, xplmFont_Proportional);
        XPLMDrawString(DRefWindow::cyan, l+10, t-40, (char *)s_vis.c_str(), nullptr, xplmFont_Proportional);
        XPLMDrawString(DRefWindow::cyan, l+10, t-52, (char *)s_temps.c_str(), nullptr, xplmFont_Proportional);
        XPLMDrawString(DRefWindow::cyan, l+10, t-64, (char *)s_clouds.c_str(), nullptr, xplmFont_Proportional);
        XPLMDrawString(DRefWindow::cyan, l+10, t-76, (char *)s_thunderStorms.c_str(), nullptr, xplmFont_Proportional);

}


int DRefWindow::MyDRefCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon){
    switch (inPhase)
    {
    case xplm_CommandBegin :
            {
            //int old=whatToShow;
            whatToShow=*(static_cast<int*>(inRefcon));
            if (whatToShow==7)
                myself->SetupWindow(true);
            else
                myself->SetupWindow();
        break;}


    case xplm_CommandEnd :
        if (isShowModeOnPress){
        XPLMSetWindowIsVisible(idDrefWindow,0);
        myself->DisposeWindow();
        break;}
    }
    return 0; //this one I've handled
}



void DRefWindow::SetupWindow(bool isReport){

    if (idDrefWindow==nullptr){//I prefer to recreate the window to keep a clean cockpit
        int addHeight(0),addWidth(0);
        if (isReport){ addHeight=85; addWidth=120;}
        const int vr_is_enabled = XPLMGetDatai(g_vr_dref);
        isInVRMode = vr_is_enabled;
        int global_desktop_bounds[4];
        XPLMGetScreenBoundsGlobal(&global_desktop_bounds[0], &global_desktop_bounds[3], &global_desktop_bounds[2], &global_desktop_bounds[1]);

        XPLMCreateWindow_t params;
         params.structSize = sizeof(params);
         params.visible = 1;
         if (isReport)
             params.drawWindowFunc= drawWReport;
         else
             params.drawWindowFunc       = drawDRef;
         params.handleMouseClickFunc = dummy_mouse_handler;
         params.handleRightClickFunc = dummy_mouse_handler;
         params.handleMouseWheelFunc = dummy_wheel_handler;
         params.handleKeyFunc        = dummy_key_handler;
         params.handleCursorFunc     = dummy_cursor_status_handler;
         params.refcon               = nullptr;
         params.layer                    = xplm_WindowLayerFloatingWindows;
         params.decorateAsFloatingWindow = xplm_WindowDecorationNone;
         params.left   = global_desktop_bounds[0] + 20;
         params.bottom = global_desktop_bounds[1] + 70;
         params.right  = global_desktop_bounds[0] + 160+addWidth ;
         params.top    = global_desktop_bounds[1] + 120+addHeight;

        idDrefWindow = XPLMCreateWindowEx(&params);
         XPLMSetWindowPositioningMode(idDrefWindow, isInVRMode?xplm_WindowVR:xplm_WindowPositionFree, -1);
         XPLMSetWindowResizingLimits(idDrefWindow, 160, 20, 160, 20);
         XPLMSetWindowTitle(idDrefWindow, "VAL");
         //Determine period of callback, then function pointer to callback
         switch (whatToShow){
            case 0 : period=0.10f;counter=0;break;
            case 1 : period=0.2f;break;
            case 2 : period=0.2f;break;
            case 3 : period=0.2f;break;
            case 4 : period=0.1f;break;
            case 5 : period=0.1f;break;
            case 6 : period=0.1f;break;
            case 7 : period=60.0f;break; // for weather report
         }
        XPLMRegisterFlightLoopCallback(UpdateValue,-1,nullptr);

    }
    else {if (!isShowModeOnPress){
            XPLMSetWindowIsVisible(idDrefWindow,0);
            myself->DisposeWindow();
        }
    }
}

void DRefWindow::DisposeWindow(){
    if (idDrefWindow!=nullptr)
    {
        XPLMDestroyWindow(idDrefWindow);
        idDrefWindow=nullptr;
        whatToShow=0;
        period=0.1f;
        for (ulong I(0);I<10;I++){
            movingAverage[I]=0;
        }
        counter=0;
        XPLMUnregisterFlightLoopCallback(UpdateValue,nullptr);
    }
}

void DRefWindow::GetAOA(){
    currentDRefValue=XPLMGetDataf(g_AOA);
    bool neg=(currentDRefValue<0);
    currentDRefValue=std::abs(currentDRefValue);
    int conv=static_cast<int>(std::floor(currentDRefValue));
    float frac=(currentDRefValue-conv)*100.0f;
    int rem=static_cast<int>(std::floor(frac));
    std::string ngs=(neg?"-":"");
    valueToShow=("AOA: ")+ngs+std::to_string(conv)+"."+std::to_string(rem);
}

void DRefWindow::GetGForceDown(){
    currentDRefValue=XPLMGetDataf(g_GFcD);
    bool neg=(currentDRefValue<0);
    currentDRefValue=std::abs(currentDRefValue);
    int conv=static_cast<int>(std::floor(currentDRefValue));
    float frac=(currentDRefValue-conv)*100.0f;
    int rem=static_cast<int>(std::floor(frac));
    std::string ngs=(neg?"-":"");
    valueToShow=("G-F Dn: ")+ngs+std::to_string(conv)+"."+std::to_string(rem);
}

void DRefWindow::GetGForceCalculated(){
    float ax=XPLMGetDataf(g_accX);
    float ay=XPLMGetDataf(g_accY);
    float az=XPLMGetDataf(g_accZ);
    currentDRefValue=(std::sqrt(std::pow(ax,2.0f)+std::pow((ay+9.82f),2)+std::pow(az,2.0f)))/9.82f;
    bool neg=(currentDRefValue<0);
    currentDRefValue=std::abs(currentDRefValue);
    int conv=static_cast<int>(std::floor(currentDRefValue));
    float frac=(currentDRefValue-conv)*100.0f;
    int rem=static_cast<int>(std::floor(frac));
    std::string ngs=(neg?"-":"");
    valueToShow=("G-F comp: ")+ngs+std::to_string(conv)+"."+std::to_string(rem);
}

void DRefWindow::GetFPS(){
    currentDRefValue=1/(XPLMGetDataf(g_FPS));
    valueToShow= std::to_string(currentDRefValue);
    valueToShow= "FPS: "+valueToShow.substr(0,4);
}

void DRefWindow::GetIAS(){
    currentDRefValue=XPLMGetDataf(g_IAS);
    int conv=static_cast<int>(std::round(currentDRefValue));
    valueToShow=("IAS: ")+std::to_string(conv);
}

void DRefWindow::GetGS(){
   currentDRefValue=XPLMGetDataf(g_GS);
   int conv=static_cast<int>(std::round(currentDRefValue*1.94384f));
   valueToShow=("GS: ")+std::to_string(conv);
}

void DRefWindow::GetTAS(){
    currentDRefValue=XPLMGetDataf(g_TAS);
    int conv=static_cast<int>(std::round(currentDRefValue*1.94384f));
    valueToShow=("TAS: ")+std::to_string(conv);
}

bool DRefWindow::GetShowModeOnPress(){
    return isShowModeOnPress;
}

void DRefWindow::GetWeatherReport(){
    float qnh=XPLMGetDataf(w_qnh);
    float qnhINhPa=qnh*33.8639f;
    s_qnh="qnh = "+stringOps::ConvertFloatToString(qnhINhPa,0);
    float windDir=XPLMGetDataf(w_windDirection),windSpeed=XPLMGetDataf(w_windSpeed);
        s_winds="Winds From"+stringOps::ConvertFloatToString(windDir,0) + " @"+stringOps::ConvertFloatToString(windSpeed,0)+" kts";
    s_thunderStorms="";
    s_temps="";
    s_vis="";
    s_clouds="no comms available";
    s_winds="";
    if (alwaysWeather) GetATISWeatherReport();
    else{
        if ((XPLMGetDatai(w_radio1Tuned)||XPLMGetDatai(w_radio2Tuned))){
            if (XPLMGetDatai(w_ATISon)){
                GetATISWeatherReport();
            }
            else{
                GetSmallWeatherReport();
                s_thunderStorms="radio 1 to " +std::to_string(XPLMGetDatai(w_radio1Tuned))
                        +" radio 2 to "+std::to_string(XPLMGetDatai(w_radio2Tuned));
            }
        }
    }
}

void DRefWindow::GetSmallWeatherReport(){
    //qnh float is in mmHg, convert to hPa
    float qnh=XPLMGetDataf(w_qnh);
    float qnhINhPa=qnh*33.8639f;
    s_qnh="qnh = "+stringOps::ConvertFloatToString(qnhINhPa,0)+" hPa "+stringOps::ConvertFloatToString(qnh,2) + (" inHg");
    //T° float
    float temp=XPLMGetDataf(w_temp);
    s_temps="Ground temp = "+stringOps::ConvertFloatToString(temp,0)+ " °C";
    //wind Dir float
    //Wind speed float
    float windDir=XPLMGetDataf(w_windDirection),windSpeed=XPLMGetDataf(w_windSpeed);
        s_winds="Winds From"+stringOps::ConvertFloatToString(windDir,0) + " @"+stringOps::ConvertFloatToString(windSpeed,0)+" kts";
    //vis float
    float vis=XPLMGetDataf(w_visibility);
    if (vis<=5.0f)
        s_vis="Visibility around "+stringOps::ConvertFloatToString(vis,0)+" nm";
    else
        s_vis="Visibility : Clear";
 //clouds
    int cloudType=XPLMGetDatai(w_cloudType0);
    float cloudHeight=XPLMGetDataf(w_CloudBase0);
    if (cloudType==0){
         cloudType=XPLMGetDatai(w_cT1);
         cloudHeight=XPLMGetDataf(w_cB1);
    }
    if (cloudType==0){
        cloudType=XPLMGetDatai(w_cT2);
        cloudHeight=XPLMGetDataf(w_cB2);
    }
        cloudHeight*=3.28084f;
    if (cloudType==0)
        s_clouds="no clouds";
    else
        s_clouds=" ceiling at "+stringOps::ConvertFloatToString(cloudHeight,0) +" MSL";
}

void DRefWindow::GetATISWeatherReport(){
//qnh
    float qnh=XPLMGetDataf(w_qnh);
    float qnhINhPa=qnh*33.8639f;
    s_qnh="qnh = "+stringOps::ConvertFloatToString(qnhINhPa,0)+" hPa "+stringOps::ConvertFloatToString(qnh,2) + (" inHg");
//winds
    float windDir=XPLMGetDataf(w_windDirection),windSpeed=XPLMGetDataf(w_windSpeed);
        s_winds="Winds From"+stringOps::ConvertFloatToString(windDir,0) + " @"+stringOps::ConvertFloatToString(windSpeed,0)+" kts";
//temps
    float temp=XPLMGetDataf(w_temp),dwp=XPLMGetDataf(w_dewP);
    s_temps="Ground temp = "+stringOps::ConvertFloatToString(temp,0)+" °C, dewpoint = "+stringOps::ConvertFloatToString(dwp,0)+" °C at sea level";
//visibility
    float vis=XPLMGetDataf(w_visibility);
    if (vis<=5.0f)
        s_vis="Visibility around "+stringOps::ConvertFloatToString(vis,0)+" nm";
    else
        s_vis="Visibility : Clear";
//clouds
    int cloudType=XPLMGetDatai(w_cloudType0);
    float cloudHeight=XPLMGetDataf(w_CloudBase0);
    if (cloudType==0){
         cloudType=XPLMGetDatai(w_cT1);
         cloudHeight=XPLMGetDataf(w_cB1);
    }
    if (cloudType==0){
        cloudType=XPLMGetDatai(w_cT2);
        cloudHeight=XPLMGetDataf(w_cB2);
    }
    string cType("");
    cloudHeight*=3.28084f;
    switch (cloudType){
       case 0 : cType="none";break;
       case 1 : cType="few";break;
       case 2 : cType="Scattered";break;
       case 3 : cType="Broken";break;
       case 4 : cType="Overcast";break;
       case 5 : cType="Stratus";break;

    }
    if (cloudType==0)
        s_clouds="sky clear";
    else
        s_clouds="clouds "+cType+" ceiling at "+stringOps::ConvertFloatToString(cloudHeight,0) +" ft";
//thunderstorms
    float thunder=XPLMGetDataf(w_thunderS)*100;
    if (thunder<10) s_thunderStorms="low risk of thunderstorms";
    if (thunder<1) s_thunderStorms="no significant risk of thunderstorms";
    if (thunder>=10) s_thunderStorms="estimated risk of thunderstorms at "+stringOps::ConvertFloatToString(thunder,0)="%";
    if (thunder>30) s_thunderStorms="heavy risk of thunderstroms estimated at "+stringOps::ConvertFloatToString(thunder,0)="%";

}

//Callbacks

float DRefWindow::UpdateValue(float,float,int,void*){
    switch (whatToShow){
       case 0 : myself->GetFPS();break;
       case 1 : myself->GetIAS();break;
       case 2 : myself->GetTAS();break;
       case 3 : myself->GetGS();break;
       case 4 : myself->GetGForceDown();break;
       case 5 : myself->GetAOA();break;
       case 6 : myself->GetGForceCalculated();break;
       case 7 : myself->GetWeatherReport();break;
    }
    return period;
}

int					DRefWindow::dummy_mouse_handler(XPLMWindowID, int, int, int, void * ) { return 0; }
XPLMCursorStatus	DRefWindow::dummy_cursor_status_handler(XPLMWindowID, int, int, void *) { return xplm_CursorDefault; }
int					DRefWindow::dummy_wheel_handler(XPLMWindowID, int, int, int, int, void * ) { return 0; }
void                DRefWindow::dummy_key_handler(XPLMWindowID ,char,XPLMKeyFlags,char,void* ,int){}
