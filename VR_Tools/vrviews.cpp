#include "vrviews.h"
#include "managemodalwindow.h"
#include "fontman.h"

#include "vrcommandfilter.h"

VrViews       *VrViews::myself;



VrViews::VrViews():List_Box_With_ScrB (true),

    CmdRight(nullptr),
    CmdLeft(nullptr),
    CmdUp(nullptr),
    CmdDown(nullptr),
    CmdForward(nullptr),
    CmdBackward(nullptr),
    CommandLaunched(nullptr),

    yesButton(nullptr),
    noButton(nullptr),
    cancelButton(nullptr),
    relogButton(nullptr),
    upButton(nullptr),
    downButton(nullptr),
    renameButton(nullptr),
    createButton(nullptr),
    deleteButton(nullptr),
    repositionButton(nullptr),
    vaftButton(nullptr),
    vforwButton(nullptr),
    vupButton(nullptr),
    vdownButton(nullptr),
    vleftButton(nullptr),
    vrightButton(nullptr),
    clicked(nullptr),
    width(320),
    height(400),
    answer(0),
    myStringN2(0),
    myStringN3(0),
    textOffsetX(0),
    textOffsetY(0),
    selectedHotsp(0),
    targetX(0),
    targetY(0),
    targetZ(0),
    userLine(""),
    keyb(nullptr),
    editLine(true),
    logCoords(true),
    tcursor(),
    specialKey(false),
    actionLaunched(false),
    mightSave(false),
    disableEdit(false),
    filterblock(true),
    action(0),
    mouseLocated(false),
    left(0),top(0),bottom(0),right(0),
    myXPWindow(nullptr),
    forCursor(),
    mouseDrag(false),
    mouseUp(true),
    epochClick(0),
    callBack(nullptr)

{
}

VrViews::~VrViews(){
    if (       yesButton!=nullptr) delete yesButton;
    if (        noButton!=nullptr) delete noButton;
    if (    cancelButton!=nullptr) delete cancelButton;
    if (        upButton!=nullptr) delete upButton;
    if (      downButton!=nullptr) delete downButton;
    if (    createButton!=nullptr) delete createButton;
    if (    deleteButton!=nullptr) delete deleteButton;
    if (    renameButton!=nullptr) delete renameButton;
    if (repositionButton!=nullptr) delete repositionButton;
    if (            keyb!=nullptr) delete keyb;
    if (     vdownButton!=nullptr) delete vdownButton;
    if (       vupButton!=nullptr) delete vupButton;
    if (    vrightButton!=nullptr) delete vrightButton;
    if (     vleftButton!=nullptr) delete vleftButton;
    if (      vaftButton!=nullptr) delete vaftButton;
    if (     vforwButton!=nullptr) delete vforwButton;
    callBack=nullptr;
}

void VrViews::MakeDialog(const string &yesStr, const string &noStr, const string &cancelStr, const string alertStr, const std::string &in_String, std::function<void()>cBck, int in_width){
    /* geometry :
     * general text line at 5
     * list box for hotspots : 10 lines , 120 height,250 width, offset x : 30 for buttons, y 20 from upside
     * buttons under box : 30 (y 140)
     * text line at y=170 height 20
     * line for editing text 25 y 190
     * warning line y 115 height 20
     * buttons for general commands (SAVE, MOVE TO, CANCEL) height 30, y 135
     * keyb  y 165
*/
    int buttonHeight(20);
    int buttonwidth(80),cmdBWidth(60),textWidth(0),pos(20);
    int posFirstLine(0),posSecondLine(0),posListBox(0),posCommandButtons(0),posThirdLine(0),posEditLine(0),posButtons(0),posKeyb(0);

    CmdRight      = XPLMFindCommand("sim/general/right_slow");
    CmdLeft       = XPLMFindCommand("sim/general/left_slow");
    CmdUp         = XPLMFindCommand("sim/general/up_slow");
    CmdDown       = XPLMFindCommand("sim/general/down_slow");
    CmdForward    = XPLMFindCommand("sim/general/forward_slow");
    CmdBackward   = XPLMFindCommand("sim/general/backward_slow");

    disableEdit=false;
    callBack=cBck;

    myself=this;
    userLine=in_String;
    width=in_width;
    std::function<void()> upd=std::bind(&VrViews::Update,this);
    myXPWindow=ManageModalWindow::CreateMousedModalWindow(MouseHandler,DrawMyself,Clr_LighterGray,width,height);
    ManageModalWindow::AddUpdatefunction(upd);
    answer=0;
    left=0;
    top=0;
    selectedHotsp=0;
    action=0;

    //position keyboard
    posKeyb=10;
    forCursor.clear();
    forCursor.push_back(userLine);
    keyb=new Keyboard(true);
    keyb->MakeKeyboard(5,posKeyb);

    //define heightzones
    posButtons= keyb->MyHeight()+posKeyb+10;
    posEditLine=posButtons+buttonHeight+5;
    posThirdLine=posEditLine+25;
    posCommandButtons=posThirdLine+12;
    posListBox=posCommandButtons+buttonHeight+5;
    posSecondLine=posListBox+170;
    posFirstLine=posSecondLine+15;

    //general dimensions and setup listbox
    height=posFirstLine+20;
    textOffsetX=10;textOffsetY=posFirstLine;
    Setup(160,240,88,posListBox);

    //compute width vs keyboard
    int kwidth = keyb->MyWidth()+10;
    width=width>kwidth?width:kwidth;
    tcursor.Initiate(&forCursor,10,posEditLine+10,20,1);

    //compute width vs textline
    int strwidth=fontMan::MeasureString(alertStr);
    textWidth=strwidth+textOffsetX+textOffsetX;
    if (textWidth>width) {
        width=textWidth;
        pos=(width-((buttonwidth+20)*4))/2;
    }
    else{
        textOffsetX=(width-textWidth)/2;
        pos=(width-((buttonwidth+20)*4))/2;
    }

    keyb->Relocate(width/2-keyb->MyWidth()/2,posKeyb);

    ManageModalWindow::ResizeModalWindow(width,height);
    point p;
    logCoords.SetDimensions(width-textOffsetX,12);
    logCoords.SetOrigin(textOffsetX,textOffsetY);
    logCoords.SetBackGroundColor(Clr_LighterGray);
    logCoords.setText(alertStr);
    p.SetCoords(100,posSecondLine);
    myStringN2=DrawLogic::AddString("If needed, Select a hotspot :",Clr_BlackInk,Clr_LighterGray,p);
    p.SetCoords(textOffsetX,posThirdLine);
    myStringN3=DrawLogic::AddString("Type a name to create a hotspot or rename an existing one :",Clr_BlackInk,Clr_LighterGray,p);

    yesButton=        new button_VR("yesButton",false);
    noButton=         new button_VR("noButton",false);
    cancelButton=     new button_VR("cancelButton",false);
    advancedButton=   new button_VR("advancedButton",false);

    relogButton=      new button_VR("relog",false);
    vupButton  =      new button_VR("vup",false);
    vdownButton=      new button_VR("vdown",false);
    vforwButton=      new button_VR("vforw",false);
    vaftButton =      new button_VR("vbackw",false);
    vleftButton=      new button_VR("vleftButton",false);
    vrightButton=     new button_VR("vright",false);
    vaftButton =      new button_VR("vaft",false);
    upButton=         new button_VR("up",false);
    downButton=       new button_VR("down",false);

    renameButton=     new button_VR("rename",false);
    createButton=     new button_VR("create",false);
    deleteButton=     new button_VR("delete",false);
    repositionButton= new button_VR("reposition",false);

    yesButton->SetDimensions(buttonwidth,buttonHeight);
    yesButton->SetOrigin(pos,posButtons);
    yesButton->setText(yesStr);
    yesButton->setVisibility(yesStr!=""?true:false);
    yesButton->SetToStateColor();
    pos+=buttonwidth+20;

    noButton->SetDimensions(buttonwidth,buttonHeight);
    noButton->SetOrigin(pos,posButtons);
    noButton->setText(noStr);
    noButton->setVisibility(noStr!=""?true:false);
    noButton->SetToStateColor();
    pos+=buttonwidth+20;

    cancelButton->SetDimensions(buttonwidth,buttonHeight);
    cancelButton->SetOrigin(pos,posButtons);
    cancelButton->setText(cancelStr);
    cancelButton->SetToStateColor();
    cancelButton->setVisibility(cancelStr!=""?true:false);
    pos+=buttonwidth+20;

    advancedButton->SetDimensions(buttonwidth,buttonHeight);
    advancedButton->SetOrigin(pos,posButtons);
    advancedButton->setText("Advanced");
    advancedButton->SetToStateColor();
    advancedButton->setVisibility(true);

    relogButton->SetDimensions(40,40);
    relogButton->SetOrigin(365,textOnly.GetTop()-relogButton->GetHeight());
    relogButton->setText("Relog");
    relogButton->setButtonColor(Clr_Green);
    relogButton->setVisibility(true);

    vforwButton->SetDimensions(20,20);
    vforwButton->SetOrigin(375,relogButton->GetBottom()-22);
    vforwButton->setText("\xcb\x84");
    vforwButton->SetToStateColor();
    vforwButton->setVisibility(true);

    vleftButton->SetDimensions(20,20);
    vleftButton->SetOrigin(359,vforwButton->GetBottom()-22);
    vleftButton->setText("\xcb\x82");
    vleftButton->SetToStateColor();
    vleftButton->setVisibility(true);

    vrightButton->SetDimensions(20,20);
    vrightButton->SetOrigin(391,vleftButton->GetBottom());
    vrightButton->setText("\xcb\x83");
    vrightButton->SetToStateColor();
    vrightButton->setVisibility(true);

    vaftButton->SetDimensions(20,20);
    vaftButton->SetOrigin(375,vleftButton->GetBottom()-22);
    vaftButton->setText("\xcb\x85");
    vaftButton->SetToStateColor();
    vaftButton->setVisibility(true);

    vupButton->SetDimensions(20,20);
    vupButton->SetOrigin(330,vleftButton->GetBottom());
    vupButton->setText("\xe2\x86\xa5");
    vupButton->SetToStateColor();
    vupButton->setVisibility(true);

    vdownButton->SetDimensions(20,20);
    vdownButton->SetOrigin(419,vleftButton->GetBottom());
    vdownButton->setText("\xe2\x86\xa7");
    vdownButton->SetToStateColor();
    vdownButton->setVisibility(true);

    pos=20;
    upButton->SetDimensions(20,buttonHeight);
    upButton->SetOrigin(50,textOnly.GetHeight()/2+5+textOnly.GetBottom());
    upButton->setText("\xe2\x86\x91");
    upButton->SetToStateColor();
    upButton->setVisibility(true);

    downButton->SetDimensions(20,buttonHeight);
    downButton->SetOrigin(50,upButton->GetBottom()-10-buttonHeight);
    downButton->setText("\xe2\x86\x93");
    downButton->SetToStateColor();
    downButton->setVisibility(true);

    createButton->SetDimensions(cmdBWidth,buttonHeight);    
    createButton->SetOrigin(pos,posCommandButtons);
    createButton->setText("Create");
    createButton->SetToStateColor();
    createButton->setVisibility(true);
    pos=pos+cmdBWidth+2;

    renameButton->SetDimensions(cmdBWidth,buttonHeight);
    renameButton->SetOrigin(pos,posCommandButtons);
    renameButton->setText("Rename");
    renameButton->SetToStateColor();
    renameButton->setVisibility(true);
    pos=pos+cmdBWidth+2;

    repositionButton->SetDimensions(cmdBWidth,buttonHeight);
    repositionButton->SetOrigin(pos,posCommandButtons);
    repositionButton->setText("Update");
    repositionButton->SetToStateColor();
    repositionButton->setVisibility(true);
    pos=pos+cmdBWidth+cmdBWidth+2;

    deleteButton->SetDimensions(cmdBWidth,buttonHeight);
    deleteButton->SetOrigin(pos,posCommandButtons);
    deleteButton->setText("Delete");
    deleteButton->SetToStateColor();
    deleteButton->setVisibility(true);

    editLine.SetDimensions(width-20,15);
    editLine.SetOrigin(10,posEditLine);
    editLine.setText(in_String);
    editLine.PrintString();

    ManageModalWindow::ResizeModalWindow(width,height);
    keyb->SetVisibility(true);
    SetSplitPolicy(ForceSplitAtSize);
    ShowAll();
    CheckButtonsVisibility();
    DrawLogic::UpdateTexture();
    editLine.PrintString();
    epochClick=0;

    //activate physical keyboard, rebuild keyboard for handling different types of callers

}
void VrViews::DisableEdit(){
    disableEdit=true;
}

bool VrViews::IsEditDisabled(){
    return disableEdit;
}

int  VrViews::GetAnswer(){
 return answer;
}

string VrViews::GetUserLine(){
    return userLine;
}

int  VrViews::GetSelectedHotspot(){
    return selectedHotsp;
}

int VrViews::GetActionLaunched(){
    return action;
}

void VrViews::DrawMyself(XPLMWindowID, void *){
    ManageModalWindow::MakeTopWindow();
    DrawLogic::RenderContent();
    myself->tcursor.DrawCursor();
}

int VrViews::MouseHandler(XPLMWindowID in_window_id, int in_x, int in_y, int is_down, void *){
    ManageModalWindow::MakeTopWindow();
    int x(in_x-ManageModalWindow::GetLeft()),y(in_y-ManageModalWindow::GetBottom());
    switch (is_down){
    case xplm_MouseDown:{
        myself->epochClick=0;
        myself->mouseUp=false;
        if(!XPLMIsWindowInFront(in_window_id))
        {
            XPLMBringWindowToFront(in_window_id);
        }else{
            myself->mouseDrag=false;
            myself->mouseLocated=false;
            myself->ProceedClick(x,y);
            if (!myself->needToContClick){
                if (myself->yesButton->IsHere(x,y)){
                    myself->mouseLocated=true;
                    myself->answer=1;
                    myself->clicked=myself->yesButton;
                }
                if (myself->noButton->IsHere(x,y)){
                   myself-> mouseLocated=true;
                    myself->answer=2;
                    myself->clicked=myself->noButton;
                }
                if (myself->cancelButton->IsHere(x,y)){
                    myself->mouseLocated=true;
                    myself->answer=3;
                    myself->clicked=myself->cancelButton;
                }

                if (myself->advancedButton->IsHere(x,y)){
                    myself->mouseLocated=true;
                    myself->answer=4;
                    myself->clicked=myself->advancedButton;
                }

                if (myself->relogButton->IsHere(x,y)){
                    myself->LaunchAction(action_Relog);
                    myself->clicked=myself->relogButton;
                }

                if (myself->upButton->IsHere(x,y)){
                    myself->LaunchAction(action_Up);
                    myself->clicked=myself->upButton;

                }
                if (myself->downButton->IsHere(x,y)){
                    myself->LaunchAction(action_Down);
                    myself->clicked=myself->downButton;
                }
                if (myself->renameButton->IsHere(x,y)){
                    myself->LaunchAction(action_Rename);
                    myself->clicked=myself->renameButton;
                }
                if (myself->createButton->IsHere(x,y)){
                    myself->LaunchAction(action_Create);
                    myself->clicked=myself->createButton;
                }
                if (myself->repositionButton->IsHere(x,y)){
                    myself->LaunchAction(action_Reposition);
                    myself->clicked=myself->repositionButton;
                }
                if (myself->deleteButton->IsHere(x,y)){
                    myself->LaunchAction(action_Delete);
                    myself->clicked=myself->deleteButton;
                }
                if (myself->vaftButton->IsHere(x,y)){
                    myself->LaunchAction(action_MoveAft);
                    myself->clicked=myself->vaftButton;
                }
                if (myself->vforwButton->IsHere(x,y)){
                    myself->LaunchAction(action_MoveFor);
                    myself->clicked=myself->vforwButton;
                }
                if (myself->vupButton->IsHere(x,y)){
                    myself->LaunchAction(action_MoveUp);
                    myself->clicked=myself->vupButton;
                }
                if (myself->vdownButton->IsHere(x,y)){
                    myself->LaunchAction(action_MoveDown);
                    myself->clicked=myself->vdownButton;
                }
                if (myself->vrightButton->IsHere(x,y)){
                    myself->LaunchAction(action_MoveRight);
                    myself->clicked=myself->vrightButton;
                }
                if (myself->vleftButton->IsHere(x,y)){
                    myself->LaunchAction(action_MoveLeft);
                    myself->clicked=myself->vleftButton;
                }

                std::string keyName,keyVal;
                if (myself->keyb->PressKey(x,y,myself->specialKey,keyName,keyVal)){
                    if (myself->keyb->IsKeyPressed()) myself->ProcessKeyPress(keyName,keyVal);
                }
                if (myself->editLine.isHere(x,y)){
                    myself->tcursor.FindPos(0,x);
                    myself->epochClick=XPLMGetElapsedTime();
                }
                if (myself->clicked!=nullptr) {
                    myself->clicked->Press();
                }
            }
        }
        break;
    }

    case xplm_MouseDrag:{
 ManageModalWindow::MakeTopWindow();
        myself->ContinueKeyPress();
        myself->ProceedClickCont(x,y);
        if (myself->epochClick>0){
           float now=XPLMGetElapsedTime();
           if ((now-myself->epochClick)>0.3f) myself->mouseDrag=true;
           if (myself->mouseDrag){
               myself->tcursor.DragPos(0,x);
            }
        }
        if (x>(myself->width-4)||x<4||y>(myself->height-4)||y<4)
        {
            myself->MouseToUp();
        }
        break;
    }

    case xplm_MouseUp:{
            myself->MouseToUp();
            return 1;
        }
    }

    return 1;
}

void VrViews::MouseToUp(){
     ManageModalWindow::MakeTopWindow();
    myself->mouseUp=true;
    if (myself->mouseDrag) {
        myself->tcursor.EndOfDrag();
        myself->epochClick=0;
    }
    myself->keyb->ReleaseCurrentKey();
    if (myself->clicked!=nullptr){
        myself->clicked->Release();
        myself->clicked=nullptr;

        if (myself->action>=8 && myself->action<=13 && myself->CommandLaunched!=nullptr){
            XPLMCommandEnd(myself->CommandLaunched);
            myself->action=action_Relog;
            myself->actionLaunched=true;
            myself->callBack();
            myself->CommandLaunched=nullptr;
            string toPass="View logged at X="
                          +stringOps::ConvertFloatToString(myself->targetX)
                    +" Y="+stringOps::ConvertFloatToString(myself->targetY)
                    +" Z="+stringOps::ConvertFloatToString(myself->targetZ)
                    +" PSI="+stringOps::ConvertFloatToString(0);
            logCoords.setTextAndUpdate(toPass);
            myself->actionLaunched=false;
            VRCommandFilter::commandBlock=myself->filterblock;
            myself->action=0;
        }
    }
    myself->ProceedEndClick();
    myself->SetInkColor(Clr_BlackInk);
    myself->selectedHotsp=myself->SelectedLineNumber();
    myself->CheckButtonsVisibility();
    editLine.PrintBox();
    if (myself->tcursor.HasCursor()) DrawLogic::SetCursorPosition(myself->tcursor.PosToX(),myself->editLine.GetTextY()+2);
    if (myself->tcursor.HasSelection()) {
        int l(0),r(0);
        if (tcursor.IsIndexInSelection(0,l,r)){
            DrawLogic::PrintRectOnTexture(l,editLine.GetBottom(),r,editLine.GetTop(),Clr_TextSelectBlue);}
    }
    editLine.PrintStringOnLocalT();
    if (mouseLocated) {
        myself->EndAlert();}
}

void VrViews::LaunchAction(int in_action){
    answer=0;
    action=in_action;
    selectedHotsp=SelectedLineNumber();
    switch (action){
    case action_Relog:{
        actionLaunched=true;
        callBack();
        string toPass="View logged at X="
                      +stringOps::ConvertFloatToString(targetX)
                +" Y="+stringOps::ConvertFloatToString(targetY)
                +" Z="+stringOps::ConvertFloatToString(targetZ)
                +" PSI="+stringOps::ConvertFloatToString(0);
        logCoords.setTextAndUpdate(toPass);
        actionLaunched=false;

    }break;
    case action_Up:{
        //internal action but calls back for immediate action
        if (hasSelection){
            if (selectedHotsp>1) {
                actionLaunched=true;
                callBack();
                ShowAll();
                actionLaunched=false;
                mightSave=true;
            }
        }
        break;
    }
    case action_Down:{
        //internal action but calls back for immediate action
        if (hasSelection){
            if ((selectedHotsp>0)&&(selectedHotsp<(totalNbL-1))) {
                actionLaunched=true;
                callBack();
                ShowAll();
                actionLaunched=false;mightSave=true;
            }
        }
    }break;
    case action_Create:{
        answer=0;
        if (userLine!=""){
            actionLaunched=true;mightSave=true;
            callBack();
        }
    }break;
    case action_Delete:{
        if (hasSelection){
            if (selectedHotsp>0){//you cannot delete pilote position
                actionLaunched=true;
                selectedHotsp=SelectedLineNumber();
                callBack();
                ShowAll();
                mightSave=true;
                actionLaunched=false;
            }
        }
    }break;
    case action_Reposition:{
        if (hasSelection){
            selectedHotsp=SelectedLineNumber();
            actionLaunched=true;
            callBack();
            mightSave=true;
        }
    }break;
    case action_Rename:{
        if (hasSelection&&userLine!=""){
            selectedHotsp=SelectedLineNumber();
            actionLaunched=true;
            callBack();
            mightSave=true;
            ShowAll();
        }break;
    case action_MoveAft:{
            CommandLaunched=CmdBackward;
            LaunchMoveCommand();
        }break;
    case action_MoveFor:{
            CommandLaunched=CmdForward;
            LaunchMoveCommand();
            }break;
    case action_MoveRight:{
            CommandLaunched=CmdRight;
            LaunchMoveCommand();
            }break;
    case action_MoveLeft:{
            CommandLaunched=CmdLeft;
            LaunchMoveCommand();
            }break;
    case action_MoveUp:{
            CommandLaunched=CmdUp;
            LaunchMoveCommand();
            }break;
    case action_MoveDown:{
            CommandLaunched=CmdDown;
            LaunchMoveCommand();
            }break;
    }
    }
}

void VrViews::LaunchMoveCommand(){
    filterblock=VRCommandFilter::commandBlock;
    VRCommandFilter::commandBlock=false;
    XPLMCommandBegin(CommandLaunched);
}


void VrViews::CheckButtonsVisibility(){
    bool utext=(userLine!=""),selc=myself->hasSelection;

    myself->upButton->        setVisibility(selc&&!actionLaunched&&!disableEdit&&(lineSelected>1));
    myself->downButton->      setVisibility(selc&&!actionLaunched&&!disableEdit&&(lineSelected>0)&&(lineSelected<(totalNbL-1)));
    myself->createButton->    setVisibility(!selc&&!actionLaunched&&!disableEdit&&utext);
    myself->deleteButton->    setVisibility(selc&&!actionLaunched&&!disableEdit);
    myself->repositionButton->setVisibility(selc&&!actionLaunched&&!disableEdit);
    myself->renameButton->    setVisibility(selc&&!actionLaunched&&!disableEdit&&utext);
    myself->advancedButton->  setVisibility(selc&&!disableEdit);
    myself->noButton->        setVisibility(selc&&!actionLaunched&&!disableEdit);
    myself->yesButton->       setVisibility(!disableEdit&&mightSave);
    myself->cancelButton->    setVisibility(true);
}

void VrViews::Relog(float inX, float inY, float inZ){
 targetX=inX;
 targetY=inY;
 targetZ=inZ;
}

void VrViews::ProcessKeyPress(std::string keyName,std::string in_String){
    if (!specialKey&&(in_String!="")) {
        if (!keyb->IsControlKeyActive())
          {myself->InsertLetter(in_String);}
        else{
            keyb->ReleaseStates();
            if (keyName=="X"){Cut();}
            if (keyName=="C"){Copy();}
            if (keyName=="V"){Paste();}
            if (keyName=="Q"){mouseLocated=true;answer=1;} //Like Return
            if (keyName=="A"){mouseLocated=true;answer=3;} //Like Cancel
            if (keyName=="D") {myself->Supress();}//Reload file,discarding non saved changes
        }
    }
    if (specialKey){
        if (keyName=="BckSpc") {Backspace();}
        if (keyName=="TAB") {InsertLetter(" ");}
        if (keyName=="ShTAB") {InsertLetter(" ");}
        if (keyName=="Enter") {}
        if (keyName=="RIGHT") {myself->MoveCursorRight();}
        if (keyName=="LEFT") {myself->MoveCursorLeft();}
    }
}

void VrViews::Cut(){
    Copy();
    if (tcursor.HasSelection()) EraseSelection();
}

void VrViews::Copy(){
    if (tcursor.HasSelection()){
        string copyString("");
         ulong bPos(static_cast<ulong>(tcursor.GetSelectionStartCharPos())),
               ePos(static_cast<ulong>(tcursor.GetSelectionEndCharPos()));
        int bp=tcursor.FindPositionInNativeLine(userLine,static_cast<int>(bPos));
        int ep=tcursor.FindPositionInNativeLine(userLine,static_cast<int>(ePos));
        copyString=userLine.substr(bp,ep-bp);
    }

}

void VrViews::Paste(){
string in_clipboard=LocalClipBoard::PullText();
InsertLetter(in_clipboard);
}

void VrViews::Backspace(){
    if (tcursor.HasCursor()){
        int cP=tcursor.GetPos();
        if (cP>0){
            EraseFromLine(cP-1,cP);
            tcursor.SetCursorAt(0,cP-1);
        }
    }
    if (tcursor.HasSelection()){
        int cP=tcursor.GetSelectionStartCharPos();
        EraseSelection();
        tcursor.SetCursorAt(0,cP-1);
    }
}

void VrViews::Supress(){
    if (tcursor.HasCursor()){
        int cP=tcursor.GetPos();
        if (static_cast<ulong>(cP)<(userLine.size()-1)){
            EraseFromLine(cP,cP+1);
        }
    }
    if (tcursor.HasSelection()) EraseSelection();
}

void VrViews::ContinueKeyPress(){

}

bool VrViews::IsLineNotTooWide(){
  float lWidth= XPLMMeasureString(xplmFont_Proportional,userLine.c_str(),static_cast<int>(userLine.size()));
  if (lWidth<width-20) return true; else return false;
}

void VrViews::MoveCursorRight(){
    if (tcursor.HasCursor()) tcursor.MoveCursorRight();
    if (tcursor.HasSelection()) tcursor.MoveSelectionRight();
}

void VrViews::MoveCursorLeft(){
    if (tcursor.HasCursor()) tcursor.MoveCursorLeft();
    if (tcursor.HasSelection()) tcursor.MoveSelectionLeft();
}

void VrViews::EraseFromLine(int begin, int end){
    ulong sP=static_cast<ulong>(tcursor.FindPositionInNativeLine(userLine,begin));
    ulong eP=static_cast<ulong>(tcursor.FindPositionInNativeLine(userLine,end));
    userLine.erase(sP,eP-sP);
    editLine.setText(userLine);
    tcursor.EraseCursor();
    tcursor.MakeLinePosAgain(0,userLine);
    tcursor.SetCursorAt(0,begin);
}

void VrViews::EraseSelection(){
    if (tcursor.HasSelection()){
        int startPos=tcursor.GetSelectionStartCharPos();
        int endPos=tcursor.GetSelectionEndCharPos();
        EraseFromLine(startPos,endPos);
    }
}

void VrViews::InsertLetter(string fromKeyb){
    if (fromKeyb!=""){
        if (tcursor.HasSelection()) EraseSelection();
        if (tcursor.HasCursor()){
            int cP=tcursor.GetPos();
            ulong sP=static_cast<ulong>(tcursor.FindPositionInNativeLine(userLine,cP));
            string oldLine=userLine;
            userLine.insert(sP,fromKeyb);
            if (IsLineNotTooWide()){
                editLine.setText(userLine);
                int insL=tcursor.GetLengthOfUTFString(fromKeyb);
                cP+=insL;
                tcursor.AddPositionsToLine(insL);
                tcursor.MakeLinePosAgain(0,userLine);
                tcursor.SetCursorAt(0,cP);
            }
            else userLine=oldLine;
        }
    }
}

void VrViews::MessageLine3(const string &in_line){
    DrawLogic::ChangeString(myStringN3,in_line);
}

void VrViews::EndAlert(){
    ManageModalWindow::MakeTopWindow();//should be, but it is essential the right drawpad is pointed for all the delete ops to come
    myXPWindow=nullptr;
    //DrawLogic::WriteDebug("vrviews end alert going to delete buttons");
    delete keyb;
    delete yesButton;
    delete noButton;
    delete cancelButton;
    delete upButton;
    delete downButton;
    delete createButton;
    delete deleteButton;
    delete renameButton;
    delete repositionButton;
    delete vdownButton;
    delete vupButton;
    delete vrightButton;
    delete vleftButton;
    delete vaftButton;
    delete vforwButton;
    ManageModalWindow::DestroyModalWindow();
    keyb            =nullptr;
    yesButton       =nullptr;
    noButton        =nullptr;
    cancelButton    =nullptr;
    upButton        =nullptr;
    downButton      =nullptr;
    createButton    =nullptr;
    deleteButton    =nullptr;
    renameButton    =nullptr;
    repositionButton=nullptr;
    vdownButton     =nullptr;
    vupButton       =nullptr;
    vrightButton    =nullptr;
    vleftButton     =nullptr;
    vaftButton      =nullptr;
    vforwButton     =nullptr;
    clicked         =nullptr;

    actionLaunched  =false;
    width=0;
    height=0;
    targetX=0;
    targetY=0;
    targetZ=0;
    epochClick=0;
    //DrawLogic::WriteDebug("vrviews end alert going to callback");
    callBack();
}

void VrViews::ShowAll(){
    clearText();
    vector<string> allNames=VRCReader::ExportHotspotNames();
    for (auto name:allNames){
        AddLine(name);
    }
    SetupforText();
    SetBackGround(true);

    SetBckColor(Clr_White);
    SetInkColor(Clr_BlackInk);
}

void VrViews::Update(){
    if (NeedsToDisplay()){
        DisplayPage(true);
    }
}

bool VrViews::IsRunning(){
    return(myXPWindow!=nullptr);
}
