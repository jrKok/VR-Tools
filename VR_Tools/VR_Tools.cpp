#include "VR_Tools.h"


/* Variables */

static XPLMWindowID	g_textWindow,g_FileWindow;

bool is_In_Edit_Mode(false);
Layout wLayout;
LayoutWithEdit wELayout;
Layout* ptrLayout(nullptr);

int menuIdx,idxOfModeMenuItem;
XPLMMenuID menuId;
XPLMDataRef g_vr_dref;

static bool g_in_vr = false;
XPLMCommandRef CommandText(nullptr);
XPLMCommandRef CmdFirstLine(nullptr),CmdNextLine(nullptr),CmdPrevLine(nullptr),CmdDelLine(nullptr),CmdReload(nullptr);
ShowDir *dispDir;
DRefWindow drefW;
VRCommandFilter commandFilter;

/*Forward declarations */

void                drawText(XPLMWindowID in_window_id, void * in_refcon);
void                drawTextNoResize(XPLMWindowID in_window_id, void * in_refcon);
void                drawFileWindow(XPLMWindowID in_window_id, void * in_refcon);
int                 handle_mouse_for_TextW (XPLMWindowID in_window_id, int x, int y, XPLMMouseStatus mouse_status, void * in_refcon);
int                 handle_mouse_for_FileS(XPLMWindowID in_window_id, int x, int y, XPLMMouseStatus mouse_status, void * in_refcon);
void                handle_physical_keyboard(XPLMWindowID in_window_id,char in_key,XPLMKeyFlags in_flag,char in_VK,void* refcon,int is_losing_focus);

int					dummy_mouse_handler(XPLMWindowID in_window_id, int x, int y, int is_down, void * in_refcon) { return 0; }
XPLMCursorStatus	dummy_cursor_status_handler(XPLMWindowID in_window_id, int x, int y, void * in_refcon) { return xplm_CursorDefault; }
int					dummy_wheel_handler(XPLMWindowID in_window_id, int x, int y, int wheel, int clicks, void * in_refcon) { return 0; }
void                dummy_key_handler(XPLMWindowID in_window_id,char in_key,XPLMKeyFlags in_flag,char in_VK,void* refcon,int is_losing_focus){}

void                menuHandler(void* inMenuRef, void* inItemRef);

int  MyTextReaderCommandHandler  (XPLMCommandRef     inCommand,
                                  XPLMCommandPhase   inPhase,
                                  void               *inRefcon);//toggle the text window
void MakeTextWindow();
void MakeMenus();
void MakeFileWindow();
void ReadNewFile();

/*End Forward Declarations*/

/* Helper Function */

void MakeMenus(){
   menuIdx= XPLMAppendMenuItem(XPLMFindPluginsMenu(), "VR Tools", nullptr, 0);
   menuId= XPLMCreateMenu("VR Tools",XPLMFindPluginsMenu(),menuIdx,menuHandler,nullptr);
   XPLMAppendMenuItemWithCommand(menuId,"Toggle Text Window",CommandText);
   void * nb1=new(int*);(*(int*)nb1)=1;
   XPLMAppendMenuItem(menuId,"Fit Window to File",nb1,0);
   void * nb2=new(int*);(*(int*)nb2)=2;
      XPLMAppendMenuItem(menuId,"Keep File",nb2,0);
   void * nb3=new(int*);(*(int*)nb3)=3;
      XPLMAppendMenuItem(menuId,"Keep Size",nb3,0);
   void * nb4=new(int*);
   (*(int*)nb4)=4;
      XPLMAppendMenuItem(menuId,"FPS in Text Window",nb4,0);
   void * nb5=new(int*);
   (*(int*)nb5)=5;
      idxOfModeMenuItem=XPLMAppendMenuItem(menuId,"Change View Mode : Toggle",nb5,0);

}

void writeDebug(string in_String){
 in_String="VR Tools : " +in_String+"\r\n";
 XPLMDebugString((char*)in_String.c_str());
}

/*End HelperFunction*/

/* Start Implementations */

PLUGIN_API int XPluginStart(
                        char *		outName,
                        char *		outSig,
                        char *		outDesc)
{

    strcpy(outName, "FPS show in VR, Filter for VRStick commands, displays a text file");
    strcpy(outSig, "a plug-in by jrKok");
    strcpy(outDesc, "A plug-in that shows FPS in a small window in VR and toggles a text file");

    g_vr_dref    = XPLMFindDataRef("sim/graphics/VR/enabled");


    CommandText   = XPLMCreateCommand("VR/Custom/Toggle_Text_File","Toggle text");
    CmdFirstLine  = XPLMCreateCommand("VR/Custom/Text/Select_First_Line","Select First Line");
    CmdNextLine   = XPLMCreateCommand("VR/Custom/Text/Next_Line","Next Line");
    CmdPrevLine   = XPLMCreateCommand("VR/Custom/Text/Previous_Line","Previous Line");
    CmdDelLine    = XPLMCreateCommand("VR/Custom/Text/Delete_Line","Hide Line");
    CmdReload     = XPLMCreateCommand("VR/Custom/Text/Reload","Reload");

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

    //create plugin's menu
    MakeMenus();
    drefW.Setup();
    commandFilter.init();

    if (is_In_Edit_Mode){
        ptrLayout=&wELayout;
    }else{
        ptrLayout=(&wLayout);
    }

    return g_vr_dref != nullptr;
}



PLUGIN_API void	XPluginStop(void)
{
    commandFilter.UndoFiltering();

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

PLUGIN_API void XPluginDisable(void) {commandFilter.UndoFiltering(); }

PLUGIN_API int  XPluginEnable(void)  { return 1; }

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int msg, void * inParam){

    if ((!g_textWindow && msg == XPLM_MSG_SCENERY_LOADED)){
        wLayout.Begin();
        if ((*ptrLayout).OpenWindowAtStart()) MakeTextWindow();
    }
}

void menuHandler(void *inMenuRef, void *inItemRef){
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
    }
}




/* And here the code for the text reader which is a custom command and a drawing function which uses
 * the textreader class to tailor the text file and iterate among the lines
 and a custom mouse handler*/

int  MyTextReaderCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon){
    int cmdIssued=*((int*)inRefcon);
    switch(cmdIssued){
    case 1:{//toggle Text Window
            switch (inPhase)
            {
                case xplm_CommandBegin : //opens or closes the textreader, creates or destroys the window
                    {
                    if (g_textWindow==nullptr){//Toggle window in and out of existence
                        MakeTextWindow();  }
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

void drawText(XPLMWindowID in_window_id, void * in_refcon){
            (*ptrLayout).DrawTextW(g_textWindow);
}

void drawTextNoResize(XPLMWindowID in_window_id, void *in_refcon){
           (*ptrLayout).DrawNoResize(g_textWindow);
}

void drawFileWindow(XPLMWindowID in_window_id, void * in_refcon){

    dispDir->DrawDirWindow(in_window_id);
}


int	handle_mouse_for_TextW (XPLMWindowID in_window_id, int x, int y, XPLMMouseStatus mouse_status, void * in_refcon)
{ switch (mouse_status){

    case xplm_MouseDown: {
        if(!XPLMIsWindowInFront(g_textWindow))
        {
            XPLMBringWindowToFront(g_textWindow);
            //if (is_In_Edit_Mode)
                // XPLMTakeKeyboardFocus(g_textWindow);
        }else{
        (*ptrLayout).findClick(x,y);}

    break;}

    case xplm_MouseDrag: {
           (*ptrLayout).HandleMouseKeepDown(x,y);
    break;}

    case xplm_MouseUp: {
        int cmd=(*ptrLayout).HandleMouseUp(x,y);
        if (cmd==2){//load command has been pressed
        MakeFileWindow();
        }
        if (cmd==3){//edit command is toggled,first coding try : don't destroy window but reinitiate the layout,
            //XPLMDestroyWindow(g_textWindow);
            (*ptrLayout).ClosegWindow();
            //g_textWindow=nullptr;
            is_In_Edit_Mode=!is_In_Edit_Mode;
            if (is_In_Edit_Mode){
                ptrLayout=&wELayout;
            }else{
                ptrLayout=(&wLayout);
            }
            //MakeTextWindow();
            ptrLayout->initiate();
            ptrLayout->SetWindowHandle(g_textWindow);
            ptrLayout->CheckButtonsVisibility();
        }
    break;}

}return 1;//mouse events handled here
}



void MakeTextWindow(){
    if (g_textWindow==nullptr){
      const int vr_is_enabled = XPLMGetDatai(g_vr_dref);
      g_in_vr = vr_is_enabled;

      XPLMGetScreenBoundsGlobal(&(*ptrLayout).gLeft, &(*ptrLayout).gTop, &(*ptrLayout).gRight, &(*ptrLayout).gBottom);
      if ((*ptrLayout).initiate()){
          XPLMCreateWindow_t params;
          params.structSize = sizeof(params);
          params.visible = 1;
          if ((*ptrLayout).GetResizeOption()){
             params.drawWindowFunc=drawTextNoResize;
             params.decorateAsFloatingWindow=xplm_WindowDecorationNone;}
          else{
               params.drawWindowFunc = drawText;
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
          XPLMSetWindowTitle(g_textWindow, (char*)sTitle.c_str());
          (*ptrLayout).SetWindowHandle(g_textWindow);
          (*ptrLayout).CheckButtonsVisibility();
      }
          else writeDebug(("couldn't initiate the textfile, file not found"));

    }
}

void MakeFileWindow() {
    if (g_FileWindow==nullptr){
        writeDebug("begin file window");
       dispDir = new(ShowDir);
       const int vr_is_enabled = XPLMGetDatai(g_vr_dref);
       g_in_vr = vr_is_enabled;
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
        params.left = dispDir->general.left;
        params.bottom = dispDir->general.bottom;
        params.right = dispDir->general.right;
        params.top = dispDir->general.top;

       g_FileWindow = XPLMCreateWindowEx(&params);
       XPLMSetWindowPositioningMode(g_FileWindow, g_in_vr?xplm_WindowVR:xplm_WindowPositionFree, -1);
       XPLMSetWindowTitle(g_FileWindow,"Choose Directory and File");
       XPLMSetWindowResizingLimits(g_FileWindow, dispDir->general.width, dispDir->general.height,dispDir->general.width+150, dispDir->general.height+150); // Limit resizing our window
       XPLMDestroyWindow(g_textWindow);
       (*ptrLayout).ClosegWindow();
       g_textWindow=nullptr;

    }
    else writeDebug("erronous call to MakeFileWindow");
}

void ReadNewFile(){

    //string newTitle=dispDir->GetSelectedFile();
    //string newFile=dispDir->GetSelectedPath();

    MakeTextWindow();

    XPLMSetWindowTitle(g_textWindow,(char*)(FilePointer::GetCurrentFileName()).c_str());
    //(*ptrLayout).SetNewFile(IniSettings::GetCurrentFileName());
}

int handle_mouse_for_FileS(XPLMWindowID in_window_id, int x, int y, XPLMMouseStatus mouse_status, void * in_refcon){
    { switch (mouse_status){

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
            if (cmd==0){//load command has been pressed
                ReadNewFile();
                XPLMDestroyWindow(g_FileWindow);

                g_FileWindow=nullptr;
                delete dispDir;
                dispDir=nullptr;
            }
            if (cmd==1 ){

                MakeTextWindow();
                XPLMDestroyWindow(g_FileWindow);

                g_FileWindow=nullptr;
                delete dispDir;
                dispDir=nullptr;
            }
        break;}

    }return 1;//mouse events handled here
    }
}

void handle_physical_keyboard(XPLMWindowID in_window_id, char in_key, XPLMKeyFlags in_flag, char in_VK, void* refcon, int is_losing_focus){
    writeDebug("got called");
    if ((in_window_id==g_textWindow)&&(!is_losing_focus)&&is_In_Edit_Mode){
       wELayout.PhysicalKeyPressed(in_key,in_flag,in_VK);
    }
}
