#include "drefwindow.h"
#include "drawlogic.h"
#include "vrcommandfilter.h"

XPLMWindowID DRefWindow::idDrefWindow;

bool  DRefWindow::isInVRMode=true;
bool  DRefWindow::isShowModeOnPress=true;
float DRefWindow::cyan[]={0,1.0,1.0};
float DRefWindow::gray[]={0.375f,0.375f,0.368f};
vector<float> DRefWindow::movingAverage(10,0);
int   DRefWindow::counter(0);
int   DRefWindow::whatToShow(0);
float DRefWindow::currentDRefValue(0);
float DRefWindow::currentmeasure(0);
float DRefWindow::period(0);

string DRefWindow::valueToShow("");
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


XPLMCommandRef DRefWindow::CommandFPS(nullptr);
XPLMCommandRef DRefWindow::CommandIAS(nullptr);
XPLMCommandRef DRefWindow::CommandTAS(nullptr);
XPLMCommandRef DRefWindow::CommandGS(nullptr);
XPLMCommandRef DRefWindow::CommandGFD(nullptr);
XPLMCommandRef DRefWindow::CommandAOA(nullptr);
XPLMCommandRef DRefWindow::CommandGFA(nullptr);



DRefWindow::DRefWindow()

{

}

DRefWindow::~DRefWindow(){
myself=nullptr;
}

void DRefWindow::WriteDebug(string in_String){
    in_String="VR Tools : " +in_String+"\r\n";
    XPLMDebugString((char*)in_String.c_str());
}

void DRefWindow::Setup(){

    g_vr_dref    = XPLMFindDataRef("sim/graphics/VR/enabled");
    g_FPS        = XPLMFindDataRef("sim/time/framerate_period");
    if (g_FPS==nullptr){
        g_FPS    = XPLMFindDataRef("sim/operation/misc/frame_rate_period");
        WriteDebug("will use misc/frame_rate_period for FPS");}
    g_IAS        = XPLMFindDataRef("sim/flightmodel/position/indicated_airspeed");
    g_TAS        = XPLMFindDataRef("sim/flightmodel/position/true_airspeed");
    g_GS         = XPLMFindDataRef("sim/flightmodel/position/groundspeed");
    g_GFcD       = XPLMFindDataRef("sim/flightmodel/forces/g_nrml");
    g_AOA        = XPLMFindDataRef("sim/flightmodel/position/alpha");
    g_GFcAcf     = XPLMFindDataRef("sim/flightmodel/forces/g_axil");
    g_accX       = XPLMFindDataRef("sim/flightmodel/position/local_ax");
    g_accY       = XPLMFindDataRef("sim/flightmodel/position/local_ay");
    g_accZ       = XPLMFindDataRef("sim/flightmodel/position/local_az");
    mousejoy     = XPLMFindDataRef("sim/operation/override/override_joystick");



    CommandFPS    = XPLMCreateCommand("VR_Tools/Custom/Show_Data/ShowFPS", "Show FPS");
    CommandIAS    = XPLMCreateCommand("VR_Tools/Custom/Show_Data/ShowIAS", "Show IAS");
    CommandTAS    = XPLMCreateCommand("VR_Tools/Custom/Show_Data/ShowTAS", "Show TAS");
    CommandGS     = XPLMCreateCommand("VR_Tools/Custom/Show_Data/ShowGS", "Show GS");
    CommandGFD    = XPLMCreateCommand("VR_Tools/Custom/Show_Data/ShowGFDown", "Show G-Force Down");
    CommandAOA    = XPLMCreateCommand("VR_Tools/Custom/Show_Data/ShowAOA", "Show Angle of Attack");
    CommandGFA    = XPLMCreateCommand("VR_Tools/Custom/Show_Data/ShowGFComp", "Show G-Force Computed");


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

    myself=this;

    rectangles rect(true,true);
    DrawLogic::AddModalRect(rect,Clr_Black);
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

void DRefWindow::drawDRef(XPLMWindowID in_window_id, void * in_refcon) // draw FPS Window's content
{
    XPLMSetGraphicsState(
            0 /* no fog */,
            0 /* 0 texture units */,
            0 /* no lighting */,
            0 /* no alpha testing */,
            1 /* do alpha blend */,
            1 /* do depth testing */,
            0 /* no depth writing */
    );

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


int DRefWindow::MyDRefCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon){
    switch (inPhase)
    {
    case xplm_CommandBegin :
            {
            //int old=whatToShow;
            whatToShow=*(static_cast<int*>(inRefcon));
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



void DRefWindow::SetupWindow(){

    if (idDrefWindow==nullptr){//I prefer to recreate the window to keep a clean cockpit
        const int vr_is_enabled = XPLMGetDatai(g_vr_dref);
        isInVRMode = vr_is_enabled;
        int global_desktop_bounds[4];
        XPLMGetScreenBoundsGlobal(&global_desktop_bounds[0], &global_desktop_bounds[3], &global_desktop_bounds[2], &global_desktop_bounds[1]);

        XPLMCreateWindow_t params;
         params.structSize = sizeof(params);
         params.visible = 1;
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
         params.bottom = global_desktop_bounds[1] + 50;
         params.right  = global_desktop_bounds[0] + 160;
         params.top    = global_desktop_bounds[1] + 100;

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
  //          case 7 : period=0.5f;break;
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

//Callbacks

float DRefWindow::UpdateValue(float elpSc,float elpTime,int countr,void* refcon){
    switch (whatToShow){
       case 0 : myself->GetFPS();break;
       case 1 : myself->GetIAS();break;
       case 2 : myself->GetTAS();break;
       case 3 : myself->GetGS();break;
       case 4 : myself->GetGForceDown();break;
       case 5 : myself->GetAOA();break;
       case 6 : myself->GetGForceCalculated();break;
 //      case 7 : myself->GetLatched();break;
    }
    return period;
}

int					DRefWindow::dummy_mouse_handler(XPLMWindowID in_window_id, int x, int y, int is_down, void * in_refcon) { return 0; }
XPLMCursorStatus	DRefWindow::dummy_cursor_status_handler(XPLMWindowID in_window_id, int x, int y, void * in_refcon) { return xplm_CursorDefault; }
int					DRefWindow::dummy_wheel_handler(XPLMWindowID in_window_id, int x, int y, int wheel, int clicks, void * in_refcon) { return 0; }
void                DRefWindow::dummy_key_handler(XPLMWindowID in_window_id,char in_key,XPLMKeyFlags in_flag,char in_VK,void* refcon,int is_losing_focus){}
