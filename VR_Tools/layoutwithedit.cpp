#include "layoutwithedit.h"
#include "drawlogic.h"
#include "temporarywindow.h"
#include "opcenter.h"

LayoutWithEdit *LayoutWithEdit::myself(nullptr);

LayoutWithEdit::LayoutWithEdit(DrawLogic *newPad): Layout(newPad),
    keyb(),
    showKeyb(true),
    keyPressed(false),
    specialKey(false),
    utfKey(false),
    editMode(true),
    hasToSave(false),
    physK(false),
    firstChar('\0'),
    secondChar('\0'),
    fName(""),
    tEdFileReader(new TextEdit()),
    tabchar('\t'),
    tabstring(1, tabchar),
    quitWoSave()
{
}

//End of constructor

void LayoutWithEdit::ReActivateWindow(){
    myDrawPad->ToUpperLevel();
}

void LayoutWithEdit::StartEdit(){

     ReActivateWindow();
     Begin();
     keyb.MakeKeyboard(colWidth,10);
}

void LayoutWithEdit::BeginEdit(){   

    //if (!showFPS) ToggleFPS();
    editMode=true;
    showKeyb=true;
    hasToSave=false;
    enableFreqs=false;
    splitLinePolicy=BestSplitAtSpace;
    myDrawPad->GenerateCurrentTexture();
}

bool LayoutWithEdit::initiate(){

    ReActivateWindow();
    tEdFileReader->PointToFile();
    tEdFileReader->InitialiseEdit();
    keyb.SetVisibility(showKeyb);
    canUTF=false;
    if (!nButtons){
        if (resize())
        {
            wLeft=150;
            wRight=wLeft+wWidth;
            wBottom=200;
            wTop=wBottom+wHeight;
            defineButtons();
            tButtons[B_NAV1]->setVisibility(false);
            tButtons[B_NAV2]->setVisibility(false);
            tButtons[B_COM1]->setVisibility(false);
            tButtons[B_COM2]->setVisibility(false);
            tButtons[B_ADF1]->setVisibility(false);
            tButtons[B_ADF2]->setVisibility(false);

            fCom.SetVisibility(false);
            fNav.SetVisibility(false);
            fAdf.SetVisibility(false);
            lFPS.SetVisibility(false);
            nButtons=static_cast<int>(tButtons.size());
            //keyb.Relocate(colWidth,generalR.GetHeight()-lowerMargin+58);
            ReLabelButtons();
            //recalculate();
            tButtons[B_UTF8]->setSelect(false);
            return true;
        }else return false;
    }else return resize();
}

bool LayoutWithEdit::resize(){//calculate offsets; areas of rectangles}
    dayPart=3;
    tEdFileReader->SetSplitPolicy(BestSplitAtSpace);
    lowerMargin=54+keyb.MyHeight()+4;
    if (textWidth<keyb.MyWidth()) textWidth=keyb.MyWidth();
    if (textHeight<150) textHeight=150;
    //center keyboard
    int mid=(textWidth-keyb.MyWidth())/2;
    keyb.Relocate(colWidth+mid,10);
    // continue setup
    tEdFileReader->Setup(textHeight,textWidth,colWidth,lowerMargin);
    if (tEdFileReader->ReadFileToBuff())
    {
        if (noResize){
            generalR.setVisibility(true);
            generalR.setColor(Clr_DarkGray);
            myDrawPad->SetBackGroundColor(Clr_DarkGray);
        }
        else {
            generalR.setVisibility(false);
            myDrawPad->SetBackGroundColor(Clr_BckgrdW);
        }

        if (fitSizeToFile){
            int hgt=(charHeight+2)*tEdFileReader->GetNumberOfLines();
            int wdth=tEdFileReader->GetMaxWidth()+25;//15 for the scroll box,10 for the text margins
            if (wdth<260) wdth=260;
            if (wdth>1500) wdth=1500;
            if (hgt<150) hgt=150;
            if (hgt>900) hgt=900;
            if ((wdth<keyb.MyWidth())&&showKeyb) wdth=keyb.MyWidth();
            if (textHeight!=hgt||textWidth!=wdth){
                textHeight=hgt;
                textWidth=wdth;
                tEdFileReader->Setup(textHeight,textWidth,colWidth,lowerMargin);
                tEdFileReader->ReadFileToBuff();
                fitSizeToFile=false;}
        }
        generalR.SetDimensions(textWidth+colWidth+10,textHeight+upperMargin+lowerMargin);//offsetY upper margin and lower margin
        wWidth=generalR.GetWidth();
        wHeight=generalR.GetHeight();
        vrWidth=wWidth;
        vrHeight=wHeight;
        myDrawPad->SetNewSize(wWidth,wHeight);
        if (XPLMWindowIsInVR(myWindow)){
            if (myWindow!=nullptr) XPLMSetWindowGeometryVR(myWindow,vrWidth,vrHeight);}
        else {
            if (myWindow!=nullptr) {
                XPLMGetWindowGeometry(myWindow,&l,&t,&r,&b);
                //vrWidth=r-l;vrHeight=t-b;
                XPLMSetWindowGeometry(myWindow,l,t,l+vrWidth,t-vrHeight);
            }
        }
        XPLMGetWindowGeometry(myWindow,&l,&t,&r,&b);
        minHeight=tEdFileReader->GetOffSetY()+180;
        maxHeight=tEdFileReader->GetOffSetY()+935;
        return true;
    }else return false;
}

bool LayoutWithEdit::newSize(int wth, int hgt){//called by recalculate
 int oldW=vrWidth,oldH=vrHeight;
 vrHeight=hgt;vrWidth=wth;
 if ((oldW!=vrWidth)|(oldH!=vrHeight)){
     //canUTF=true;
     int deltaW(0),deltaH(0);
     deltaW=vrWidth-oldW;
     deltaH=vrHeight-oldH;
     textWidth=textWidth+deltaW;
     textHeight=textHeight+deltaH;
     if (textWidth<keyb.MyWidth()) textWidth=keyb.MyWidth();
     if (textWidth>1500) textWidth=1500;
     if (textHeight<150) textHeight=150;
     if (textHeight>900) textHeight=900;
     resize();
     int middle=tEdFileReader->GetOffSetY()+(tEdFileReader->GetHeight()/2);
     RelocateButtons(middle);
     myDrawPad->GenerateCurrentTexture();
     return true;
 }
 return false;
}

void LayoutWithEdit::FitToFile(){
    int hgt=(charHeight+2)*tEdFileReader->GetNumberOfLines();
    int wdth=tEdFileReader->GetMaxWidth()+25;//15 for the scroll box,10 for the text margins
    if (wdth<keyb.MyWidth()) wdth=keyb.MyWidth();
    if (wdth>1500) wdth=1500;
    if (hgt<150) hgt=150;
    if (hgt>900) hgt=900;
    textHeight=hgt;
    textWidth=wdth;
    tEdFileReader->Setup(textHeight,textWidth,colWidth,upperMargin);
    tEdFileReader->ReadFileToBuff();
    generalR.SetDimensions(textWidth+colWidth+10,textHeight+tEdFileReader->GetOffSetY()+lowerMargin);//offsetY upper margin and 54lower margin
    wWidth=generalR.GetWidth();
    wHeight=generalR.GetHeight();
    myDrawPad->SetNewSize(wWidth,wHeight);
    myDrawPad->GenerateCurrentTexture();
    if (keepSize) KeepCurrentSize();
}

void LayoutWithEdit::ReLabelButtons(){
    tButtons[B_Load_File]->setText("Save");
    tButtons[B_Reload]->setText("Save&Quit");
    tButtons[B_Auto]->setText("Quit");
    tButtons[B_Edit_Line]->setText("Cancel");
    tButtons[B_UTF8]->setText("Keyb");
}

void LayoutWithEdit::recalculate(){ //gets called at every draw callback so this is used also to process other frequent checks
    bool newGeometry (false);
    if (!noResize){
       int wdt=r-l,hg=t-b;
       if (XPLMWindowIsInVR(myWindow)==1){
           XPLMGetWindowGeometryVR(myWindow,&wdt,&hg);
           newGeometry=newSize(wdt,hg);}
        else newGeometry=newSize(wdt,hg);
    }
}

void LayoutWithEdit::Update(){
    if (myWindow){
        myDrawPad->ToUpperLevel();
        XPLMGetWindowGeometry(myWindow, &l, &t, &r, &b);
        recalculate();

        if (showFPS){
            lFPS.setText(textToShow);
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
                    tEdFileReader->SetBckColor(Clr_PaperWhite);
                    tEdFileReader->SetInkColor(Clr_BlackInk);
                    break;}
                case 1:{
                    tEdFileReader->SetBckColor(Clr_PaperDusk);
                    tEdFileReader->SetInkColor(Clr_BlackInk);
                    break;}
                case 2:{
                    tEdFileReader->SetBckColor(Clr_Black);//night paper
                    tEdFileReader->SetInkColor(Clr_Amber);
                    break;}
                }
            }
            must_print=true;
        }
        if (must_print){
            tEdFileReader->DisplayPage();
            must_print=false;
        }
        myDrawPad->UpdateDrawPad(vrWidth,vrHeight,l,b,r,t);
    }
}

void LayoutWithEdit::DrawTextW(XPLMWindowID g_textWindow){
    //intialize
    myDrawPad->ToUpperLevel();

    myDrawPad->RenderElements();
    tEdFileReader->DrawCursor();
}

void LayoutWithEdit::DrawNoResize(XPLMWindowID g_textWindow){
    DrawTextW(g_textWindow);//always resize
}

void LayoutWithEdit::findClick(int cX, int cY){
    int mX(cX-l),mY(cY-b);
// Clean up if a mouse down occurs while no mouse up before was received
    if (continueClick||buttonClick){
        if (clickresult>-1) tButtons[static_cast<ulong>(clickresult)]->Release();
        if (continueClick) tEdFileReader->ProceedEndClick();
        continueClick=false;
        buttonClick=false;
        clickresult=-1;
        return;//if previous mouse click had been released outside window
        //discharge all pending mousedown and drag handlers and discard this click
    }
// Pass on to textedit to see if it proceeds the click
    if (tEdFileReader->ProceedClick(mX,mY)){//Click in the text or scrB ? will handle the cursor
        CheckButtonsVisibility();
        continueClick=true;
        return;
      }
// Now check if the click was on the virtual keyboard
    std::string keyName,keyVal;
    if (keyb.PressKey(mX,mY,specialKey,keyName,keyVal)){
    if (keyb.IsKeyPressed()) ProcessKeyPress(keyName,keyVal);}
// Finally pass the click on to the buttons, if one of them is concerned
    for (int I(0);I<nButtons;I++){//Click on a button ?
        ulong indx=static_cast<ulong>(I);
        if (tButtons[indx]->isHere(mX,mY)){
           buttonClick=true;
           clickresult=I;
           tButtons[indx]->Press();
           return;
           }
    }
}

void LayoutWithEdit::HandleMouseKeepDown(int cX,int cY){
    int mX(cX-l),mY(cY-b);
   must_print=true;
    if (continueClick) tEdFileReader->ProceedClickCont(mX,mY);
}

int LayoutWithEdit::HandleMouseUp(int,int){
    must_print=true;
    EndKeyPress();
    epoch=XPLMGetElapsedTime();
    int retVal=-1;
    if (!editMode) {
        UnfocusPhysicalKeyboard();
        FilePointer::ReleaseBackups();
        retVal=3;}
    if (continueClick){
        continueClick=false;
        buttonClick=false;
        clickresult=-1;
        tEdFileReader->ProceedEndClick();
        /*tEdFileReader->CheckForFrequencies();
        fNav.setText(tEdFileReader->GetNavStr());
        fCom.setText(tEdFileReader->GetComStr());
        fAdf.setText(tEdFileReader->GetADFStr());*/
        CheckButtonsVisibility();}

    if (buttonClick){
        tButtons[clickresult]->Release();
        LaunchCommand(clickresult);
        continueClick=false;
        buttonClick=false;
        clickresult=-1;
        if (!editMode){
            retVal=3;
            FilePointer::ReleaseBackups();
        }
        CheckButtonsVisibility();
    }
    return retVal;
}

void LayoutWithEdit::SetSpecialKey(bool spec){
    specialKey=spec;
}

void LayoutWithEdit::ProcessKeyPress(std::string keyName, std::string in_String){
    //Name of special keys : "BckSp""ShftLck""Return""Shft""Ctrl""Alt""Abrv"
    //should pass on chars to text edit
    //process special keys return (with text edit), bcksp,
    // process control commands
    // then alt chars
    must_print=true;
    if (!specialKey&&(in_String!="")) {
        if (!keyb.IsControlKeyActive())
          {tEdFileReader->InsertLetter(in_String);hasToSave=true;}
        else{
            keyb.ReleaseStates();
            if (keyName=="S"){tEdFileReader->Save();hasToSave=false;} //Save
            if (keyName=="X"){ tEdFileReader->Cut();hasToSave=true;}
            if (keyName=="C"){ tEdFileReader->Copy();hasToSave=true;}
            if (keyName=="V"){ tEdFileReader->Paste();hasToSave=true;}
            if (keyName=="Q"){QuitCommand();} //Quit : Save and Quit
            if (keyName=="A"){FilePointer::RestoreBackup();editMode=false;} //Abandon : Quit without saving
            if (keyName=="D") {tEdFileReader->Suppress();hasToSave=true;}
            if (keyName=="T"){ToggleKeyboard();} //Toggle VR keyboard
            if (keyName=="K"){} //Use physical keyboard (stops processing keyboards commands)
            if (keyName=="R"){tEdFileReader->ReadFileToBuff();hasToSave=false;} //Reload file,discarding non saved changes
            if (keyName=="F"){tEdFileReader->FlipMoveSelectionEnd();} //Flip moveSelectVR
            if (keyName==" "){XPLMCommandOnce(screenShot);}
            if (keyName=="H") {ToggleFPS();}
        }
    }
    if (specialKey){
        if (keyName=="BckSpc") {tEdFileReader->Backspace();hasToSave=true;}
        if (keyName=="TAB") {tEdFileReader->InsertLetter("   ");hasToSave=true;}
        if (keyName=="ShTAB") {tEdFileReader->InsertLetter(tabstring);hasToSave=true;}
        if (keyName=="Enter") {tEdFileReader->Enter();hasToSave=true;}
        if (keyName=="UP") {tEdFileReader->MoveCursorUp();}
        if (keyName=="RIGHT") {tEdFileReader->MoveCursorRight();}
        if (keyName=="LEFT") {tEdFileReader->MoveCursorLeft();}
        if (keyName=="DOWN") {tEdFileReader->MoveCursorDown();}
    }
}

void LayoutWithEdit::ContinueKeyPress(){

}

void LayoutWithEdit::EndKeyPress(){
    must_print=true;
    keyb.ReleaseCurrentKey();
    CheckButtonsVisibility();
}

//The following can also be used by the plugin handlers to launch a custom command
//to implement functionnality by pressing physical commands, keys, menus ...
//enumeration to be found in show_fps_test_global.h

void LayoutWithEdit::LaunchCommand(int refCommand){

    switch(refCommand){
         case B_Edit_Line : {//Cancel Command
           FilePointer::RestoreBackup();
           editMode=false;
           break;
         }
         case B_Load_File:{//Save command
            tEdFileReader->Save();
            hasToSave=false;
            break; }

         case B_Reload:{//Save and quit Command
           tEdFileReader->Save();editMode=false;
             //canUTF=true;
           break;}

         case B_Undo:{//not used in edit mode
             tEdFileReader->UndoDelete();
             break;}

         case B_Auto:{//quit command (doesn't save last modifications but keep modifications since last save)
             //end edit
             QuitCommand();
             break;}

         case B_UTF8:{//Toggle physical keyboard
             canUTF=!canUTF;
             if (canUTF) UsePhysicalKeyboard();
             else UnfocusPhysicalKeyboard();
             break;}

         case B_Toggle_Line:{//not used
             if (enableDelete)
             tEdFileReader->DeleteSelectedLine();
              break;}

         case B_Show_All:{//not implemented here
             tEdFileReader->ShowAll();
             tEdFileReader->SetupforText();
         break;}

         case B_NAV1:{
             XPLMSetDatai(nav1freq,tEdFileReader->freqNAV);
         break;}

         case B_NAV2:{
             XPLMSetDatai(nav2freq,tEdFileReader->freqNAV);
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
             XPLMSetDatai(adf1freq,tEdFileReader->freqADF);
         break;}
         case B_ADF2:{
             XPLMSetDatai(adf2freq,tEdFileReader->freqADF);
         break;}
         case B_NextLine:{
             if (tEdFileReader->HasSelection()){
                int slL=tEdFileReader->SelectedLineNumber();
                tEdFileReader->MoveDnNLines(1);
                tEdFileReader->SelectLine(slL+1);
        }
         break;}
         case B_PrevLine:{
             if (tEdFileReader->HasSelection()){
                int slL=tEdFileReader->SelectedLineNumber();
                tEdFileReader->MoveUpNLines(1);
                tEdFileReader->SelectLine(slL-1);
             }
         break;}
         case B_FirstLine:{
             tEdFileReader->SelectFirstLine();
         break;  }
         }

    CheckButtonsVisibility();
}

void LayoutWithEdit::QuitCommand(){
    myself=this;
    if (hasToSave){
        quitWoSave.MakeAlert("Don't Save","Save","Cancel","All recent modifications will be lost ?",HandleAlertResult);
    }
    else editMode=false;
}

void LayoutWithEdit::HandleAlertResult(){
    int response=myself->quitWoSave.GetAnswer();
    if (response==1) {myself->editMode=false;myself->hasToSave=false;}
    if (response==2) {myself->tEdFileReader->Save();myself->editMode=false;myself->hasToSave=false;}
    if (!myself->editMode) OpCenter::EndEditMode();
 }

void LayoutWithEdit::CheckButtonsVisibility(){
    tButtons[B_Save]->setVisibility(false);
    tButtons[B_Edit_Line]->setVisibility(true);
    tButtons[B_More_Lines]->setVisibility(false);
    tButtons[B_Less_Lines]->setVisibility(false);

    tButtons[B_Load_File]->setVisibility(hasToSave);
    tButtons[B_Reload]->setVisibility(hasToSave);
    tButtons[B_Auto]->setVisibility(true);
    tButtons[B_Undo]->setVisibility(tEdFileReader->CanUndo()&(!hasToSave)&enableDelete);
    tButtons[B_UTF8]->setVisibility(true);
    tButtons[B_Toggle_Line]->setVisibility(tEdFileReader->HasSelection()&(!hasToSave)&(enableDelete));
    tButtons[B_Show_All]->setVisibility(tEdFileReader->HasHiddenLine()&(!hasToSave)&(enableDelete));
}

std::string LayoutWithEdit::GetFileName(){
    return tEdFileReader->GetFileName();
}

void LayoutWithEdit::ToggleKeyboard(){
    showKeyb=!showKeyb;
    if (showKeyb){
        lowerMargin=54+keyb.MyHeight()+4;
    }
    else{
        lowerMargin=54;
    }
    resize();
    myDrawPad->SetNewSize(wWidth,wHeight);
    myDrawPad->GenerateCurrentTexture();
}


void LayoutWithEdit::UsePhysicalKeyboard(){
    keyb.EnablePhysicalKeyboard(this);
    tButtons[B_UTF8]->setSelect(true);
    physK=true;
    temporaryWindow::ShowAlert("Plane's Keyboard commands are disabled",2);
}

void LayoutWithEdit::UnfocusPhysicalKeyboard(){
    keyb.DisablePhysicalKeyboard(this);
    tButtons[B_UTF8]->setSelect(false);
    physK=false;
    temporaryWindow::ShowAlert("Plane's Keyboad commands are enabled",1.5);
}

void LayoutWithEdit::SetTextToShow(string in_text){
   textToShow=in_text;
}

void LayoutWithEdit::EndEdit(){
    editMode=false;
}

void LayoutWithEdit::ClosegWindow(){
   myWindow=nullptr;
   myDrawPad->CloseWindow();
   tEdFileReader->closeReader();
   tButtons.clear();
   wWidth=0;wHeight=0;
   charWidth=0;
   textPointX=0;textPointY=0;
   t=0;b=0;l=0;r=0;
   colWidth=
   30;idxSelected=-1;nButtons=0;
}
void LayoutWithEdit::ToggleFPS(){
    showFPS=!showFPS;
    int offSetH=charHeight+2;
    if (showFPS)
    {
        textHeight-=offSetH;
        upperMargin+=offSetH;
        newSize(vrWidth,vrHeight+offSetH);
    }
    else{
        textHeight+=offSetH;
        upperMargin-=offSetH;
        newSize(vrWidth,vrHeight-offSetH);
    }

}
