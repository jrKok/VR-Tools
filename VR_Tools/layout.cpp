#include <layout.h>
#include "temporarywindow.h"
#include "linedialog.h"
#include "fontman.h"

int Layout::cycle=0;

Layout::Layout(DrawLogic *newPad) :
   myDrawPad(newPad),
   //gTop(0),gLeft(0),gRight(0),gBottom(0),
  wTop(0),wBottom(0),wLeft(0),wRight(0),
  wWidth(0),wHeight(0),minWidth(0),maxWidth(0),minHeight(0),maxHeight(0),
  charHeight(0),charWidth(0),textPointX(0),textPointY(0),//Used for initial setup and resize instructions
  t(0),b(0),l(0),r(0),//input from draw instruction mostly
  textHeight(0),textWidth(0),colWidth(45),idxSelected(-1),nButtons(0),
  reloadPeriod(1.0f),
  generalR("layout's general",true),
  titleR("Ribbon",true),
  decoR("decor",true),
  bckg(nullptr),
  tFileReader(new TextReader()),
  tButtons(),
  myWindow(nullptr),
  fNav(),fCom(),fAdf(),lFPS(),lTitle(),
  charSep(""),
  epoch(0),
  editMode(false),openAtStart(false),goToLastPage(false),
  continueClick(false),buttonClick(false),autoReload(false),saveAuto(false),canUTF(true),autoReloadOnSize(true),
  useBackGround(true),
  flash(false),flashWhenChange(false),
  must_print(true),
  text_visible(true),
  clickresult(-1),
  upperMargin(45),
  lowerMargin(60),
  dayPart(3),
  currentFPS(0)

//Constructor
{
    IniSettings::OrientFilePointer();
}

//Destructor

Layout::~Layout(){
    if (bckg){
        delete bckg;
        bckg=nullptr;
    }
    delete tFileReader;
    if (tButtons.size()>0)
    {
        for (auto &bt:tButtons){
            delete bt.second;
            bt.second=nullptr;
        }
        tButtons.clear();
    }
    if (tBButtons.size()>0){
        for (auto &bt:tBButtons){
            delete bt.second;
            bt.second=nullptr;
        }
        tBButtons.clear();
    }
    delete myDrawPad;
    myDrawPad=nullptr;
}

//Helpers

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
    charHeight=fontMan::GetFontSize(0);
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
    bottom=0;
    myDrawPad->ToUpperLevel();
    if (!nButtons){
        generalR.SetOrigin(0,0);
        if (noResize) {
            generalR.SetVisibility(true);
            generalR.SetColor(Clr_DarkGray);
            DrawLogic::SetBackGroundColor(Clr_DarkGray);
        }
        else{
            generalR.SetVisibility(true);
            generalR.SetColor(Clr_DarkGray);
            DrawLogic::SetBackGroundColor(Clr_DarkGray);
        }
        tFileReader->PointToFile();;
        if (resize()){
            wLeft=150;
            wRight=wLeft+wWidth;
            wBottom=50;
            wTop=wBottom+wHeight;
            defineButtons();
            nButtons=static_cast<int>(tButtons.size());
            if (goToLastPage) tFileReader->GoToLastPage();

            myDrawPad->GenerateCurrentTexture();
            return true;
        }else {
            return false;
        }
    }
    else{
        tFileReader->PointToFile();
        bool retb=resize();
        myDrawPad->GenerateCurrentTexture();
        return retb;
    }

}

void Layout::HandleDialog(){

}

bool Layout::resize(){//calculate offsets; areas of rectangles}
    dayPart=3;
    text_visible=true;
    tFileReader->SetSplitPolicy(splitLinePolicy);
    if (textHeight<150) textHeight=150;
    tFileReader->Setup(textHeight,textWidth,colWidth,lowerMargin);

    if (tFileReader->ReadFileToBuff())
    {
        if (fitSizeToFile){
            int hgt=(charHeight+2)*tFileReader->GetNumberOfLines();
            int wdth=tFileReader->GetMaxWidth()+25;//15 for the scroll box,10 for the text margins
            if (wdth<295) wdth=295;
            if (wdth>(MaxWWidth-colWidth-10)) wdth=MaxWWidth-colWidth-10;
            if (hgt<150) hgt=150;
            if (hgt>(MaxWHeight-upperMargin-lowerMargin)) hgt=MaxWHeight-upperMargin-lowerMargin;
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
        MakeHeader("");
        myDrawPad->SetNewSize(wWidth,wHeight);
        if (XPLMWindowIsInVR(myWindow)==1){
            if (myWindow!=nullptr) XPLMSetWindowGeometryVR(myWindow,wWidth,wHeight);}
        else {
            if (myWindow!=nullptr) {
                XPLMGetWindowGeometry(myWindow,&l,&t,&r,&b);
                XPLMSetWindowGeometry(myWindow,l,b+wHeight,l+wWidth,b);
            }
        }
        minHeight=tFileReader->GetOffSetY()+180;
        maxHeight=tFileReader->GetOffSetY()+935;
        if (noResize){
            generalR.SetVisibility(true);
            generalR.SetColor(Clr_DarkGray);
        }
        else generalR.SetVisibility(true);
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
    if (wdth<295) wdth=295;
    if (wdth>(MaxWWidth-colWidth-10)) wdth=MaxWWidth-colWidth-10;
    if (hgt<150) hgt=150;
    if (hgt>(MaxWHeight-upperMargin-lowerMargin)) hgt=MaxWHeight-upperMargin-lowerMargin;
    textHeight=hgt;
    textWidth=wdth;
    tFileReader->Setup(textHeight,textWidth,colWidth,upperMargin);
    tFileReader->ReadFileToBuff();

    wWidth=textWidth+colWidth+10;
    wHeight=textHeight+upperMargin+lowerMargin;
    generalR.SetDimensions(wWidth,wHeight);//offsetY upper margin and 54lower margin
    if (keepSize) KeepCurrentSize();
}

bool Layout::newSize(int wth,int hgt){//called by recalculate

    if (wth!=wWidth||hgt!=wHeight){
        //canUTF=true;
        int oldW(wWidth),oldH(wHeight);
        wWidth=wth;wHeight=hgt;
        int deltaW(wWidth-oldW),deltaH(wHeight-oldH);
        textWidth+=deltaW;
        textHeight+=deltaH;
        if (textWidth<295)  textWidth=295;
        if (textWidth>(MaxWWidth-colWidth-10)) textWidth=MaxWWidth-colWidth-10;
        if (textHeight<150) textHeight=150;
        if (textHeight>(MaxWHeight-upperMargin-lowerMargin)) textHeight=MaxWHeight-upperMargin-lowerMargin;
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
        if (isWindowInVR()) XPLMSetWindowGeometryVR(myWindow,wWidth,wHeight);
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

void Layout::Update(){
    if (myWindow){
        myDrawPad->ToUpperLevel();
        if (!text_visible){
            bckg->KeepSize();
            return;
        }
        float curT=XPLMGetElapsedTime();
        XPLMGetWindowGeometry(myWindow, &l, &t, &r, &b);
        recalculate(curT);
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
                    decoR.SetColor(Clr_PaperWhite);
                    tFileReader->SetInkColor(Clr_BlackInk);
                    break;}
                case 1:{
                    tFileReader->SetBckColor(Clr_PaperDusk);
                    decoR.SetColor(Clr_PaperDusk);
                    tFileReader->SetInkColor(Clr_BlackInk);
                    break;}
                case 2:{
                    tFileReader->SetBckColor(Clr_Black);//night paper
                    decoR.SetColor(Clr_Black);
                    tFileReader->SetInkColor(Clr_Amber);
                    break;}
                }
                must_print=true;
            }
        }
        if (must_print||tFileReader->NeedsToDisplay()) {
            tFileReader->DisplayPage(true);
            must_print=false;
        }
        myDrawPad->UpdateDrawPad(wWidth,wHeight,l,b,r,t);
    }
}

void Layout::DrawTextW(XPLMWindowID){
    //intialize
    myDrawPad->ToUpperLevel();
    DrawLogic::RenderContent();
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
    resize();
    myDrawPad->SetNewSize(wWidth,wHeight);
    myDrawPad->GenerateCurrentTexture();
}

void Layout::UpdateFrequencies(){
    tFileReader->CheckForFrequencies();
    fNav.setText(tFileReader->GetNavStr());
    fCom.setText(tFileReader->GetComStr());
    fAdf.setText(tFileReader->GetADFStr());
    CheckButtonsVisibility();
}

//process a mouse or controller click

void Layout::findClick(int mX, int mY){
    myDrawPad->ToUpperLevel();
    int cX(mX-l),cY(mY-b);

    if (continueClick||buttonClick){
        if (clickresult>-1) tButtons[clickresult]->Release();
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
    if (!text_visible) {
        buttonClick=true;
        clickresult=B_Refresh;
        return;
    }
    if (tFileReader->ProceedClick(cX,cY)){//Click in the text or scrB ?
        CheckButtonsVisibility();
        continueClick=true;

        return;
      }
    for (auto bt:tButtons){       //Click on a button ?
      if (bt.second->IsHere(cX,cY)){
           buttonClick=true;
           clickresult=bt.first;
           bt.second->Press();
           return;
           }
    }
    for (auto bt:tBButtons){       //Click on a button ?
      if (bt.second->IsHere(cX,cY)){
           buttonClick=true;
           clickresult=bt.first;
           bt.second->Press();
           return;
           }
    }
}

void Layout::HandleMouseKeepDown(int mX,int mY){
    myDrawPad->ToUpperLevel();
        int cX(mX-l),cY(mY-b);
    //dayPart=3;
    if (continueClick) tFileReader->ProceedClickCont(cX,cY);
}

int Layout::HandleMouseUp(int,int){
    myDrawPad->ToUpperLevel();
    //dayPart=3;
    autoReload=saveAuto;
    epoch=XPLMGetElapsedTime();
    int retVal=-1;
    if (continueClick){
        continueClick=false;
        buttonClick=false;
        clickresult=-1;
        tFileReader->ProceedEndClick();
        UpdateFrequencies();}

    if (buttonClick){
        if (tButtons.find(clickresult)!=tButtons.end()) tButtons[clickresult]->Release();
        if (tBButtons.find(clickresult)!=tBButtons.end()) tBButtons[clickresult]->Release();
        continueClick=false;
        buttonClick=false;
        switch (clickresult) {
        case B_Open:{retVal=2;break;}
        case B_Edit:{retVal=3;break;}
        case B_Hide:{retVal=1;break;}
        case B_Next_File:{retVal=4;break;}
        case B_Prev_File:{retVal=5;break;}
        default: LaunchCommand(clickresult);
        }
    }
    clickresult=-1;
    return retVal;
}
//The following can also be used by the plugin handlers to launch a custom command
//to implement functionnality by pressing physical commands, keys, menus ...
//enumeration to be found in show_fps_test_global.h

void Layout::LaunchCommand(int refCommand){
    switch(refCommand){
         case B_Refresh:{
             if (!text_visible){
                 text_visible=true;
                 delete bckg;
                 bckg=nullptr;
                 resize();
                 CheckButtonsVisibility();
                 myDrawPad->GenerateCurrentTexture();
             }
             StopStreaming();
             tFileReader->Reload();
             break;}

         case B_Undo:{
             StopStreaming();
             tFileReader->UndoDelete();
             UpdateFrequencies();
             break;}

         case B_Stream:{
             if (autoReload) StopStreaming();
             else{
                 autoReload=true;
                 tFileReader->SetStreamingMode(autoReload);
                 tButtons[B_Stream]->setSelect(true);
                 epoch=XPLMGetElapsedTime();
              }
              break;}

         case B_UTF8:{
             tFileReader->SetNeedsUTF(true);
             canUTF=false;
             break;}

         case B_Toggle:{
             if (enableDelete)
             tFileReader->DeleteSelectedLine();
             UpdateFrequencies();
             break;}

         case B_Show_All:{
             StopStreaming();
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
         case B_Close:{
             text_visible=false;
             tFileReader->HideMyself();
             CheckButtonsVisibility();
             decoR.SetVisibility(false);
             titleR.SetVisibility(false);
             bckg = new Background;
             char ink(Clr_PaperWhite);
             if (dayPart==1) ink=Clr_PaperDusk;
             if (dayPart==2) ink=Clr_Amber;
             bckg->MakeBackGround(myDrawPad,myWindow,ink);
             bckg->GenerateMyTexture();
             bckg->LoadTexture();
             break; }
    }
    CheckButtonsVisibility();
}

void Layout::StopStreaming(){
    if (autoReload){
       tButtons[B_Stream]->setSelect(false);
       autoReload=false;
       tFileReader->SetStreamingMode(false);
       tFileReader->Reload();
    }
}

void Layout::ClosegWindow(){
    myDrawPad->ToUpperLevel();
    tFileReader->closeReader();
    myWindow=nullptr;
    for (auto &bt:tButtons){
        delete (bt.second);
        bt.second=nullptr;
    }
    for (auto &bbt:tBButtons){
        delete (bbt.second);
        bbt.second=nullptr;
    }
    tBButtons.clear();
    tButtons.clear();
    nButtons=0;
    wTop=0;wBottom=0;wLeft=0;wRight=0;
    wWidth=0;wHeight=0;
    charWidth=0;
    textPointX=0;textPointY=0;
    t=0;b=0;l=0;r=0;
    idxSelected=-1;nButtons=0;
    fNav.setText("");
    fCom.setText("");
    fAdf.setText("");
    myDrawPad->CloseWindow();
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
    myDrawPad->ToUpperLevel();
    myDrawPad->SetWindowHandle(thisW);
    XPLMGetWindowGeometry(myWindow, &l, &t, &r, &b);
    if (isWindowInVR())
    XPLMSetWindowGeometryVR(myWindow,wWidth,wHeight);
    else {
        XPLMSetWindowGeometry(myWindow,l,b+wHeight,l+wWidth,b);
    }
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

void Layout::CheckButtonsVisibility(){

    tButtons[B_Save]->setVisibility(false);
    tButtons[B_Edit]->setVisibility(text_visible);
    tButtons[B_Next_File]->setVisibility(text_visible);
    tButtons[B_Prev_File]->setVisibility(text_visible);

    tButtons[B_Open]->setVisibility(text_visible);
    tButtons[B_Refresh]->setVisibility(text_visible);
    tButtons[B_Stream]->setVisibility(text_visible);
    tButtons[B_Undo]->setVisibility(text_visible&&tFileReader->CanUndo()&&(!autoReload)&enableDelete);
    tButtons[B_UTF8]->setVisibility(text_visible&&!(tFileReader->HasSelection())&&(!autoReload)&canUTF);
    tButtons[B_Toggle]->setVisibility(text_visible&&tFileReader->HasSelection()&&(!autoReload)&(enableDelete));
    tButtons[B_Show_All]->setVisibility(text_visible&&tFileReader->HasHiddenLine()&&(!autoReload)&(enableDelete));

    tButtons[B_NAV1]->setVisibility(text_visible&&tFileReader->HasNav()&&XPLMGetDatai(nav1on)&&enableFreqs);
    tButtons[B_NAV2]->setVisibility(text_visible&&tFileReader->HasNav()&&XPLMGetDatai(nav2on)&&enableFreqs);
    tButtons[B_COM1]->setVisibility(text_visible&&tFileReader->HasCom()&&XPLMGetDatai(com1on)&&enableFreqs);
    tButtons[B_COM2]->setVisibility(text_visible&&tFileReader->HasCom()&&XPLMGetDatai(com2on)&&enableFreqs);
    tButtons[B_ADF1]->setVisibility(text_visible&&tFileReader->HasADF()&&XPLMGetDatai(adf1on)&&enableFreqs);
    tButtons[B_ADF2]->setVisibility(text_visible&&tFileReader->HasADF()&&XPLMGetDatai(adf2on)&&enableFreqs);

    tBButtons[B_Hide]->SetVisibility(text_visible);
    tBButtons[B_Close]->SetVisibility(text_visible&&!editMode);

    fNav.SetVisibility(text_visible&&tFileReader->HasNav());
    if (tFileReader->HasNav()&&text_visible)fNav.PrintString();

    fCom.SetVisibility(text_visible&&tFileReader->HasCom());
    if (tFileReader->HasCom()&&text_visible)fCom.PrintString();

    fAdf.SetVisibility(text_visible&&tFileReader->HasADF());
    if(tFileReader->HasADF()&&text_visible) fAdf.PrintString();

    lFPS.SetVisibility(showFPS);
    if (showFPS) lFPS.PrintString();
}

void Layout::RelocateButtons(int middle){

    int buttonsPos=textWidth/2-106;
    if (buttonsPos<137) buttonsPos=137;

    fAdf.SetOrigin(10,bottom+5);
    fCom.SetOrigin(fAdf.GetLeft(),fAdf.GetTop()+4);
    fNav.SetOrigin(fAdf.GetLeft(),fCom.GetTop()+4);

    tButtons[B_ADF1]->     SetOrigin(fAdf.GetRight()+3,fAdf.GetBottom());
    tButtons[B_ADF2]->     SetOrigin(tButtons[B_ADF1]->GetRight()+2,tButtons[B_ADF1]->GetBottom());
    tButtons[B_COM1]->     SetOrigin(tButtons[B_ADF1]->GetLeft(),fCom.GetBottom());
    tButtons[B_COM2]->     SetOrigin(tButtons[B_ADF2]->GetLeft(),tButtons[B_COM1]->GetBottom());
    tButtons[B_NAV1]->     SetOrigin(tButtons[B_ADF1]->GetLeft(),fNav.GetBottom());
    tButtons[B_NAV2]->     SetOrigin(tButtons[B_ADF2]->GetLeft(),tButtons[B_NAV1]->GetBottom());

    tButtons[B_Edit]->     SetOrigin(buttonsPos,bottom+7);
    tButtons[B_Open]->     SetOrigin(tButtons[B_Edit]->GetLeft(),tButtons[B_Edit]->GetTop()+5);

    tButtons[B_Stream]->   SetOrigin(tButtons[B_Edit]->GetRight()+5,tButtons[B_Edit]->GetBottom());
    tButtons[B_Refresh]->  SetOrigin(tButtons[B_Open]->GetRight()+5,tButtons[B_Open]->GetBottom());

    tButtons[B_Save]->     SetOrigin(tButtons[B_Refresh]->GetRight()+2,tButtons[B_Open]->GetBottom());
    tButtons[B_Toggle]->   SetOrigin(5,middle+13);

    tButtons[B_UTF8]->     SetOrigin(1,lowerMargin+2);
    tButtons[B_Undo]->     SetOrigin(5,middle-5);
    tButtons[B_Next_File]->SetOrigin(tButtons[B_Refresh]->GetRight()+5,tButtons[B_Refresh]->GetBottom());
    tButtons[B_Prev_File]->SetOrigin(tButtons[B_Stream]->GetRight()+5,tButtons[B_Stream]->GetBottom());
    tBButtons[B_Hide]->    SetOrigin(1,titleR.GetBottom()+1);
    tBButtons[B_Close]->   SetOrigin(wWidth-18,titleR.GetBottom()+1);

    lFPS.SetOrigin(14,generalR.GetTop()-20);

    if (keepSize) KeepCurrentSize();
    CheckButtonsVisibility();
}
void Layout::defineButtons(){
    int buttonsPos=textWidth/2-106;
    if (buttonsPos<137) buttonsPos=137;
    int middle=lowerMargin+(textHeight/2);

    fAdf.SetOrigin(10,bottom+5);
    fAdf.SetDimensions(47,charHeight+2);
    fAdf.SetBackGroundColor(Clr_DarkGray);
    fAdf.SetTextColor(Clr_Amber);

    fCom.SetOrigin(fAdf.GetLeft(),fAdf.GetTop()+4);
    fCom.SetDimensions(fAdf.GetWidth(),fAdf.GetHeight());
    fCom.SetBackGroundColor(Clr_DarkGray);
    fCom.SetTextColor(Clr_Amber);

    fNav.SetOrigin(fAdf.GetLeft(),fCom.GetTop()+4);
    fNav.SetDimensions(fAdf.GetWidth(),fAdf.GetHeight());
    fNav.SetBackGroundColor(Clr_DarkGray);
    fNav.SetTextColor(Clr_Amber);

    MakeButton(B_ADF1,true,"Adf1",35,15,fAdf.GetRight()+3,fAdf.GetBottom());
    MakeButton(B_ADF2,true,"Adf2",35,15,tButtons[B_ADF1]->GetRight()+2,tButtons[B_ADF1]->GetBottom());
    MakeButton(B_COM1,true,"com1",35,15,tButtons[B_ADF1]->GetLeft(),fCom.GetBottom());
    MakeButton(B_COM2,true,"com2",35,15,tButtons[B_ADF2]->GetLeft(),tButtons[B_COM1]->GetBottom());
    MakeButton(B_NAV1,true,"Nav1",35,15,tButtons[B_ADF1]->GetLeft(),fNav.GetBottom());
    MakeButton(B_NAV2,true,"Nav2",35,15,tButtons[B_ADF2]->GetLeft(),tButtons[B_NAV1]->GetBottom());



    MakeButton(B_Edit,true,"Edit",67,16,buttonsPos,bottom+7);
    MakeButton(B_Open,true,"Open",67,16,tButtons[B_Edit]->GetLeft(),tButtons[B_Edit]->GetTop()+5);
    MakeButton(B_Stream,true,"Stream",67,16,tButtons[B_Edit]->GetRight()+5,tButtons[B_Edit]->GetBottom());
    MakeButton(B_Refresh,true,"Refresh",67,16,tButtons[B_Open]->GetRight()+5,tButtons[B_Open]->GetBottom());
    MakeButton(B_Save,false,"Save",67,16,tButtons[B_Refresh]->GetRight()+2,tButtons[B_Open]->GetBottom());

    MakeButton(B_Toggle,true,"\xE2\x8C\xAB",28,20,5,middle+16);
    MakeButton(B_UTF8,true,"UTF8",30,25,1,lowerMargin+2);
    MakeButton(B_Undo,true,"\xE2\x87\x9B",28,20,5,middle-7);

    MakeButton(B_Next_File,true,"Next",67,16,tButtons[B_Refresh]->GetRight()+5,tButtons[B_Refresh]->GetBottom());
    MakeButton(B_Prev_File,true,"Prev",67,16,tButtons[B_Stream]->GetRight()+5,tButtons[B_Stream]->GetBottom());

    MakeButton(B_Show_All,true,"\xE2\x86\xBA",tButtons[B_Toggle]->GetWidth(),20,5,middle-30);
    MakeBoxedButton(B_Hide,true,"\xE2\x9A\xAB",18,18,1,titleR.GetBottom()+1,1);
    MakeBoxedButton(B_Close,true,"X",18,18,wWidth-18,titleR.GetBottom()+1,1);
    tBButtons[B_Hide]->SetBoxColor(Clr_SparkSilver);
    tBButtons[B_Hide]->setStateColor(Clr_SparkSilver);
    tBButtons[B_Hide]->setTextColor(Clr_Red);
    tBButtons[B_Hide]->SetToStateColor();

    tBButtons[B_Close]->SetBoxColor(Clr_SparkSilver);
    tBButtons[B_Close]->setStateColor(Clr_SparkSilver);
    tBButtons[B_Close]->setTextColor(Clr_Black);
    tBButtons[B_Close]->SetToStateColor();

    fpsTag=XPLMGetElapsedTime();
    lFPS.SetOrigin(14,generalR.GetTop()-20);
    lFPS.SetDimensions(60,charHeight+2);
    lFPS.SetBackGroundColor(Clr_DarkGray);
    lFPS.SetTextColor(Clr_Amber);

}

void Layout::MakeButton(int btnCmd, bool visible, string in_Label, int width, int height, int oX, int oY){
    button_VR * newB(new button_VR(in_Label,false));
    newB->SetDimensions(width,height);
    newB->SetOrigin(oX,oY);
    newB->setText(in_Label);
    newB->SetToStateColor();
    newB->setVisibility(visible);
    tButtons[btnCmd]=newB;
}

void Layout::MakeBoxedButton(int number, bool visible, string in_Label, int width, int height, int oX, int oY, int fontSize){
    Boxed_Button * newB(new Boxed_Button(in_Label,false));
    newB->SetDimensions(width,height);
    newB->SetOrigin(oX,oY);
    newB->setText(in_Label);
    newB->SetToStateColor();
    newB->SetBoxColor(Clr_White);
    newB->setVisibility(visible);
    newB->SetFontSize(fontSize);
    tBButtons[number]=newB;
}

void Layout::MakeHeader(string in_String){
    titleR.SetDimensions(wWidth,20);
    titleR.SetOrigin(0,lowerMargin+textHeight);
    titleR.SetColor(Clr_SparkSilver);
    titleR.SetVisibility(true);
    string fileN=tFileReader->GetStemFileName();
    decoR.SetDimensions(textWidth-15,3); //decoR is the small white rectangle as large as the text window
    decoR.SetOrigin(colWidth,titleR.GetTop());
    decoR.SetVisibility(true);
    fileN=in_String+fileN;
    int nameSize=fontMan::MeasureString(fileN,1);
    lTitle.setText(fileN);
    lTitle.SetFontSize(1);
    lTitle.SetBackGroundColor(Clr_SparkSilver);
    lTitle.SetDimensions(nameSize,15);
    lTitle.SetOrigin(titleR.GetWidth()/2-nameSize/2,titleR.GetBottom()+2);
    lTitle.SetVisibility(true);
    lTitle.PrintString();
}
