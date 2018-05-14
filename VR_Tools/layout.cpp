#include <layout.h>


Layout::Layout() :
  lFPS(),
  iniParams(),
  charSep(""),
  gTop(0),gBottom(0),gLeft(0),gRight(0),//Used for initial calculations and coordinates,global coordinates
  wTop(0),wBottom(0),wLeft(0),wRight(0),wWidth(0),wHeight(0),minWidth(0),maxWidth(0),minHeight(0),maxHeight(0),
  charHeight(0),charWidth(0),textPointX(0),textPointY(0),//Used for initial setup and resize instructions
  t(0),b(0),l(0),r(0),//input from draw instruction mostly
  textHeight(0),textWidth(0),colWidth(35),idxSelected(-1),nButtons(0),
  reloadPeriod(1.0f),
  //tFileReader(new TextReader("log.txt")),
  tFileReader(new TextReader("Output\\textfiles\\flightnotes.txt")),
  generalR(),tButtons(),
  myWindow(NULL),
  epoch(0),
  openAtStart(false),goToLastPage(false),
  continueClick(false),buttonClick(false),autoReload(false),saveAuto(false),canUTF(true),autoReloadOnSize(true),
  useBackGround(true),clickresult(-1),vrWidth(0),vrHeight(0),
  upperMargin(20),
  flash(false),flashWhenChange(false),
  currentFPS(0),
  lightGray{0.820f,0.820f,0.820f},
  cyan{0.0f, 1.0f, 1.0f},
  green{0.0f,0.95f,0.0f},
  gray{0.375f, 0.375f, 0.368f},
  lightergray{0.725f,0.725f,0.770f},
  paperWhitesomber{0.9529f,0.7686f,0.4706f},
  // red {0.90f,0.0f,0.0f},
  darkgray{0.009f,0.009f,0.009f},
  blackpaper{0.020f,0.020f,0.020f},
  colorPaper{0.9922f,0.9608f,0.9098f}, //Duron paper white
  colorInk{0.005f,0.005f,0.005f},
  amberInk{1.0f,0.65f,0.0f}




//Constructor

{
    nav1on       = XPLMFindDataRef("sim/cockpit2/radios/actuators/nav1_power");
    nav2on       = XPLMFindDataRef("sim/cockpit2/radios/actuators/nav2_power");
    com1on       = XPLMFindDataRef("sim/cockpit2/radios/actuators/com1_power");
    com2on       = XPLMFindDataRef("sim/cockpit2/radios/actuators/com2_power");
    adf1on       = XPLMFindDataRef("sim/cockpit2/radios/actuators/adf1_power"); //returns an int 0 for Off 2 for ON
    adf2on       = XPLMFindDataRef("sim/cockpit2/radios/actuators/adf2_power");
    nav1freq     = XPLMFindDataRef("sim/cockpit2/radios/actuators/nav1_standby_frequency_hz");
    nav2freq     = XPLMFindDataRef("sim/cockpit2/radios/actuators/nav2_standby_frequency_hz");
    com1freq     = XPLMFindDataRef("sim/cockpit2/radios/actuators/com1_standby_frequency_hz");
    com2freq     = XPLMFindDataRef("sim/cockpit2/radios/actuators/com2_standby_frequency_hz");
    com1freqk    = XPLMFindDataRef("sim/cockpit2/radios/actuators/com1_standby_frequency_khz");
    com2freqk    = XPLMFindDataRef("sim/cockpit2/radios/actuators/com2_standby_frequency_khz");
    adf1freq     = XPLMFindDataRef("sim/cockpit2/radios/actuators/adf1_standby_frequency_hz");
    adf2freq     = XPLMFindDataRef("sim/cockpit2/radios/actuators/adf2_standby_frequency_hz");
    dref_SunPitch= XPLMFindDataRef("sim/graphics/scenery/sun_pitch_degrees");//float to estimate pos of sun
    XPLMGetFontDimensions(xplmFont_Proportional,NULL,&charHeight,NULL);
    minWidth=280+colWidth;//can vary if there are no buttons
    maxWidth=1510+colWidth;
    openAtStart=iniParams.GetOptStart();
    textWidth=iniParams.GetWidth();
    textHeight=iniParams.GetHeight();
    autoReload=iniParams.GetOptReload();
    autoReloadOnSize=!iniParams.GetOptReloadProc();
    useBackGround=!iniParams.GetOptBckG();
    reloadPeriod=iniParams.GetReloadPeriod();
    goToLastPage=iniParams.GetOptLastPg();
    flashWhenChange=iniParams.GetOptFlash();
    keepLastFile=iniParams.GetOptLastFile();
    canUTF=!iniParams.GetOptUTF();
    tFileReader->SetNeedsUTF(iniParams.GetOptUTF());
    noResize=iniParams.GetOptFixed();
    keepSize=iniParams.GetOptKeepSize();
    fitSizeToFile=iniParams.GetOptFit();
    enableDelete=iniParams.GetOptDelete();
    enableFreqs=iniParams.GetOptFreqs();
    splitLinePolicy=iniParams.GetOptTrim();
    showFPS=iniParams.GetOptFPS();
    g_FPS = XPLMFindDataRef("sim/operation/misc/frame_rate_period");
    charSep=XPLMGetDirectorySeparator();
    std::string fName=iniParams.GetFile();
    std::string dirName=iniParams.GetDir();
    WriteDebug ("default directory is now "+dirName);
    if (fName!=""){
    std::string fPath=dirName+((dirName=="")?"":charSep)+fName;
    std::experimental::filesystem::path pth(fPath);
    if (std::experimental::filesystem::is_regular_file(pth)) {
        WriteDebug("File referenced in .ini "+fPath+" is valid");
        tFileReader->OpenFile(fPath);}
    else{WriteDebug("In .ini file "+fPath+" isn't a valid Filepath");}
    }
}

//Destructor

Layout::~Layout(){
    delete tFileReader;
}

//Helpers

void Layout::WriteDebug(std::string message){
    std::string in_String="VR Tools : " +message+"\n";
    XPLMDebugString((char*)in_String.c_str());
}

bool Layout::OpenWindowAtStart(){
    return openAtStart;
}
//Initialisers

bool Layout::initiate(){
    resize();
    wLeft=gLeft+150;
    wRight=wLeft+wWidth;
    wTop=gTop-100;//moreover this will force a new recalculation at next draw call
    wBottom=wTop-wHeight;
    defineButtons();
    nButtons=tButtons.size();
    if (goToLastPage) tFileReader->GoToLastPage();
    //LaunchCommand(B_Auto);
    return true;
}

void Layout::resize(){//calculate offsets; areas of rectangles}

    upperMargin=charHeight+12;
    if (showFPS) upperMargin+=charHeight;
    tFileReader->SetSplitPolicy(splitLinePolicy);
    tFileReader->Setup(textHeight,textWidth,colWidth,upperMargin);
    tFileReader->ReadFileToBuff();
    if (fitSizeToFile){
        int hgt=(charHeight+2)*tFileReader->GetNumberOfLines();
        int wdth=tFileReader->GetMaxWidth()+25;//15 for the scroll box,10 for the text margins
        if (wdth<260) wdth=260;
        if (wdth>1500) wdth=1500;
        if (hgt<150) hgt=150;
        if (hgt>900) hgt=900;
        textHeight=hgt;
        textWidth=wdth;
        tFileReader->Setup(textHeight,textWidth,colWidth,upperMargin);
        tFileReader->ReadFileToBuff();
        fitSizeToFile=false;
        return;
    }
    generalR.width=textWidth+colWidth+10;//10 bx right margin
    generalR.height=textHeight+tFileReader->GetOffSetY()+54;//offsetY upper margin and 54lower margin
    wWidth=generalR.width;
    wHeight=generalR.height;
    vrWidth=wWidth;
    vrHeight=wHeight;
    if (myWindow!=NULL) XPLMSetWindowGeometryVR(myWindow,vrWidth,vrHeight);
    generalR.offsetX=0;
    generalR.offsetY=0;
    minHeight=tFileReader->GetOffSetY()+180;
    maxHeight=tFileReader->GetOffSetY()+935;
}

void Layout::newSize(){
 int oldW=vrWidth,oldH=vrHeight;
 XPLMGetWindowGeometryVR(myWindow,&vrWidth,&vrHeight);
 if ((oldW!=vrWidth)|(oldH!=vrHeight)){
     //canUTF=true;
     int deltaW(0),deltaH(0);
     deltaW=vrWidth-oldW;
     deltaH=vrHeight-oldH;
     textWidth=textWidth+deltaW;
     textHeight=textHeight+deltaH;
     if (textWidth<260) textWidth=260;
     if (textWidth>1500) textWidth=1500;
     if (textHeight<150) textHeight=150;
     if (textHeight>900) textHeight=900;
     resize();
     //XPLMSetWindowGeometryVR(myWindow,generalR.width,generalR.height);
     int middle=tFileReader->GetOffSetY()+(int)(tFileReader->GetHeight()/2);
     tButtons[B_Toggle_Line].offsetY=middle-13;
     tButtons[B_Toggle_Line].setText("\xE2\x8C\xAB");
     tButtons[B_Show_All].offsetY=middle+25;
     tButtons[B_Show_All].setText("\xE2\x86\xBA");
     tButtons[B_Undo].offsetY=middle+5;
     tButtons[B_Undo].setText("\xE2\x87\x9B");
     tButtons[B_NAV1].offsetY=generalR.height-52;
     tButtons[B_NAV1].setText("Nav1");
     tButtons[B_NAV2].offsetY=generalR.height-52;
     tButtons[B_NAV2].setText("Nav2");
     tButtons[B_COM1].offsetY=generalR.height-35;
     tButtons[B_COM1].setText("com1");
     tButtons[B_COM2].offsetY=generalR.height-35;
     tButtons[B_COM2].setText("com2");
     tButtons[B_ADF1].offsetY=generalR.height-18;
     tButtons[B_ADF1].setText("Adf1");
     tButtons[B_ADF2].offsetY=generalR.height-18;
     tButtons[B_ADF2].setText("Adf2");
     tButtons[B_UTF8].offsetY=generalR.height-31;
     tButtons[B_UTF8].setText("UTF8");

     fNav.offSetY=tButtons[B_NAV1].offsetTextY;
     fCom.offSetY=tButtons[B_COM1].offsetTextY;
     fAdf.offSetY=tButtons[B_ADF1].offsetTextY;
     if (keepSize) KeepCurrentSize();
     CheckButtonsVisibility();
 }
}

void Layout::resizeVRWindow(){
int wd,hg;
XPLMGetWindowGeometryVR(myWindow,&wd,&hg);
if (myWindow!=NULL){
if (wd<minWidth) wd=minWidth;
if (wd>=maxWidth) wd=maxWidth;
if (hg<minHeight) hg=minHeight;
if (hg>maxHeight) hg=maxHeight;
XPLMSetWindowGeometryVR(myWindow,wd,hg);}
}

void Layout::FitToFile(){
    int hgt=(charHeight+2)*tFileReader->GetNumberOfLines();
    int wdth=tFileReader->GetMaxWidth()+25;//15 for the scroll box,10 for the text margins
    if (wdth<260) wdth=260;
    if (wdth>1500) wdth=1500;
    if (hgt<150) hgt=150;
    if (hgt>900) hgt=900;
    textHeight=hgt;
    textWidth=wdth;
    tFileReader->Setup(textHeight,textWidth,colWidth,upperMargin);
    tFileReader->ReadFileToBuff();
    generalR.width=textWidth+colWidth+10;//10 bx right margin
    generalR.height=textHeight+tFileReader->GetOffSetY()+54;//offsetY upper margin and 54lower margin
    wWidth=generalR.width;
    wHeight=generalR.height;
    if (keepSize) KeepCurrentSize();
}

void Layout::defineButtons(){
    int middle=tFileReader->GetOffSetY()+(int)(tFileReader->GetHeight()/2);
    button_VR emptyBn;
    for (int box(0);box <17;box++){tButtons.push_back(emptyBn);}//create 16 buttons
  /*  B_Load_File=0*/
    tButtons[B_Load_File].isVisible=true;
    tButtons[B_Load_File].height=16;
    tButtons[B_Load_File].width=90;
    tButtons[B_Load_File].offsetX=5;
    tButtons[B_Load_File].offsetY=tFileReader->GetOffSetY()-18;
    tButtons[B_Load_File].setText("Load");

         //B_Page_Up=1;
    tButtons[B_Reload].isVisible=true;
    tButtons[B_Reload].height=16;
    tButtons[B_Reload].width=90;
    tButtons[B_Reload].offsetX=100;
    tButtons[B_Reload].offsetY=tButtons[B_Load_File].offsetY;
    tButtons[B_Reload].setText("Reload");

         //B_Line_Up=2;
    tButtons[B_Auto].isVisible=true;
    tButtons[B_Auto].height=16;
    tButtons[B_Auto].width=90;
    tButtons[B_Auto].offsetX=195;
    tButtons[B_Auto].offsetY=tButtons[B_Load_File].offsetY;
    tButtons[B_Auto].setText("Auto Reload");

         //B_Toggle_Line=3;
    tButtons[B_Toggle_Line].isVisible=true;
    tButtons[B_Toggle_Line].height=15;
    tButtons[B_Toggle_Line].width=20;
    tButtons[B_Toggle_Line].offsetX=5;
    tButtons[B_Toggle_Line].offsetY=middle-13;
    tButtons[B_Toggle_Line].setText("\xE2\x8C\xAB");//\xE2\x8C\xAB

         //B_UTF8=4;
    tButtons[B_UTF8].isVisible=true;
    tButtons[B_UTF8].height=25;
    tButtons[B_UTF8].width=30;
    tButtons[B_UTF8].offsetX=1;
    tButtons[B_UTF8].offsetY=generalR.height-31;
    tButtons[B_UTF8].setText("UTF8");

        // B_Undo=5;
    tButtons[B_Undo].isVisible=false;
    tButtons[B_Undo].height=15;
    tButtons[B_Undo].width=20;
    tButtons[B_Undo].offsetX=5;
    tButtons[B_Undo].offsetY=middle+5;
    tButtons[B_Undo].setText("\xE2\x87\x9B");//\u2C4 bas\CB85

         //B_Edit_Line=6;
    tButtons[B_Edit_Line].isVisible=false;
    tButtons[B_Edit_Line].height=15;
    tButtons[B_Edit_Line].width=20;
    tButtons[B_Edit_Line].offsetX=colWidth+5;
    tButtons[B_Edit_Line].offsetY=generalR.height-31;
    tButtons[B_Edit_Line].setText("Edit");//\u2C4 bas\CB85

        // B_Save=7;
    tButtons[B_Save].isVisible=false;
    tButtons[B_Save].height=15;
    tButtons[B_Save].width=20;
    tButtons[B_Save].offsetX=colWidth+47;
    tButtons[B_Save].offsetY=generalR.height-31;
    tButtons[B_Save].setText("Save");//\u2C4 bas\CB85

         //B_More_Lines=8;
    tButtons[B_More_Lines].isVisible=true;
    tButtons[B_More_Lines].height=15;
    tButtons[B_More_Lines].width=20;
    tButtons[B_More_Lines].offsetX=colWidth+142;
    tButtons[B_More_Lines].offsetY=generalR.height-31;
    tButtons[B_More_Lines].setText("+Lines");//\u2C4 bas\CB85

        // B_Less_Lines=9;
    tButtons[B_Less_Lines].isVisible=true;
    tButtons[B_Less_Lines].height=15;
    tButtons[B_Less_Lines].width=20;
    tButtons[B_Less_Lines].offsetX=colWidth+190;
    tButtons[B_Less_Lines].offsetY=generalR.height-31;
    tButtons[B_Less_Lines].setText("-Lines");//\u2C4 bas\CB85

         //B_NAV1 = 10;
    tButtons[B_NAV1].isVisible=true;
    tButtons[B_NAV1].height=15;
    tButtons[B_NAV1].width=35;
    tButtons[B_NAV1].offsetX=colWidth+46;
    tButtons[B_NAV1].offsetY=generalR.height-52;
    tButtons[B_NAV1].setText("Nav1");//\u2C4 bas\CB85

        // B_NAV2 = 11;
    tButtons[B_NAV2].isVisible=true;
    tButtons[B_NAV2].height=15;
    tButtons[B_NAV2].width=35;
    tButtons[B_NAV2].offsetX=tButtons[B_NAV1].offsetX+38;
    tButtons[B_NAV2].offsetY=tButtons[B_NAV1].offsetY;
    tButtons[B_NAV2].setText("Nav2");//\u2C4 bas\CB85

         //B_COM1 = 12;
    tButtons[B_COM1].isVisible=true;
    tButtons[B_COM1].height=15;
    tButtons[B_COM1].width=35;
    tButtons[B_COM1].offsetX=colWidth+46;
    tButtons[B_COM1].offsetY=generalR.height-35;
    tButtons[B_COM1].setText("com1");

        // B_COM2 = 13;
    tButtons[B_COM2].isVisible=true;
    tButtons[B_COM2].height=15;
    tButtons[B_COM2].width=35;
    tButtons[B_COM2].offsetX=tButtons[B_COM1].offsetX+38;
    tButtons[B_COM2].offsetY=tButtons[B_COM1].offsetY;
    tButtons[B_COM2].setText("com2");

        // B_ADF1 = 14;
    tButtons[B_ADF1].isVisible=true;
    tButtons[B_ADF1].height=15;
    tButtons[B_ADF1].width=35;
    tButtons[B_ADF1].offsetX=colWidth+46;
    tButtons[B_ADF1].offsetY=generalR.height-18;
    tButtons[B_ADF1].setText("Adf1");

        // B_ADF2 = 15*/
    tButtons[B_ADF2].isVisible=true;
    tButtons[B_ADF2].height=15;
    tButtons[B_ADF2].width=35;
    tButtons[B_ADF2].offsetX=tButtons[B_ADF1].offsetX+38;
    tButtons[B_ADF2].offsetY=tButtons[B_ADF1].offsetY;
    tButtons[B_ADF2].setText("Adf2");

    //B_Show_All=16
    tButtons[B_Show_All].isVisible=true;
    tButtons[B_Show_All].height=17;
    tButtons[B_Show_All].width=tButtons[B_Toggle_Line].width;
    tButtons[B_Show_All].offsetX=5;
    tButtons[B_Show_All].offsetY=middle+25;
    tButtons[B_Show_All].setText("\xE2\x86\xBA");

    fNav.offSetX=colWidth;
    fNav.offSetY=tButtons[B_NAV1].offsetTextY;
    fNav.SetTextColor(lightGray);
    fNav.height=charHeight;
    fNav.width=45;

    fCom.offSetX=colWidth;
    fCom.offSetY=tButtons[B_COM1].offsetTextY;
    fCom.height=charHeight;
    fCom.SetTextColor(lightGray);
    fCom.width=45;

    fAdf.offSetX=colWidth;
    fAdf.offSetY=tButtons[B_ADF1].offsetTextY;
    fAdf.height=charHeight;
    fAdf.SetTextColor(lightGray);
    fAdf.width=45;


    fpsTag=XPLMGetElapsedTime();
    lFPS.offSetX=14;
    lFPS.offSetY=tButtons[B_Load_File].offsetY-2;
    lFPS.SetTextColor(amberInk);

}

//members, functions for running the class

void Layout::recalculate(float cT){ //gets called at every draw callback so used to process other frequent checks
    if (!noResize) newSize();

    if (autoReload){
        if (flash) DoFlash();
        if (autoReloadOnSize){
            if (tFileReader->ReloadIfSizeChanged()&&flashWhenChange) {
                DoFlash();}
            else tFileReader->ReloadIfSizeChanged();
        }else{
           if ((cT-epoch)>=reloadPeriod) {
               epoch=cT;
               bool rlld=tFileReader->Reload();
               if (rlld&&flashWhenChange) {DoFlash();}
           }
        }
    }

    if ((t!=wTop)|(l!=wLeft)){
        if (!useBackGround){
           tFileReader->SetInkColor(colorPaper);
           tFileReader->SetBackGround(false);}
        wTop=t;
        wLeft=l;
        generalR.in_left=l;
        generalR.in_top=t;
        generalR.recalculate();
        tFileReader->Recalculate(l,t);
        fNav.recalculate(l,t);
        fCom.recalculate(l,t);
        fAdf.recalculate(l,t);
        lFPS.recalculate(l,t);
        for (int I(0);I<nButtons;I++){
            tButtons[I].in_top=t;
            tButtons[I].in_left=l;
            tButtons[I].recalculate();
        }
}
}

void Layout::CheckButtonsVisibility(){
    tButtons[B_Save].isVisible       =false;
    tButtons[B_Edit_Line].isVisible  =false;
    tButtons[B_More_Lines].isVisible =false;
    tButtons[B_Less_Lines].isVisible =false;

    tButtons[B_Load_File].isVisible  =true;
    tButtons[B_Reload].isVisible     =!autoReload;
    tButtons[B_Auto].isVisible       =true;
    tButtons[B_Undo].isVisible       =tFileReader->CanUndo()&(!autoReload)&enableDelete;
    tButtons[B_UTF8].isVisible       =!(tFileReader->HasSelection())&(!autoReload)&canUTF;
    tButtons[B_Toggle_Line].isVisible=tFileReader->HasSelection()&(!autoReload)&(enableDelete);
    tButtons[B_Show_All].isVisible   =tFileReader->HasHiddenLine()&(!autoReload)&(enableDelete);

    tButtons[B_NAV1].isVisible       =tFileReader->HasNav()&XPLMGetDatai(nav1on)&enableFreqs;
    tButtons[B_NAV2].isVisible       =tFileReader->HasNav()&XPLMGetDatai(nav2on)&enableFreqs;
    tButtons[B_COM1].isVisible       =tFileReader->HasCom()&XPLMGetDatai(com1on)&enableFreqs;
    tButtons[B_COM2].isVisible       =tFileReader->HasCom()&XPLMGetDatai(com2on)&enableFreqs;
    tButtons[B_ADF1].isVisible       =tFileReader->HasADF()&XPLMGetDatai(adf1on)&enableFreqs;
    tButtons[B_ADF2].isVisible       =tFileReader->HasADF()&XPLMGetDatai(adf2on)&enableFreqs;

}

int Layout::rightRectangle(int idx){
    return (tButtons[idx].right);
}
int Layout::leftRectangle(int idx){
    return (tButtons[idx].left);
}
int Layout::topRectangle(int idx){
    return (tButtons[idx].top);
}
int Layout::bottomRectangle(int idx){
    return (tButtons[idx].bottom);
}


//process a mouse or controller click

void Layout::findClick(int mX, int mY){
    if (continueClick||buttonClick){
        if (clickresult>-1) tButtons[clickresult].Release();
        if (continueClick) tFileReader->ProceedEndClick();
        autoReload=saveAuto;
        continueClick=false;
        buttonClick=false;
        clickresult=-1;
        return;//if previous mouse click had been released outside window
        //discharge all pending mousedown and drag handlers and discard this click
    }
    saveAuto=autoReload;
    autoReload=false;
    if (tFileReader->ProceedClick(mX,mY)){//Click in the text or scrB ?
        CheckButtonsVisibility();
        continueClick=true;
        return;
      }

    for (int I(0);I<nButtons;I++){       //Click on a button ?
      if (tButtons[I].isHere(mX,mY)){
           buttonClick=true;
           clickresult=I;
           tButtons[I].Press();
           return;
           }
    }

}

void Layout::HandleMouseKeepDown(int mX,int mY){
    if (continueClick) tFileReader->ProceedClickCont(mX,mY);
}

int Layout::HandleMouseUp(int mX,int mY){
    autoReload=saveAuto;
    epoch=XPLMGetElapsedTime();
    int retVal=-1;
    if (continueClick){
        continueClick=false;
        buttonClick=false;
        clickresult=-1;
        tFileReader->ProceedEndClick();
        tFileReader->CheckForFrequencies();
        fNav.setText(tFileReader->GetNavStr());
        fCom.setText(tFileReader->GetComStr());
        fAdf.setText(tFileReader->GetADFStr());
        CheckButtonsVisibility();}

    if (buttonClick){
        tButtons[clickresult].Release();
        if (clickresult==B_Load_File) retVal=2;
        else LaunchCommand(clickresult);
        continueClick=false;
        buttonClick=false;
        clickresult=-1;
    }
    return retVal;
}
//The following can also be used by the plugin handlers to launch a custom command
//to implement functionnality by pressing physical commands, keys, menus ...
//enumeration to be found in show_fps_test_global.h

void Layout::LaunchCommand(int refCommand){
    switch(refCommand){
         case B_Reload:{
             tFileReader->Reload();
             //canUTF=true;
         break;}

         case B_Undo:{
             tFileReader->UndoDelete();
         break;}

         case B_Auto:{
             autoReload=!autoReload;
             if (autoReload) {
                 //tButtons[B_Auto].setText("manual Reload");
                 tButtons[B_Auto].setSelect(true);
                 epoch=XPLMGetElapsedTime();
             }else{
                 //tButtons[B_Auto].setText("auto Reload");
                 tButtons[B_Auto].setSelect(false);
             }
         break;}

         case B_UTF8:{
             tFileReader->SetNeedsUTF(true);
             canUTF=false;
         break;}

         case B_Toggle_Line:{
             if (enableDelete)
             tFileReader->DeleteSelectedLine();
         break;}

         case B_Show_All:{
             tFileReader->ShowAll();
             tFileReader->SetupforText();
         break;}

         case B_NAV1:{
             XPLMSetDatai(nav1freq,tFileReader->freqNAV);
         break;}

         case B_NAV2:{
             XPLMSetDatai(nav2freq,tFileReader->freqNAV);
         break;}

         case B_COM1:{
             int frac(0);
             float val=tFileReader->freqCOM;
             XPLMSetDatai(com1freq,(int)val);
             val-=(float)XPLMGetDatai(com1freq);
             if (val!=0){
                 frac=XPLMGetDatai(com1freqk);
                 frac+=(int)(val*10.0);
                 XPLMSetDatai(com1freqk,frac);
             }
         break;}
         case B_COM2:{
             int frac(0);
             float val=tFileReader->freqCOM;
             XPLMSetDatai(com2freq,(int)val);
             val-=(float)XPLMGetDatai(com2freq);
             if (val!=0){
                 frac=XPLMGetDatai(com2freqk);
                 frac+=(int)(val*10.0);
                 XPLMSetDatai(com2freqk,frac);
             }
         break;}
         case B_ADF1:{
             XPLMSetDatai(adf1freq,tFileReader->freqADF);
         break;}
         case B_ADF2:{
             XPLMSetDatai(adf2freq,tFileReader->freqADF);
         break;}
         case B_NextLine:{
             if (tFileReader->HasSelection()){
                int slL=tFileReader->SelectedLineNumber();
                tFileReader->MoveDnNLines(1);
                tFileReader->SelectLine(slL+1);
        }
         break;}
         case B_PrevLine:{
             if (tFileReader->HasSelection()){
                int slL=tFileReader->SelectedLineNumber();
                tFileReader->MoveUpNLines(1);
                tFileReader->SelectLine(slL-1);
             }
         break;}
         case B_FirstLine:{
             tFileReader->SelectFirstLine();
         break;  }
         }

    CheckButtonsVisibility();
}

void Layout::ClosegWindow(){
    myWindow=NULL;
    tFileReader->closeReader();
    tButtons.clear();
    gTop=0;gBottom=0;gLeft=0;gRight=0;
    wTop=0;wBottom=0;wLeft=0;wRight=0;
    wWidth=0;wHeight=0;
    charWidth=0;
    textPointX=0;textPointY=0;
    t=0;b=0;l=0;r=0;
    colWidth=30;idxSelected=-1;nButtons=0;
    //I keep charHeight,textHeight and textWidth,
}

void Layout::SetNewFile(std::string newFilePath){
    canUTF=true;
    tFileReader->SetNeedsUTF(false);
    tFileReader->OpenFile(newFilePath);
    tFileReader->ShowAll();
    if (keepLastFile) KeepFile();
}

void Layout::KeepFile(){
    iniParams.SetFile(tFileReader->GetFileName());
    iniParams.SetDir(tFileReader->GetDirName());
    iniParams.WriteIniFile();
}

std::string Layout::GetFileName(){
    return tFileReader->GetFileName();
}

std::string Layout::GetDirName(){
    std::string iniDir= iniParams.GetDir();
    std::experimental::filesystem::path pth(iniDir);
    if (std::experimental::filesystem::is_directory(pth)) return iniDir;
    else return "";
}

void Layout::SetWindowHandle(XPLMWindowID thisW){
    myWindow=thisW;
    XPLMGetWindowGeometryVR(myWindow,&vrWidth,&vrHeight);
}

bool Layout::isWindowInVR(){
    return (XPLMWindowIsInVR(myWindow));
}

void Layout::DrawTextW(XPLMWindowID g_textWindow){
    //intialize
    float curT=XPLMGetElapsedTime();
    XPLMSetGraphicsState(
        0 /* no fog */,
        0 /* 0 texture units */,
        0 /* no lighting */,
        0 /* no alpha testing */,
        1 /* do alpha blend */,
        1 /* do depth testing */,
        0 /* no depth writing */
        );

    XPLMGetWindowGeometry(g_textWindow, &l, &t, &r, &b);
    recalculate(curT);

    if (showFPS){
        if ((curT-fpsTag)>0.5) {
            fpsTag=curT;
            currentFPS=1/(XPLMGetDataf(g_FPS));
            std::string fps=std::to_string(currentFPS);
            fps=fps.substr(0,5);
            lFPS.setText("FPS : "+fps);
        }
        lFPS.DrawTextLine();
    }

    float sunPtc = XPLMGetDataf(dref_SunPitch); //time of day ?

     //draw commands for the buttons
    for (auto tb:tButtons){tb.drawButton();}
     //draw text's background and set color of characters (ink)
    if (useBackGround){
     if (sunPtc>=5){//sun bright in sky
        tFileReader->SetBckColor(colorPaper);
        tFileReader->SetInkColor(colorInk);
     }else{
          if ((sunPtc>-3)&(sunPtc<5)){//sun at sunset darker duron paper white
             tFileReader->SetBckColor(paperWhitesomber);
             tFileReader->SetInkColor(colorInk);
                                       }
          else {
             tFileReader->SetBckColor(blackpaper);//night paper
             tFileReader->SetInkColor(amberInk);}
               }
       }

    tFileReader->DrawMySelf(); //Draw Text and background if necessary

    if (tFileReader->HasNav()) fNav.DrawTextLine();
    if (tFileReader->HasCom()) fCom.DrawTextLine();
    if (tFileReader->HasADF()) fAdf.DrawTextLine();
}


void Layout::DrawNoResize(XPLMWindowID g_textWindow){
    float curT=XPLMGetElapsedTime();
    XPLMSetGraphicsState(
        0 /* no fog */,
        0 /* 0 texture units */,
        0 /* no lighting */,
        0 /* no alpha testing */,
        1 /* do alpha blend */,
        1 /* do depth testing */,
        0 /* no depth writing */
        );
    XPLMGetWindowGeometry(g_textWindow, &l, &t, &r, &b);
    recalculate(curT);

    if (showFPS){
        if ((curT-fpsTag)>0.5) {
            fpsTag=curT;
            std::string fps=std::to_string(currentFPS);
            fps=fps.substr(0,5);
            lFPS.setText("FPS : "+fps);
        }
        lFPS.DrawTextLine();
    }

    glColor3fv(darkgray);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glBegin(GL_TRIANGLE_FAN);
   {
     glVertex2i(generalR.left, generalR.top);
     glVertex2i(generalR.right, generalR.top);
     glVertex2i(generalR.right, generalR.bottom);
     glVertex2i(generalR.left, generalR.bottom);
   }
   glEnd();
    float sunPtc = XPLMGetDataf(dref_SunPitch); //time of day ?

     //draw commands for the buttons
    for (auto tb:tButtons){tb.drawButton();}
     //draw text's background and set color of characters (ink)
    if (useBackGround){
     if (sunPtc>=5){//sun bright in sky
        tFileReader->SetBckColor(colorPaper);
        tFileReader->SetInkColor(colorInk);
     }else{
          if ((sunPtc>-3)&(sunPtc<5)){//sun at sunset darker duron paper white
             tFileReader->SetBckColor(paperWhitesomber);
             tFileReader->SetInkColor(colorInk);
                                       }
          else {
             tFileReader->SetBckColor(blackpaper);//night paper
             tFileReader->SetInkColor(amberInk);}
               }
       }

    tFileReader->DrawMySelf(); //Draw Text and background if necessary

    if (tFileReader->HasNav()) fNav.DrawTextLine();
    if (tFileReader->HasCom()) fCom.DrawTextLine();
    if (tFileReader->HasADF()) fAdf.DrawTextLine();
}

void Layout::DoFlash(){
    if (!flash){
    flash=true;
    beginFlash=XPLMGetElapsedTime();
    useBackGround=!useBackGround;
    if (!useBackGround){
       tFileReader->SetInkColor(colorPaper);
       tFileReader->SetBackGround(false);}
    else{
        tFileReader->SetBackGround(true);
    }
    }
    if (flash){
        float current=XPLMGetElapsedTime();
        if ((current-beginFlash)>0.3){
            flash=false;
            useBackGround=!useBackGround;
            if (!useBackGround){
               tFileReader->SetInkColor(colorPaper);
               tFileReader->SetBackGround(false);}
            else{ tFileReader->SetBackGround(true);}
        }
    }
}
void Layout::ToggleFPS(){
    showFPS=!showFPS;
    resize();
}

bool Layout::GetResizeOption(){
    return (noResize);
}

void Layout::KeepCurrentSize(){
    iniParams.SetWidth(textWidth);
    iniParams.SetHeight(textHeight);
}
