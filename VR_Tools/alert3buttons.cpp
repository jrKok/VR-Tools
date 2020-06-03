#include "alert3buttons.h"
#include "managemodalwindow.h"
#include "drawlogic.h"

Alert3Buttons *Alert3Buttons::myself(nullptr);
XPLMWindowID Alert3Buttons::myXPWindow(nullptr);
int Alert3Buttons::left(0);
int Alert3Buttons::bottom(0);
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
    int buttonHeight(20),buttonwidth(80),textWidth(0),pos(20),nbButtons(0);
    answer=0;
    left=0;
    bottom=0;
    height=90;
    myself=this;
    callBack=cBck;
    textOffsetX=10;textOffsetY=40;

    if (yesStr!="") nbButtons++;
    if (noStr!="") nbButtons++;
    if (cancelStr!="") nbButtons++;
    width=nbButtons*(buttonwidth+20)+20;
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
    myXPWindow=ManageModalWindow::CreateMousedModalWindow(MouseHandler,DrawMyself,Clr_LightGray,width,height);
    myStringNumber=DrawLogic::AddString(alertStr,Clr_Black,Clr_LightGray,p);

    yesButton=new button_VR();
    noButton=new button_VR();
    cancelButton=new button_VR();

    yesButton->SetDimensions(buttonwidth,buttonHeight);
    yesButton->SetOrigin(pos,10);
    yesButton->setText(yesStr);
    yesButton->setVisibility(yesStr!=""?true:false);
    yesButton->SetToStateColor();
    if (yesStr!="") pos+=buttonwidth+20;

    noButton->SetDimensions(buttonwidth,buttonHeight);
    noButton->SetOrigin(pos,10);
    noButton->setText(noStr);
    noButton->setVisibility(noStr!=""?true:false);
    noButton->SetToStateColor();
    if (noStr!="") pos+=buttonwidth+20;

    cancelButton->SetDimensions(buttonwidth,buttonHeight);
    cancelButton->SetOrigin(pos,10);
    cancelButton->setText(cancelStr);
    cancelButton->SetToStateColor();
    cancelButton->setVisibility(cancelStr!=""?true:false);
    DrawLogic::UpdateTexture();
    ManageModalWindow::ConstrainGeometry();
}

int  Alert3Buttons::GetAnswer(){
 return answer;
}

void Alert3Buttons::DrawMyself(XPLMWindowID, void *){
    ManageModalWindow::MakeTopWindow();
    DrawLogic::RenderContent();
}

int Alert3Buttons::MouseHandler(XPLMWindowID in_window_id, int ix, int iy, int is_down, void *){
     ManageModalWindow::MakeTopWindow();
    int x(ix-ManageModalWindow::GetLeft()),y(iy-ManageModalWindow::GetBottom());
    switch (is_down){
    case xplm_MouseDown:{
        if(!XPLMIsWindowInFront(in_window_id))
        {
            ManageModalWindow::MakeTopWindow();
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
