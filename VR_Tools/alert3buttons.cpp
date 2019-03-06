#include "alert3buttons.h"
#include "managemodalwindow.h"
#include "drawlogic.h"

Alert3Buttons *Alert3Buttons::myself(nullptr);
XPLMWindowID Alert3Buttons::myXPWindow(nullptr);
int Alert3Buttons::left(0);
int Alert3Buttons::top(0);
bool Alert3Buttons::mouseLocated(false);

Alert3Buttons::Alert3Buttons():
  callBack(),
  yesButton(nullptr),
  noButton(nullptr),
  cancelButton(nullptr),
  width(0),
  height(0),
  answer(0),
  myStringNumber(0),
  textOffsetX(0),
  textOffsetY(0)

{

}

void Alert3Buttons::MakeAlert(const string &yesStr, const string &noStr, const string &cancelStr,const string alertStr, std::function<void()>cBck){
    int buttonHeight(20);
    answer=0;
    left=0;
    top=0;
    height=90;
    myself=this;
    callBack=cBck;
    width = 20;
    int buttonwidth(80),textWidth(0),pos(20),nbButtons(0);
    textOffsetX=10;textOffsetY=10;

    if (yesStr!="") nbButtons++;
    if (noStr!="") nbButtons++;
    if (cancelStr!="") nbButtons++;

    float strwidth=XPLMMeasureString(xplmFont_Proportional,alertStr.c_str(),static_cast<int>(alertStr.size()));
    textWidth=static_cast<int>(strwidth)+textOffsetX+textOffsetX;
    width=nbButtons*(buttonwidth+20)+20;

    if (textWidth>width) {
        width=textWidth;
        pos=(width-((buttonwidth+20)*nbButtons))/2;
    }
    else{
        textOffsetX=(width-textWidth)/2;
    }

    point p;
    p.myX=textOffsetX;
    p.myY=textOffsetY;
    myStringNumber=DrawLogic::AddModalString(alertStr,Clr_Black,p);
    myXPWindow=ManageModalWindow::CreateMousedModalWindow(MouseHandler,DrawMyself,Clr_LightGray,width,height);

    yesButton=new ModalButton();
    noButton=new ModalButton();
    cancelButton=new ModalButton();

    yesButton->SetDimensions(buttonwidth,buttonHeight);
    yesButton->SetOffsets(pos,40);
    yesButton->setText(yesStr);
    yesButton->setVisibility(yesStr!=""?true:false);
    yesButton->SetToStateColor();
    if (yesStr!="") pos+=buttonwidth+20;

    noButton->SetDimensions(buttonwidth,buttonHeight);
    noButton->SetOffsets(pos,40);
    noButton->setText(noStr);
    noButton->setVisibility(noStr!=""?true:false);
    noButton->SetToStateColor();
    if (noStr!="") pos+=buttonwidth+20;

    cancelButton->SetDimensions(buttonwidth,buttonHeight);
    cancelButton->SetOffsets(pos,40);
    cancelButton->setText(cancelStr);
    cancelButton->SetToStateColor();
    cancelButton->setVisibility(cancelStr!=""?true:false);

}

int  Alert3Buttons::GetAnswer(){
 return answer;
}

void Alert3Buttons::Recalculate (){
    myself->yesButton->recalculate(left,top);
    myself->noButton->recalculate(left,top);
    myself->cancelButton->recalculate(left,top);
    ManageModalWindow::Recalculate(left,top);
    point p;
    p.myX=left+myself->textOffsetX;
    p.myY=top-myself->textOffsetY;
    DrawLogic::RelocateModalString(myself->myStringNumber,p);
}

void Alert3Buttons::DrawMyself(XPLMWindowID in_window_id, void * unused){
    int lft(left),tp(top),right,bottom;
    XPLMGetWindowGeometry(in_window_id, &left, &top, &right, &bottom);
    if (lft!=left||tp!=top) Recalculate();
    DrawLogic::DrawModalElements();
    DrawLogic::DrawModalStrings();
}

int Alert3Buttons::MouseHandler(XPLMWindowID in_window_id, int x, int y, int is_down, void * unused){
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
        }
        break;
    }
    case xplm_MouseDrag:{
        break;
    }
    case xplm_MouseUp:{
        if (mouseLocated){
            mouseLocated=false;
            myself->EndAlert();
        }
        break;
    }
    }
    return 1;
}

void Alert3Buttons::EndAlert(){
    delete yesButton;
    delete noButton;
    delete cancelButton;
    ManageModalWindow::DestroyModalWindow();
    myXPWindow=nullptr;
    yesButton=nullptr;
    noButton=nullptr;
    cancelButton=nullptr;
    width=0;
    height=0;
    callBack();
}
