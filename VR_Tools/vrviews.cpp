#include "vrviews.h"
#include "managemodalwindow.h"
#include "drawlogic.h"

VrViews *VrViews::myself;
bool VrViews::mouseLocated(false);
int VrViews::top(0);
int VrViews::left(0);
XPLMWindowID VrViews::myXPWindow(nullptr);

VrViews::VrViews():List_Box_With_ScrB (true),
    callBack(),
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
    clicked(nullptr),
    textRect(nullptr),
    width(320),
    height(300),
    answer(0),
    myStringNumber(0),
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
    cursor(),
    specialKey(false),
    actionLaunched(false),
    mightSave(false),
    disableEdit(false),
    action(""),
    forCursor()

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
    if (        textRect!=nullptr) delete textRect;
    if (            keyb!=nullptr) delete keyb;
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
    int buttonwidth(80),cmdBWidth(60),textWidth(0),pos(20),nbButtons(0);
    int posFirstLine(15),posSecondLine(27),posListBox(30),posCommandButtons(200),posThirdLine(230),posEditLine(240),posButtons(265),posKeyb(290);

    disableEdit=false;
    callBack=cBck;
    Setup(160,240,100,posListBox);
    myself=this;
    userLine=in_String;
    width=in_width;
    answer=0;
    left=0;
    top=0;
    selectedHotsp=0;
    textOffsetX=10;textOffsetY=posFirstLine;
    action="";

    myXPWindow=ManageModalWindow::CreateMousedModalWindow(MouseHandler,DrawMyself,Clr_LighterGray,width,height);

    forCursor.clear();
    forCursor.push_back(userLine);
    keyb=new Keyboard(true);
    keyb->MakeKeyboard(5,posKeyb);
    height= keyb->MyHeight()+posKeyb+10;
    int kwidth = keyb->MyWidth()+10;
    width=width>kwidth?width:kwidth;
    cursor.Initiate(&forCursor,10,posEditLine+10,20,1);
    if (yesStr!="") nbButtons++;
    if (noStr!="") nbButtons++;
    if (cancelStr!="") nbButtons++;

    float strwidth=XPLMMeasureString(xplmFont_Proportional,alertStr.c_str(),static_cast<int>(alertStr.size()));
    textWidth=static_cast<int>(strwidth)+textOffsetX+textOffsetX;
    if (textWidth>width) {
        width=textWidth;
        pos=(width-((buttonwidth+20)*nbButtons))/2;
    }
    else{
        textOffsetX=(width-textWidth)/2;
        pos=(width-((buttonwidth+20)*nbButtons))/2;
    }

    point p;
    p.myX=textOffsetX;
    p.myY=textOffsetY;
    myStringNumber=DrawLogic::AddModalString(alertStr,Clr_Green,p);
    p.myX=100;
    p.myY=posSecondLine;
    myStringN2=DrawLogic::AddModalString("If needed, Select a hotspot :",Clr_BlackInk,p);
    p.myX=textOffsetX;
    p.myY=posThirdLine;
    myStringN3=DrawLogic::AddModalString("Type a name to create a hotspot or rename an existing one :",Clr_BlackInk,p);

    yesButton=        new ModalButton();
    noButton=         new ModalButton();
    cancelButton=     new ModalButton();

    relogButton=      new ModalButton();
    upButton=         new ModalButton();
    downButton=       new ModalButton();

    renameButton=     new ModalButton();
    createButton=     new ModalButton();
    deleteButton=     new ModalButton();
    repositionButton= new ModalButton();

    yesButton->SetDimensions(buttonwidth,buttonHeight);
    yesButton->SetOffsets(pos,posButtons);
    yesButton->setText(yesStr);
    yesButton->setVisibility(yesStr!=""?true:false);
    yesButton->SetToStateColor();
    if (yesStr!="") pos+=buttonwidth+20;

    noButton->SetDimensions(buttonwidth,buttonHeight);
    noButton->SetOffsets(pos,posButtons);
    noButton->setText(noStr);
    noButton->setVisibility(noStr!=""?true:false);
    noButton->SetToStateColor();
    if (noStr!="") pos+=buttonwidth+20;

    cancelButton->SetDimensions(buttonwidth,buttonHeight);
    cancelButton->SetOffsets(pos,posButtons);
    cancelButton->setText(cancelStr);
    cancelButton->SetToStateColor();
    cancelButton->setVisibility(cancelStr!=""?true:false);

    relogButton->SetDimensions(40,40);
    relogButton->SetOffsets(360,25);
    relogButton->setText("Relog");
    relogButton->setColor(Clr_Green);
    relogButton->setVisibility(true);

    pos=20;
    upButton->SetDimensions(20,buttonHeight);
    upButton->SetOffsets(50,posListBox+40);
    upButton->setText("\xe2\x86\x91");
    upButton->SetToStateColor();
    upButton->setVisibility(true);

    downButton->SetDimensions(20,buttonHeight);
    downButton->SetOffsets(50,posListBox+65);
    downButton->setText("\xe2\x86\x93");
    downButton->SetToStateColor();
    downButton->setVisibility(true);

    createButton->SetDimensions(cmdBWidth,buttonHeight);    
    createButton->SetOffsets(pos,posCommandButtons);
    createButton->setText("Create");
    createButton->SetToStateColor();
    createButton->setVisibility(true);
    pos=pos+cmdBWidth+2;

    renameButton->SetDimensions(cmdBWidth,buttonHeight);
    renameButton->SetOffsets(pos,posCommandButtons);
    renameButton->setText("Rename");
    renameButton->SetToStateColor();
    renameButton->setVisibility(true);
    pos=pos+cmdBWidth+2;

    repositionButton->SetDimensions(cmdBWidth,buttonHeight);
    repositionButton->SetOffsets(pos,posCommandButtons);
    repositionButton->setText("Update");
    repositionButton->SetToStateColor();
    repositionButton->setVisibility(true);
    pos=pos+cmdBWidth+cmdBWidth+2;

    deleteButton->SetDimensions(cmdBWidth,buttonHeight);
    deleteButton->SetOffsets(pos,posCommandButtons);
    deleteButton->setText("Delete");
    deleteButton->SetToStateColor();
    deleteButton->setVisibility(true);

    textRect= new rectangles(true,true);
    textRect->SetDimensions(width-20,20);
    textRect->SetOffsets(10,posEditLine);
    textRect->setColor(Clr_White);

    editLine.SetOffsets(12,posEditLine+10);
    ManageModalWindow::ResizeModalWindow(width,height);
    cursor.SetCursorAt(0,0);
    keyb->SetVisibility(true);
    SetSplitPolicy(ForceSplitAtSize);
    ShowAll();
    CheckButtonsVisibility();

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

string VrViews::GetActionLaunched(){
    return action;
}

void VrViews::RecalculateDialog (){
                      myself->Recalculate(left,top);
           myself->yesButton->recalculate(left,top);
            myself->noButton->recalculate(left,top);
        myself->cancelButton->recalculate(left,top);
         myself->relogButton->recalculate(left,top);
        myself->renameButton->recalculate(left,top);
        myself->createButton->recalculate(left,top);
        myself->deleteButton->recalculate(left,top);
    myself->repositionButton->recalculate(left,top);
            myself->upButton->recalculate(left,top);
          myself->downButton->recalculate(left,top);
                myself->keyb->Recalculate(left,top);
            myself->textRect->recalculate(left,top);
             myself->editLine.recalculate(left,top);
           ManageModalWindow::Recalculate(left,top);

    point p;
    p.myX=left+myself->textOffsetX;
    p.myY=top-myself->textOffsetY;
    DrawLogic::RelocateModalString(myself->myStringNumber,p);
    p.myY=p.myY-12;
    p.myX=left+100;
    DrawLogic::RelocateModalString(myself->myStringN2,p);
    p.myY=p.myY-203;
    p.myX=left+myself->textOffsetX;
    DrawLogic::RelocateModalString(myself->myStringN3,p);


    myself->cursor.Recalculate(myself->editLine.GetX(),myself->editLine.GetY());
}

void VrViews::DrawMyself(XPLMWindowID in_window_id, void * unused){
    if (myself->keyb==nullptr) myself->WriteDebug("vrviews draw : got a call while keyb was deleted");
    int lft(left),tp(top),right,bottom;
    XPLMGetWindowGeometry(in_window_id, &left, &top, &right, &bottom);
    if (lft!=left||tp!=top) RecalculateDialog();
    DrawLogic::DrawModalElements();
    DrawLogic::DrawModalStrings();
    if (myself->cursor.HasCursor()) myself->cursor.DrawCursor(myself->editLine.GetY());
    if (myself->cursor.HasSelection()){
        int l,r;
        myself->cursor.IsIndexInSelection(0,l,r);
        myself->cursor.DrawRectangle(l,myself->editLine.GetTop(),r,myself->editLine.GetBottom());}
    //drawCursor
    //drawSelectionrect
}

int VrViews::MouseHandler(XPLMWindowID in_window_id, int x, int y, int is_down, void * unused){
    switch (is_down){
    case xplm_MouseDown:{
        if(!XPLMIsWindowInFront(in_window_id))
        {
            XPLMBringWindowToFront(in_window_id);
        }else{
            mouseLocated=false;
            myself->ProceedClick(x,y);
            if (!myself->needToContClick){
                if (myself->yesButton->isHere(x,y)){
                    mouseLocated=true;
                    myself->answer=1;
                    myself->clicked=myself->yesButton;
                }
                if (myself->noButton->isHere(x,y)){
                    mouseLocated=true;
                    myself->answer=2;
                    myself->clicked=myself->noButton;
                }
                if (myself->cancelButton->isHere(x,y)){
                    mouseLocated=true;
                    myself->answer=3;
                    myself->clicked=myself->cancelButton;
                }

                if (myself->relogButton->isHere(x,y)){
                    myself->LaunchAction("Relog");
                    myself->clicked=myself->relogButton;
                }

                if (myself->upButton->isHere(x,y)){
                    myself->LaunchAction("Up");
                    myself->clicked=myself->upButton;
                }
                if (myself->downButton->isHere(x,y)){
                    myself->LaunchAction("Down");
                    myself->clicked=myself->downButton;
                }
                if (myself->renameButton->isHere(x,y)){
                    myself->LaunchAction("Rename");
                    myself->clicked=myself->renameButton;
                }
                if (myself->createButton->isHere(x,y)){
                    myself->LaunchAction("Create");
                    myself->clicked=myself->createButton;
                }
                if (myself->repositionButton->isHere(x,y)){
                    myself->LaunchAction("Reposition");
                    myself->clicked=myself->repositionButton;
                }
                if (myself->deleteButton->isHere(x,y)){
                    myself->LaunchAction("Delete");
                    myself->clicked=myself->deleteButton;
                }

                std::string keyName,keyVal;
                if (myself->keyb->PressKey(x,y,myself->specialKey,keyName,keyVal))
                    if (myself->keyb->IsKeyPressed()) myself->ProcessKeyPress(keyName,keyVal);
                if (myself->textRect->isHere(x,y)){
                    myself->cursor.FindPos(0,x);
                }
                if (myself->clicked!=nullptr) myself->clicked->Press();
            }
        }
        break;
    }
    case xplm_MouseDrag:{
        myself->ContinueKeyPress();
        myself->ProceedClickCont(x,y);
        break;
    }
    case xplm_MouseUp:{
        if (mouseLocated) {
            myself->EndAlert();
            return 1;
        }
        myself->keyb->ReleaseCurrentKey();
        if (myself->clicked!=nullptr){
            myself->clicked->Release();
            if (myself->clicked==myself->relogButton) myself->relogButton->setColor(Clr_Green);
            myself->clicked=nullptr;
        }
        myself->ProceedEndClick();
        myself->SetInkColor(Clr_BlackInk);
        myself->selectedHotsp=myself->SelectedLineNumber();
        myself->CheckButtonsVisibility();
        break;
    }

    }

    return 1;
}

void VrViews::LaunchAction(string in_action){
    answer=0;
    action=in_action;
    selectedHotsp=SelectedLineNumber();
    if (in_action=="Relog"){
        actionLaunched=true;
        callBack();
        string toPass="View logged at X="
                      +VRCReader::ConvertFloatToString(targetX)
                +" Y="+VRCReader::ConvertFloatToString(targetY)
                +" Z="+VRCReader::ConvertFloatToString(targetZ)
                +" PSI="+VRCReader::ConvertFloatToString(0);
        DrawLogic::ChangeModalString(myStringNumber,toPass);
        actionLaunched=false;

    }
    if (in_action=="Up"){
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
    }
    if (in_action=="Down"){
        //internal action but calls back for immediate action
        if (hasSelection){
            if ((selectedHotsp>0)&&(selectedHotsp<(totalNbL-1))) {
                actionLaunched=true;
                callBack();
                ShowAll();
                actionLaunched=false;mightSave=true;
            }
        }
    }
    if (in_action=="Create"){
        answer=0;
        if (userLine!=""){
            actionLaunched=true;mightSave=true;
            callBack();
        }
    }
    if (in_action=="Delete"){
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
    }
    if (in_action=="Reposition"){
        if (hasSelection){
            selectedHotsp=SelectedLineNumber();
            actionLaunched=true;
            callBack();
            mightSave=true;
        }
    }
    if (in_action=="Rename"){
        if (hasSelection&&userLine!=""){
            selectedHotsp=SelectedLineNumber();
            actionLaunched=true;
            callBack();
            mightSave=true;
            ShowAll();
        }
    }
}

void VrViews::CheckButtonsVisibility(){
    bool utext=(userLine!=""),selc=myself->hasSelection;
    myself->upButton->setVisibility(selc&&!actionLaunched&&!disableEdit);
    myself->downButton->setVisibility(selc&&!actionLaunched&&!disableEdit);
    myself->createButton->setVisibility(utext&&!actionLaunched&&!selc&&!disableEdit);
    myself->deleteButton->setVisibility(selc&&!actionLaunched&&!disableEdit);
    myself->repositionButton->setVisibility(selc&&!actionLaunched&&!disableEdit);
    myself->renameButton->setVisibility(selc&&utext&&!actionLaunched&&!disableEdit);
    myself->yesButton->setVisibility(mightSave&&!disableEdit);
    myself->noButton->setVisibility(!actionLaunched&&selc&&!disableEdit);
    myself->cancelButton->setVisibility(true);

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
        if (keyName=="TAB") {InsertLetter("   ");}
        if (keyName=="ShTAB") {InsertLetter("   ");}
        if (keyName=="Enter") {mouseLocated=true;answer=1;}
        if (keyName=="RIGHT") {myself->MoveCursorRight();}
        if (keyName=="LEFT") {myself->MoveCursorLeft();}
    }


}

void VrViews::Cut(){
    Copy();
    if (cursor.HasSelection()) EraseSelection();
}

void VrViews::Copy(){
    if (cursor.HasSelection()){
        string copyString("");
        ulong bPos(static_cast<ulong>(cursor.GetSelectionStartCharPos())),
                ePos(static_cast<ulong>(cursor.GetSelectionEndCharPos()));
        int bp=cursor.FindPositionInNativeLine(userLine,static_cast<int>(bPos));
        int ep=cursor.FindPositionInNativeLine(userLine,static_cast<int>(ePos));
        copyString=userLine.substr(bp,ep-bp);
    }

}

void VrViews::Paste(){
string in_clipboard=LocalClipBoard::PullText();
InsertLetter(in_clipboard);
}

void VrViews::Backspace(){
    if (cursor.HasCursor()){
        int cP=cursor.GetPos();
        if (cP>0){
            EraseFromLine(cP-1,cP);
        }
    }
    if (cursor.HasSelection()) EraseSelection();
}

void VrViews::Supress(){
    if (cursor.HasCursor()){
        int cP=cursor.GetPos();
        if (static_cast<ulong>(cP)<(userLine.size()-1)){
            EraseFromLine(cP,cP+1);
        }
    }
    if (cursor.HasSelection()) EraseSelection();
}

void VrViews::ContinueKeyPress(){

}

bool VrViews::IsLineNotTooWide(){
  float lWidth= XPLMMeasureString(xplmFont_Proportional,userLine.c_str(),static_cast<int>(userLine.size()));
  if (lWidth<width-20) return true; else return false;
}

void VrViews::MoveCursorRight(){
    if (cursor.HasCursor()) cursor.MoveCursorRight();
    if (cursor.HasSelection()) cursor.MoveSelectionRight();
}

void VrViews::MoveCursorLeft(){
    if (cursor.HasCursor()) cursor.MoveCursorLeft();
    if (cursor.HasSelection()) cursor.MoveSelectionLeft();
}

void VrViews::EraseFromLine(int begin, int end){
    ulong sP=static_cast<ulong>(cursor.FindPositionInNativeLine(userLine,begin));
    ulong eP=static_cast<ulong>(cursor.FindPositionInNativeLine(userLine,end));
    userLine.erase(sP,eP-sP);
    editLine.setText(userLine);
    cursor.EraseCursor();
    cursor.MakeLinePosAgain(0,userLine);
    cursor.SetCursorAt(0,begin);
}

void VrViews::EraseSelection(){
    if (cursor.HasSelection()){
        int startPos=cursor.GetSelectionStartCharPos();
        int endPos=cursor.GetSelectionEndCharPos();
        EraseFromLine(startPos,endPos);

    }
}

void VrViews::InsertLetter(string fromKeyb){
    if (fromKeyb!=""){
        if (cursor.HasSelection()) EraseSelection();
        if (cursor.HasCursor()){
            int cP=cursor.GetPos();
            ulong sP=static_cast<ulong>(cursor.FindPositionInNativeLine(userLine,cP));
            string oldLine=userLine;
            userLine.insert(sP,fromKeyb);
            if (IsLineNotTooWide()){
                editLine.setText(userLine);
                int insL=cursor.GetLengthOfUTFString(fromKeyb);
                cP+=insL;
                cursor.AddPositionsToLine(insL);
                cursor.MakeLinePosAgain(0,userLine);
                cursor.SetCursorAt(0,cP);
            }
            else userLine=oldLine;
        }
    }
}

void VrViews::MessageLine3(const string &in_line){
    DrawLogic::ChangeModalString(myStringN3,in_line);
}

void VrViews::EndAlert(){
    ManageModalWindow::DestroyModalWindow();
    myXPWindow=nullptr;
    delete yesButton;
    delete noButton;
    delete cancelButton;
    delete textRect;
    delete keyb;
    delete upButton;
    delete downButton;
    delete createButton;
    delete deleteButton;
    delete renameButton;
    delete repositionButton;
    keyb=nullptr;
    textRect=nullptr;
    yesButton=nullptr;
    noButton=nullptr;
    cancelButton=nullptr;
    upButton=nullptr;
    downButton=nullptr;
    createButton=nullptr;
    deleteButton=nullptr;
    renameButton=nullptr;
    repositionButton=nullptr;
    actionLaunched=false;
    width=0;
    height=0;
    targetX=0;
    targetY=0;
    targetZ=0;
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
