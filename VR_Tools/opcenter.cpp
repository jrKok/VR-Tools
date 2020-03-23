#include "opcenter.h"

OpCenter*       OpCenter::pointerToMe(nullptr);
bool            OpCenter::g_in_vr(false);
bool            OpCenter::is_In_Edit_Mode(false);
XPLMWindowID    OpCenter::g_textWindow(nullptr);
XPLMWindowID    OpCenter::g_FileWindow(nullptr);
Layout         *OpCenter::wLayout(nullptr);
LayoutWithEdit *OpCenter::wELayout(nullptr);
Layout*         OpCenter::ptrLayout(nullptr);
ShowDir        *OpCenter::dispDir(nullptr);
DRefWindow      OpCenter::drefW;
VRCommandFilter OpCenter::commandFilter;
Hotspots        OpCenter::htsp;

int        OpCenter::menuIdx(0);
int        OpCenter::idxOfModeMenuItem(0);
int        OpCenter::itemAdjusted(0);
int        OpCenter::itemFast(0);
int        OpCenter::itemSlow(0);
int        OpCenter::itemReload(0);
XPLMMenuID OpCenter::menuId(nullptr);
XPLMMenuID OpCenter::menuTextOpt(nullptr);
XPLMMenuID OpCenter::menuHotspots(nullptr);
bool       OpCenter::IsLaunched(false);

OpCenter::OpCenter():
    g_vr_dref(nullptr),
    CommandText(nullptr),
    CmdFirstLine(nullptr),
    CmdNextLine(nullptr),
    CmdPrevLine(nullptr),
    CmdDelLine(nullptr),
    CmdReload(nullptr),
    ini(),
    //drw(),
    tw(),
    colordefs()
{

}

OpCenter::~OpCenter(){
    fontmanager.EndFreeType();
    delete (wLayout);
    delete (wELayout);
    if (dispDir!=nullptr) delete dispDir;
    wLayout=nullptr;
    wELayout=nullptr;
    dispDir=nullptr;
    ptrLayout=nullptr;
}

void OpCenter::WriteDebug(string in_String){
    in_String="VR Tools : " +in_String+"\r\n";
    XPLMDebugString((char*)in_String.c_str());
}

int OpCenter::SetupCenter(){

    WriteDebug("Go To GetIniParams");
    colordefs.DefineColors();
    IniSettings::GetIniParams();
    g_vr_dref    = XPLMFindDataRef("sim/graphics/VR/enabled");
    pointerToMe  = this;

    CommandText   = XPLMCreateCommand("VR_Tools/Custom/Toggle_Text_File","Toggle text");
    CmdFirstLine  = XPLMCreateCommand("VR_Tools/Custom/Text/Select_First_Line","Select First Line");
    CmdNextLine   = XPLMCreateCommand("VR_Tools/Custom/Text/Next_Line","Next Line");
    CmdPrevLine   = XPLMCreateCommand("VR_Tools/Custom/Text/Previous_Line","Previous Line");
    CmdDelLine    = XPLMCreateCommand("VR_Tools/Custom/Text/Delete_Line","Hide Line");
    CmdReload     = XPLMCreateCommand("VR_Tools/Custom/Text/Reload","Reload");

    fontmanager.Initialise();

    //Register numbered text commands
    void * nb=new(int*);
    (*(int*)nb)=0;
    nb=new(int*);(*(int*)nb)=1;
    XPLMRegisterCommandHandler(CommandText,MyTextReaderCommandHandler,1,nb);
    nb=new(int*);(*(int*)nb)=2;
    XPLMRegisterCommandHandler(CmdFirstLine,MyTextReaderCommandHandler,1,nb);
    nb=new(int*);(*(int*)nb)=3;
    XPLMRegisterCommandHandler(CmdNextLine,MyTextReaderCommandHandler,1,nb);
    nb=new(int*);(*(int*)nb)=4;
    XPLMRegisterCommandHandler(CmdPrevLine,MyTextReaderCommandHandler,1,nb);
    nb=new(int*);(*(int*)nb)=5;
    XPLMRegisterCommandHandler(CmdDelLine,MyTextReaderCommandHandler,1,nb);
    nb=new(int*);(*(int*)nb)=6;
    XPLMRegisterCommandHandler(CmdReload,MyTextReaderCommandHandler,1,nb);
    drefW.Setup();
    htsp.Setup();
    MakeMenus();
    commandFilter.init();

    WriteDebug("VR Tools version 1.3.3 final - Show FPS, speeds, g-forces, filter commands, edit text files");
    IsLaunched=true;
    return g_vr_dref != nullptr;
}

void OpCenter::LaunchOperations(){

    DrawLogic *ndp=new DrawLogic();
    ndp->Initiate();
    ndp->SetId("Layout");
    wLayout=new Layout(ndp);
   ndp=new DrawLogic();
   ndp->Initiate();
   ndp->SetId("LayoutWEdit");

    wELayout=new LayoutWithEdit(ndp);
   ndp=new DrawLogic();
   ndp->Initiate();
   ndp->SetId("Directory");
   dispDir=new ShowDir(ndp);

    if (is_In_Edit_Mode){
        ptrLayout=wELayout;
    }else{
        ptrLayout=wLayout;
    }

    if (!g_textWindow ){
        wLayout->Begin();
        wELayout->StartEdit();//like Begin(), makes keyboard as well
        if ((*ptrLayout).OpenWindowAtStart()) {
            MakeTextWindow();
        }
    }
    ndp=nullptr;
}

void OpCenter::HaltOperations(){
    commandFilter.UndoFiltering();
    FilePointer::ReleaseBackups();

    drefW.Unload();

    if (g_FileWindow!=nullptr){
        XPLMDestroyWindow(g_FileWindow);
        g_FileWindow=nullptr;
    }

    if (dispDir!=nullptr){
        delete dispDir;
        dispDir=nullptr;
    }

}

void OpCenter::SuspendOperations(){
    commandFilter.UndoFiltering();
}

int  OpCenter::ResumeOperations(){
    return 1;
}

/************************************************************************************/
void  OpCenter::MakeMenus(){
    menuIdx= XPLMAppendMenuItem(XPLMFindPluginsMenu(), "VR Tools", nullptr, 0);
    menuId= XPLMCreateMenu("VR Tools",XPLMFindPluginsMenu(),menuIdx,menuHandler,nullptr);
    XPLMAppendMenuItemWithCommand(menuId,"Toggle Text Window",CommandText);
    XPLMAppendMenuSeparator(menuId);
    int menuTD=XPLMAppendMenuItem(menuId,"Options for Text Display",nullptr,0);
    int menuHS=XPLMAppendMenuItem(menuId,"Hotspots",nullptr,0);
    menuTextOpt=XPLMCreateMenu("text F",menuId,menuTD,menuHandler,nullptr);
    menuHotspots=XPLMCreateMenu("hotspots",menuId,menuHS,menuHandler,nullptr);
    //Menu text options
       void * nb1=new(int*);(*(int*)nb1)=1;
       XPLMAppendMenuItem(menuTextOpt,"Fit Window Size to File",nb1,0);
       void * nb2=new(int*);(*(int*)nb2)=2;
       XPLMAppendMenuItem(menuTextOpt,"Keep File",nb2,0);
       void * nb3=new(int*);(*(int*)nb3)=3;
       XPLMAppendMenuItem(menuTextOpt,"Keep Size",nb3,0);
       void * nb4=new(int*);(*(int*)nb4)=4;
       XPLMAppendMenuItem(menuTextOpt,"Show FPS in Text Window",nb4,0);
    //Menu for Hotspots
       XPLMAppendMenuItemWithCommand(menuHotspots,"Hotspot Editor",Hotspots::CmdEditHotspot);
       XPLMAppendMenuItemWithCommand(menuHotspots,"Move To Next",Hotspots::CmdJumpNext);
       XPLMAppendMenuItemWithCommand(menuHotspots,"Move To Previous",Hotspots::CmdJumpBack);
       XPLMAppendMenuSeparator(menuHotspots);
       void * nb6=new(int*);(*(int*)nb6)=6;
       itemReload=XPLMAppendMenuItem(menuHotspots,"Reload Model if Hotspot modified",nb6,0);
       XPLMAppendMenuSeparator(menuHotspots);
       int opt=IniSettings::GetSpeedMove();
       void * nb8=new(int*);(*(int*)nb8)=8;
       itemAdjusted=XPLMAppendMenuItem(menuHotspots,"Adjusted Move",nb8,0);
       void * nb9=new(int*);(*(int*)nb9)=9;
       itemSlow=XPLMAppendMenuItem(menuHotspots,"Slow Move",nb9,0);
       void * nb10=new(int*);(*(int*)nb10)=10;
       itemFast=XPLMAppendMenuItem(menuHotspots,"Fast Move",nb10,0);

    //Options for dataref
    XPLMAppendMenuSeparator(menuId);
    void * nb5=new(int*);(*(int*)nb5)=5;
    idxOfModeMenuItem=XPLMAppendMenuItem(menuId,"Change View Mode : Toggle",nb5,0);
    if (opt==1) XPLMCheckMenuItem(menuHotspots,itemAdjusted,xplm_Menu_Checked);
    if (opt==0) XPLMCheckMenuItem(menuHotspots,itemSlow,xplm_Menu_Checked);
    if (opt==2) XPLMCheckMenuItem(menuHotspots,itemFast,xplm_Menu_Checked);
    if (IniSettings::GetOptReloadModel()) XPLMCheckMenuItem(menuHotspots,itemReload,xplm_Menu_Checked);


}
void  OpCenter::menuHandler(void*, void* inItemRef){
    int menuItem=*((int*)inItemRef);
        switch (menuItem){
            case 1:{
                (*ptrLayout).FitToFile();
            }
                break;
            case 2:{
                (*ptrLayout).KeepFile();
            }
                break;
            case 3:{
                (*ptrLayout).KeepCurrentSize();
                break;
            }
            case 4:{
                (*ptrLayout).ToggleFPS();
                break;
            }
            case 5:{
                drefW.DisposeWindow();
                drefW.ToggleShowMode();
                XPLMSetMenuItemName(  menuId,idxOfModeMenuItem,
                                      drefW.GetShowModeOnPress()?"Change View Mode : Toggle":"Change View Mode : On Press",0);
                break;
            }
        case 6:{
            IniSettings::SetOptReloadModel(!IniSettings::GetOptReloadModel());
            if (IniSettings::GetOptReloadModel()) XPLMCheckMenuItem(menuHotspots,itemReload,xplm_Menu_Checked);
            else XPLMCheckMenuItem(menuHotspots,itemReload,xplm_Menu_Unchecked);
            IniSettings::WriteIniFile();
            break;
        }
        case 8:{
            XPLMCheckMenuItem(menuHotspots,itemAdjusted,xplm_Menu_Checked);
            XPLMCheckMenuItem(menuHotspots,itemSlow,xplm_Menu_Unchecked);
            XPLMCheckMenuItem(menuHotspots,itemFast,xplm_Menu_Unchecked);
            IniSettings::SetSpeedMove(1);
            IniSettings::WriteIniFile();
            break;}
        case 9:{
            XPLMCheckMenuItem(menuHotspots,itemAdjusted,xplm_Menu_Unchecked);
            XPLMCheckMenuItem(menuHotspots,itemSlow,xplm_Menu_Checked);
            XPLMCheckMenuItem(menuHotspots,itemFast,xplm_Menu_Unchecked);
            IniSettings::SetSpeedMove(0);
            IniSettings::WriteIniFile();
            break;}
        case 10:{
            XPLMCheckMenuItem(menuHotspots,itemAdjusted,xplm_Menu_Unchecked);
            XPLMCheckMenuItem(menuHotspots,itemSlow,xplm_Menu_Unchecked);
            XPLMCheckMenuItem(menuHotspots,itemFast,xplm_Menu_Checked);
            IniSettings::SetSpeedMove(2);
            IniSettings::WriteIniFile();
            break;}
        }
}
void  OpCenter::drawText(XPLMWindowID , void *){
    (*ptrLayout).DrawTextW(g_textWindow);
}

void  OpCenter::drawFileWindow(XPLMWindowID in_window_id, void *){
    dispDir->DrawDirWindow(in_window_id);
}

int   OpCenter::handle_mouse_for_TextW (XPLMWindowID, int x, int y, XPLMMouseStatus mouse_status, void *)

{
    if (DrawLogic::IsModalWindow()) return 1;
    switch (mouse_status){

        case xplm_MouseDown: {
            if(!XPLMIsWindowInFront(g_textWindow))
            {
                XPLMBringWindowToFront(g_textWindow);
            }else{
            (*ptrLayout).findClick(x,y);}

        break;}

        case xplm_MouseDrag: {
               (*ptrLayout).HandleMouseKeepDown(x,y);
        break;}

        case xplm_MouseUp: {
            int cmd=(*ptrLayout).HandleMouseUp(x,y);
            if (cmd==2){//load command has been pressed
            pointerToMe->MakeFileWindow();
            }
            if (cmd==3){//edit command is toggled,first coding try : don't destroy window but reinitiate the layout,
               //XPLMDestroyWindow(g_textWindow);
                //(*ptrLayout).ClosegWindow();
                //g_textWindow=nullptr;
                is_In_Edit_Mode=!is_In_Edit_Mode;
                if (is_In_Edit_Mode){
                    ptrLayout=wELayout;
                    wELayout->SetWindowHandle(g_textWindow);
                    wELayout->initiate();
                    wELayout->CheckButtonsVisibility();
                    wELayout->BeginEdit();
                }else{
                    ptrLayout=wLayout;
                    ptrLayout->SetWindowHandle(g_textWindow);
                    ptrLayout->initiate();
                    ptrLayout->CheckButtonsVisibility();
                }
            }
        break;}
}
return 1;//mouse events handled here
}

void OpCenter::EndEditMode(){
    ptrLayout=wLayout;
    ptrLayout->SetWindowHandle(g_textWindow);
    ptrLayout->initiate();
    ptrLayout->CheckButtonsVisibility();
    is_In_Edit_Mode=false;
}

int   OpCenter::handle_mouse_for_FileS(XPLMWindowID, int x, int y, XPLMMouseStatus mouse_status, void *){
    if (DrawLogic::IsModalWindow()) return 1;
    switch (mouse_status){

        case xplm_MouseDown: {
            if(!XPLMIsWindowInFront(g_FileWindow))
            {
                XPLMBringWindowToFront(g_FileWindow);

            }else{
            dispDir->processMouseDn(x,y);}

        break;}

        case xplm_MouseDrag: {
               dispDir->processMouseDrag(x,y);
        break;}

        case xplm_MouseUp: {

         int cmd=dispDir->processMouseUp(x,y);
            if (cmd==0){//OK command has been pressed            
                XPLMDestroyWindow(g_FileWindow);
                g_FileWindow=nullptr;
                dispDir->CloseDirWindow();
                if (IniSettings::GetOptLastFile()) pointerToMe->wLayout->KeepFile();
                pointerToMe->ReadNewFile();
            }
            if (cmd==1 ){//Cancel has been pressed
                pointerToMe->MakeTextWindow();
                XPLMDestroyWindow(g_FileWindow);
                g_FileWindow=nullptr;
            }
        break;
       }
    }
  return 1;//mouse events handled here
}



void  OpCenter::handle_physical_keyboard(XPLMWindowID in_window_id,char in_key,XPLMKeyFlags in_flag,char in_VK,void* refcon,int is_losing_focus){ return;}


int   OpCenter::MyTextReaderCommandHandler  (XPLMCommandRef     inCommand,
                                             XPLMCommandPhase   inPhase,
                                             void               *inRefcon)
{
    int cmdIssued=*((int*)inRefcon);
    switch(cmdIssued){
    case 1:{//toggle Text Window
            switch (inPhase)
            {
                case xplm_CommandBegin : //opens or closes the textreader, creates or destroys the window
                    {
                    if (g_textWindow==nullptr){//Toggle window in and out of existence
                        pointerToMe->MakeTextWindow();
                        }
                    else
                        {if (XPLMGetWindowIsVisible(g_textWindow)==0)
                            {XPLMSetWindowIsVisible(g_textWindow,1);
                            (*ptrLayout).CheckButtonsVisibility();}
                         else{
                            XPLMSetWindowIsVisible(g_textWindow,0);
                   /*an .ini option will tell if toggling means destroying window or hiding it
                     XPLMDestroyWindow(g_textWindow);
                     (*ptrLayout).ClosegWindow();
                     g_textWindow=nullptr;*/
                        }
                       }
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
        if (inPhase==xplm_CommandBegin) (*ptrLayout).LaunchCommand(B_FirstLine);
        break;}

    case 3:{ //Next Line
        if (inPhase==xplm_CommandBegin) (*ptrLayout).LaunchCommand(B_NextLine);
    }
        break;
    case 4:{ //Previous Line
        if (inPhase==xplm_CommandBegin) (*ptrLayout).LaunchCommand(B_PrevLine);
    }
        break;
    case 5:{ //Delete Line
        if (inPhase==xplm_CommandBegin) (*ptrLayout).LaunchCommand(B_Toggle_Line);
    }
        break;
    case 6:{ //Reload File ie restore display
       if (inPhase==xplm_CommandBegin) (*ptrLayout).LaunchCommand(B_Reload);
    }
        break;
    }

    return 0;//no one else needs to handle this
}

void  OpCenter::MakeTextWindow(){

    if (g_textWindow==nullptr){
         const int vr_is_enabled = XPLMGetDatai(g_vr_dref);
         g_in_vr = vr_is_enabled;
         int left,bottom,top,right;
         XPLMGetScreenBoundsGlobal(&left, &top, &right,&bottom);
         if ((*ptrLayout).initiate()){
             XPLMCreateWindow_t params;
             params.structSize = sizeof(params);
             params.visible = 1;
             params.drawWindowFunc=drawText;
             if ((*ptrLayout).GetResizeOption()){
                params.decorateAsFloatingWindow=xplm_WindowDecorationNone;}
             else{
                  params.decorateAsFloatingWindow = xplm_WindowDecorationRoundRectangle;}
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
             else WriteDebug(("couldn't initiate the textfile, file not found"));

       }
}

void  OpCenter::MakeFileWindow(){
    if (g_FileWindow==nullptr){
       const int vr_enabled = XPLMGetDatai(g_vr_dref);
       g_in_vr = vr_enabled;
       int l,t,b,r;
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
       XPLMDestroyWindow(g_textWindow);
       dispDir->SetWindowId(g_FileWindow);
       dispDir->ActivateWindow();
            //****(*ptrLayout).ClosegWindow();
       g_textWindow=nullptr;

    }
    else WriteDebug("erronous call to MakeFileWindow");
}

void  OpCenter::ReadNewFile(){
    MakeTextWindow();
    XPLMSetWindowTitle(g_textWindow,(char*)(FilePointer::GetCurrentFileName()).c_str());
}

/********** Dummys ******************/

int	  OpCenter::dummy_mouse_handler(XPLMWindowID in_window_id, int x, int y, int is_down, void * in_refcon) { return 0; }
XPLMCursorStatus OpCenter::dummy_cursor_status_handler(XPLMWindowID in_window_id, int x, int y, void * in_refcon) { return xplm_CursorDefault; }
int	  OpCenter::dummy_wheel_handler(XPLMWindowID in_window_id, int x, int y, int wheel, int clicks, void * in_refcon) { return 0; }
void  OpCenter::dummy_key_handler(XPLMWindowID in_window_id,char in_key,XPLMKeyFlags in_flag,char in_VK,void* refcon,int is_losing_focus){}
