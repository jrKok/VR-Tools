#include "opcenter.h"
#include "drawlogic.h"
#include "layoutwithedit.h"

OpCenter *OpCenter::myself(nullptr);

OpCenter::OpCenter():
    wLayout(nullptr),
    wELayout(nullptr),
    dispDir(nullptr),
    manageMW(nullptr),
    dpd{nullptr},
    ptrLayout(nullptr),
    DLoop(),
    DLoopId(nullptr),
    g_textWindow(nullptr),
    g_FileWindow(nullptr),
    is_In_Edit_Mode(false),
    drefW(),
    htsp(),
    commandFilter(),
    menuIdx(0),opt{0},
    idxOfModeMenuItem(0),itemAdjusted(0),itemFast(0),itemSlow(0),itemReload(0),moveNext(0),movePrev(0),
    iFPS(0),iAoA(0),iTAS(0),iIAS(0),iGS(0),iGF(0),iWeather(0),
    menuId(nullptr),menuText(nullptr),menuTextOpt(nullptr),menuHotspots(nullptr),menuData(nullptr),menuShowData(nullptr),
    g_in_vr(false),hasModalWindow(false),
    has_been_setup(false), has_been_launched(false),
    g_vr_dref(nullptr),
    CommandText(nullptr),CmdFirstLine(nullptr),CmdNextLine(nullptr),CmdPrevLine(nullptr),CmdDelLine(nullptr),CmdReload(nullptr),
    ini(),
    tw(),
    colordefs(),
    fontmanager()
{
}

OpCenter::~OpCenter(){
    fontmanager.EndFreeType();
}

int OpCenter::SetupCenter(){
        myself=this;
        DrawLogic::WriteDebug("Go To GetIniParams");
        colordefs.DefineColors();
        IniSettings::GetIniParams();
        g_vr_dref    = XPLMFindDataRef("sim/graphics/VR/enabled");
        FilePointer::Initiate();

        CommandText   = XPLMCreateCommand("VR_Tools/Custom/Toggle_Text_File","Toggle text");
        CmdFirstLine  = XPLMCreateCommand("VR_Tools/Custom/Text/Select_First_Line","Select First Line");
        CmdNextLine   = XPLMCreateCommand("VR_Tools/Custom/Text/Next_Line","Next Line");
        CmdPrevLine   = XPLMCreateCommand("VR_Tools/Custom/Text/Previous_Line","Previous Line");
        CmdDelLine    = XPLMCreateCommand("VR_Tools/Custom/Text/Delete_Line","Hide Line");
        CmdReload     = XPLMCreateCommand("VR_Tools/Custom/Text/Reload","Reload");

        fontmanager.Initialise();

        //Register numbered text commands
        void * nb=new(int*);
        //(*(int*)nb)=0;
        nb=new(int*);*(static_cast<int*>(nb))=1;
        XPLMRegisterCommandHandler(CommandText,MyTextReaderCommandHandler,1,nb);
        nb=new(int*);*(static_cast<int*>(nb))=2;
        XPLMRegisterCommandHandler(CmdFirstLine,MyTextReaderCommandHandler,1,nb);
        nb=new(int*);*(static_cast<int*>(nb))=3;
        XPLMRegisterCommandHandler(CmdNextLine,MyTextReaderCommandHandler,1,nb);
        nb=new(int*);*(static_cast<int*>(nb))=4;
        XPLMRegisterCommandHandler(CmdPrevLine,MyTextReaderCommandHandler,1,nb);
        nb=new(int*);*(static_cast<int*>(nb))=5;
        XPLMRegisterCommandHandler(CmdDelLine,MyTextReaderCommandHandler,1,nb);
        nb=new(int*);*(static_cast<int*>(nb))=6;
        XPLMRegisterCommandHandler(CmdReload,MyTextReaderCommandHandler,1,nb);
        drefW.Setup();
        htsp =std::make_unique<Hotspots>();
        htsp->Setup();
        htsp->SetMyCenter(this);
        MakeMenus();
        commandFilter.init();

        DrawLogic::WriteDebug("VR Tools version 1.3.3 final - Show FPS, speeds, g-forces, filter commands, edit text files");
        IsLaunched=true;
        myself->has_been_setup=true;

    return g_vr_dref != nullptr;
}

void OpCenter::LaunchOperations(){

        DrawLogic::MakeGLContext();
        DrawLogic::WriteDebug("OpCenter Launchoperations : made GLContext");

        dpd[0]=std::make_unique<DrawLogic>();//ndp = New DrawPad
        dpd[0]->Initiate();
        dpd[0]->SetId("Layout");
        wLayout=std::make_unique<Layout>(dpd[0].get());

        dpd[1]=std::make_unique<DrawLogic>();//ndp = New DrawPad
        dpd[1]->Initiate();
        dpd[1]->SetId("LayoutWEdit");
        wELayout=std::make_unique<LayoutWithEdit>(dpd[1].get(),myself);

        dpd[2]=std::make_unique<DrawLogic>();//ndp = New DrawPad
        dpd[2]->Initiate();
        dpd[2]->SetId("Directory");
        dispDir=std::make_unique<ShowDir>(dpd[2].get());

        dpd[3]=std::make_unique<DrawLogic>();//ndp = New DrawPad
        dpd[3]->Initiate();
        dpd[3]->SetId("ModalWindow");
        manageMW=std::make_unique<ManageModalWindow>(dpd[3].get(),myself);

        if (is_In_Edit_Mode){
            LayoutWithEdit *ptLE=wELayout.get();
            ptrLayout=ptLE;
        }else{
            ptrLayout=wLayout.get();
        }

        if (!g_textWindow ){
            wLayout->Begin();
            wELayout->StartEdit();//like Begin(); makes keyboard as well
            if ((*ptrLayout).OpenWindowAtStart()) {
                MakeTextWindow();
            }
        }

        DLoop.callbackFunc=DisplayLoop;
        DLoop.structSize=sizeof (DLoop);
        DLoop.refcon=nullptr;
        DLoopId=XPLMCreateFlightLoop(&DLoop);

        XPLMScheduleFlightLoop(DLoopId,-1,1);
        has_been_launched=true;

}

void OpCenter::HaltOperations(){
    DrawLogic::WriteDebug("going to Halt Operations");

    if (has_been_setup){
        FilePointer::ReleaseBackups();
        drefW.Unload();
        commandFilter.UndoFiltering();
    }
    if (has_been_launched){
        XPLMDestroyFlightLoop(DLoopId);
        has_been_launched=false;
        DLoopId=nullptr;
    }

    htsp-> MakeMoveComplete();
    htsp.reset();

    if (g_FileWindow!=nullptr){
        XPLMDestroyWindow(g_FileWindow);
        g_FileWindow=nullptr;
    }
    if (g_textWindow!=nullptr){
        XPLMDestroyWindow(g_textWindow);
        g_textWindow=nullptr;
    }

    if (dispDir!=nullptr){
        dispDir.reset();
        dispDir=nullptr;
    }

    if (wLayout!=nullptr){
        wLayout.reset();
        wLayout=nullptr;
    }
    if (wELayout!=nullptr){
        wELayout.reset();
        wELayout=nullptr;
    }

    if (manageMW!=nullptr){
        manageMW.reset();
        manageMW=nullptr;
    }

   dpd[0].reset();
   dpd[1].reset();
   dpd[2].reset();
   dpd[3].reset();
   DrawLogic::WriteDebug("all processes halted successfully");
   has_been_launched=false;
}

void OpCenter::SuspendOperations(){
    commandFilter.UndoFiltering();
    if (has_been_launched){
        XPLMDestroyFlightLoop(DLoopId);
        has_been_launched=false;
        DLoopId=nullptr;
    }
    //HaltOperations();
}

int  OpCenter::ResumeOperations(){
    //if (!has_been_setup) SetupCenter();
    if (!has_been_launched) {
        DLoopId=XPLMCreateFlightLoop(&DLoop);
        XPLMScheduleFlightLoop(DLoopId,-1,1);
        has_been_launched=true;
    }
    return 1;
}

float OpCenter::DisplayLoop(float, float, int, void*){
    myself->ptrLayout->Update();
    myself->dispDir->Update();
    myself->manageMW->ConstrainGeometry();
    myself->drefW.UpdateValue();
    return -1;
}

/************************************************************************************/

void  OpCenter::MakeMenus(){
    menuIdx= XPLMAppendMenuItem(XPLMFindPluginsMenu(), "VR Tools", nullptr, 0);
    menuId= XPLMCreateMenu("VR Tools",XPLMFindPluginsMenu(),menuIdx,menuHandler,nullptr);
    int menuTD=XPLMAppendMenuItem(menuId,"Text Files  ",nullptr,0);
    int menuHS=XPLMAppendMenuItem(menuId,"Hotspots    ",nullptr,0);
    int menuDT=XPLMAppendMenuItem(menuId,"Data Display",nullptr,0);
        menuText=XPLMCreateMenu("text F  ",menuId,menuTD,menuHandler,nullptr);
    menuHotspots=XPLMCreateMenu("hotspots",menuId,menuHS,menuHandler,nullptr);
        menuData=XPLMCreateMenu("dataM   ",menuId,menuDT,menuHandler,nullptr);
    //Menu text options
       XPLMAppendMenuItemWithCommand(menuText,"Toggle Text Window",CommandText);
           opt[0]=XPLMAppendMenuItem(menuText,"Options",nullptr,0);
       menuTextOpt=XPLMCreateMenu("tOpt",menuText,opt[0],menuHandler,nullptr);
          void * nb4=new(int*);*(static_cast<int*>(nb4))=4;
          opt[1]=XPLMAppendMenuItem(menuTextOpt,"Show FPS in Title Bar",nb4,0);
          XPLMAppendMenuSeparator(menuTextOpt);
          void * nb1=new(int*);*(static_cast<int*>(nb1))=1;
          opt[2]=XPLMAppendMenuItem(menuTextOpt,"Fit Window Size to File",nb1,0);
          void * nb2=new(int*);*(static_cast<int*>(nb2))=2;
          opt[3]=XPLMAppendMenuItem(menuTextOpt,"Keep File",nb2,0);
          void * nb3=new(int*);*(static_cast<int*>(nb3))=3;
          opt[4]=XPLMAppendMenuItem(menuTextOpt,"Keep Size",nb3,0);

    //Menu for Hotspots
       XPLMAppendMenuItemWithCommand(menuHotspots,"Hotspot Editor",Hotspots::CmdEditHotspot);
       XPLMAppendMenuSeparator(menuHotspots);
       void * nb6=new(int*);*(static_cast<int*>(nb6))=6;
       itemReload=XPLMAppendMenuItem(menuHotspots,"Reload Model if Hotspot modified",nb6,0);
       XPLMAppendMenuSeparator(menuHotspots);
       moveNext=XPLMAppendMenuItemWithCommand(menuHotspots,"Move To Next",Hotspots::CmdJumpNext);
       movePrev=XPLMAppendMenuItemWithCommand(menuHotspots,"Move To Previous",Hotspots::CmdJumpBack);
       XPLMAppendMenuSeparator(menuHotspots);
       int opt=IniSettings::GetSpeedMove();
       void * nb8=new(int*);*(static_cast<int*>(nb8))=8;
       itemAdjusted=XPLMAppendMenuItem(menuHotspots,"Adjusted Move",nb8,0);
       void * nb9=new(int*);*(static_cast<int*>(nb9))=9;
       itemSlow=XPLMAppendMenuItem(menuHotspots,"Slow Move",nb9,0);
       void * nb10=new(int*);*(static_cast<int*>(nb10))=10;
       itemFast=XPLMAppendMenuItem(menuHotspots,"Fast Move",nb10,0);

    //Options for dataref

        void * nb5=new(int*);*(static_cast<int*>(nb5))=5;
        idxOfModeMenuItem=XPLMAppendMenuItem(menuData,"show data permanently",nb5,0);
        int menuSD=XPLMAppendMenuItem(menuData,"Show",nullptr,0);
        menuShowData=XPLMCreateMenu("Show Data",menuData,menuSD,menuHandler,nullptr);
           void *nnb;
           nnb=new(int*);*(static_cast<int*>(nnb))=11;
           iFPS=XPLMAppendMenuItem(menuShowData,"FPS",nnb,0);
           nnb=new(int*);*(static_cast<int*>(nnb))=12;
           iIAS=XPLMAppendMenuItem(menuShowData,"IAS",nnb,0);
           nnb=new(int*);*(static_cast<int*>(nnb))=13;
           iTAS=XPLMAppendMenuItem(menuShowData,"TAS",nnb,0);
           nnb=new(int*);*(static_cast<int*>(nnb))=14;
           iGS=XPLMAppendMenuItem(menuShowData,"Ground Speed",nnb,0);
           nnb=new(int*);*(static_cast<int*>(nnb))=15;
           iAoA=XPLMAppendMenuItem(menuShowData,"AoA",nnb,0);
           nnb=new(int*);*(static_cast<int*>(nnb))=16;
           iGF=XPLMAppendMenuItem(menuShowData,"gForces",nnb,0);
           nnb=new(int*);*(static_cast<int*>(nnb))=17;
           iWeather=XPLMAppendMenuItem(menuShowData,"weather",nnb,0);


    SetEnableTextOptions();
    if (opt==1) XPLMCheckMenuItem(menuHotspots,itemAdjusted,xplm_Menu_Checked);
    if (opt==0) XPLMCheckMenuItem(menuHotspots,itemSlow,xplm_Menu_Checked);
    if (opt==2) XPLMCheckMenuItem(menuHotspots,itemFast,xplm_Menu_Checked);
    if (IniSettings::GetOptReloadModel()) XPLMCheckMenuItem(menuHotspots,itemReload,xplm_Menu_Checked);


}
void  OpCenter::menuHandler(void*, void* inItemRef){
    int menuItem=*(static_cast<int*>(inItemRef));
    switch (menuItem){
        case 1:
            if (myself->ptrLayout!=nullptr)
                if (myself->ptrLayout->HasActiveWindow())
                    myself->ptrLayout->FitToFile();
            break;
        case 2:
            if (myself->ptrLayout!=nullptr)
                if (myself->ptrLayout->HasActiveWindow())
                    myself->ptrLayout->KeepFile();
            break;
        case 3:
            if (myself->ptrLayout!=nullptr)
                if (myself->ptrLayout->HasActiveWindow())
                    myself->ptrLayout->KeepCurrentSize();
            break;

        case 4:{
            if (myself->ptrLayout!=nullptr) {
               if (myself->ptrLayout->HasActiveWindow())
                   {myself->ptrLayout->ToggleFPS();}

            }
            break;}
        case 5:
            myself->drefW.DisposeWindow();
            myself->drefW.ToggleShowMode();
            XPLMCheckMenuItem(myself->menuData,myself->idxOfModeMenuItem,(myself->drefW.GetShowModeOnPress()?xplm_Menu_Unchecked:xplm_Menu_Checked));
            break;
        case 6:
            IniSettings::SetOptReloadModel(!IniSettings::GetOptReloadModel());
            if (IniSettings::GetOptReloadModel())
                XPLMCheckMenuItem(myself->menuHotspots,myself->itemReload,xplm_Menu_Checked);
            else XPLMCheckMenuItem(myself->menuHotspots,myself->itemReload,xplm_Menu_Unchecked);
            IniSettings::WriteIniFile();
            break;

        case 8:
            XPLMCheckMenuItem(myself->menuHotspots,myself->itemAdjusted,xplm_Menu_Checked);
            XPLMCheckMenuItem(myself->menuHotspots,myself->itemSlow,xplm_Menu_Unchecked);
            XPLMCheckMenuItem(myself->menuHotspots,myself->itemFast,xplm_Menu_Unchecked);
            IniSettings::SetSpeedMove(1);
            IniSettings::WriteIniFile();
            break;
        case 9:
            XPLMCheckMenuItem(myself->menuHotspots,myself->itemAdjusted,xplm_Menu_Unchecked);
            XPLMCheckMenuItem(myself->menuHotspots,myself->itemSlow,xplm_Menu_Checked);
            XPLMCheckMenuItem(myself->menuHotspots,myself->itemFast,xplm_Menu_Unchecked);
            IniSettings::SetSpeedMove(0);
            IniSettings::WriteIniFile();
            break;
        case 10:
            XPLMCheckMenuItem(myself->menuHotspots,myself->itemAdjusted,xplm_Menu_Unchecked);
            XPLMCheckMenuItem(myself->menuHotspots,myself->itemSlow,xplm_Menu_Unchecked);
            XPLMCheckMenuItem(myself->menuHotspots,myself->itemFast,xplm_Menu_Checked);
            IniSettings::SetSpeedMove(2);
            IniSettings::WriteIniFile();
            break;
        case 11:
            myself->TriggerDRefCommand(myself->drefW.GetFPSCommand());
            break;
        case 12:
        myself->TriggerDRefCommand(myself->drefW.GetIASCommand());
            break;
        case 13:
        myself->TriggerDRefCommand(myself->drefW.GetTASCommand());
            break;
        case 14:
        myself->TriggerDRefCommand(myself->drefW.GetGSCommand());
            break;
        case 15:
        myself->TriggerDRefCommand(myself->drefW.GetAoACommand());
            break;
        case 16:
        myself->TriggerDRefCommand(myself->drefW.GetgForceCommand());
            break;
        case 17:
        myself->TriggerDRefCommand(myself->drefW.GetWeatherCommand());
            break;
        }
}
void  OpCenter::TriggerDRefCommand(XPLMCommandRef in_command){
    if (drefW.GetShowModeOnPress()) drefW.ToggleShowMode();
    XPLMCheckMenuItem(menuData,idxOfModeMenuItem,drefW.GetShowModeOnPress()?xplm_Menu_Unchecked:xplm_Menu_Checked);
    XPLMCommandOnce(in_command);
    SetCheckDataShow();
}

void OpCenter::SetCheckDataShow(){
    int currentData=drefW.GetWhatToShow();
    XPLMCheckMenuItem(menuShowData,iFPS,currentData==1?xplm_Menu_Checked:xplm_Menu_Unchecked);
    XPLMCheckMenuItem(menuShowData,iIAS,currentData==2?xplm_Menu_Checked:xplm_Menu_Unchecked);
    XPLMCheckMenuItem(menuShowData,iTAS,currentData==3?xplm_Menu_Checked:xplm_Menu_Unchecked);
    XPLMCheckMenuItem(menuShowData,iGS,currentData==4?xplm_Menu_Checked:xplm_Menu_Unchecked);
    XPLMCheckMenuItem(menuShowData,iGF,currentData==7?xplm_Menu_Checked:xplm_Menu_Unchecked);
    XPLMCheckMenuItem(menuShowData,iAoA,currentData==6?xplm_Menu_Checked:xplm_Menu_Unchecked);
    XPLMCheckMenuItem(menuShowData,iWeather,currentData==8?xplm_Menu_Checked:xplm_Menu_Unchecked);
}

void OpCenter::SetEnableHSMoves(bool has_hotspots){
    XPLMEnableMenuItem(menuHotspots,itemAdjusted,has_hotspots);
    XPLMEnableMenuItem(menuHotspots,itemFast,has_hotspots);
    XPLMEnableMenuItem(menuHotspots,itemSlow,has_hotspots);
    XPLMEnableMenuItem(menuHotspots,moveNext,has_hotspots);
    XPLMEnableMenuItem(menuHotspots,movePrev,has_hotspots);
}

void OpCenter::SetEnableTextOptions(){
    if (g_textWindow){
        XPLMEnableMenuItem(menuText,opt[0],XPLMGetWindowIsVisible(g_textWindow));
        for (int I(1);I<5;I++)
        XPLMEnableMenuItem(menuTextOpt,opt[I],XPLMGetWindowIsVisible(g_textWindow));
    }
    else {
        XPLMEnableMenuItem(menuText,opt[0],false);
        for (int I(1);I<5;I++)
            XPLMEnableMenuItem(menuTextOpt,opt[I],false);
    }

}

void OpCenter::CheckMenu(){
    myself->SetCheckDataShow();
}
int   OpCenter::MyTextReaderCommandHandler  (XPLMCommandRef,
                                             XPLMCommandPhase   inPhase,
                                             void               *inRefcon)
{
    int cmdIssued=*(static_cast<int*>(inRefcon));
    switch(cmdIssued){
    case 1:{
        //toggle Text Window Command
            switch (inPhase)
            {
                case xplm_CommandBegin :
                    {
                    if (myself->g_textWindow==nullptr){
                        myself->MakeTextWindow();
                        }
                    else{
                        const int vr_is_enabled = XPLMGetDatai(myself->g_vr_dref);
                        if (myself->g_in_vr&&!vr_is_enabled){
                            XPLMDestroyWindow(myself->g_textWindow);
                            myself->g_textWindow=nullptr;
                            myself->MakeTextWindow();
                        }
                        myself->g_in_vr = vr_is_enabled;
                        if (XPLMGetWindowIsVisible(myself->g_textWindow)==0){
                            XPLMSetWindowPositioningMode(myself->g_textWindow, myself->g_in_vr?xplm_WindowVR:xplm_WindowPositionFree, -1);
                            XPLMSetWindowIsVisible(myself->g_textWindow,1);
                            myself->ptrLayout->CheckButtonsVisibility();}
                         else{
                            XPLMSetWindowIsVisible(myself->g_textWindow,0);
                   /*an .ini option will tell if toggling means destroying window or hiding it
                     XPLMDestroyWindow(g_textWindow);
                     (*ptrLayout).ClosegWindow();
                     g_textWindow=nullptr;*/
                        }
                       }
                    myself->SetEnableTextOptions();
                    }
                    break;
                case xplm_CommandContinue :
                    break;
                case xplm_CommandEnd :
                    break;
            }
            break;
           }
    case 2:{ // Select First Line in the display
        if (inPhase==xplm_CommandBegin &&myself-> ptrLayout!=nullptr)
            if (myself->ptrLayout->HasActiveWindow())
                myself->ptrLayout->LaunchCommand(B_FirstLine);
        break;}

    case 3:{ //Next Line
        if (inPhase==xplm_CommandBegin && myself->ptrLayout!=nullptr)
            if (myself->ptrLayout->HasActiveWindow())
                myself->ptrLayout->LaunchCommand(B_NextLine);
    }
        break;
    case 4:{ //Previous Line
        if (inPhase==xplm_CommandBegin && myself->ptrLayout!=nullptr)
            if (myself->ptrLayout->HasActiveWindow())
                myself->ptrLayout->LaunchCommand(B_PrevLine);
    }
        break;
    case 5:{ //Delete Line
        if (inPhase==xplm_CommandBegin && myself->ptrLayout!=nullptr)
            if (myself->ptrLayout->HasActiveWindow())
                myself->ptrLayout->LaunchCommand(B_Toggle);
    }
        break;
    case 6:{ //Reload File ie restore display
       if (inPhase==xplm_CommandBegin && myself->ptrLayout!=nullptr)
           if (myself->ptrLayout->HasActiveWindow())
               myself->ptrLayout->LaunchCommand(B_Refresh);
    }
        break;
    }

    return 0;//no one else needs to handle this
}

void  OpCenter::drawText(XPLMWindowID , void *){
    myself->ptrLayout->DrawTextW(myself->g_textWindow);
}

void  OpCenter::drawFileWindow(XPLMWindowID in_window_id, void *){
    myself->dispDir->DrawDirWindow(in_window_id);
}

int   OpCenter::handle_mouse_for_TextW (XPLMWindowID, int x, int y, XPLMMouseStatus mouse_status, void *)
{
    if (myself->HasModalWindow()) return 1;
    switch (mouse_status){

        case xplm_MouseDown: {
            if(!XPLMIsWindowInFront(myself->g_textWindow))
            {
                XPLMBringWindowToFront(myself->g_textWindow);
            }else{
            myself->ptrLayout->findClick(x,y);}

        break;}

        case xplm_MouseDrag: {
               myself->ptrLayout->HandleMouseKeepDown(x,y);
        break;}

        case xplm_MouseUp: {
            int cmd=myself->ptrLayout->HandleMouseUp(x,y);
            switch (cmd){
              case 1 : // Hide Command
                if (XPLMGetWindowIsVisible(myself->g_textWindow)){
                    XPLMSetWindowIsVisible(myself->g_textWindow,0);
                    myself->SetEnableTextOptions();
                }
                break;
              case 2 : // Open Command
                myself->MakeFileWindow();
                break;
              case 3 : // Edit Command
                myself->ptrLayout->ClosegWindow();//Very important to stop DrawLogic's flightloop !
               myself->is_In_Edit_Mode=!myself->is_In_Edit_Mode;
               if (myself->is_In_Edit_Mode){
                  myself->ptrLayout=myself->wELayout.get();
                  myself->wELayout->SetWindowHandle(myself->g_textWindow);
                  myself->wELayout->initiate();
                  myself->wELayout->CheckButtonsVisibility();
                  myself->wELayout->BeginEdit();
               }else{
                 myself->ptrLayout=myself->wLayout.get();
                 myself->ptrLayout->SetWindowHandle(myself->g_textWindow);
                 myself->ptrLayout->initiate();
                 myself->ptrLayout->CheckButtonsVisibility();
               }
                break;
              case 4 : // Next File Command

                    FilePointer::GetNext();
                    myself->ptrLayout->initiate();
                    myself->ptrLayout->CheckButtonsVisibility();

                break;
              case 5 : // Previous File Command
                    FilePointer::GetPrevious();
                    myself->ptrLayout->initiate();
                    myself->ptrLayout->CheckButtonsVisibility();

                break;
            }
        break;}
}
return 1;//mouse events handled here
}

void OpCenter::EndEditMode(){
    ptrLayout=wLayout.get();
    ptrLayout->SetWindowHandle(g_textWindow);
    ptrLayout->initiate();
    ptrLayout->CheckButtonsVisibility();
    is_In_Edit_Mode=false;
}

int   OpCenter::handle_mouse_for_FileS(XPLMWindowID, int x, int y, XPLMMouseStatus mouse_status, void *){
    if (myself->HasModalWindow()) return 1;
    switch (mouse_status){

        case xplm_MouseDown: {
            if(!XPLMIsWindowInFront(myself->g_FileWindow))
            {
                XPLMBringWindowToFront(myself->g_FileWindow);

            }else{
            myself->dispDir->processMouseDn(x,y);}

        break;}

        case xplm_MouseDrag: {
               myself->dispDir->processMouseDrag(x,y);
        break;}

        case xplm_MouseUp: {

         int cmd=myself->dispDir->processMouseUp(x,y);
            if (cmd==0){//OK command has been pressed            
                XPLMDestroyWindow(myself->g_FileWindow);
                myself->g_FileWindow=nullptr;
                myself->dispDir->CloseDirWindow();
                if (IniSettings::GetOptLastFile()) {
                    myself->wLayout->KeepFile();
                }
                myself->ReadNewFile();
            }
            if (cmd==1 ){//Cancel has been pressed
                myself->MakeTextWindow();
                XPLMDestroyWindow(myself->g_FileWindow);
                myself->g_FileWindow=nullptr;
                myself->dispDir->CloseDirWindow();
            }
        break;
       }
    }
  return 1;//mouse events handled here
}

void  OpCenter::handle_physical_keyboard(XPLMWindowID,char,XPLMKeyFlags,char,void*,int){ return;}

void  OpCenter::MakeTextWindow(){

    if (g_textWindow==nullptr){
         const int vr_is_enabled = XPLMGetDatai(g_vr_dref);
         g_in_vr = vr_is_enabled;
         int left(0),bottom(0),top(0),right(0);
         XPLMGetScreenBoundsGlobal(&left, &top, &right,&bottom);
         if ((*ptrLayout).initiate()){
             XPLMCreateWindow_t params;
             params.structSize = sizeof(params);
             params.visible = 1;
             params.drawWindowFunc=drawText;
             if (vr_is_enabled) params.decorateAsFloatingWindow = xplm_WindowDecorationSelfDecoratedResizable;
             else               params.decorateAsFloatingWindow = xplm_WindowDecorationRoundRectangle;
             params.handleMouseClickFunc = handle_mouse_for_TextW;
             params.handleRightClickFunc = dummy_mouse_handler;
             params.handleMouseWheelFunc = dummy_wheel_handler;
             params.handleKeyFunc = handle_physical_keyboard;
             params.handleCursorFunc = dummy_cursor_status_handler;
             params.refcon = nullptr;
             params.layer = xplm_WindowLayerFloatingWindows;
             params.left = (*ptrLayout).wLeft;
             params.bottom = (*ptrLayout).wBottom-1;
             params.right = (*ptrLayout).wRight+2;
             params.top = (*ptrLayout).wTop+1;
             g_textWindow = XPLMCreateWindowEx(&params);
             XPLMSetWindowPositioningMode(g_textWindow, g_in_vr?xplm_WindowVR:xplm_WindowPositionFree, -1);
             XPLMSetWindowResizingLimits(g_textWindow,(*ptrLayout).minWidth, (*ptrLayout).minHeight, (*ptrLayout).maxWidth, (*ptrLayout).maxHeight);
             string sTitle("");
             sTitle=FilePointer::GetCurrentFileName();
             if (is_In_Edit_Mode)
                 wELayout->BeginEdit();
             XPLMSetWindowTitle(g_textWindow, (char*)sTitle.c_str());
             (*ptrLayout).SetWindowHandle(g_textWindow);
             (*ptrLayout).CheckButtonsVisibility();
         }
             else DrawLogic::WriteDebug(("couldn't initiate the textfile, file not found"));

       }
}

void  OpCenter::MakeFileWindow(){
    if (g_FileWindow==nullptr){

       const int vr_enabled = XPLMGetDatai(g_vr_dref);
       g_in_vr = vr_enabled;
       int l(0),t(0),b(0),r(0);
       XPLMGetScreenBoundsGlobal(&l, &t, &r, &b);
       dispDir->SetupDirWindow(l,t);
        XPLMCreateWindow_t params;
        params.structSize = sizeof(params);
        params.visible = 1;
        params.drawWindowFunc = drawFileWindow;
        params.handleMouseClickFunc = handle_mouse_for_FileS;
        params.handleRightClickFunc = dummy_mouse_handler;
        params.handleMouseWheelFunc = dummy_wheel_handler;
        params.handleKeyFunc = dummy_key_handler;
        params.handleCursorFunc = dummy_cursor_status_handler;
        params.refcon = nullptr;
        params.layer = xplm_WindowLayerFloatingWindows;
        params.decorateAsFloatingWindow = xplm_WindowDecorationNone;
        params.left = dispDir->GetLeft()+l+250;
        params.bottom = dispDir->GetBottom()+b+150;
        params.right = dispDir->GetRight()+l+250;
        params.top = dispDir->GetTop()+b+150;

       g_FileWindow = XPLMCreateWindowEx(&params);
       XPLMSetWindowPositioningMode(g_FileWindow, g_in_vr?xplm_WindowVR:xplm_WindowPositionFree, -1);
       XPLMSetWindowTitle(g_FileWindow,"Choose Directory and File");
       XPLMSetWindowResizingLimits(g_FileWindow, dispDir->GetWidth(), dispDir->GetHeight(),dispDir->GetWidth()+150, dispDir->GetHeight()+150); // Limit resizing our window
       ptrLayout->ClosegWindow();
       XPLMDestroyWindow(g_textWindow);
       dispDir->SetWindowId(g_FileWindow);
       dispDir->ActivateWindow();
       g_textWindow=nullptr;


    }
    else DrawLogic::WriteDebug("erronous call to MakeFileWindow");
}

void  OpCenter::ReadNewFile(){
    MakeTextWindow();
    XPLMSetWindowTitle(g_textWindow,(char*)(FilePointer::GetCurrentFileName()).c_str());
}

void OpCenter::SetModalWindow(bool mw){
    hasModalWindow=mw;
}

bool OpCenter::HasModalWindow(){
    return hasModalWindow;
}

/********** Dummys ******************/

int	  OpCenter::dummy_mouse_handler(XPLMWindowID, int, int, int, void *) { return 0; }
XPLMCursorStatus OpCenter::dummy_cursor_status_handler(XPLMWindowID, int, int, void *) { return xplm_CursorDefault;}
int	  OpCenter::dummy_wheel_handler(XPLMWindowID, int, int, int, int, void *) { return 0; }
void  OpCenter::dummy_key_handler(XPLMWindowID,char,XPLMKeyFlags,char,void*,int){}
