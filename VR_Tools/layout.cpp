#include <layout.h>
#include "drawlogic.h"
#include "temporarywindow.h"
#include "linedialog.h"

int Layout::cycle=0;

Layout::Layout(int windowNb) :
  myWindowNumber(windowNb),
  gTop(0),gLeft(0),gRight(0),gBottom(0),
  wTop(0),wBottom(0),wLeft(0),wRight(0),wWidth(0),wHeight(0),minWidth(0),maxWidth(0),minHeight(0),maxHeight(0),
  charHeight(0),charWidth(0),textPointX(0),textPointY(0),//Used for initial setup and resize instructions
  t(0),b(0),l(0),r(0),//input from draw instruction mostly
  textHeight(0),textWidth(0),colWidth(35),idxSelected(-1),nButtons(0),
  reloadPeriod(1.0f),
  tFileReader(new TextReader()),
  generalR(true),
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
  upperMargin(20),
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
}

//Helpers

void Layout::WriteDebug(std::string message){
    std::string in_String="VR Tools : " +message+"\n";
    XPLMDebugString((char*)in_String.c_str());
}

void Layout::InitCycleDebug(int nbs){
    cycle=nbs;
}

void Layout::WriteDebugInCycles(std::string message){
    if (cycle>0){
        std::string in_String="VR Tools : " +message+"\n";
        XPLMDebugString((char*)in_String.c_str());
        cycle--;
    }
}

bool Layout::OpenWindowAtStart(){
    return openAtStart;
}
//Initialisers

void Layout::Begin(){
    if (DrawLogic::GetActiveWindowNumber()!=myWindowNumber)
       DrawLogic::ShowMyWindow(myWindowNumber);
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
    screenShot   = XPLMFindCommand("sim/operation/screenshot");
    XPLMGetFontDimensions(xplmFont_Proportional,nullptr,&charHeight,nullptr);
    minWidth=280+colWidth;//can vary if there are no buttons
    maxWidth=1510+colWidth;

    openAtStart=IniSettings::GetOptStart();
    textWidth=IniSettings::GetWidth();
    textHeight=IniSettings::GetHeight();
    autoReload=IniSettings::GetOptReload();
    autoReloadOnSize=!IniSettings::GetOptReloadProc();
    useBackGround=!IniSettings::GetOptBckG();
    reloadPeriod=IniSettings::GetReloadPeriod();
    goToLastPage=IniSettings::GetOptLastPg();
    flashWhenChange=IniSettings::GetOptFlash();
    keepLastFile=IniSettings::GetOptLastFile();
    canUTF=!IniSettings::GetOptUTF();
    tFileReader->SetNeedsUTF(IniSettings::GetOptUTF());
    noResize=IniSettings::GetOptFixed();
    keepSize=IniSettings::GetOptKeepSize();
    fitSizeToFile=IniSettings::GetOptFit();
    enableDelete=IniSettings::GetOptDelete();
    enableFreqs=IniSettings::GetOptFreqs();
    splitLinePolicy=IniSettings::GetOptTrim();
    showFPS=IniSettings::GetOptFPS();
    g_FPS = XPLMFindDataRef("sim/operation/misc/frame_rate_period");
    IniSettings::OrientFilePointer();
}

bool Layout::initiate(){
    if (DrawLogic::GetActiveWindowNumber()!=myWindowNumber)
       DrawLogic::ShowMyWindow(myWindowNumber);
    if (noResize) {
        generalR.setVisibility(true);
        generalR.setColor(Clr_DarkGray);
    }
    else{
        generalR.setVisibility(false);
    }
    tFileReader->PointToFile();
    upperMargin=20;
    lowerMargin=54;
    if (showFPS) upperMargin+=charHeight+2;
    resize();
    wLeft=gLeft+150;
    wRight=wLeft+wWidth;
    wTop=gTop-150;//moreover this will force a new recalculation at next draw call
    wBottom=wTop-wHeight;
    defineButtons();
    nButtons=static_cast<int>(tButtons.size());
    if (goToLastPage) tFileReader->GoToLastPage();
    return true;

}

void Layout::HandleDialog(){

}

void Layout::resize(){//calculate offsets; areas of rectangles}
    dayPart=3;
    tFileReader->SetSplitPolicy(splitLinePolicy);
    if (textHeight<150) textHeight=150;
    tFileReader->Setup(textHeight,textWidth,colWidth,upperMargin);
    tFileReader->ReadFileToBuff();
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
            tFileReader->Setup(textHeight,textWidth,colWidth,upperMargin);
            tFileReader->ReadFileToBuff();
            fitSizeToFile=false;}
        return;
    }
    generalR.SetDimensions(textWidth+colWidth+10,textHeight+upperMargin+lowerMargin);
    wWidth=generalR.GetWidth();
    wHeight=generalR.GetHeight();
    vrWidth=wWidth;
    vrHeight=wHeight;
    if (XPLMWindowIsInVR(myWindow)==1){
       if (myWindow!=nullptr) XPLMSetWindowGeometryVR(myWindow,vrWidth,vrHeight);}
    else {
       if (myWindow!=nullptr) {
          XPLMGetWindowGeometry(myWindow,&l,&t,&r,&b);
          XPLMSetWindowGeometry(myWindow,l,t,l+vrWidth,t-vrHeight);
       }
    }
    generalR.SetOffsets(0,0);

    minHeight=tFileReader->GetOffSetY()+180;
    maxHeight=tFileReader->GetOffSetY()+935;
    if (noResize){
        generalR.setVisibility(true);
        generalR.setColor(Clr_DarkGray);
    }
    else generalR.setVisibility(false);
    if (!useBackGround) tFileReader->SetBackGround(false);
}



void Layout::RelocateButtons(int middle){
    tButtons[B_Toggle_Line].SetOffsetY(middle-13);
    tButtons[B_Toggle_Line].setText("\xE2\x8C\xAB");
    tButtons[B_Show_All].SetOffsetY(middle+25);
    tButtons[B_Show_All].setText("\xE2\x86\xBA");
    tButtons[B_Undo].SetOffsetY(middle+5);
    tButtons[B_Undo].setText("\xE2\x87\x9B");
    tButtons[B_NAV1].SetOffsetY(generalR.GetHeight()-lowerMargin+2);
    tButtons[B_NAV1].setText("Nav1");
    tButtons[B_NAV2].SetOffsetY(tButtons[B_NAV1].GetOffsetY());
    tButtons[B_NAV2].setText("Nav2");
    tButtons[B_COM1].SetOffsetY(tButtons[B_NAV1].GetOffsetY()+18);
    tButtons[B_COM1].setText("com1");
    tButtons[B_COM2].SetOffsetY(tButtons[B_COM1].GetOffsetY());
    tButtons[B_COM2].setText("com2");
    tButtons[B_ADF1].SetOffsetY(tButtons[B_COM1].GetOffsetY()+18);
    tButtons[B_ADF1].setText("Adf1");
    tButtons[B_ADF2].SetOffsetY(tButtons[B_ADF1].GetOffsetY());
    tButtons[B_ADF2].setText("Adf2");
    tButtons[B_UTF8].SetOffsetY(generalR.GetHeight()-31);
    string utfText=tButtons[B_UTF8].buttonText;
    tButtons[B_UTF8].setText(utfText);

    fNav.SetOffsets(fNav.GetOffsetX(),tButtons[B_NAV1].GetTextOffsetY());
    fCom.SetOffsets(fCom.GetOffsetX(),tButtons[B_COM1].GetTextOffsetY());
    fAdf.SetOffsets(fAdf.GetOffsetX(),tButtons[B_ADF1].GetTextOffsetY());
    if (keepSize) KeepCurrentSize();
    CheckButtonsVisibility();
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
    generalR.SetDimensions(textWidth+colWidth+10,textHeight+tFileReader->GetOffSetY()+lowerMargin);//offsetY upper margin and 54lower margin
    wWidth=generalR.GetWidth();
    wHeight=generalR.GetHeight();
    if (keepSize) KeepCurrentSize();
}

bool Layout::newSize(int wth,int hgt){//called by recalculate
    int oldW=vrWidth,oldH=vrHeight;
    vrWidth=wth;vrHeight=hgt;
    if ((oldW!=vrWidth)|(oldH!=vrHeight)){
        //canUTF=true;
        int deltaW(0),deltaH(0);
        deltaW=vrWidth-oldW;
        deltaH=vrHeight-oldH;
        textWidth+=deltaW;
        textHeight+=deltaH;
        if (textWidth<260)  textWidth=260;
        if (textWidth>1500) textWidth=1500;
        if (textHeight<150) textHeight=150;
        if (textHeight>900) textHeight=900;
        resize();

        int middle=tFileReader->GetOffSetY()+(tFileReader->GetHeight()/2);
        RelocateButtons(middle);
        return true;

 }
    return false;
}
void Layout::recalculate(float cT){ //gets called at every draw callback so this is used also to process other frequent checks

    bool newGeometry(false);
    if (!noResize){
       int wdt=r-l,hg=t-b;
       if (isWindowInVR()){
           XPLMGetWindowGeometryVR(myWindow,&wdt,&hg);
           newGeometry=newSize(wdt,hg);}
        else newGeometry=newSize(wdt,hg);
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
        wTop=t;
        wLeft=l;
        generalR.recalculate(l,t);
        tFileReader->Recalculate(l,t);
        fNav.recalculate(l,t);
        fCom.recalculate(l,t);
        fAdf.recalculate(l,t);
        lFPS.recalculate(l,t);

        for (auto &btns:tButtons){
            btns.SetOrigin(l,t);
            btns.recalculate();
        }
}
}
void Layout::DrawTextW(XPLMWindowID g_textWindow){
    //intialize
    float curT=XPLMGetElapsedTime();
    XPLMGetWindowGeometry(g_textWindow, &l, &t, &r, &b);
    recalculate(curT);

    if (showFPS){
        if ((curT-fpsTag)>0.5f) {
            fpsTag=curT;
            currentFPS=1/(XPLMGetDataf(g_FPS));
            std::string fps=std::to_string(currentFPS);
            fps=fps.substr(0,5);
            lFPS.setText("FPS : "+fps);
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
                break;}
            case 1:{
                tFileReader->SetBckColor(Clr_PaperDusk);
                tFileReader->SetInkColor(Clr_BlackInk);
                break;}
            case 2:{
                tFileReader->SetBckColor(Clr_Black);//night paper
                tFileReader->SetInkColor(Clr_Amber);
                break;}
            }
        }
    }
    DrawLogic::DrawElements();
    DrawLogic::DrawStrings();
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
}

void Layout::CheckButtonsVisibility(){
    tButtons[B_Save].setVisibility(false);
    tButtons[B_Edit_Line].setVisibility(true);
    tButtons[B_More_Lines].setVisibility(false);
    tButtons[B_Less_Lines].setVisibility(false);

    tButtons[B_Load_File].setVisibility(true);
    tButtons[B_Reload].setVisibility(!autoReload);
    tButtons[B_Auto].setVisibility(true);
    tButtons[B_Undo].setVisibility(tFileReader->CanUndo()&(!autoReload)&enableDelete);
    tButtons[B_UTF8].setVisibility(!(tFileReader->HasSelection())&(!autoReload)&canUTF);
    tButtons[B_Toggle_Line].setVisibility(tFileReader->HasSelection()&(!autoReload)&(enableDelete));
    tButtons[B_Show_All].setVisibility(tFileReader->HasHiddenLine()&(!autoReload)&(enableDelete));

    tButtons[B_NAV1].setVisibility(tFileReader->HasNav()&XPLMGetDatai(nav1on)&enableFreqs);
    tButtons[B_NAV2].setVisibility(tFileReader->HasNav()&XPLMGetDatai(nav2on)&enableFreqs);
    tButtons[B_COM1].setVisibility(tFileReader->HasCom()&XPLMGetDatai(com1on)&enableFreqs);
    tButtons[B_COM2].setVisibility(tFileReader->HasCom()&XPLMGetDatai(com2on)&enableFreqs);
    tButtons[B_ADF1].setVisibility(tFileReader->HasADF()&XPLMGetDatai(adf1on)&enableFreqs);
    tButtons[B_ADF2].setVisibility(tFileReader->HasADF()&XPLMGetDatai(adf2on)&enableFreqs);

    fNav.SetVisibility(tFileReader->HasNav());
    fCom.SetVisibility(tFileReader->HasCom());
    fAdf.SetVisibility(tFileReader->HasADF());
    lFPS.SetVisibility(showFPS);

}

int Layout::rightRectangle(int idx){
    ulong uidx=static_cast<ulong>(idx);
    return (tButtons[uidx].GetRight());
}
int Layout::leftRectangle(int idx){
    ulong uidx=static_cast<ulong>(idx);
    return (tButtons[uidx].GetLeft());
}
int Layout::topRectangle(int idx){
    ulong uidx=static_cast<ulong>(idx);
    return (tButtons[uidx].GetTop());
}
int Layout::bottomRectangle(int idx){
    ulong uidx=static_cast<ulong>(idx);
    return (tButtons[uidx].GetBottom());
}


//process a mouse or controller click

void Layout::findClick(int mX, int mY){

    if (continueClick||buttonClick){
        if (clickresult>-1) tButtons[static_cast<ulong>(clickresult)].Release();
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
    ulong uNB=static_cast<ulong>(nButtons);
    for (ulong I(0);I<uNB;I++){       //Click on a button ?
      if (tButtons[I].isHere(mX,mY)){
           buttonClick=true;
           clickresult=static_cast<int>(I);
           tButtons[I].Press();
           return;
           }
    }


}

void Layout::HandleMouseKeepDown(int mX,int mY){
    dayPart=3;
    if (continueClick) tFileReader->ProceedClickCont(mX,mY);
}

int Layout::HandleMouseUp(int mX,int mY){
    dayPart=3;
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
        tButtons[static_cast<ulong>(clickresult)].Release();
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
}

void Layout::ClosegWindow(){
    myWindow=nullptr;
    tFileReader->closeReader();
    tButtons.clear();

    wTop=0;wBottom=0;wLeft=0;wRight=0;
    wWidth=0;wHeight=0;
    charWidth=0;
    textPointX=0;textPointY=0;
    t=0;b=0;l=0;r=0;
    colWidth=30;idxSelected=-1;nButtons=0;
    //I keep charHeight,textHeight and textWidth,
}

void Layout::KeepFile(){
    IniSettings::SetFile();
    IniSettings::SetDir();
    IniSettings::WriteIniFile();
}

std::string Layout::GetFileName(){
    return FilePointer::GetCurrentFileName();
}

std::string Layout::GetDirName(){
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
void Layout::defineButtons(){
    int middle=upperMargin+(textHeight/2);

    ClearUpButtons();
//create 16 buttons
  /*  B_Load_File=0*/
    MakeButton(true,"Load",90,16,5,upperMargin-18);

         //B_Page_Up=1;
    MakeButton(true,"ReLoad",90,16,100,tButtons[B_Load_File].GetOffsetY());

         //B_Line_Up=2;
    MakeButton(true,"Auto ReLoad",90,16,195,tButtons[B_Load_File].GetOffsetY());

         //B_Toggle_Line=3;
    MakeButton(true,"\xE2\x8C\xAB",20,15,5,middle-13);

         //B_UTF8=4;
    MakeButton(true,"UTF8",30,25,1,generalR.GetHeight()-31);

        // B_Undo=5;
    MakeButton(true,"\xE2\x87\x9B",20,15,5,middle+5);

         //B_Edit_Line=6;
    MakeButton(true,"Edit",20,15,290,tButtons[B_Load_File].GetOffsetY());

        // B_Save=7;
    MakeButton(false,"Save",20,15,colWidth+47,generalR.GetHeight()-31);

         //B_More_Lines=8;
    MakeButton(true,"+Lines",20,15,colWidth+142,generalR.GetHeight()-31);

        // B_Less_Lines=9;
    MakeButton(true,"-Lines",20,15,colWidth+190,generalR.GetHeight()-31);

         //B_NAV1 = 10;
    MakeButton(true,"Nav1",35,15,colWidth+46,generalR.GetHeight()-lowerMargin+2);

        // B_NAV2 = 11;
    MakeButton(true,"Nav2",35,15,tButtons[B_NAV1].GetOffsetX()+38,tButtons[B_NAV1].GetOffsetY());

         //B_COM1 = 12;
    MakeButton(true,"com1",35,15,colWidth+46,tButtons[B_NAV1].GetOffsetY()+18);

        // B_COM2 = 13;
    MakeButton(true,"com2",35,15,tButtons[B_COM1].GetOffsetX()+38,tButtons[B_COM1].GetOffsetY());

        // B_ADF1 = 14;
    MakeButton(true,"Adf1",35,15,colWidth+46,tButtons[B_COM1].GetOffsetY()+18);

        // B_ADF2 = 15
    MakeButton(true,"Adf2",35,15,tButtons[B_ADF1].GetOffsetX()+38,tButtons[B_ADF1].GetOffsetY());

    //B_Show_All=16
    MakeButton(true,"\xE2\x86\xBA",tButtons[B_Toggle_Line].GetWidth(),17,5,middle+25);

    fNav.SetOffsets(colWidth,tButtons[B_NAV1].GetTextOffsetY());    
    fNav.SetDimensions(45,charHeight);
    fNav.SetTextColor(Clr_Amber);

    fCom.SetOffsets(colWidth,tButtons[B_COM1].GetTextOffsetY());  
    fCom.SetDimensions(45,charHeight);
    fCom.SetTextColor(Clr_Amber);

    fAdf.SetOffsets(colWidth,tButtons[B_ADF1].GetTextOffsetY());
    fAdf.SetDimensions(45,charHeight);
    fAdf.SetTextColor(Clr_Amber);


    fpsTag=XPLMGetElapsedTime();
    lFPS.SetOffsets(14,tButtons[B_Load_File].GetOffsetY()-2);
    lFPS.SetTextColor(Clr_Amber);

}

void Layout::MakeButton(bool visible,string in_Label,int width,int height,int oX, int oY){
    button_VR newB;
    newB.SetOffsets(oX,oY);
    newB.SetDimensions(width,height);
    newB.setText(in_Label);
    newB.SetToStateColor();
    newB.setVisibility(visible);
    tButtons.push_back(newB);
}

void Layout::ClearUpButtons(){
    for (auto bt:tButtons){
        bt.DeleteButton();
    }
    tButtons.clear();
}

