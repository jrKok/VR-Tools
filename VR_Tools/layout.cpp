#include <layout.h>
#include "temporarywindow.h"
#include "linedialog.h"

int Layout::cycle=0;

Layout::Layout(DrawLogic *newPad) :
   myDrawPad(newPad),
   //gTop(0),gLeft(0),gRight(0),gBottom(0),
  wTop(0),wBottom(0),wLeft(0),wRight(0),
  wWidth(0),wHeight(0),minWidth(0),maxWidth(0),minHeight(0),maxHeight(0),
  charHeight(0),charWidth(0),textPointX(0),textPointY(0),//Used for initial setup and resize instructions
  t(0),b(0),l(0),r(0),//input from draw instruction mostly
  textHeight(0),textWidth(0),colWidth(35),idxSelected(-1),nButtons(0),
  reloadPeriod(1.0f),
  generalR("layout's general",true),
  tFileReader(new TextReader()),
  tButtons(),
  myWindow(nullptr),
  fNav(),fCom(),fAdf(),lFPS(),
  charSep(""),
  epoch(0),
  openAtStart(false),goToLastPage(false),
  continueClick(false),buttonClick(false),autoReload(false),saveAuto(false),canUTF(true),autoReloadOnSize(true),
  useBackGround(true),
  flash(false),flashWhenChange(false),
  clickresult(-1),vrWidth(0),vrHeight(0),
  upperMargin(40),
  lowerMargin(54),
  dayPart(3),
  currentFPS(0)

//Constructor
{
    IniSettings::OrientFilePointer();
}

//Destructor

Layout::~Layout(){
    delete tFileReader;
    for (auto &bt:tButtons){
        delete bt;
        bt=nullptr;
    }
    tButtons.clear();

}

//Helpers

void Layout::WriteDebug(string message){
    string in_String="VR Tools : " +message+"\n";
    XPLMDebugString((char*)in_String.c_str());
}

void Layout::InitCycleDebug(int nbs){
    cycle=nbs;
}

void Layout::WriteDebugInCycles(string message){
    if (cycle>0){
        string in_String="VR Tools : " +message+"\n";
        XPLMDebugString((char*)in_String.c_str());
        cycle--;
    }
}

bool Layout::OpenWindowAtStart(){
    return openAtStart;
}
//Initialisers

void Layout::Begin(){
    myDrawPad->ToUpperLevel();

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
    g_FPS        = XPLMFindDataRef("sim/operation/misc/frame_rate_period");

    screenShot   = XPLMFindCommand("sim/operation/screenshot");
    XPLMGetFontDimensions(xplmFont_Proportional,nullptr,&charHeight,nullptr);
    minWidth=280+colWidth;//can vary if there are no buttons
    maxWidth=1510+colWidth;

    openAtStart=IniSettings::GetOptStart();

    textWidth=         IniSettings::GetWidth();
    textHeight=        IniSettings::GetHeight();
    autoReload=        IniSettings::GetOptReload();
    autoReloadOnSize= !IniSettings::GetOptReloadProc();
    useBackGround=    !IniSettings::GetOptBckG();
    reloadPeriod=      IniSettings::GetReloadPeriod();
    goToLastPage=      IniSettings::GetOptLastPg();
    flashWhenChange=   IniSettings::GetOptFlash();
    keepLastFile=      IniSettings::GetOptLastFile();
    canUTF=           !IniSettings::GetOptUTF();
    noResize=          IniSettings::GetOptFixed();
    keepSize=          IniSettings::GetOptKeepSize();
    fitSizeToFile=     IniSettings::GetOptFit();
    enableDelete=      IniSettings::GetOptDelete();
    enableFreqs=       IniSettings::GetOptFreqs();
    splitLinePolicy=   IniSettings::GetOptTrim();
    showFPS=           IniSettings::GetOptFPS();
    tFileReader->SetNeedsUTF(canUTF);
    IniSettings::OrientFilePointer();
    wHeight=lowerMargin+textHeight+upperMargin;
    wWidth=textWidth+colWidth+10;

}

bool Layout::initiate(){
    myDrawPad->ToUpperLevel();
    if (!nButtons){
        generalR.SetOrigin(0,0);
        if (noResize) {
            generalR.setVisibility(true);
            generalR.setColor(Clr_DarkGray);
            DrawLogic::SetBackGroundColor(Clr_DarkGray);
        }
        else{
            generalR.setVisibility(false);
            generalR.setColor(Clr_DarkGray);
            DrawLogic::SetBackGroundColor(Clr_BckgrdW);
        }
        tFileReader->PointToFile();
        if (resize()){
            if (showFPS) upperMargin=40 + charHeight+2;
            else upperMargin=40;
            wLeft=150;
            wRight=wLeft+wWidth;
            wBottom=50;
            wTop=wBottom+wHeight;
            defineButtons();
            nButtons=static_cast<int>(tButtons.size());
            if (goToLastPage) tFileReader->GoToLastPage();

            myDrawPad->SetNewSize(wWidth,wHeight);
            myDrawPad->GenerateCurrentTexture();
            return true;
        }else return false;
    }
    else{
        tFileReader->PointToFile();
        return resize();
    }

}

void Layout::HandleDialog(){

}

bool Layout::resize(){//calculate offsets; areas of rectangles}
    dayPart=3;
    tFileReader->SetSplitPolicy(splitLinePolicy);
    if (textHeight<150) textHeight=150;
    tFileReader->Setup(textHeight,textWidth,colWidth,lowerMargin);
    if (tFileReader->ReadFileToBuff())
    {
        if (fitSizeToFile){
            int hgt=(charHeight+2)*tFileReader->GetNumberOfLines();
            int wdth=tFileReader->GetMaxWidth()+25;//15 for the scroll box,10 for the text margins
            if (wdth<260) wdth=260;
            if (wdth>1500) wdth=1500;
            if (hgt<150) hgt=150;
            if (hgt>900) hgt=900;
            if (textHeight!=hgt||textWidth!=wdth){
                textHeight=hgt;
                textWidth=wdth;
                tFileReader->Setup(textHeight,textWidth,colWidth,lowerMargin);
                tFileReader->ReadFileToBuff();
                fitSizeToFile=false;}
            return true;
        }
        wWidth=textWidth+colWidth+10;
        wHeight=textHeight+upperMargin+lowerMargin;
        generalR.SetDimensions(wWidth,wHeight);
        vrWidth=wWidth;
        vrHeight=wHeight;
        myDrawPad->SetNewSize(wWidth,wHeight);
        if (XPLMWindowIsInVR(myWindow)==1){
            if (myWindow!=nullptr) XPLMSetWindowGeometryVR(myWindow,vrWidth,vrHeight);}
        else {
            if (myWindow!=nullptr) {
                XPLMGetWindowGeometry(myWindow,&l,&t,&r,&b);
                XPLMSetWindowGeometry(myWindow,l,t,l+vrWidth,t-vrHeight);
            }
        }
        minHeight=tFileReader->GetOffSetY()+180;
        maxHeight=tFileReader->GetOffSetY()+935;
        if (noResize){
            generalR.setVisibility(true);
            generalR.setColor(Clr_DarkGray);
        }
        else generalR.setVisibility(false);
        if (!useBackGround) tFileReader->SetBackGround(false);
        return true;
    }else{
        return false;
    }
}

//members, functions for running the class

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

    wWidth=textWidth+colWidth+10;
    wHeight=textHeight+tFileReader->GetOffSetY()+lowerMargin;
    generalR.SetDimensions(wWidth,wHeight);//offsetY upper margin and 54lower margin
    if (keepSize) KeepCurrentSize();
}

bool Layout::newSize(int wth,int hgt){//called by recalculate

    if (wth!=vrWidth||hgt!=vrHeight){
        //canUTF=true;
        int oldW(vrWidth),oldH(vrHeight);
        vrWidth=wth;vrHeight=hgt;
        int deltaW(vrWidth-oldW),deltaH(vrHeight-oldH);
        textWidth+=deltaW;
        textHeight+=deltaH;
        if (textWidth<260)  textWidth=260;
        if (textWidth>1500) textWidth=1500;
        if (textHeight<150) textHeight=150;
        if (textHeight>900) textHeight=900;
        resize();

        int middle=tFileReader->GetBottom()+(tFileReader->GetHeight()/2);
        RelocateButtons(middle);
        myDrawPad->GenerateCurrentTexture();
        return true;

 }
    return false;
}
void Layout::recalculate(float cT){ //gets called at every draw callback so this is used also to process other frequent checks

    bool newGeometry(false);
    if (!noResize){
       int wdt=r-l,hg=t-b;
       if (isWindowInVR()){
           XPLMGetWindowGeometryVR(myWindow,&wdt,&hg);}
        newGeometry=newSize(wdt,hg);
    }
    else{
        if (isWindowInVR()) XPLMSetWindowGeometryVR(myWindow,vrWidth,vrHeight);
        else XPLMSetWindowGeometry(myWindow,wLeft,wTop,wRight,wBottom);
    }

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

    if ((t!=wTop)|(l!=wLeft)|newGeometry){
        if (!useBackGround){
           tFileReader->SetInkColor(Clr_White);
           tFileReader->SetBackGround(false);}
}
}
void Layout::DrawTextW(XPLMWindowID g_textWindow){
    //intialize
    float curT=XPLMGetElapsedTime();
    XPLMGetWindowGeometry(g_textWindow, &l, &t, &r, &b);
    recalculate(curT);
    myDrawPad->SetScreenOrigin(l,b,r,t);

    if (showFPS){
        if ((curT-fpsTag)>0.5f) {
            fpsTag=curT;
            currentFPS=1/(XPLMGetDataf(g_FPS));
            string fps=std::to_string(currentFPS);
            fps=fps.substr(0,5);
            lFPS.setText("FPS : "+fps);
            lFPS.PrintString();
        }
    }

    float sunPtc = XPLMGetDataf(dref_SunPitch); //time of day ?
    int actualD(0);
    if ((sunPtc>-3)&&(sunPtc<5)) actualD=1;
    if (sunPtc<=-3) actualD=2;
    if (actualD!=dayPart)
    {
        dayPart=actualD;
        if (useBackGround){
            switch (dayPart){
            case 0:{
                tFileReader->SetBckColor(Clr_PaperWhite);
                tFileReader->SetInkColor(Clr_BlackInk);
                tFileReader->PrintMyText();
                tFileReader->DisplayPage();
                break;}
            case 1:{
                tFileReader->SetBckColor(Clr_PaperDusk);
                tFileReader->SetInkColor(Clr_BlackInk);
                tFileReader->PrintMyText();
                tFileReader->DisplayPage();
                break;}
            case 2:{
                tFileReader->SetBckColor(Clr_Black);//night paper
                tFileReader->SetInkColor(Clr_Amber);
                tFileReader->PrintMyText();
                tFileReader->DisplayPage();
                break;}
            }
        }
    }
    DrawLogic::DrawContent();
}

void Layout::DoFlash(){
    if (!flash){
    flash=true;
    beginFlash=XPLMGetElapsedTime();
    useBackGround=!useBackGround;
    if (!useBackGround){
       tFileReader->SetInkColor(Clr_PaperWhite);
       tFileReader->SetBackGround(false);}
    else{
        tFileReader->SetBackGround(true);
    }
    }
    if (flash){
        float current=XPLMGetElapsedTime();
        if ((current-beginFlash)>0.3f){
            flash=false;
            useBackGround=!useBackGround;
            if (!useBackGround){
               tFileReader->SetInkColor(Clr_PaperWhite);
               tFileReader->SetBackGround(false);}
            else{
                tFileReader->SetBackGround(true);
                tFileReader->SetInkColor(Clr_BlackInk);
            }
        }
    }
}
void Layout::ToggleFPS(){
    showFPS=!showFPS;
    if (showFPS)upperMargin+=charHeight+2;
    else upperMargin-=(charHeight+2);
    resize();
    myDrawPad->SetNewSize(wWidth,wHeight);
    myDrawPad->GenerateCurrentTexture();
}

void Layout::CheckButtonsVisibility(){
    tButtons[B_Save]->setVisibility(false);
    tButtons[B_Edit_Line]->setVisibility(true);
    tButtons[B_More_Lines]->setVisibility(false);
    tButtons[B_Less_Lines]->setVisibility(false);

    tButtons[B_Load_File]->setVisibility(true);
    tButtons[B_Reload]->setVisibility(!autoReload);
    tButtons[B_Auto]->setVisibility(true);
    tButtons[B_Undo]->setVisibility(tFileReader->CanUndo()&(!autoReload)&enableDelete);
    tButtons[B_UTF8]->setVisibility(!(tFileReader->HasSelection())&(!autoReload)&canUTF);
    tButtons[B_Toggle_Line]->setVisibility(tFileReader->HasSelection()&(!autoReload)&(enableDelete));
    tButtons[B_Show_All]->setVisibility(tFileReader->HasHiddenLine()&(!autoReload)&(enableDelete));

    tButtons[B_NAV1]->setVisibility(tFileReader->HasNav()&XPLMGetDatai(nav1on)&enableFreqs);
    tButtons[B_NAV2]->setVisibility(tFileReader->HasNav()&XPLMGetDatai(nav2on)&enableFreqs);
    tButtons[B_COM1]->setVisibility(tFileReader->HasCom()&XPLMGetDatai(com1on)&enableFreqs);
    tButtons[B_COM2]->setVisibility(tFileReader->HasCom()&XPLMGetDatai(com2on)&enableFreqs);
    tButtons[B_ADF1]->setVisibility(tFileReader->HasADF()&XPLMGetDatai(adf1on)&enableFreqs);
    tButtons[B_ADF2]->setVisibility(tFileReader->HasADF()&XPLMGetDatai(adf2on)&enableFreqs);

    if (tFileReader->HasNav() ){
        fNav.SetVisibility(true);
        fNav.PrintString();
    }
    if (tFileReader->HasCom()){
        fCom.SetVisibility(true);
        fCom.PrintString();
    }
    if(tFileReader->HasADF()){
        fAdf.SetVisibility(true);
        fAdf.PrintString();
    }
    if (showFPS){
        lFPS.SetVisibility(showFPS);
        lFPS.PrintString();
    }

}

int Layout::rightRectangle(int idx){
    ulong uidx=static_cast<ulong>(idx);
    return (tButtons[uidx]->GetRight());
}
int Layout::leftRectangle(int idx){
    ulong uidx=static_cast<ulong>(idx);
    return (tButtons[uidx]->GetLeft());
}
int Layout::topRectangle(int idx){
    ulong uidx=static_cast<ulong>(idx);
    return (tButtons[uidx]->GetTop());
}
int Layout::bottomRectangle(int idx){
    ulong uidx=static_cast<ulong>(idx);
    return (tButtons[uidx]->GetBottom());
}


//process a mouse or controller click

void Layout::findClick(int mX, int mY){
    int cX(mX-l),cY(mY-b);

    if (continueClick||buttonClick){
        if (clickresult>-1) tButtons[static_cast<ulong>(clickresult)]->Release();
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

    if (tFileReader->ProceedClick(cX,cY)){//Click in the text or scrB ?
        CheckButtonsVisibility();
        continueClick=true;

        return;
      }
    ulong uNB=static_cast<ulong>(nButtons);
    for (ulong I(0);I<uNB;I++){       //Click on a button ?
      if (tButtons[I]->isHere(cX,cY)){
           buttonClick=true;
           clickresult=static_cast<int>(I);
           tButtons[I]->Press();
           return;
           }
    }


}

void Layout::HandleMouseKeepDown(int mX,int mY){
        int cX(mX-l),cY(mY-b);
    //dayPart=3;
    if (continueClick) tFileReader->ProceedClickCont(cX,cY);
}

int Layout::HandleMouseUp(int,int){
    //dayPart=3;
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
        tButtons[static_cast<ulong>(clickresult)]->Release();
        continueClick=false;
        buttonClick=false;
        if (clickresult==B_Load_File){
            clickresult=-1;
            retVal=2;
            return retVal;
        }
        if (clickresult==B_Edit_Line){
            clickresult=-1;
            retVal=3;
            return retVal;
        }
        else LaunchCommand(clickresult);
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
                 tButtons[B_Auto]->setSelect(true);
                 epoch=XPLMGetElapsedTime();
             }else{
                 //tButtons[B_Auto].setText("auto Reload");
                 tButtons[B_Auto]->setSelect(false);
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
        WriteDebug("layout commandprocess color code for step 1 scrollBar"+std::to_string(tFileReader->GetScrollBarsColorCode()));
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
             int valInt=static_cast<int>(tFileReader->freqCOM);
             XPLMSetDatai(com1freq,valInt);
             valInt-=XPLMGetDatai(com1freq);
             if (valInt!=0){
                 frac=XPLMGetDatai(com1freqk);
                 frac+=valInt*10;
                 XPLMSetDatai(com1freqk,frac);
             }
         break;}
         case B_COM2:{
             int frac(0);
             int valInt=static_cast<int>(tFileReader->freqCOM);
             XPLMSetDatai(com2freq,valInt);
             valInt-=XPLMGetDatai(com2freq);
             if (valInt!=0){
                 frac=XPLMGetDatai(com2freqk);
                 frac+=valInt*10;
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
            WriteDebug("layout commandprocess color code for afterBV scrollBar"+std::to_string(tFileReader->GetScrollBarsColorCode()));
}

void Layout::ClosegWindow(){
    myWindow=nullptr;
    tFileReader->closeReader();
    tButtons.clear();
    nButtons=0;

    wTop=0;wBottom=0;wLeft=0;wRight=0;
    wWidth=0;wHeight=0;
    charWidth=0;
    textPointX=0;textPointY=0;
    t=0;b=0;l=0;r=0;
    colWidth=30;idxSelected=-1;nButtons=0;
    myDrawPad->FlushContent();
    //I keep charHeight,textHeight and textWidth,
}

void Layout::KeepFile(){
    IniSettings::SetFile();
    IniSettings::SetDir();
    IniSettings::WriteIniFile();
}

string Layout::GetFileName(){
    return FilePointer::GetCurrentFileName();
}

string Layout::GetDirName(){
    return FilePointer::GetCurrentDirName();
}

void Layout::SetWindowHandle(XPLMWindowID thisW){
    myWindow=thisW;
    XPLMGetWindowGeometry(myWindow, &l, &t, &r, &b);
    if (isWindowInVR())
    XPLMGetWindowGeometryVR(myWindow,&vrWidth,&vrHeight);
}

bool Layout::isWindowInVR(){
    return (XPLMWindowIsInVR(myWindow));
}


bool Layout::GetResizeOption(){
    return (noResize);
}

void Layout::KeepCurrentSize(){
    IniSettings::SetWidth(textWidth);
    IniSettings::SetHeight(textHeight);
}

void Layout::RelocateButtons(int middle){
    tButtons[B_Load_File]->  SetOrigin(-1,generalR.GetHeight()-20);
    tButtons[B_Reload]->     SetOrigin(-1,tButtons[B_Load_File]->GetBottom());
    tButtons[B_Auto]->       SetOrigin(-1,tButtons[B_Load_File]->GetBottom());
    tButtons[B_Edit_Line]->  SetOrigin(-1,tButtons[B_Load_File]->GetBottom());
    tButtons[B_Toggle_Line]->SetOrigin(-1,middle+13);
    tButtons[B_Show_All]->   SetOrigin(-1,middle-25);
    tButtons[B_Undo]->       SetOrigin(-1,middle-5);
    tButtons[B_NAV1]->       SetOrigin(colWidth+46,lowerMargin-17);
    tButtons[B_NAV2]->       SetOrigin(tButtons[B_NAV1]->GetLeft()+38,tButtons[B_NAV1]->GetBottom());
    tButtons[B_COM1]->       SetOrigin(tButtons[B_NAV1]->GetLeft(),tButtons[B_NAV1]->GetBottom()-18);
    tButtons[B_COM2]->       SetOrigin(tButtons[B_NAV2]->GetLeft(),tButtons[B_COM1]->GetBottom());
    tButtons[B_ADF1]->       SetOrigin(tButtons[B_NAV1]->GetLeft(),tButtons[B_COM1]->GetBottom()-18);
    tButtons[B_ADF2]->       SetOrigin(tButtons[B_NAV2]->GetLeft(),tButtons[B_ADF1]->GetBottom());
    tButtons[B_UTF8]->       SetOrigin(-1,-1);

    fNav.SetOrigin(fNav.GetLeft(),tButtons[B_NAV1]->GetBottom()+2);
    fCom.SetOrigin(fCom.GetLeft(),tButtons[B_COM1]->GetBottom()+2);
    fAdf.SetOrigin(fAdf.GetLeft(),tButtons[B_ADF1]->GetBottom()+2);
    if (keepSize) KeepCurrentSize();
    CheckButtonsVisibility();
}
void Layout::defineButtons(){
    int middle=lowerMargin+(textHeight/2);

//create 16 buttons
  /*  B_Load_File=0*/
    MakeButton(true,"Load",90,16,5,generalR.GetHeight()-20);

         //B_Page_Up=1;
    MakeButton(true,"ReLoad",90,16,100,tButtons[B_Load_File]->GetBottom());

         //B_Line_Up=2;
    MakeButton(true,"Auto ReLoad",90,16,195,tButtons[B_Load_File]->GetBottom());

         //B_Toggle_Line=3;
    MakeButton(true,"\xE2\x8C\xAB",20,15,5,middle+13);

         //B_UTF8=4;
    MakeButton(true,"UTF8",30,25,1,31);

        // B_Undo=5;
    MakeButton(true,"\xE2\x87\x9B",20,15,5,middle-5);

         //B_Edit_Line=6;
    MakeButton(true,"Edit",20,16,290,tButtons[B_Load_File]->GetBottom());

        // B_Save=7;
    MakeButton(false,"Save",20,16,colWidth+47,31);

         //B_More_Lines=8;
    MakeButton(true,"+Lines",20,15,colWidth+142,31);

        // B_Less_Lines=9;
    MakeButton(true,"-Lines",20,15,colWidth+190,31);

         //B_NAV1 = 10;
    MakeButton(true,"Nav1",35,15,colWidth+46,lowerMargin-17);

        // B_NAV2 = 11;
    MakeButton(true,"Nav2",35,15,tButtons[B_NAV1]->GetLeft()+38,tButtons[B_NAV1]->GetBottom());

         //B_COM1 = 12;
    MakeButton(true,"com1",35,15,colWidth+46,tButtons[B_NAV1]->GetBottom()-18);

        // B_COM2 = 13;
    MakeButton(true,"com2",35,15,tButtons[B_COM1]->GetLeft()+38,tButtons[B_COM1]->GetBottom());

        // B_ADF1 = 14;
    MakeButton(true,"Adf1",35,15,colWidth+46,tButtons[B_COM1]->GetBottom()-18);

        // B_ADF2 = 15
    MakeButton(true,"Adf2",35,15,tButtons[B_ADF1]->GetLeft()+38,tButtons[B_ADF1]->GetBottom());

    //B_Show_All=16
    MakeButton(true,"\xE2\x86\xBA",tButtons[B_Toggle_Line]->GetWidth(),17,5,middle-25);

    fNav.SetOrigin(colWidth,tButtons[B_NAV1]->GetTextY());
    fNav.SetDimensions(45,charHeight);
    fNav.SetBackGroundColor(Clr_BckgrdW);
    fNav.SetTextColor(Clr_Amber);

    fCom.SetOrigin(colWidth,tButtons[B_COM1]->GetTextY());
    fCom.SetDimensions(45,charHeight);
    fCom.SetBackGroundColor(Clr_BckgrdW);
    fCom.SetTextColor(Clr_Amber);

    fAdf.SetOrigin(colWidth,tButtons[B_ADF1]->GetTextY());
    fAdf.SetDimensions(45,charHeight);
    fAdf.SetBackGroundColor(Clr_BckgrdW);
    fAdf.SetTextColor(Clr_Amber);


    fpsTag=XPLMGetElapsedTime();
    lFPS.SetOrigin(14,tButtons[B_Load_File]->GetBottom()-2);
    lFPS.SetBackGroundColor(Clr_BckgrdW);
    lFPS.SetTextColor(Clr_Amber);

}

void Layout::MakeButton(bool visible,string in_Label,int width,int height,int oX, int oY){
    button_VR * newB(new button_VR(in_Label,false));
    newB->SetDimensions(width,height);
    newB->SetOrigin(oX,oY);
    newB->setText(in_Label);
    newB->SetToStateColor();
    newB->setVisibility(visible);
    tButtons.push_back(newB);
}

