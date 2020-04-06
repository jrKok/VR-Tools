#include "linedialog.h"
#include "managemodalwindow.h"
#include "drawlogic.h"

LineDialog *LineDialog::myself;
bool LineDialog::mouseLocated(false);
int LineDialog::top(0);
int LineDialog::left(0);
XPLMWindowID LineDialog::myXPWindow(nullptr);

LineDialog::LineDialog():
  callBack(),
  yesButton(nullptr),
  noButton(nullptr),
  cancelButton(nullptr),
  textRect(nullptr),
  width(320),
  height(50),
  answer(0),
  myStringNumber(0),
  textOffsetX(0),
  textOffsetY(0),
  userLine(""),
  keyb(nullptr),
  editLine(nullptr),
  cursor(),
  forCursor()

{

}

LineDialog::~LineDialog(){
    if (       yesButton!=nullptr) delete yesButton;
    if (        noButton!=nullptr) delete noButton;
    if (    cancelButton!=nullptr) delete cancelButton;
    if (        textRect!=nullptr) delete textRect;
    if (            keyb!=nullptr) delete keyb;
    if (        editLine!=nullptr) delete editLine;
}

void LineDialog::MakeDialog(const string &yesStr, const string &noStr, const string &cancelStr, const string alertStr, const std::string &in_String, std::function<void()>cBck, int in_width){
    int buttonHeight(20);
    callBack=cBck;
    myself=this;
    userLine=in_String;
    width=in_width;
    answer=0;
    left=0;
    top=0;
    textOffsetX=10;textOffsetY=10;
    int buttonwidth(80),textWidth(0),pos(20),nbButtons(0);

    myXPWindow=ManageModalWindow::CreateMousedModalWindow(MouseHandler,DrawMyself,Clr_LighterGray,width,height);

    forCursor.clear();
    forCursor.push_back(userLine);
    keyb=new Keyboard(true);
    keyb->MakeKeyboard(5,120);
    height= keyb->MyHeight()+120;
    width = keyb->MyWidth()+10;
    cursor.Initiate(&forCursor,10,30,20,1);
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
    }
    point p;
    p.SetCoords(textOffsetX,textOffsetY);
    myStringNumber=DrawLogic::AddString(alertStr,Clr_Black,Clr_LighterGray,p);

    yesButton=new button_VR();
    noButton=new button_VR();
    cancelButton=new button_VR();
    editLine    =new TextLine();

    yesButton->SetDimensions(buttonwidth,buttonHeight);
    yesButton->SetOrigin(pos,80);
    yesButton->setText(yesStr);
    yesButton->setVisibility(yesStr!=""?true:false);
    yesButton->SetToStateColor();
    if (yesStr!="") pos+=buttonwidth+20;

    noButton->SetDimensions(buttonwidth,buttonHeight);
    noButton->SetOrigin(pos,80);
    noButton->setText(noStr);
    noButton->setVisibility(noStr!=""?true:false);
    noButton->SetToStateColor();
    if (noStr!="") pos+=buttonwidth+20;

    cancelButton->SetDimensions(buttonwidth,buttonHeight);
    cancelButton->SetOrigin(pos,80);
    cancelButton->setText(cancelStr);
    cancelButton->SetToStateColor();
    cancelButton->setVisibility(cancelStr!=""?true:false);

    textRect= new rectangles(true);
    textRect->SetDimensions(width-20,20);
    textRect->SetOrigin(10,30);
    textRect->setColor(Clr_White);

    editLine->SetOrigin(12,40);
    ManageModalWindow::ResizeModalWindow(width,height);
    keyb->SetVisibility(true);
    cursor.SetCursorAt(0,0);
    //activate physical keyboard, rebuild keyboard for handling different types of callers

}

int  LineDialog::GetAnswer(){
 return answer;
}

string LineDialog::GetUserLine(){
    return userLine;
}

void LineDialog::DrawMyself(XPLMWindowID in_window_id, void * unused){
    ManageModalWindow::ConstrainGeometry();
    DrawLogic::DrawContent();
    if (myself->cursor.HasCursor()) myself->cursor.DrawCursor();
    if (myself->cursor.HasSelection()){
        int l,r;
        myself->cursor.IsIndexInSelection(0,l,r);
        myself->cursor.DrawRectangle(l,myself->editLine->GetTop(),r,myself->editLine->GetBottom());}
    //drawCursor
    //drawSelectionrect
}

int LineDialog::MouseHandler(XPLMWindowID in_window_id, int x, int y, int is_down, void * unused){
    ManageModalWindow::MakeTopWindow();
    switch (is_down){
    case xplm_MouseDown:{
        if(!XPLMIsWindowInFront(in_window_id))
        {
            XPLMBringWindowToFront(in_window_id);
        }else{
        if (myself->yesButton->isHere(x,y)){
            mouseLocated=true;
            myself->answer=1;
        }
        if (myself->noButton->isHere(x,y)){
            mouseLocated=true;
            myself->answer=2;
        }
        if (myself->cancelButton->isHere(x,y)){
            mouseLocated=true;
            myself->answer=3;
        }
        std::string keyName,keyVal;
        if (myself->keyb->PressKey(x,y,myself->specialKey,keyName,keyVal))
            if (myself->keyb->IsKeyPressed()) myself->ProcessKeyPress(keyName,keyVal);
        }
        if (myself->textRect->isHere(x,y)){
            myself->cursor.FindPos(0,x);
            DrawLogic::SetCursorPosition(myself->cursor.PosToX(),myself->textRect->GetBottom());
        }
        break;
    }
    case xplm_MouseDrag:{
        myself->ContinueKeyPress();
        break;
    }
    case xplm_MouseUp:{
        myself->keyb->ReleaseCurrentKey();
        if (mouseLocated){
            mouseLocated=false;
            myself->EndAlert();
        }
        break;
    }
    }
    return 1;
}

void LineDialog::ProcessKeyPress(std::string keyName,std::string in_String){
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

void LineDialog::Cut(){
    Copy();
    if (cursor.HasSelection()) EraseSelection();

}

void LineDialog::Copy(){
    if (cursor.HasSelection()){
        string copyString("");
        ulong bPos(static_cast<ulong>(cursor.GetSelectionStartCharPos())),
                ePos(static_cast<ulong>(cursor.GetSelectionEndCharPos()));
        int bp=cursor.FindPositionInNativeLine(userLine,static_cast<int>(bPos));
        int ep=cursor.FindPositionInNativeLine(userLine,static_cast<int>(ePos));
        copyString=userLine.substr(bp,ep-bp);
    }

}

void LineDialog::Paste(){
string in_clipboard=LocalClipBoard::PullText();
InsertLetter(in_clipboard);
}

void LineDialog::Backspace(){
    if (cursor.HasCursor()){
        int cP=cursor.GetPos();
        if (cP>0){
            EraseFromLine(cP-1,cP);
        }
    }
    if (cursor.HasSelection()) EraseSelection();
}

void LineDialog::Supress(){
    if (cursor.HasCursor()){
        int cP=cursor.GetPos();
        if (static_cast<ulong>(cP)<(userLine.size()-1)){
            EraseFromLine(cP,cP+1);
        }
    }
    if (cursor.HasSelection()) EraseSelection();
}

void LineDialog::ContinueKeyPress(){

}

bool LineDialog::IsLineNotTooWide(){
  float lWidth= XPLMMeasureString(xplmFont_Proportional,userLine.c_str(),static_cast<int>(userLine.size()));
  if (lWidth<width-20) return true; else return false;
}

void LineDialog::MoveCursorRight(){
    if (cursor.HasCursor()) cursor.MoveCursorRight();
    if (cursor.HasSelection()) cursor.MoveSelectionRight();
}

void LineDialog::MoveCursorLeft(){
    if (cursor.HasCursor()) cursor.MoveCursorLeft();
    if (cursor.HasSelection()) cursor.MoveSelectionLeft();
}

void LineDialog::EraseFromLine(int begin, int end){
    ulong sP=static_cast<ulong>(cursor.FindPositionInNativeLine(userLine,begin));
    ulong eP=static_cast<ulong>(cursor.FindPositionInNativeLine(userLine,end));
    userLine.erase(sP,eP-sP);
    editLine->setText(userLine);
    cursor.EraseCursor();
    cursor.MakeLinePosAgain(0,userLine);
    cursor.SetCursorAt(0,begin);
}

void LineDialog::EraseSelection(){
    if (cursor.HasSelection()){
        int startPos=cursor.GetSelectionStartCharPos();
        int endPos=cursor.GetSelectionEndCharPos();
        EraseFromLine(startPos,endPos);

    }
}

void LineDialog::InsertLetter(string fromKeyb){
    if (fromKeyb!=""){
        if (cursor.HasSelection()) EraseSelection();
        if (cursor.HasCursor()){
            int cP=cursor.GetPos();
            ulong sP=static_cast<ulong>(cursor.FindPositionInNativeLine(userLine,cP));
            string oldLine=userLine;
            userLine.insert(sP,fromKeyb);
            if (IsLineNotTooWide()){
                editLine->setText(userLine);
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

void LineDialog::EndAlert(){
    delete yesButton;
    delete noButton;
    delete cancelButton;
    delete textRect;
    delete keyb;
    keyb=nullptr;
    textRect=nullptr;
    yesButton=nullptr;
    noButton=nullptr;
    cancelButton=nullptr;
    ManageModalWindow::DestroyModalWindow();
    myXPWindow=nullptr;
    yesButton=nullptr;
    noButton=nullptr;
    cancelButton=nullptr;
    width=0;
    height=0;
    callBack();
}
