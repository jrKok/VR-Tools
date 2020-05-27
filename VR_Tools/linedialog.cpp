#include "linedialog.h"
#include "managemodalwindow.h"
#include "drawlogic.h"

LineDialog *LineDialog::myself;
bool LineDialog::mouseLocated(false);
XPLMWindowID LineDialog::myXPWindow(nullptr);

LineDialog::LineDialog():
  callBack(),
  yesButton(nullptr),
  noButton(nullptr),
  cancelButton(nullptr),
  width(320),
  height(50),
  answer(0),
  myStringNumber(0),
  textOffsetX(0),
  textOffsetY(0),
  userLine(""),
  keyb(nullptr),
  editLine(nullptr),
  tcursor(),
  forCursor()

{

}

LineDialog::~LineDialog(){
    if (       yesButton!=nullptr) delete yesButton;
    if (        noButton!=nullptr) delete noButton;
    if (    cancelButton!=nullptr) delete cancelButton;
    if (            keyb!=nullptr) delete keyb;
    if (        editLine!=nullptr) delete editLine;
}

void LineDialog::MakeDialog(const string &yesStr,
                            const string &noStr,
                            const string &cancelStr,
                            const string alertStr,
                            const std::string &in_String,
                            std::function<void()>cBck,
                            int in_width)
{
    int buttonHeight(20);
    callBack=cBck;
    myself=this;
    userLine=in_String;
    width=in_width;
    answer=0;
    textOffsetX=10;textOffsetY=10;
    int buttonwidth(80),textWidth(0),pos(20),nbButtons(0);

    myXPWindow=ManageModalWindow::CreateMousedModalWindow(MouseHandler,DrawMyself,Clr_LighterGray,width,height);

    forCursor.clear();
    forCursor.push_back(userLine);
    keyb=new Keyboard(true);
    keyb->MakeKeyboard(5,5);
    height= keyb->MyHeight()+125;
    width = keyb->MyWidth()+10;
    tcursor.Initiate(&forCursor,10,30,20,1);
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

    yesButton    =new button_VR();
    noButton     =new button_VR();
    cancelButton =new button_VR();
    editLine     =new TextLine();

    int buttonline=(keyb->MyHeight()+keyb->MyBottom()+5);

    yesButton->SetDimensions(buttonwidth,buttonHeight);
    yesButton->SetOrigin(pos,buttonline);
    yesButton->setText(yesStr);
    yesButton->setVisibility(yesStr!=""?true:false);
    yesButton->SetToStateColor();
    if (yesStr!="") pos+=buttonwidth+20;

    noButton->SetDimensions(buttonwidth,buttonHeight);
    noButton->SetOrigin(pos,buttonline);
    noButton->setText(noStr);
    noButton->setVisibility(noStr!=""?true:false);
    noButton->SetToStateColor();
    if (noStr!="") pos+=buttonwidth+20;

    cancelButton->SetDimensions(buttonwidth,buttonHeight);
    cancelButton->SetOrigin(pos,buttonline);
    cancelButton->setText(cancelStr);
    cancelButton->SetToStateColor();
    cancelButton->setVisibility(cancelStr!=""?true:false);

    editLine=new TextLine(true);
    editLine->SetOrigin(12,buttonline+buttonHeight+5);
    editLine->SetDimensions(width-20,20);
    editLine->SetBackGroundColor(Clr_White);

    point p(10,editLine->GetTop()+5);
    myStringNumber=DrawLogic::AddString(alertStr,Clr_Black,Clr_LighterGray,p);
    height=editLine->GetTop()+25;

    ManageModalWindow::ResizeModalWindow(width,height);
    keyb->SetVisibility(true);
    editLine->setTextAndUpdate(userLine);
    tcursor.Initiate(&forCursor,editLine->GetLeft(),editLine->GetBottom(),20,1);
    tcursor.SetCursorAt(0,0);
    DrawLogic::SetCursorPosition(tcursor.PosToX(),editLine->GetTextY()+2);
    DrawLogic::UpdateTexture();
    editLine->setTextAndUpdate(userLine);
}

int  LineDialog::GetAnswer(){
 return answer;
}

string LineDialog::GetUserLine(){
    return userLine;
}

void LineDialog::DrawMyself(XPLMWindowID, void*){
    ManageModalWindow::MakeTopWindow();
    DrawLogic::RenderContent();
    myself->tcursor.DrawCursor();
    //drawCursor
    //drawSelectionrect
}

int LineDialog::MouseHandler(XPLMWindowID in_window_id, int in_x, int in_y, int is_down, void *){
    int x(in_x-ManageModalWindow::GetLeft()),y(in_y-ManageModalWindow::GetBottom());
    ManageModalWindow::MakeTopWindow();
    switch (is_down){
    case xplm_MouseDown:{
        if(!XPLMIsWindowInFront(in_window_id))
        {
            XPLMBringWindowToFront(in_window_id);
        }else{
        if (myself->yesButton->IsHere(x,y)){
            mouseLocated=true;
            myself->answer=1;
        }
        if (myself->noButton->IsHere(x,y)){
            mouseLocated=true;
            myself->answer=2;
        }
        if (myself->cancelButton->IsHere(x,y)){
            mouseLocated=true;
            myself->answer=3;
        }
        std::string keyName,keyVal;
        if (myself->keyb->PressKey(x,y,myself->specialKey,keyName,keyVal))
            if (myself->keyb->IsKeyPressed()) myself->ProcessKeyPress(keyName,keyVal);
        }
        if (myself->editLine->isHere(x,y)){
            myself->tcursor.FindPos(0,x);
            DrawLogic::SetCursorPosition(myself->tcursor.PosToX(),myself->editLine->GetBottom());
        }
        break;
    }
    case xplm_MouseDrag:{
        myself->ContinueKeyPress();
        break;
    }
    case xplm_MouseUp:{
        myself->keyb->ReleaseCurrentKey();
        DrawLogic::SetCursorPosition(myself->tcursor.PosToX(),myself->editLine->GetTextY()+2);
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
    if (tcursor.HasSelection()) EraseSelection();
}

void LineDialog::Copy(){
    if (tcursor.HasSelection()){
        string copyString("");
        ulong bPos(static_cast<ulong>(tcursor.GetSelectionStartCharPos())),
                ePos(static_cast<ulong>(tcursor.GetSelectionEndCharPos()));
        int bp=tcursor.FindPositionInNativeLine(userLine,static_cast<int>(bPos));
        int ep=tcursor.FindPositionInNativeLine(userLine,static_cast<int>(ePos));
        copyString=userLine.substr(bp,ep-bp);
        editLine->setTextAndUpdate(userLine);
    }
}

void LineDialog::Paste(){
string in_clipboard=LocalClipBoard::PullText();
InsertLetter(in_clipboard);
}

void LineDialog::Backspace(){
    if (tcursor.HasCursor()){
        int cP=tcursor.GetPos();
        if (cP>0){
            EraseFromLine(cP-1,cP);
        }
    }
    if (tcursor.HasSelection()) EraseSelection();
}

void LineDialog::Supress(){
    if (tcursor.HasCursor()){
        int cP=tcursor.GetPos();
        if (static_cast<ulong>(cP)<(userLine.size()-1)){
            EraseFromLine(cP,cP+1);
        }
    }
    if (tcursor.HasSelection()) EraseSelection();
}

void LineDialog::ContinueKeyPress(){

}

bool LineDialog::IsLineNotTooWide(){
  float lWidth= XPLMMeasureString(xplmFont_Proportional,userLine.c_str(),static_cast<int>(userLine.size()));
  if (lWidth<width-20) return true; else return false;
}

void LineDialog::MoveCursorRight(){
    if (tcursor.HasCursor()) tcursor.MoveCursorRight();
    if (tcursor.HasSelection()) tcursor.MoveSelectionRight();
}

void LineDialog::MoveCursorLeft(){
    if (tcursor.HasCursor()) tcursor.MoveCursorLeft();
    if (tcursor.HasSelection()) tcursor.MoveSelectionLeft();
}

void LineDialog::EraseFromLine(int begin, int end){
    ulong sP=static_cast<ulong>(tcursor.FindPositionInNativeLine(userLine,begin));
    ulong eP=static_cast<ulong>(tcursor.FindPositionInNativeLine(userLine,end));
    userLine.erase(sP,eP-sP);
    editLine->setTextAndUpdate(userLine);
    tcursor.EraseCursor();
    tcursor.MakeLinePosAgain(0,userLine);
    tcursor.SetCursorAt(0,begin);
}

void LineDialog::EraseSelection(){
    if (tcursor.HasSelection()){
        int startPos=tcursor.GetSelectionStartCharPos();
        int endPos=tcursor.GetSelectionEndCharPos();
        EraseFromLine(startPos,endPos);
    }
}

void LineDialog::InsertLetter(string fromKeyb){
    if (fromKeyb!=""){
        if (tcursor.HasSelection()) EraseSelection();
        if (tcursor.HasCursor()){
            int cP=tcursor.GetPos();
            ulong sP=static_cast<ulong>(tcursor.FindPositionInNativeLine(userLine,cP));
            string oldLine=userLine;
            userLine.insert(sP,fromKeyb);
            if (IsLineNotTooWide()){
                editLine->setTextAndUpdate(userLine);
                int insL=tcursor.GetLengthOfUTFString(fromKeyb);
                cP+=insL;
                tcursor.AddPositionsToLine(insL);
                tcursor.MakeLinePosAgain(0,userLine);
                tcursor.SetCursorAt(0,cP);
            }
            else {
                userLine=oldLine;
                editLine->setTextAndUpdate(userLine);
            }
        }
    }
}

void LineDialog::EndAlert(){
    delete yesButton;
    delete noButton;
    delete cancelButton;
    delete keyb;
    delete editLine;
    editLine=nullptr;
    keyb=nullptr;
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
