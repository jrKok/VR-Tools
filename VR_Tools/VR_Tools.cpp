#include "VR_Tools.h"


/* Variables */

static XPLMWindowID	g_window, g_textWindow,g_FileWindow;
int counter;
float currentFPS, currentmeasure;
bool commandBlock=false,filterEnabled=false,del_wLayout=false,del_showDir=false;
Layout wLayout;

int menuIdx;
XPLMMenuID menuId;
std::vector<int> state;
std::vector<XPLMCommandRef> Commands;
std::vector<void*> refCons;
XPLMDataRef g_vr_dref,g_FPS,dref_SunPitch;


static bool g_in_vr = false;
static float g_vr_button_lbrt[4]; // left, bottom, right, top
XPLMCommandRef CommandFPS = NULL,CommandFilter=NULL,CommandText=NULL;
ShowDir *dispDir;

float cyan[] = {0.0f, 1.0f, 1.0f};
float green[]={0.0f,0.95f,0.0f};
float gray[] = {0.375f, 0.375f, 0.368f};
float lightergray[]={0.725f,0.725f,0.770f};
float paperWhitesomber[] = {0.9529f,0.7686f,0.4706f};
//float red[]= {0.90f,0.0f,0.0f};
float darkgray[]={0.009f,0.009f,0.009f};
float blackpaper[]={0.020f,0.020f,0.020f};
float colorPaper[]={0.9922f,0.9608f,0.9098f}; //Duron paper white
float colorInk[]={0.005f,0.005f,0.005f};
float amberInk[]={1.0f,0.65f,0.0f};

/*Forward declarations */

void				drawFPS(XPLMWindowID in_window_id, void * in_refcon);
void                drawText(XPLMWindowID in_window_id, void * in_refcon);
void                drawFileWindow(XPLMWindowID in_window_id, void * in_refcon);
int					handle_mouse(XPLMWindowID in_window_id, int x, int y, int is_down, void * in_refcon);
int                 handle_mouse_for_TextW (XPLMWindowID in_window_id, int x, int y, XPLMMouseStatus mouse_status, void * in_refcon);
int                 handle_mouse_for_FileS(XPLMWindowID in_window_id, int x, int y, XPLMMouseStatus mouse_status, void * in_refcon);

int					dummy_mouse_handler(XPLMWindowID in_window_id, int x, int y, int is_down, void * in_refcon) { return 0; }
XPLMCursorStatus	dummy_cursor_status_handler(XPLMWindowID in_window_id, int x, int y, void * in_refcon) { return xplm_CursorDefault; }
int					dummy_wheel_handler(XPLMWindowID in_window_id, int x, int y, int wheel, int clicks, void * in_refcon) { return 0; }
void				dummy_key_handler(XPLMWindowID in_window_id, char key, XPLMKeyFlags flags, char virtual_key, void * in_refcon, int losing_focus) { }

int  MyFPSCommandHandler         (XPLMCommandRef     inCommand, //Custom command handler for FPS window
                                  XPLMCommandPhase   inPhase,
                                  void *             inRefcon);
int  MyFilterCommandHandler      (XPLMCommandRef     inCommand, //This one receives the custom command
                                  XPLMCommandPhase   inPhase,
                                  void *             inRefcon);
int  MyBlockFilterCommandHandler (XPLMCommandRef     inCommand,//This one intercepts the commands of the registered stick commands
                                  XPLMCommandPhase   inPhase,
                                  void *             inRefcon);
int  MyTextReaderCommandHandler  (XPLMCommandRef     inCommand,
                                  XPLMCommandPhase   inPhase,
                                  void               *inRefcon);//toggle the text window


bool SetupFiltering();
void UndoFiltering();
void MakeTextWindow();
void MakeMenus();
void MakeFileWindow();
void ReadNewFile();

/*End Forward Declarations*/

/* Helper Function */

void MakeMenus(){
   menuIdx= XPLMAppendMenuItem(XPLMFindPluginsMenu(), "Text Files", 0, 0);
   menuId= XPLMCreateMenu("Text Files",XPLMFindPluginsMenu(),menuIdx,NULL,NULL);
   XPLMAppendMenuItemWithCommand(menuId,"Toggle Text Window",CommandText);
}

void writeDebug(std::string in_String){
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
    writeDebug("start Plugin");

    currentFPS=0.0f;
    counter=0;
    currentmeasure=0.0f;

    g_vr_dref    = XPLMFindDataRef("sim/graphics/VR/enabled");
    g_FPS        = XPLMFindDataRef("sim/operation/misc/frame_rate_period");
    dref_SunPitch= XPLMFindDataRef("sim/graphics/scenery/sun_pitch_degrees");//float to estimate pos of sun


    CommandFPS    = XPLMCreateCommand("VR/Custom/ShowFPS", "Show FPS");
    CommandFilter = XPLMCreateCommand("VR/Custom/Filter_Stick_Commands","Filter Stick");
    CommandText   = XPLMCreateCommand("VR/Custom/Toggle_Text_File","Toggle text");

    XPLMRegisterCommandHandler(CommandFPS,              // in Command name
                                MyFPSCommandHandler,       // in Handler
                                1,                      // Receive input before plugin windows.
                                (void *) 0);
    XPLMRegisterCommandHandler(CommandFilter,MyFilterCommandHandler,1,(void *)0); //to apply to the button handling the filtering like a "ctrl" key
    XPLMRegisterCommandHandler(CommandText,MyTextReaderCommandHandler,1,(void *)0);//Command to toggle tchotchke displaying text file
    MakeMenus();

    return g_vr_dref != NULL;
}



PLUGIN_API void	XPluginStop(void)
{//if the window hasn't been destroyed, we do it now
    XPLMDebugString("VR_Tool Plugin : am going to undo all filters\r\n");
    UndoFiltering();//I think I have to do this to delete the refCon pointers and avoid memory leaks
    XPLMDebugString("VR_Tool Plugin : am going to close open windows");
    if (g_window != NULL){
        XPLMDebugString("Unloading VR window\r\n");
        XPLMSetWindowPositioningMode(g_window,xplm_WindowPositionFree,0);
        XPLMDestroyWindow(g_window);
        g_window = NULL;}

    if (g_textWindow!=NULL){
        XPLMDestroyWindow(g_textWindow);
        g_textWindow=NULL;
    }

    if (g_FileWindow!=NULL){
        XPLMDestroyWindow(g_FileWindow);
        g_FileWindow=NULL;
    }

    if (dispDir!=nullptr){
        delete dispDir;
        dispDir=nullptr;
    }
}

PLUGIN_API void XPluginDisable(void) {UndoFiltering(); }
PLUGIN_API int  XPluginEnable(void)  { return 1; }
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int msg, void * inParam){
    //if(!g_textWindow && msg == XPLM_MSG_SCENERY_LOADED) MakeTextWindow();
    //comment the instruction above for those who don't want the text, only the FPS.
    //this will be taken care of by the .ini file
}

void	drawFPS(XPLMWindowID in_window_id, void * in_refcon) // draw FPS Window
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
    XPLMGetWindowGeometry(g_window, &l, &t, &r, &b);


    glColor3fv(gray);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glBegin(GL_TRIANGLE_FAN);
    {
        glVertex2i(l+1, t-1);
        glVertex2i(r-1, t-1);
        glVertex2i(r-1, b+1);
        glVertex2i(l+1, b+1);
    }
    glEnd();

        // Draw the FPS
        std::string chFPS=std::to_string(currentFPS); //that's why I prefer C++ strings !
        chFPS="FPS: "+chFPS.substr(0,4);

        XPLMDrawString(cyan, l+10, t-16, (char *)chFPS.c_str(), NULL, xplmFont_Proportional);

}

int	handle_mouse(XPLMWindowID in_window_id, int x, int y, XPLMMouseStatus mouse_status, void * in_refcon)
{
    return 1; //mouse events handled by caller
}

/* the very simple FPS displayer command
 * builds a VR window or even a regular small window if vr is not enabled
 * which will be triggered by a button choosen by the user
 * makes a mean of 15 values measured. The values get displayed when draw is called, which happens often enough - I hope
 */

int MyFPSCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon){
    switch (inPhase)
    {
    case xplm_CommandBegin :

        if (g_window==NULL){//I prefer to recreate the window to keep a clean cockpit
            const int vr_is_enabled = XPLMGetDatai(g_vr_dref);
            g_in_vr = vr_is_enabled;
            int global_desktop_bounds[4];
            XPLMGetScreenBoundsGlobal(&global_desktop_bounds[0], &global_desktop_bounds[3], &global_desktop_bounds[2], &global_desktop_bounds[1]);

            XPLMCreateWindow_t params;
             params.structSize = sizeof(params);
             params.visible = 1;
             params.drawWindowFunc = drawFPS;
             params.handleMouseClickFunc = dummy_mouse_handler;
             params.handleRightClickFunc = dummy_mouse_handler;
             params.handleMouseWheelFunc = dummy_wheel_handler;
             params.handleKeyFunc = dummy_key_handler;
             params.handleCursorFunc = dummy_cursor_status_handler;
             params.refcon = NULL;
             params.layer = xplm_WindowLayerFloatingWindows;
             params.decorateAsFloatingWindow = xplm_WindowDecorationNone;
             params.left = global_desktop_bounds[0] + 20;
             params.bottom = global_desktop_bounds[1] + 50;
             params.right = global_desktop_bounds[0] + 160;
             params.top = global_desktop_bounds[1] + 100;

            g_window = XPLMCreateWindowEx(&params);
            XPLMSetWindowPositioningMode(g_window, g_in_vr?xplm_WindowVR:xplm_WindowPositionFree, -1);
            XPLMSetWindowResizingLimits(g_window, 160, 20, 160, 20);
            XPLMSetWindowTitle(g_window, "FPS");}
        else XPLMSetWindowIsVisible(g_window,1);

        break;

    case xplm_CommandContinue :
    {float value;value=1/(XPLMGetDataf(g_FPS)); //instead of showing an instant of FPS, I will compute a mean of 15 values
        if (counter==0){ //begin counting
            counter=1;
            currentmeasure=value;
            currentFPS=value; //of course show first FPS immediatly, currentFPS is the value displayed
        }else{
            currentmeasure+=value; //sum of measured values, don't alter currentFPS which will still be displayed
            if(counter<15){
                counter++;
            }else{
                currentFPS=currentmeasure/15; //mean and change the value displayed
                currentmeasure=value; //reinitialize counter and buffer for sum
                counter=1;
            }
          }}
        break;

    case xplm_CommandEnd :
        XPLMSetWindowIsVisible(g_window,0);//I'm going to remove the small Window to unclutter cockpit, otherwise handlers get displayed
        //XPLMSetWindowPositioningMode(g_window,xplm_WindowPositionFree,0); useless now since 11.20b3, unless tests say otherwise
        XPLMDestroyWindow(g_window);
        g_window=NULL;
        counter=0;
        break;
    }
    return 0; //this one I've handled
}

/* NEXT TOOL : Filter VR stick commands with a button
 * button is choosen by user, best on a controller but not necessary
 * the button links to CommandFilter which sets up switches which are
 * used by the command blocker*/


bool SetupFiltering(){
    /* Reads output/settings/joystick preferences
     * Finds commands related to axis/commands
     * Builds a vector of these commands
     * assigns MyBlockFilterCommand to each of these commands
     * Builds a vector of states for each command (which will block or pass the command)
     */
    XPLMDebugString("VR_Tools plugin : setup Filtering commands\r\n");
    int nbcmd=0;
    std::ifstream jSettings("Output\\preferences\\X-Plane Joystick Settings.prf", std::ifstream::in);
    std::string lineRead;
    stringOps sto;
    while (getline(jSettings,lineRead)){
        if (sto.contains(lineRead,"AXIS_cmnd")){
            std::string eStr=sto.splitRight(lineRead," ");
            eStr=sto.cleanOut(eStr,"\r");
            eStr=sto.cleanOut(eStr,"\n");

            if (eStr!=""){//give this command, if it exists, a custom handler
               XPLMCommandRef cRef=XPLMFindCommand((char*)eStr.c_str());
               if (cRef!=NULL){
                void * nb=new(int*);
                (*(int*)nb)=nbcmd;
                XPLMRegisterCommandHandler(cRef,MyBlockFilterCommandHandler,1,nb);
                Commands.push_back(cRef);
                state.push_back(0);
                refCons.push_back(nb);
                //Will delete the pointers later
               nbcmd++;}
            }
       }
    }
    jSettings.close();
    if (nbcmd>0) return true; else return false;
}

void UndoFiltering(){
    //will unassign MyBlockCommandHandler for each command in the vector
    std::size_t lg=Commands.size();
    if (lg>0){
    for (int I(0);I<lg;I++){
        XPLMUnregisterCommandHandler(Commands[I],MyBlockFilterCommandHandler,1,refCons[I]);
        delete refCons[I];//I hope that is enough to avoid memory leaks
    }
    Commands.clear();
    state.clear();
    refCons.clear();
    filterEnabled=false;
    commandBlock=false;}

}

int MyFilterCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon){//activated when a button for custom command for filtering is pressed
    switch (inPhase)
    {
    case xplm_CommandBegin:
       {const int vr_is_enabled = XPLMGetDatai(g_vr_dref);
        g_in_vr = vr_is_enabled;
        if (filterEnabled & g_in_vr)//I only can block commands if in VR and if CustomCommandHandlers are set
            commandBlock=false; //commands are processed
        else {if (g_in_vr)
                   filterEnabled=SetupFiltering();
              else filterEnabled=false;}
              }//If the filter button has never been pressed, commands are processed normally. I only begin filtering when button is pressed first
        break;
    case xplm_CommandContinue: //do nothing
        break;
    case xplm_CommandEnd:
        if (filterEnabled)
             commandBlock=true; //selected commands are blocked since filter button is released
        else commandBlock=false;
        break;
    }
    return 0;//in every case its my command and i've handled it
}

int MyBlockFilterCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon){//Handels built-in commands selected by the filtering function
    switch (inPhase)
    {
    case xplm_CommandBegin: //at the start of the command I will evaluate if I have to block the concerned command
       { if (commandBlock){
            int refVal=*((int*)inRefcon);
            state[refVal]=0;
            return 0;}//block command here
        else{
            int refVal=*((int*)inRefcon);
            state[refVal]=1;
            return 1;//its up to X Plane or others to process command
        }}
        break;
    case xplm_CommandContinue:
     {int refVal=*((int*)inRefcon);
      return (state[refVal]);}//we've thought out before if we block or pass on the command
     break;
    case xplm_CommandEnd:
     {int refVal=*((int*)inRefcon);
      return (state[refVal]);} //same as before ...
     break;
    }
    return 1;//if nothing else go and do what you need X Plane !
}

/* And here the code for the text reader which is a custom command and a drawing function which uses
 * the textreader class to tailor the text file and iterate among the lines
 and a custom mouse handler*/

int  MyTextReaderCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon){
    switch (inPhase)
    {
    case xplm_CommandBegin : //opens or closes the textreader, creates or destroys the window
    {
        if (g_textWindow==NULL){//Toggle window in and out of existence
            MakeTextWindow();
        }
        else
        {   if (XPLMGetWindowIsVisible(g_textWindow)==0)
               {XPLMSetWindowIsVisible(g_textWindow,1);
                wLayout.CheckButtonsVisibility();}
            else{
                XPLMSetWindowIsVisible(g_textWindow,0);
                //an .ini option will tell if toggling means destroying window or hiding it
                   /* XPLMDestroyWindow(g_textWindow);
                    wLayout.ClosegWindow();
                    g_textWindow=NULL;*/}
        }
    }
        break;
    case xplm_CommandContinue :
        break;
    case xplm_CommandEnd :
        break;
        return 0;
    }
    return 0;//no one else needs to handle this
}

void drawText(XPLMWindowID in_window_id, void * in_refcon){
//intialize
    XPLMSetGraphicsState(
            0 /* no fog */,
            0 /* 0 texture units */,
            0 /* no lighting */,
            0 /* no alpha testing */,
            1 /* do alpha blend */,
            1 /* do depth testing */,
            0 /* no depth writing */
    );

    XPLMGetWindowGeometry(g_textWindow, &wLayout.l, &wLayout.t, &wLayout.r, &wLayout.b);
    wLayout.recalculate();

    float sunPtc = XPLMGetDataf(dref_SunPitch); //time of day ?

           //draw commands for the buttons
            for (int I(0);I<wLayout.nButtons;I++){
                wLayout.tButtons[I].drawButton();
            }

            //draw text rectangle and text in right side, different colors
            if (sunPtc>=5){//sun bright in sky
                wLayout.tFileReader->SetBckColor(colorPaper);
                wLayout.tFileReader->SetInkColor(colorInk);
            }else
            {if ((sunPtc>-3)&(sunPtc<5)){//sun at sunset darker duron paper white
                    wLayout.tFileReader->SetBckColor(paperWhitesomber);
                    wLayout.tFileReader->SetInkColor(colorInk);

                }
             else {wLayout.tFileReader->SetBckColor(blackpaper);//night paper
                    wLayout.tFileReader->SetInkColor(amberInk);}
            }
            wLayout.tFileReader->DrawMySelf();
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
        }else{
        wLayout.findClick(x,y);}

    break;}

    case xplm_MouseDrag: {
           wLayout.HandleMouseKeepDown(x,y);
    break;}

    case xplm_MouseUp: {
        int cmd=wLayout.HandleMouseUp(x,y);
        if (cmd==2){//load command has been pressed
        MakeFileWindow();
        }
    break;}

}return 1;//mouse events handled here
}



void MakeTextWindow(){
    if (g_textWindow==NULL){
    const int vr_is_enabled = XPLMGetDatai(g_vr_dref);
    g_in_vr = vr_is_enabled;
    XPLMGetScreenBoundsGlobal(&wLayout.gLeft, &wLayout.gTop, &wLayout.gRight, &wLayout.gBottom);
    if (wLayout.initiate()){
    XPLMCreateWindow_t params;
     params.structSize = sizeof(params);
     params.visible = 1;
     params.drawWindowFunc = drawText;
     params.handleMouseClickFunc = handle_mouse_for_TextW;
     params.handleRightClickFunc = dummy_mouse_handler;
     params.handleMouseWheelFunc = dummy_wheel_handler;
     params.handleKeyFunc = dummy_key_handler;
     params.handleCursorFunc = dummy_cursor_status_handler;
     params.refcon = NULL;
     params.layer = xplm_WindowLayerFloatingWindows;
     params.decorateAsFloatingWindow = xplm_WindowDecorationRoundRectangle;
     params.left = wLayout.wLeft;
     params.bottom = wLayout.wBottom-1;
     params.right = wLayout.wRight+2;
     params.top = wLayout.wTop+1;

    g_textWindow = XPLMCreateWindowEx(&params);
    XPLMSetWindowPositioningMode(g_textWindow, g_in_vr?xplm_WindowVR:xplm_WindowPositionFree, -1);
    XPLMSetWindowResizingLimits(g_textWindow, wLayout.minWidth, wLayout.minHeight, wLayout.maxWidth, wLayout.maxHeight);
    XPLMSetWindowTitle(g_textWindow, (char*)wLayout.GetFileName().c_str());
    wLayout.SetWindowHandler(g_textWindow);
    wLayout.CheckButtonsVisibility();}
    else writeDebug(("couldn't initiate the textfile, file not found"));
    }
}

void MakeFileWindow(){
    if (g_FileWindow==NULL){
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
        params.refcon = NULL;
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
       wLayout.ClosegWindow();
       g_textWindow=NULL;

    }
    else writeDebug("erronous call to MakeFileWindow");
}

void ReadNewFile(){

    std::string newTitle=dispDir->GetSelectedFile();
    std::string newFile=dispDir->GetSelectedPath();

    MakeTextWindow();

    XPLMSetWindowTitle(g_textWindow,(char*)newTitle.c_str());
    wLayout.SetNewFile(newFile);
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

             g_FileWindow=NULL;
             delete dispDir;
             dispDir=nullptr;
            }
            if (cmd==1 ){

                MakeTextWindow();
                XPLMDestroyWindow(g_FileWindow);

                g_FileWindow=NULL;
                delete dispDir;
                dispDir=nullptr;
            }
        break;}

    }return 1;//mouse events handled here
    }
}

