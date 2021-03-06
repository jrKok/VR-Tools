/*Class to manage dynamically hotspots :
 * move to a hotspot, log head position, initiate edition of hotspot
 * or advanced edition
 */

#include "hotspots.h"
#include "drawlogic.h"
#include "vrcommandfilter.h"
#include "managemodalwindow.h"
#include "opcenter.h"
#include "temporarywindow.h"

float Hotspots::targetX(0);
float Hotspots::targetY(0);
float Hotspots::targetZ(0);
float Hotspots::targetPsi(0);
int   Hotspots::signX(0);
int   Hotspots::signY(0);
int   Hotspots::signZ(0);
int   Hotspots::signRot(0);
int   Hotspots::phaseMove(0);
bool Hotspots::dialog_ongoing(false);
bool Hotspots::filterblock(false);
Hotspots * Hotspots::myself;
LineDialog *Hotspots::dlg(nullptr);
VrViews    *Hotspots::vrconfigEditor(nullptr);
advanced   *Hotspots::advancedEditor(nullptr);

XPLMDataRef    Hotspots::g_vr_dref(nullptr);
XPLMCommandRef Hotspots::CmdRight(nullptr);
XPLMCommandRef Hotspots::CmdLeft(nullptr);
XPLMCommandRef Hotspots::CmdUp(nullptr);
XPLMCommandRef Hotspots::CmdDown(nullptr);
XPLMCommandRef Hotspots::CmdForward(nullptr);
XPLMCommandRef Hotspots::CmdBackward(nullptr);
XPLMCommandRef Hotspots::CmdRightF(nullptr);
XPLMCommandRef Hotspots::CmdLeftF(nullptr);
XPLMCommandRef Hotspots::CmdUpF(nullptr);
XPLMCommandRef Hotspots::CmdDownF(nullptr);
XPLMCommandRef Hotspots::CmdForwardF(nullptr);
XPLMCommandRef Hotspots::CmdBackwardF(nullptr);
XPLMCommandRef Hotspots::CmdX(nullptr);
XPLMCommandRef Hotspots::CmdY(nullptr);
XPLMCommandRef Hotspots::CmdZ(nullptr);
XPLMCommandRef Hotspots::CmdTurn(nullptr);
XPLMCommandRef Hotspots::CmdTurnLeft(nullptr);
XPLMCommandRef Hotspots::CmdTurnRight(nullptr);
XPLMCommandRef Hotspots::ReloadAC(nullptr);
XPLMCommandRef Hotspots::CmdCreateHotspot(nullptr);
XPLMCommandRef Hotspots::CmdEditHotspot(nullptr);
XPLMCommandRef Hotspots::VRReset(nullptr);
XPLMCommandRef Hotspots::CmdJumpNext(nullptr);
XPLMCommandRef Hotspots::CmdJumpBack(nullptr);
XPLMCommandRef Hotspots::MoveToHS1(nullptr);
XPLMCommandRef Hotspots::MoveToHS2(nullptr);
XPLMCommandRef Hotspots::MoveToHS3(nullptr);
XPLMCommandRef Hotspots::MoveToHS4(nullptr);
XPLMCommandRef Hotspots::CmdReceived(nullptr);
XPLMDataRef Hotspots::pilotX(nullptr);
XPLMDataRef Hotspots::pilotY(nullptr);
XPLMDataRef Hotspots::pilotZ(nullptr);
XPLMDataRef Hotspots::pilotPsi(nullptr);
XPLMDataRef Hotspots::outside(nullptr);
bool Hotspots::fastX(false);
bool Hotspots::fastY(false);
bool Hotspots::fastZ(false);
bool Hotspots::doneX(false);
bool Hotspots::doneY(false);
bool Hotspots::doneZ(false);
bool Hotspots::beginMove(false);
OpCenter *Hotspots::myCenter(nullptr);

Hotspots::Hotspots()

{
}

Hotspots::~Hotspots(){
    myself=nullptr;
    if (dlg!=nullptr) delete dlg;
    if (vrconfigEditor!=nullptr) delete vrconfigEditor;
    if (advancedEditor!=nullptr) delete advancedEditor;
    dlg=nullptr;
    vrconfigEditor=nullptr;
    advancedEditor=nullptr;
}

void Hotspots::SetMyCenter(OpCenter *opc){
    myCenter=opc;
}

void Hotspots::Setup(){
    g_vr_dref     = XPLMFindDataRef("sim/graphics/VR/enabled");
    pilotX        = XPLMFindDataRef("sim/graphics/view/pilots_head_x");
    pilotY        = XPLMFindDataRef("sim/graphics/view/pilots_head_y");
    pilotZ        = XPLMFindDataRef("sim/graphics/view/pilots_head_z");
    pilotPsi      = XPLMFindDataRef("sim/graphics/view/pilots_head_psi");
    outside       = XPLMFindDataRef("sim/graphics/VR/teleport_on_ground");
    CmdJumpNext   = XPLMCreateCommand("VR_Tools/Custom/Hotspots/Next_Hotspot","Move To Next Hotspot");
    CmdJumpBack   = XPLMCreateCommand("VR_Tools/Custom/Hotspots/Previous_Hotspot","Move To Previous Hotspot");
 CmdCreateHotspot = XPLMCreateCommand("VR_Tools/Custom/Hotspots/Create_Hotspot","Create Hotspot");
   CmdEditHotspot = XPLMCreateCommand("VR_Tools/Custom/Hotspots/Edit_Hotspots","Edit Hotspots");

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
    (*static_cast<int*>(nb))=0;
    XPLMRegisterCommandHandler(CmdJumpNext,MyJumpCommandHandler,1,nb);
    XPLMRegisterCommandHandler(CmdJumpBack,MyJumpCommandHandler,1,nb);
    XPLMRegisterCommandHandler(CmdCreateHotspot,Create_Hotspot_Handler,1,nb);
    XPLMRegisterCommandHandler(CmdEditHotspot,Edit_Hotspot_Handler,1,nb);
    XPLMRegisterCommandHandler(VRReset,MyJumpCommandHandler,1,nb);
    myself=this;

}

void Hotspots::MakeMove4(){
    MoveToHS1     = XPLMCreateCommand("VR_Tools/Custom/Hotspots/Move_Hotspot1","Move To Hotspot 1");
    MoveToHS2     = XPLMCreateCommand("VR_Tools/Custom/Hotspots/Move_Hotspot2","Move To Hotspot 2");
    MoveToHS3     = XPLMCreateCommand("VR_Tools/Custom/Hotspots/Move_Hotspot3","Move To Hotspot 3");
    MoveToHS4     = XPLMCreateCommand("VR_Tools/Custom/Hotspots/Move_Hotspot4","Move To Hotspot 4");
    void * nb=new(int*);
    nb=new(int*);
    (*static_cast<int*>(nb))=1;
    XPLMRegisterCommandHandler(MoveToHS1,MyJumpCommandHandler,1,nb);
    nb=new(int*);
    (*static_cast<int*>(nb))=2;
    XPLMRegisterCommandHandler(MoveToHS2,MyJumpCommandHandler,1,nb);
    nb=new(int*);
    (*static_cast<int*>(nb))=3;
    XPLMRegisterCommandHandler(MoveToHS3,MyJumpCommandHandler,1,nb);
    nb=new(int*);
    (*static_cast<int*>(nb))=4;
    XPLMRegisterCommandHandler(MoveToHS4,MyJumpCommandHandler,1,nb);
}

void Hotspots::MakeMoveComplete(){
    if (IniSettings::GetOptHSCommands()){
        void * nb=new(int*);
        int nbhs=VRCReader::GetHotspotCount();
        for (int hsc(1);hsc<nbhs;hsc++){
            (*static_cast<int*>(nb))=hsc;
            VRCReader::GotoHotspotNumber(hsc);
            string name=VRCReader::GetCurrentHotspotName();
            string title="Move To "+name;
            string cmdName="VR Tools/Custom/Hotspots/Move_"+name;
            XPLMCommandRef comm= XPLMCreateCommand((char*)cmdName.c_str(),(char*)title.c_str());
            XPLMRegisterCommandHandler(comm,MyJumpCommandHandler,1,nb);
        }
        VRCReader::GotoHotspotNumber(0);
    }
}

void  Hotspots::LogPilotHead(float &phx,float &phy,float &phz){
    phx=XPLMGetDataf(pilotX);
    phy=XPLMGetDataf(pilotY);
    phz=XPLMGetDataf(pilotZ);
}

int Hotspots::MyJumpCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon){

    if (inPhase==xplm_CommandBegin){
        if (!VRCReader::HasHotspots()) HandleErrorHotspotList();
        if (inCommand==VRReset){
            doneX=true;doneY=true;doneZ=true;
            XPLMCommandEnd(CmdX);
            XPLMCommandEnd(CmdY);
            XPLMCommandEnd(CmdZ);          
            if (phaseMove!=0){
                phaseMove=4;
                VRCommandFilter::commandBlock=filterblock;
            }
            return 1;
        }else{

        if (VRCReader::HasHotspots()/*&&XPLMGetDatai(g_vr_dref)*/){
            int whatToDo=*(static_cast<int*>(inRefcon));
            if (whatToDo==0){
                if (VRCReader::GetHotspotCount()>=1&&phaseMove==0&&!myCenter->HasModalWindow())
                {  if (inCommand==CmdJumpNext)
                        VRCReader::NextHotspot();
                    else {
                        VRCReader::PreviousHotspot();
                    }
                    PrepareToMove();
                }
            }else{
              int hsc=VRCReader::GetHotspotCount();
              if (hsc>1){
              if (hsc<whatToDo) whatToDo=hsc;
              VRCReader::GotoHotspotNumber(whatToDo);
              PrepareToMove();
              }
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

void Hotspots::PrepareToMove(){
    if (!XPLMGetDatai(outside)){
        XPLMCommandEnd(CmdX);
        XPLMCommandEnd(CmdY);
        XPLMCommandEnd(CmdZ);

        float tilt(0),pitch(0);
        VRCReader::GetCurrentHotspotCoords(targetX,targetY,targetZ,targetPsi,pitch,tilt);
        if (!VRCReader::IsCurrentHotspotSitting()) targetY=targetY+1.70f;
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

        if (dX*signX>+0.001f) beginMove=true;
        else doneX=true;
        filterblock=VRCommandFilter::commandBlock;
        VRCommandFilter::commandBlock=false;
        OpCenter::SetHotspotCall(true);
    }
}

bool Hotspots::IsMoveOngoing(){
    return (phaseMove>0);
}

float Hotspots::MoveMeToHotSpot(){
    if (beginMove) {
        string hsName=VRCReader::GetCurrentHotspotName();
        temporaryWindow::ShowAlert("Moving to "+hsName,60);
         XPLMCommandBegin(CmdX);
         beginMove=false;
        return 0;
    }
    float posX(0),posY(0),posZ(0),dX(0),dY(0),dZ(0);
     myself->LogPilotHead(posX,posY,posZ);
     //psi= XPLMGetDataf(pilotPsi);
     if (phaseMove==1){
         dX=(0-posX)*signX;
         if (dX<=0||doneX) {
                 XPLMCommandEnd(CmdX);
                 doneX=false;
                 dY=(targetY-posY)*signY;
                 dZ=(targetZ-posZ)*signZ;
                 if (dY>+0.001f) XPLMCommandBegin(CmdY); else doneY=true;
                 if (dZ>+0.001f) XPLMCommandBegin(CmdZ); else doneZ=true;
                 phaseMove=2;
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
         OpCenter::SetHotspotCall(false);}
     return -1;
}

int Hotspots::Edit_Hotspot_Handler(XPLMCommandRef, XPLMCommandPhase in_phase, void * ){

    if (!myCenter->HasModalWindow()&&!myCenter->IsInEditMode()&&phaseMove==0&&in_phase==xplm_CommandBegin){
            if (!VRCReader::HasHotspots()) {
                HandleErrorHotspotList();
            }
            myself->LogPilotHead(targetX,targetY,targetZ);
            if (!VRCReader::HasHotspots()){
                DrawLogic::WriteDebug("edit hotspots : no hotspots exist yet, going to write pilot's hotspot");
                VRCReader::CreateHotspot("Pilot View",targetX,targetY,targetZ,targetPsi,0,0);
                VRCReader::SaveVRConfig();
                temporaryWindow::ShowAlert("Valid vrconfig created with pilot view at current position",2);
                if (IniSettings::GetOptReloadModel()) ReloadCurrentAircraft();
                return 0;
            }
            if (myself->vrconfigEditor!=nullptr)
                delete myself->vrconfigEditor;
            ManageModalWindow::MakeTopWindow();
            myself->vrconfigEditor=new VrViews();
            string toPass="View logged at X="
                          +stringOps::ConvertFloatToString(targetX)
                    +" Y="+stringOps::ConvertFloatToString(targetY)
                    +" Z="+stringOps::ConvertFloatToString(targetZ)
                    +" PSI="+stringOps::ConvertFloatToString(targetPsi);
            myCenter->SetModalWindow(true);
            myself->vrconfigEditor->MakeDialog("Commit","Goto","Cancel",toPass,"",Handle_End_Of_Edit,450);
        }
    return 0;
}

void Hotspots::Handle_End_Of_Edit(){
    int resp=myself->vrconfigEditor->GetAnswer();
    if (resp==0){// not really end of edit but a callback from vr views to handle/synchronize hotspots
        if (myself->vrconfigEditor->GetActionLaunched()==action_Relog){
            myself->LogPilotHead(targetX,targetY,targetZ);
            myself->vrconfigEditor->Relog(targetX,targetY,targetZ);
            return;
        }
        if (myself->vrconfigEditor->GetActionLaunched()==action_Up){
            int idx=myself->vrconfigEditor->GetSelectedHotspot();
            VRCReader::SwapHotspots(idx,idx-1);
            return;
        }
        if (myself->vrconfigEditor->GetActionLaunched()==action_Down){
            int idx=myself->vrconfigEditor->GetSelectedHotspot();
            VRCReader::SwapHotspots(idx,idx+1);
            return;
        }
        if (myself->vrconfigEditor->GetActionLaunched()==action_Rename){
            int idx=myself->vrconfigEditor->GetSelectedHotspot();
            VRCReader::GotoHotspotNumber(idx);
            string newName=myself->vrconfigEditor->GetUserLine();
            VRCReader::SetHotspotName(idx,newName);
            return;
        }
        if (myself->vrconfigEditor->GetActionLaunched()==action_Create){
            if (XPLMGetDatai(outside)) {
                myself->vrconfigEditor->MessageLine3("can't create a hotspot after teleport outside,press cancel");
                myself->vrconfigEditor->DisableEdit();
                return;}
            string newName=myself->vrconfigEditor->GetUserLine();
            VRCReader::CreateHotspot(newName,targetX,targetY,targetZ,targetPsi,0,0);
            return;
        }
        if (myself->vrconfigEditor->GetActionLaunched()==action_Reposition){
            if (XPLMGetDatai(outside)) {
                DrawLogic::WriteDebug("hotspots callback : got to treat reposition");
                myself->vrconfigEditor->MessageLine3("can't reposition a hotspot after teleport outside,press cancel");
                myself->vrconfigEditor->DisableEdit();
                return;}
            int idx=myself->vrconfigEditor->GetSelectedHotspot();
            VRCReader::SetHotspotCoords(idx,targetX,targetY,targetZ,0,0,0);
            VRCReader::GotoHotspotNumber(idx);
            return;
        }
        if (myself->vrconfigEditor->GetActionLaunched()==action_Delete){
             int idx=myself->vrconfigEditor->GetSelectedHotspot();
             VRCReader::DeleteHotspot(idx);
             return;
        }
    }
    if (resp==1){
        VRCReader::SaveVRConfig();
        VRCReader::AnalyzeFile();
        OpCenter::CheckVRMirror();
        if (IniSettings::GetOptReloadModel()) DrawLogic::WriteDebug("Hotspots frame : going to reload model");
        else DrawLogic::WriteDebug("Hotspots frame : don't reload model");
        if (IniSettings::GetOptReloadModel()&&
                (myself->vrconfigEditor->GetActionLaunched()==action_Create||
                 myself->vrconfigEditor->GetActionLaunched()==action_Reposition)) {
            ReloadCurrentAircraft();
        }
    }
    if (resp==2){
        VRCReader::GotoHotspotNumber(myself->vrconfigEditor->GetSelectedHotspot());
        PrepareToMove();
    }
    if (resp==3){
        VRCReader::AnalyzeFile();
    }
    if (resp==4){
        int idx=myself->vrconfigEditor->GetSelectedHotspot();
        VRCReader::GotoHotspotNumber(idx);
        Hotspot hs=VRCReader::GetCurrentHotspot();
        if (advancedEditor!=nullptr) {
            delete advancedEditor;
            advancedEditor=nullptr;
        }
        delete myself->vrconfigEditor;//have to delete before launching new window  to avoid having rects destroyed after creating the new window.
        myself->vrconfigEditor=nullptr;
        ManageModalWindow::MakeTopWindow();
        advancedEditor=new advanced();
        advancedEditor->MakeAdvancedDialog(hs,Handle_Advanced);
    }

    if (myself->vrconfigEditor!=nullptr){
        delete myself->vrconfigEditor;
        myself->vrconfigEditor=nullptr;
    }
    myCenter->SetModalWindow(false);
    if (advancedEditor !=nullptr) myCenter->SetModalWindow(true);
}

void Hotspots::Handle_Advanced(){
    if (advancedEditor->IsToCommit()){
        Hotspot hs=advancedEditor->GetHotspot();
        VRCReader::SetAdvancedOptions(hs);
        VRCReader::SaveVRConfig();
        VRCReader::AnalyzeFile();
        if (IniSettings::GetOptReloadModel()) ReloadCurrentAircraft();
        OpCenter::CheckVRMirror();
    }else {
       VRCReader::AnalyzeFile();
   }
   delete advancedEditor;
    advancedEditor=nullptr;
    myCenter->SetModalWindow(false);
}

int Hotspots::Create_Hotspot_Handler(XPLMCommandRef, XPLMCommandPhase inPhase, void*){
    if (inPhase==xplm_CommandBegin&&!myCenter->HasModalWindow()&&!myCenter->IsInEditMode()&&phaseMove==0&&XPLMGetDatai(g_vr_dref)){
        if (!VRCReader::HasHotspots()) HandleErrorHotspotList();
        if (dlg==nullptr){dlg=new LineDialog();}
            myself->LogPilotHead(targetX,targetY,targetZ);
            targetPsi=XPLMGetDataf(pilotPsi);
            ManageModalWindow::MakeTopWindow();
            dlg=new LineDialog();
            if (VRCReader::HasVRConfig()){
                myCenter->SetModalWindow(true);
                dlg->MakeDialog("Create","Hotspots","Cancel","Enter New Hotspot's name","",Handle_End_Of_Create_Command);
            }else {
                delete dlg;
                dlg=nullptr;
                VRCReader::CreateHotspot("Pilot's Seat",targetX,targetY,targetZ,targetPsi,0,0);
                VRCReader::SaveVRConfig();
                ReloadCurrentAircraft();
            }
    }
    return 0;
}

void Hotspots::Handle_End_Of_Create_Command(){
    int ans=dlg->GetAnswer();
    string nnm=dlg->GetUserLine();
    if (ans==1){
        VRCReader::CreateHotspot(nnm,targetX,targetY,targetZ,targetPsi,0,0);
        VRCReader::SaveVRConfig();
        if (IniSettings::GetOptReloadModel()) ReloadCurrentAircraft();
    }
    if (ans==2){
        if (myself->vrconfigEditor!=nullptr)
            delete myself->vrconfigEditor;
        ManageModalWindow::MakeTopWindow();
        myself->vrconfigEditor=new VrViews();
        string toPass="View logged at X="
                      +stringOps::ConvertFloatToString(targetX)
                +" Y="+stringOps::ConvertFloatToString(targetY)
                +" Z="+stringOps::ConvertFloatToString(targetZ)
                +" PSI="+stringOps::ConvertFloatToString(targetPsi);
        myself->vrconfigEditor->MakeDialog("Commit","Goto","Cancel",toPass,nnm,Handle_End_Of_Edit,450);
        myCenter->SetModalWindow(false);

    }
}

void Hotspots::ReloadCurrentAircraft(){
        DrawLogic::WriteDebug("going to reload aircraft");
        XPLMCommandOnce(ReloadAC);
}

void  Hotspots::HandleErrorHotspotList(){
    int activehs=VRCReader::GetActiveHotspotNumber();
    if (!VRCReader::HasHotspots()&&VRCReader::GetHotspotCount()>0)
        DrawLogic::WriteDebug("Spotted accidental erase of Hotspot Vector");
    VRCReader::AnalyzeFile();
    VRCReader::GotoHotspotNumber(activehs);
}
