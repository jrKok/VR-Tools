#include "layoutwithedit.h"

LayoutWithEdit::LayoutWithEdit() :
    Layout(),
    keyb(),
    showKeyb(true),
    keyPressed(false),
    specialKey(false),
    utfKey(false),
    editMode(true),
    firstChar('\0'),
    secondChar('\0'),
    fName(""),
    tEdFileReader(new TextEdit())


{
 keyb.DefineKeyboard(0);
 keyb.MakeKeyboard(0,0);
 lowerMargin=lowerMargin+keyb.MyHeight()+4;
 if (textWidth<keyb.MyWidth()) textWidth=keyb.MyWidth();
 autoReload=false;
 enableFreqs=true;
 splitLinePolicy=BestSplitAtSpace;
}

//End of constructor

bool LayoutWithEdit::initiate(){
    resize();
    editMode=true;
    tEdFileReader->PointToFile();
    wLeft=gLeft+150;
    wRight=wLeft+wWidth;
    wTop=gTop-100;//moreover this will force a new recalculation at next draw call
    wBottom=wTop-wHeight;
    defineButtons();
    keyb.Relocate(colWidth,generalR.height-lowerMargin+58);
    ReLabelButtons();
    nButtons=tButtons.size();
    return true;
}

void LayoutWithEdit::resize(){//calculate offsets; areas of rectangles}

    if (showFPS) upperMargin+=charHeight;
    tEdFileReader->SetSplitPolicy(BestSplitAtSpace);
    if (textHeight<150) textHeight=150;
    tEdFileReader->Setup(textHeight,textWidth,colWidth,upperMargin);
    tEdFileReader->ReadFileToBuff();
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
            tEdFileReader->Setup(textHeight,textWidth,colWidth,upperMargin);
            tEdFileReader->ReadFileToBuff();
            fitSizeToFile=false;}
        return;
    }
    generalR.width=textWidth+colWidth+10;//10 bx right margin
    generalR.height=textHeight+upperMargin+lowerMargin;//offsetY upper margin and lower margin
    wWidth=generalR.width;
    wHeight=generalR.height;
    vrWidth=wWidth;
    vrHeight=wHeight;
    if (XPLMWindowIsInVR(myWindow)==1){
       if (myWindow!=nullptr) XPLMSetWindowGeometryVR(myWindow,vrWidth,vrHeight);}
    else {
       if (myWindow!=nullptr) {
          XPLMGetWindowGeometry(myWindow,&l,&t,&r,&b);
          //vrWidth=r-l;vrHeight=t-b;
          XPLMSetWindowGeometry(myWindow,l,t,l+vrWidth,t-vrHeight);
       }
    }
    generalR.offsetX=0;
    generalR.offsetY=0;
    minHeight=tEdFileReader->GetOffSetY()+180;
    maxHeight=tEdFileReader->GetOffSetY()+935;
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
     tEdFileReader->Recalculate(l,t);

     int middle=tEdFileReader->GetOffSetY()+(tEdFileReader->GetHeight()/2);
     RelocateButtons(middle);
     keyb.Relocate(colWidth,generalR.height-lowerMargin+58);
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
    generalR.width=textWidth+colWidth+10;//10 bx right margin
    generalR.height=textHeight+tEdFileReader->GetOffSetY()+lowerMargin;//offsetY upper margin and 54lower margin
    wWidth=generalR.width;
    wHeight=generalR.height;
    if (keepSize) KeepCurrentSize();
}

void LayoutWithEdit::ReLabelButtons(){
    tButtons[B_Load_File].setText("Save");
    tButtons[B_Reload].setText("Save&Quit");
    tButtons[B_Auto].setText("Quit");
    tButtons[B_Edit_Line].setText("Cancel");
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

    if ((t!=wTop)|(l!=wLeft)|newGeometry){
        if (!useBackGround){
           tEdFileReader->SetInkColor(colorPaper);
           tEdFileReader->SetBackGround(false);}
        wTop=t;
        wLeft=l;
        generalR.in_left=l;
        generalR.in_top=t;
        generalR.recalculate();
        tEdFileReader->Recalculate(l,t);
        fNav.recalculate(l,t);
        fCom.recalculate(l,t);
        fAdf.recalculate(l,t);
        lFPS.recalculate(l,t);
        for (unsigned long long I(0);I<nButtons;I++){
            tButtons[I].in_top=t;
            tButtons[I].in_left=l;
            tButtons[I].recalculate();
        }
        keyb.Recalculate(l,t);
}
}

void LayoutWithEdit::DrawTextW(XPLMWindowID g_textWindow){
    //intialize
    XPLMGetWindowGeometry(g_textWindow, &l, &t, &r, &b);
    recalculate();

    XPLMSetGraphicsState(
        0 , 0, 0, 0 , 1, 1, 0);



    float sunPtc = XPLMGetDataf(dref_SunPitch); //time of day ?

     //draw commands for the buttons
    for (auto tb:tButtons){tb.drawButton();}
     //draw text's background and set color of characters (ink)
    if (useBackGround){
       if (sunPtc>=5){//sun bright in sky
          tEdFileReader->SetBckColor(colorPaper);
          tEdFileReader->SetInkColor(colorInk);
       }else{
          if ((sunPtc>-3)&(sunPtc<5)){//sun at sunset darker duron paper white
             tEdFileReader->SetBckColor(paperWhitesomber);
             tEdFileReader->SetInkColor(colorInk);
                                       }
          else {
             tEdFileReader->SetBckColor(blackpaper);//night paper
             tEdFileReader->SetInkColor(amberInk);}
               }
       }

    tEdFileReader->DrawMySelf(); //Draw Text and background if necessary

    if (tEdFileReader->HasNav()) fNav.DrawTextLine();
    if (tEdFileReader->HasCom()) fCom.DrawTextLine();
    if (tEdFileReader->HasADF()) fAdf.DrawTextLine();

    if (showKeyb) keyb.DrawKb();
}

void LayoutWithEdit::DrawNoResize(XPLMWindowID g_textWindow){
    DrawTextW(g_textWindow);//always resize
}

void LayoutWithEdit::findClick(int mX, int mY){

    if (continueClick||buttonClick){
        if (clickresult>-1) tButtons[clickresult].Release();
        if (continueClick) tEdFileReader->ProceedEndClick();
        autoReload=saveAuto;
        continueClick=false;
        buttonClick=false;
        clickresult=-1;
        return;//if previous mouse click had been released outside window
        //discharge all pending mousedown and drag handlers and discard this click
    }

    if (tEdFileReader->ProceedClick(mX,mY)){//Click in the text or scrB ? will handle the cursor
        CheckButtonsVisibility();
        continueClick=true;
        return;
      }
    std::string keyName,keyVal;
    if (keyb.PressKey(mX,mY,specialKey,keyName,keyVal)){
    if (keyb.IsKeyPressed()) ProcessKeyPress(keyName,keyVal);}

    for (int I(0);I<nButtons;I++){       //Click on a button ?
      if (tButtons[I].isHere(mX,mY)){
           buttonClick=true;
           clickresult=I;
           tButtons[I].Press();
           return;
           }
    }

}
void LayoutWithEdit::ProcessKeyPress(std::string keyName, std::string in_String){
    //Name of special keys : "BckSp""ShftLck""Return""Shft""Ctrl""Alt""Abrv"
    //should pass on chars to text edit
    //process special keys return (with text edit), bcksp,
    // process control commands
    // then alt chars
    if (!specialKey&&(in_String!="")) {
        if (!keyb.IsControlKeyActive())
            tEdFileReader->InsertLetter(in_String);
        else{
            keyb.ReleaseStates();
            if (keyName=="S"){tEdFileReader->Save();} //Save
            if (keyName=="X"){ tEdFileReader->Cut();}
            if (keyName=="C"){ tEdFileReader->Copy();}
            if (keyName=="V"){ tEdFileReader->Paste();}
            if (keyName=="Q"){tEdFileReader->Save();editMode=false;} //Quit : Save and Quit
            if (keyName=="A"){editMode=false;} //Abandon : Quit without saving
            if (keyName=="D") {tEdFileReader->Suppress();}
            if (keyName=="T"){ToggleKeyboard();} //Toggle VR keyboard
            if (keyName=="K"){} //Use physical keyboard (stops processing keyboards commands)
            if (keyName=="R"){tEdFileReader->ReadFileToBuff();} //Reload file,discarding non saved changes
            if (keyName=="F"){tEdFileReader->FlipMoveSelectionEnd();} //Flip moveSelectVR
        }
    }
    if (specialKey){
        if (keyName=="BckSpc") {tEdFileReader->Backspace();}
        if (keyName=="TAB") {tEdFileReader->InsertLetter("   ");}
        if (keyName=="Enter") {tEdFileReader->Enter();}
        if (keyName=="UP") {tEdFileReader->MoveCursorUp();}
        if (keyName=="RIGHT") {tEdFileReader->MoveCursorRight();}
        if (keyName=="LEFT") {tEdFileReader->MoveCursorLeft();}
        if (keyName=="DOWN") {tEdFileReader->MoveCursorDown();}
    }


}

void LayoutWithEdit::ContinueKeyPress(){

}

void LayoutWithEdit::EndKeyPress(){
    keyb.ReleaseCurrentKey();
}

void LayoutWithEdit::HandleMouseKeepDown(int mX,int mY){
    if (continueClick) tEdFileReader->ProceedClickCont(mX,mY);
}

int LayoutWithEdit::HandleMouseUp(int mX,int mY){
    EndKeyPress();
    autoReload=saveAuto;
    epoch=XPLMGetElapsedTime();
    int retVal=-1;
    if (!editMode) retVal=3;
    if (continueClick){
        continueClick=false;
        buttonClick=false;
        clickresult=-1;
        tEdFileReader->ProceedEndClick();
        tEdFileReader->CheckForFrequencies();
        fNav.setText(tEdFileReader->GetNavStr());
        fCom.setText(tEdFileReader->GetComStr());
        fAdf.setText(tEdFileReader->GetADFStr());
        CheckButtonsVisibility();}

    if (buttonClick){
        tButtons[clickresult].Release();
        if (clickresult==B_Load_File) tEdFileReader->Save();
        else LaunchCommand(clickresult);
        continueClick=false;
        buttonClick=false;
        clickresult=-1;
        if (!editMode) retVal=3;
    }
    return retVal;
}

void LayoutWithEdit::PhysicalKeyPressed(char in_char,XPLMKeyFlags flag,char in_VK){
    ProcessKeyPress(keyb.Physical_Key_Handler(in_char, flag, in_VK),"");
}
//The following can also be used by the plugin handlers to launch a custom command
//to implement functionnality by pressing physical commands, keys, menus ...
//enumeration to be found in show_fps_test_global.h

void LayoutWithEdit::LaunchCommand(int refCommand){

    switch(refCommand){
    case B_Edit_Line : {
        tEdFileReader->Reload();
        break;
    }
    case B_Reload:{
        tEdFileReader->Save();editMode=false;;
             //canUTF=true;
        break;}

    case B_Undo:{
        tEdFileReader->UndoDelete();
        break;}

         case B_Auto:{//autoreload not feasible, used to end edit mode
             //end edit
             editMode=false;

         break;}

         case B_UTF8:{
             tEdFileReader->SetNeedsUTF(true);
             canUTF=false;
         break;}

         case B_Toggle_Line:{
             if (enableDelete)
             tEdFileReader->DeleteSelectedLine();
         break;}

         case B_Show_All:{
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
             float val=tEdFileReader->freqCOM;
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
             float val=tEdFileReader->freqCOM;
             XPLMSetDatai(com2freq,(int)val);
             val-=(float)XPLMGetDatai(com2freq);
             if (val!=0){
                 frac=XPLMGetDatai(com2freqk);
                 frac+=(int)(val*10.0);
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

void LayoutWithEdit::CheckButtonsVisibility(){
    tButtons[B_Save].isVisible       =false;
    tButtons[B_Edit_Line].isVisible  =false;
    tButtons[B_More_Lines].isVisible =false;
    tButtons[B_Less_Lines].isVisible =false;

    tButtons[B_Load_File].isVisible  =true;
    tButtons[B_Reload].isVisible     =!autoReload;
    tButtons[B_Auto].isVisible       =true;
    tButtons[B_Undo].isVisible       =tEdFileReader->CanUndo()&(!autoReload)&enableDelete;
    tButtons[B_UTF8].isVisible       =!(tEdFileReader->HasSelection())&(!autoReload)&canUTF;
    tButtons[B_Toggle_Line].isVisible=tEdFileReader->HasSelection()&(!autoReload)&(enableDelete);
    tButtons[B_Show_All].isVisible   =tEdFileReader->HasHiddenLine()&(!autoReload)&(enableDelete);

    tButtons[B_NAV1].isVisible       =tEdFileReader->HasNav()&XPLMGetDatai(nav1on)&enableFreqs;
    tButtons[B_NAV2].isVisible       =tEdFileReader->HasNav()&XPLMGetDatai(nav2on)&enableFreqs;
    tButtons[B_COM1].isVisible       =tEdFileReader->HasCom()&XPLMGetDatai(com1on)&enableFreqs;
    tButtons[B_COM2].isVisible       =tEdFileReader->HasCom()&XPLMGetDatai(com2on)&enableFreqs;
    tButtons[B_ADF1].isVisible       =tEdFileReader->HasADF()&XPLMGetDatai(adf1on)&enableFreqs;
    tButtons[B_ADF2].isVisible       =tEdFileReader->HasADF()&XPLMGetDatai(adf2on)&enableFreqs;

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
}

void LayoutWithEdit::UsePhysicalKeyboard(){

}

void LayoutWithEdit::EndEdit(){
    editMode=false;
}

void LayoutWithEdit::ClosegWindow(){
        myWindow=nullptr;
        tEdFileReader->closeReader();
        tButtons.clear();
        gTop=0;gBottom=0;gLeft=0;gRight=0;
        wTop=0;wBottom=0;wLeft=0;wRight=0;
        wWidth=0;wHeight=0;
        charWidth=0;
        textPointX=0;textPointY=0;
        t=0;b=0;l=0;r=0;
        colWidth=30;idxSelected=-1;nButtons=0;
}
