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
float DRefWindow::targetX(0);
float DRefWindow::targetY(0);
float DRefWindow::targetZ(0);
float DRefWindow::targetPsi(0);
int   DRefWindow::signX(0);
int   DRefWindow::signY(0);
int   DRefWindow::signZ(0);
int   DRefWindow::signRot(0);
int   DRefWindow::phaseMove(0);
string DRefWindow::valueToShow("");
DRefWindow * DRefWindow::myself(nullptr);
bool DRefWindow::isLatched(false);
bool DRefWindow::dialog_ongoing(false);
bool DRefWindow::filterblock(false);

LineDialog *DRefWindow::dlg(nullptr);
VrViews    *DRefWindow::vrconfigEditor(nullptr);

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
XPLMDataRef DRefWindow::pilotX(nullptr);
XPLMDataRef DRefWindow::pilotY(nullptr);
XPLMDataRef DRefWindow::pilotZ(nullptr);
XPLMDataRef DRefWindow::pilotPsi(nullptr);
XPLMDataRef DRefWindow::outside(nullptr);

XPLMCommandRef DRefWindow::CommandFPS(nullptr);
XPLMCommandRef DRefWindow::CommandIAS(nullptr);
XPLMCommandRef DRefWindow::CommandTAS(nullptr);
XPLMCommandRef DRefWindow::CommandGS(nullptr);
XPLMCommandRef DRefWindow::CommandGFD(nullptr);
XPLMCommandRef DRefWindow::CommandAOA(nullptr);
XPLMCommandRef DRefWindow::CommandGFA(nullptr);
XPLMCommandRef DRefWindow::CmdJumpNext(nullptr);
XPLMCommandRef DRefWindow::CmdJumpBack(nullptr);
XPLMCommandRef DRefWindow::CmdRight(nullptr);
XPLMCommandRef DRefWindow::CmdLeft(nullptr);
XPLMCommandRef DRefWindow::CmdUp(nullptr);
XPLMCommandRef DRefWindow::CmdDown(nullptr);
XPLMCommandRef DRefWindow::CmdForward(nullptr);
XPLMCommandRef DRefWindow::CmdBackward(nullptr);
XPLMCommandRef DRefWindow::CmdRightF(nullptr);
XPLMCommandRef DRefWindow::CmdLeftF(nullptr);
XPLMCommandRef DRefWindow::CmdUpF(nullptr);
XPLMCommandRef DRefWindow::CmdDownF(nullptr);
XPLMCommandRef DRefWindow::CmdForwardF(nullptr);
XPLMCommandRef DRefWindow::CmdBackwardF(nullptr);
XPLMCommandRef DRefWindow::CmdX(nullptr);
XPLMCommandRef DRefWindow::CmdY(nullptr);
XPLMCommandRef DRefWindow::CmdZ(nullptr);
XPLMCommandRef DRefWindow::CmdTurn(nullptr);
XPLMCommandRef DRefWindow::CmdTurnLeft(nullptr);
XPLMCommandRef DRefWindow::CmdTurnRight(nullptr);
XPLMCommandRef DRefWindow::ReloadAC(nullptr);
XPLMCommandRef DRefWindow::CmdCreateHotspot(nullptr);
XPLMCommandRef DRefWindow::CmdEditHotspot(nullptr);
XPLMCommandRef DRefWindow::VRReset(nullptr);

XPLMCreateFlightLoop_t DRefWindow::moveLoop;
XPLMFlightLoopID DRefWindow::moveLoopId(nullptr);

bool DRefWindow::fastX(false);
bool DRefWindow::fastY(false);
bool DRefWindow::fastZ(false);
bool DRefWindow::doneX(false);
bool DRefWindow::doneY(false);
bool DRefWindow::doneZ(false);

DRefWindow::DRefWindow()

{

}

DRefWindow::~DRefWindow(){
    myself=nullptr;
    if (dlg!=nullptr) delete dlg;
    if (vrconfigEditor!=nullptr) delete vrconfigEditor;
    dlg=nullptr;
    vrconfigEditor=nullptr;
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
    pilotX       = XPLMFindDataRef("sim/graphics/view/pilots_head_x");
    pilotY       = XPLMFindDataRef("sim/graphics/view/pilots_head_y");
    pilotZ       = XPLMFindDataRef("sim/graphics/view/pilots_head_z");
    pilotPsi     = XPLMFindDataRef("sim/graphics/view/pilots_head_psi");
    outside      = XPLMFindDataRef("sim/graphics/VR/teleport_on_ground");


    CommandFPS    = XPLMCreateCommand("VR/Custom/Show_Data/ShowFPS", "Show FPS");
    CommandIAS    = XPLMCreateCommand("VR/Custom/Show_Data/ShowIAS", "Show IAS");
    CommandTAS    = XPLMCreateCommand("VR/Custom/Show_Data/ShowTAS", "Show TAS");
    CommandGS     = XPLMCreateCommand("VR/Custom/Show_Data/ShowGS", "Show GS");
    CommandGFD    = XPLMCreateCommand("VR/Custom/Show_Data/ShowGFDown", "Show G-Force Down");
    CommandAOA    = XPLMCreateCommand("VR/Custom/Show_Data/ShowAOA", "Show Angle of Attack");
    CommandGFA    = XPLMCreateCommand("VR/Custom/Show_Data/ShowGFComp", "Show G-Force Computed");
    CmdJumpNext   = XPLMCreateCommand("VR/Custom/Hotspots/Next_Hotspot","Move To Next Hotspot");
    CmdJumpBack   = XPLMCreateCommand("VR/Custom/Hotspots/Previous_Hotspot","Move To Previous Hotspot");
 CmdCreateHotspot = XPLMCreateCommand("VR/Custom/Hotspots/Create_Hotspot","Create Hotspot");
   CmdEditHotspot = XPLMCreateCommand("VR/Custom/Hotspots/Edit_Hotspots","Edit Hotspots");

    CmdRight      = XPLMFindCommand("sim/general/right");
    CmdLeft       = XPLMFindCommand("sim/general/left");
    CmdUp         = XPLMFindCommand("sim/general/up");
    CmdDown       = XPLMFindCommand("sim/general/down");
    CmdForward    = XPLMFindCommand("sim/general/forward");
    CmdBackward   = XPLMFindCommand("sim/general/backward");
    CmdRightF     = XPLMFindCommand("sim/general/right_fast");
    CmdLeftF      = XPLMFindCommand("sim/general/left_fast");
    CmdUpF        = XPLMFindCommand("sim/general/up_fast");
    CmdDownF      = XPLMFindCommand("sim/general/down_fast");
    CmdForwardF   = XPLMFindCommand("sim/general/forward_fast");
    CmdBackwardF  = XPLMFindCommand("sim/general/backward_fast");
    CmdTurnLeft   = XPLMFindCommand("sim/general/rot_left");
    CmdTurnRight  = XPLMFindCommand("sim/general/rot_right");
    ReloadAC      = XPLMFindCommand("sim/operation/reload_aircraft");
    VRReset       = XPLMFindCommand("sim/VR/general/reset_view");


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

    XPLMRegisterCommandHandler(CmdJumpNext,MyJumpCommandHandler,1,nb);
    XPLMRegisterCommandHandler(CmdJumpBack,MyJumpCommandHandler,1,nb);

    XPLMRegisterCommandHandler(CmdCreateHotspot,Create_Hotspot_Handler,1,nb);
    XPLMRegisterCommandHandler(CmdEditHotspot,Edit_Hotspot_Handler,1,nb);
    XPLMRegisterCommandHandler(VRReset,MyJumpCommandHandler,1,nb);

    myself=this;

    moveLoop.callbackFunc=MoveMeToHotSpot;
    moveLoop.structSize=sizeof (moveLoop);
    moveLoop.refcon=nullptr;
    moveLoopId=XPLMCreateFlightLoop(&moveLoop);
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

void  DRefWindow::LogPilotHead(float &phx,float &phy,float &phz){
    phx=XPLMGetDataf(pilotX);
    phy=XPLMGetDataf(pilotY);
    phz=XPLMGetDataf(pilotZ);
}

int DRefWindow::MyJumpCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon){

    if (inPhase==xplm_CommandBegin){
        if (inCommand==VRReset){
            doneX=true;doneY=true;doneZ=true;
            XPLMCommandEnd(CmdX);
            XPLMCommandEnd(CmdY);
            XPLMCommandEnd(CmdZ);
            if (phaseMove!=0) phaseMove=4;
            return 1;
        }else{
        if (VRCReader::HasHotspots()&&XPLMGetDatai(g_vr_dref))
            {
                if (VRCReader::GetHotspotCount()>=1&&phaseMove==0&&!DrawLogic::IsModalWindowActive())
                    {  if (inCommand==CmdJumpNext)
                            VRCReader::NextHotspot();
                       else {
                            VRCReader::PreviousHotspot();
                       }
                       PrepareToMove();
                    }
                }
        }
    }

    if (inPhase==xplm_CommandContinue){
        //if ((targetPsi-psi)*signRot<=0) XPLMCommandEnd(CmdTurn);
    }

    if (inPhase==xplm_CommandEnd)
    {}

    return 0;
}

void DRefWindow::PrepareToMove(){

    float tilt(0),pitch(0);
    VRCReader::GetCurrentHotspotCoords(targetX,targetY,targetZ,targetPsi,pitch,tilt);
    if (!VRCReader::IsCurrentHotspotSitting()) targetY=targetY+1.70f;
    string hsName=VRCReader::GetCurrentHotspotName();
    temporaryWindow::ShowAlert("Moving to "+hsName,60);
    XPLMCommandEnd(CmdX);
    XPLMCommandEnd(CmdY);
    XPLMCommandEnd(CmdZ);

    float posX(0),posY(0),posZ(0),dX(0),dY(0),dZ(0);
    myself->LogPilotHead(posX,posY,posZ);
    //psi =XPLMGetDataf(pilotPsi);
    int speedopt=IniSettings::GetSpeedMove();
    bool slow=(speedopt==0);
    dX=0-posX;
    dY=targetY-posY;
    dZ=targetZ-posZ;
    fastX=(speedopt==1);
    fastY=fastX;
    fastZ=fastX;
    doneX=false;
    doneY=false;
    doneZ=false;
    phaseMove=1;

    if (dX>0) {
        signX=1;
        if (dX>0.1f) {CmdX=(slow?CmdRight:CmdRightF);}
        else doneX=true;
    }else{
        signX=-1;
        if (dX<-0.1f) {CmdX=(slow?CmdLeft:CmdLeftF);}
        else doneX=true;
    }

    if (dY>0) {
        signY=1;
        if (dY>0.25f) {CmdY=(slow?CmdUp:CmdUpF);} else CmdY=CmdUp;
    }else{
        signY=-1;
        if (dY<-0.025f) {CmdY=(slow?CmdDown:CmdDownF);} else CmdY=CmdDown;
    }

    if (dZ>0) {
        signZ=1;
        if (dZ>0.25f) {CmdZ=(slow?CmdBackward:CmdBackwardF);} else CmdZ=CmdBackward;
    }else{
        signZ=-1;
        if (dZ<-0.25f) {CmdZ=(slow?CmdForward:CmdForwardF);} else CmdZ=CmdForward;
    }

    /*      if (targetPsi>psi) {
signRot=1;
if ((targetPsi-psi)<=180.0f) CmdTurn=CmdTurnRight;
else CmdTurn=CmdTurnLeft;
}else {
signRot=-1;
if ((psi-targetPsi)<=180.0f) CmdTurn=CmdTurnLeft;
else CmdTurn=CmdTurnRight;
}*/
    XPLMScheduleFlightLoop(moveLoopId,-1,1);
    filterblock=VRCommandFilter::commandBlock;
    VRCommandFilter::commandBlock=false;

    if (dX*signX>+0.001f) XPLMCommandBegin(CmdX); else doneX=true;
}

float DRefWindow::MoveMeToHotSpot(float elpSc,float elpTime,int countr,void* refcon){
    float posX(0),posY(0),posZ(0),dX(0),dY(0),dZ(0);
     myself->LogPilotHead(posX,posY,posZ);
     //psi= XPLMGetDataf(pilotPsi);
     if (phaseMove==1){
         dX=(0-posX)*signX;
         /*if (fastX){
             if (dX<=0.25f) {
                 XPLMCommandEnd(CmdX);
                 fastX=false;
                 if (signX==1){ CmdX=CmdRight;XPLMCommandBegin(CmdX);}
                 else {CmdX=CmdLeft;XPLMCommandBegin(CmdX);}
             }
         }else*/
         {if (dX<=0||doneX) {
                 XPLMCommandEnd(CmdX);
                 doneX=false;
                 dY=(targetY-posY)*signY;
                 dZ=(targetZ-posZ)*signZ;
                 if (dY>+0.001f) XPLMCommandBegin(CmdY); else doneY=true;
                 if (dZ>+0.001f) XPLMCommandBegin(CmdZ); else doneZ=true;
                 phaseMove=2;
             }
         }
     }
         if (phaseMove==2){
             dY=(targetY-posY)*signY;
             dZ=(targetZ-posZ)*signZ;
             if (fastY){
                 if (dY<=0.25f) {
                     XPLMCommandEnd(CmdY);
                     fastY=false;
                     if (signY==1){ CmdY=CmdUp;XPLMCommandBegin(CmdY);}
                     else {CmdY=CmdDown;XPLMCommandBegin(CmdY);}
                 }
             }else
                 if (dY<=0) {XPLMCommandEnd(CmdY);doneY=true;}

             if (fastZ){
                 if (dZ<=0.25f) {
                     XPLMCommandEnd(CmdZ);
                     fastZ=false;
                     if (signZ==1){ CmdZ=CmdBackward;XPLMCommandBegin(CmdZ);}
                     else {CmdZ=CmdForward;XPLMCommandBegin(CmdZ);}
                 }
             }else
                 if (dZ<=0) {XPLMCommandEnd(CmdZ);doneZ=true;}
             if (doneY&doneZ){
                 phaseMove=3;
                 dX=targetX;
                 if (dX>0) {
                    signX=1;
                     CmdX=CmdRight;
                     if (dX>=0.25f&&fastX) {CmdX=CmdRightF;}
                 }else{
                     signX=-1;
                     CmdX=CmdLeft;
                     if (dX<=-0.25f&&fastX) {CmdX=CmdLeftF;}
                 }
                 if (dX*signX>0.001f) XPLMCommandBegin(CmdX); else doneX=true;
             }
         }
         if (phaseMove==3){
             dX=(targetX-posX)*signX;
             if (fastX){
                 if (dX<=0.25f) {
                     XPLMCommandEnd(CmdX);
                     fastX=false;
                     if (signX==1){ CmdX=CmdRight;XPLMCommandBegin(CmdX);}
                     else {CmdX=CmdLeft;XPLMCommandBegin(CmdX);}
                 }
             }else
             {if (dX<=0.001f) {
                     XPLMCommandEnd(CmdX);
                     doneX=true;
                 }
             }
         }

     if (doneX&doneY&doneZ) {
         temporaryWindow::StopAlert();
         VRCommandFilter::commandBlock=filterblock;
         phaseMove=0;
         return 0;}
     return -1;
}

void DRefWindow::ReloadCurrentAircraft(){
        DrawLogic::WriteDebug("going to reload aircraft");
        XPLMCommandOnce(ReloadAC);
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

void DRefWindow::GetLatched(){
    int asBool;
    asBool=XPLMGetDatai(mousejoy);
    isLatched=(asBool>0);
    valueToShow=(isLatched?"is Latched":"is free");
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



int DRefWindow::Edit_Hotspot_Handler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void * inRefcon){
    if (inPhase==xplm_CommandBegin&&XPLMGetDatai(g_vr_dref))
    {if (!DrawLogic::IsModalWindowActive()&&phaseMove==0){
            myself->LogPilotHead(targetX,targetY,targetZ);
            if (myself->vrconfigEditor!=nullptr)
                delete myself->vrconfigEditor;
            myself->vrconfigEditor=new VrViews();
            string toPass="View logged at X="
                          +VRCReader::ConvertFloatToString(targetX)
                    +" Y="+VRCReader::ConvertFloatToString(targetY)
                    +" Z="+VRCReader::ConvertFloatToString(targetZ)
                    +" PSI="+VRCReader::ConvertFloatToString(targetPsi);
            myself->vrconfigEditor->MakeDialog("Commit","Goto","Cancel",toPass,"",Handle_End_Of_Edit,450);
        }
    }
    return 0;
}

void DRefWindow::Handle_End_Of_Edit(){
    int resp=myself->vrconfigEditor->GetAnswer();
    if (resp==0){
        if (myself->vrconfigEditor->GetActionLaunched()=="Relog"){
            myself->LogPilotHead(targetX,targetY,targetZ);
            myself->vrconfigEditor->Relog(targetX,targetY,targetZ);

            return;
        }
        if (myself->vrconfigEditor->GetActionLaunched()=="Up"){
            int idx=myself->vrconfigEditor->GetSelectedHotspot();
            VRCReader::SwapHotspots(idx,idx-1);
            return;
        }
        if (myself->vrconfigEditor->GetActionLaunched()=="Down"){
            int idx=myself->vrconfigEditor->GetSelectedHotspot();
            VRCReader::SwapHotspots(idx,idx+1);
            return;
        }

        if (myself->vrconfigEditor->GetActionLaunched()=="Rename"){
            int idx=myself->vrconfigEditor->GetSelectedHotspot();
            string newName=myself->vrconfigEditor->GetUserLine();
            VRCReader::SetHotspotName(idx,newName);
            return;
        }

        if (myself->vrconfigEditor->GetActionLaunched()=="Create"){
            if (XPLMGetDatai(outside)) {
                myself->vrconfigEditor->MessageLine3("can't create a hotspot after teleport outside,press cancel");
                myself->vrconfigEditor->DisableEdit();
                return;}
            string newName=myself->vrconfigEditor->GetUserLine();
            VRCReader::CreateHotspot(newName,targetX,targetY,targetZ,targetPsi,0,0);
            return;
        }

        if (myself->vrconfigEditor->GetActionLaunched()=="Reposition"){
            if (XPLMGetDatai(outside)) {
                myself->vrconfigEditor->MessageLine3("can't reposition a hotspot after teleport outside,press cancel");
                myself->vrconfigEditor->DisableEdit();
                return;}
            int idx=myself->vrconfigEditor->GetSelectedHotspot();
            VRCReader::SetHotspotCoords(idx,targetX,targetY,targetZ,0,0,0);
            return;
        }

        if (myself->vrconfigEditor->GetActionLaunched()=="Delete"){
             int idx=myself->vrconfigEditor->GetSelectedHotspot();
             VRCReader::DeleteHotspot(idx);
        }
    }
    if (resp==1){
        VRCReader::SaveVRConfig();
        VRCReader::AnalyzeFile();
        if (IniSettings::GetOptReloadModel()&&
                (myself->vrconfigEditor->GetActionLaunched()=="Create"||
                 myself->vrconfigEditor->GetActionLaunched()=="Reposition")) ReloadCurrentAircraft();
    }
    if (resp==2){
        VRCReader::GotoHotspotNumber(myself->vrconfigEditor->GetSelectedHotspot());
        PrepareToMove();
    }
    if (resp==3){
        VRCReader::AnalyzeFile();
    }
}

int DRefWindow::Create_Hotspot_Handler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void* inRefcon){
    if (inPhase==xplm_CommandBegin&&!DrawLogic::IsModalWindowActive()&&phaseMove==0&&XPLMGetDatai(g_vr_dref)){
        if (dlg==nullptr){dlg=new LineDialog();}
            myself->LogPilotHead(targetX,targetY,targetZ);
            targetPsi=XPLMGetDataf(pilotPsi);
            dlg=new LineDialog();
            if (VRCReader::HasVRConfig())
               dlg->MakeDialog("Create","Hotspots","Cancel","Enter New Hotspot's name","",Handle_End_Of_Create_Command);
            else {
                delete dlg;
                dlg=nullptr;
                VRCReader::CreateHotspot("Pilot View",targetX,targetY,targetZ,targetPsi,0,0);
                VRCReader::SaveVRConfig();
                ReloadCurrentAircraft();
            }
    }
    return 0;
}

void DRefWindow::Handle_End_Of_Create_Command(){
    int ans=dlg->GetAnswer();
    string nnm=dlg->GetUserLine();
    if (ans==1){
        VRCReader::CreateHotspot(nnm,targetX,targetY,targetZ,targetPsi,0,0);
        VRCReader::SaveVRConfig();
        if (IniSettings::GetOptReloadModel()) ReloadCurrentAircraft();
    }
}

int					DRefWindow::dummy_mouse_handler(XPLMWindowID in_window_id, int x, int y, int is_down, void * in_refcon) { return 0; }
XPLMCursorStatus	DRefWindow::dummy_cursor_status_handler(XPLMWindowID in_window_id, int x, int y, void * in_refcon) { return xplm_CursorDefault; }
int					DRefWindow::dummy_wheel_handler(XPLMWindowID in_window_id, int x, int y, int wheel, int clicks, void * in_refcon) { return 0; }
void                DRefWindow::dummy_key_handler(XPLMWindowID in_window_id,char in_key,XPLMKeyFlags in_flag,char in_VK,void* refcon,int is_losing_focus){}
