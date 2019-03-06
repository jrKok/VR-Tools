#include "modalbutton.h"
#include "drawlogic.h"

ModalButton::ModalButton() : rectangles(true,true),
    isSelected(false),
    isPressed(false),
    hasSymbol(false),
    warningMode(false),
    buttonText(""),
    offsetTextX(0),
    offsetTextY(0),
    numberPoints(0),
    symbolNumber(0),
    stringNumber(-1)

{
    stringLocation.myX=0;
    stringLocation.myY=0;
}

void ModalButton::AddText(){
    stringNumber=DrawLogic::AddModalString("",Clr_White,stringLocation);
}
  void ModalButton::recalculate(){
    top=in_top-offsetY;
    bottom=in_top-offsetY-height;
    left=in_left+offsetX;
    right=in_left+offsetX+width;
    DrawLogic::RelocateModalRect(drawNumber,*this);

    stringLocation.myX=in_left+offsetTextX;
    stringLocation.myY=in_top-offsetTextY+2;
    DrawLogic::RelocateModalString(stringNumber,stringLocation);


}
  void ModalButton::recalculate(int in_X,int in_Y){
      SetOrigin(in_X,in_Y);
      ModalButton::recalculate();
  }

void ModalButton::setText(const std::string &in_String){
    if (stringNumber==-1) AddText();
    buttonText=in_String;
    DrawLogic::ChangeModalString(stringNumber,in_String);
    DrawLogic::ChangeColorModalString(stringNumber,Clr_White);
    int tWidth=static_cast<int>(XPLMMeasureString(xplmFont_Proportional,(char*)in_String.c_str(),in_String.length()));
    int tHeight(0);
    XPLMGetFontDimensions(xplmFont_Proportional,nullptr,&tHeight,nullptr);
    offsetTextY=offsetY+(height/2)+(tHeight/2);
    if (tWidth>=(width-4)) width=tWidth+4;
    offsetTextX=offsetX+(width/2)-(tWidth/2);
    recalculate();
}

void ModalButton::SetTextOffsets(int oX,int oY){
    offsetTextX=oX;
    offsetTextY=oY;
}

void ModalButton::SetOffsetY(int in_oY){
    offsetY=in_oY;
}

int  ModalButton::GetTextOffsetX(){return offsetTextX;}
int  ModalButton::GetTextOffsetY(){return offsetTextY;}

void ModalButton::setTextFixedSize(const std::string &in_String){
    if (stringNumber==-1) AddText();
    buttonText=in_String;
    DrawLogic::ChangeModalString(stringNumber,in_String);
    DrawLogic::ChangeColorModalString(stringNumber,Clr_White);
    //calculates Offsets for text
    int tWidth=static_cast<int>(XPLMMeasureString(xplmFont_Proportional,(char*)in_String.c_str(),in_String.length()));
    int tHeight(0);
    XPLMGetFontDimensions(xplmFont_Proportional,nullptr,&tHeight,nullptr);
    offsetTextY=offsetY+(height/2)+(tHeight/2);
    offsetTextX=offsetX+(width/2)-(tWidth/2);
    recalculate();
}

void ModalButton::setTextColor(char to_Clr){
     DrawLogic::ChangeColorModalString(stringNumber,to_Clr);
}

void ModalButton::setButtonColor(char to_Clr){
    DrawLogic::ChangeColorModalRect(drawNumber,to_Clr);
}

void ModalButton::resetMe(){
    isVisible=true;
    isSelected=false;
    buttonText="";
    numberPoints=0;
    top=0;left=0;bottom=0;right=0;in_top=0;in_left=0;height=0;width=0;offsetX=0;offsetY=0;
    stringLocation.myX=0;stringLocation.myY=0; offsetTextX=0; offsetTextY=0;
    DrawLogic::RelocateModalRect(drawNumber,*this);
    DrawLogic::RelocateModalString(stringNumber,stringLocation);
}

void ModalButton::setVisibility(bool vis){
   DrawLogic::SetVisibilityModalRect(drawNumber,vis);
   DrawLogic::SetVisibilityModalString(stringNumber,vis);

}

void ModalButton::Press(){
    isPressed=true;
    DrawLogic::ChangeColorModalRect(drawNumber,warningMode?Clr_Red:Clr_PushedBlue);
}

void ModalButton::Release(){
    isPressed=false;
    if (isSelected)
        DrawLogic::ChangeColorModalRect(drawNumber,Clr_SelectBlue);
    else
        DrawLogic::ChangeColorModalRect(drawNumber,warningMode?Clr_RedKey:Clr_LightBlue);

}

void ModalButton::setSelect(bool selState){
    isSelected=selState;
    if (isSelected) DrawLogic::ChangeColorModalRect(drawNumber,warningMode?Clr_Red:Clr_SelectBlue);
    else DrawLogic::ChangeColorModalRect(drawNumber,warningMode?Clr_RedKey:Clr_LightBlue);
}

void ModalButton::SetToStateColor(){
    DrawLogic::ChangeColorModalRect(drawNumber,Clr_LightBlue);
}

