#include "button_vr.h"
#include "drawlogic.h"

button_VR::button_VR(bool modal) : rectangles(true,modal),

    isSelected(false),
    isPressed(false),
    hasSymbol(false),
    warningMode(false),
    buttonText(""),
    symbol(),
    isModalB(modal),
    offsetTextX(0),
    offsetTextY(0),
    numberPoints(0),
    symbolNumber(0),
    stringNumber(-1)

{
    stringLocation.myX=0;
    stringLocation.myY=0;
}

void button_VR::AddText(){
    if (!isModalB)
        stringNumber=DrawLogic::AddString("",Clr_White,stringLocation);
    else {
        stringNumber=DrawLogic::AddModalString("",Clr_White,stringLocation);
    }
}
  void button_VR::recalculate(){
    if (hasSymbol){
      for (ulong I(0);I<3;I++){
          symbol[I].recalculate(in_left,in_top);
          }
      std::array<int,6> sTriangle;
      sTriangle[0]=symbol[0].myX;
      sTriangle[1]=symbol[0].myY;
      sTriangle[2]=symbol[1].myX;
      sTriangle[3]=symbol[1].myY;
      sTriangle[4]=symbol[2].myX;
      sTriangle[5]=symbol[2].myY;
      DrawLogic::RelocateTriangle(symbolNumber,sTriangle);}

    top=in_top-offsetY;
    bottom=in_top-offsetY-height;
    left=in_left+offsetX;
    right=in_left+offsetX+width;


    stringLocation.myX=in_left+offsetTextX;
    stringLocation.myY=in_top-offsetTextY+2;
    if (!isModalB){
        DrawLogic::RelocateRect(drawNumber,*this);
        DrawLogic::RelocateString(stringNumber,stringLocation);
    }else{
        DrawLogic::RelocateModalRect(drawNumber,*this);
        DrawLogic::RelocateModalString(stringNumber,stringLocation);
    }



}

void button_VR::addPoint(int oX, int oY){
    point pt;
    pt.offsetX=oX;
    pt.offsetY=oY;
    hasSymbol=true;
    symbol.push_back(pt);
    numberPoints++;
    if (symbol.size()==3) {
        std::array<int,6> sTriangle;
        sTriangle[0]=symbol[0].myX;
        sTriangle[1]=symbol[0].myY;
        sTriangle[2]=symbol[1].myX;
        sTriangle[3]=symbol[1].myY;
        sTriangle[4]=symbol[2].myX;
        sTriangle[5]=symbol[2].myY;
        symbolNumber=DrawLogic::AddTriangle(sTriangle,Clr_DarkGray);
    }
}

void button_VR::setText(const std::string &in_String){
    if (stringNumber==-1) AddText();
    buttonText=in_String;
    if (!isModalB){
        DrawLogic::ChangeString(stringNumber,in_String);
        DrawLogic::ChangeColorString(stringNumber,Clr_White);
    }
    else{
        DrawLogic::ChangeModalString(stringNumber,in_String);
        DrawLogic::ChangeColorModalString(stringNumber,Clr_White);
    }
    int tWidth=static_cast<int>(XPLMMeasureString(xplmFont_Proportional,(char*)in_String.c_str(),in_String.length()));
    int tHeight(0);
    XPLMGetFontDimensions(xplmFont_Proportional,nullptr,&tHeight,nullptr);
    offsetTextY=offsetY+(height/2)+(tHeight/2);
    if (tWidth>=(width-4)) width=tWidth+4;
    offsetTextX=offsetX+(width/2)-(tWidth/2);
    recalculate();
}

void button_VR::SetTextOffsets(int oX,int oY){
    offsetTextX=oX;
    offsetTextY=oY;
}

void button_VR::SetOffsetY(int in_oY){
    offsetY=in_oY;
}

int  button_VR::GetTextOffsetX(){return offsetTextX;}
int  button_VR::GetTextOffsetY(){return offsetTextY;}

void button_VR::setTextFixedSize(const std::string &in_String){
    if (stringNumber==-1) AddText();
    buttonText=in_String;
    if (!isModalB){
        DrawLogic::ChangeString(stringNumber,in_String);
        DrawLogic::ChangeColorString(stringNumber,Clr_White);
    }
    else{
        DrawLogic::ChangeModalString(stringNumber,in_String);
        DrawLogic::ChangeColorModalString(stringNumber,Clr_White);
    }
    //calculates Offsets for text
    int tWidth=static_cast<int>(XPLMMeasureString(xplmFont_Proportional,(char*)in_String.c_str(),in_String.length()));
    int tHeight(0);
    XPLMGetFontDimensions(xplmFont_Proportional,nullptr,&tHeight,nullptr);
    offsetTextY=offsetY+(height/2)+(tHeight/2);
    offsetTextX=offsetX+(width/2)-(tWidth/2);
    recalculate();
}

void button_VR::setTextColor(char to_Clr){
    if (!isModalB)
        DrawLogic::ChangeColorString(stringNumber,to_Clr);
    else {
        DrawLogic::ChangeColorModalString(stringNumber,to_Clr);
    }
}

void button_VR::setSymbolColor(char to_Clr){
      DrawLogic::ChangeColorTriangle(symbolNumber,to_Clr);
}

void button_VR::setButtonColor(char to_Clr){
    if (!isModalB)
        DrawLogic::ChangeColorRect(drawNumber,to_Clr);
    else {
        DrawLogic::ChangeColorModalRect(drawNumber,to_Clr);
    }
}

void button_VR::resetMe(){
    symbol.clear();
    isVisible=true;
    isSelected=false;
    buttonText="";
    numberPoints=0;
    top=0;left=0;bottom=0;right=0;in_top=0;in_left=0;height=0;width=0;offsetX=0;offsetY=0;
    stringLocation.myX=0;stringLocation.myY=0; offsetTextX=0; offsetTextY=0;
    if (!isModalB){
        DrawLogic::RelocateRect(drawNumber,*this);
        DrawLogic::RelocateString(stringNumber,stringLocation);
    }else{
        DrawLogic::RelocateModalRect(drawNumber,*this);
        DrawLogic::RelocateModalString(stringNumber,stringLocation);
        if (hasSymbol)
        {
            std::array<int,6> sTriangle;
            DrawLogic::RelocateTriangle(symbolNumber,sTriangle);
            hasSymbol=false;
        }
    }

}

void button_VR::setVisibility(bool vis){
    isVisible=vis;
    if (!isModalB){
        DrawLogic::SetVisibilityRect(drawNumber,vis);
        DrawLogic::SetVisibilityString(stringNumber,vis);
    }
    else{
        DrawLogic::SetVisibilityModalRect(drawNumber,vis);
        DrawLogic::SetVisibilityModalString(stringNumber,vis);
        if (hasSymbol)
            DrawLogic::SetVisibilityTriangle(symbolNumber,vis);
    }

}

void button_VR::Press(){
    isPressed=true;
    if (!isModalB)
        DrawLogic::ChangeColorRect(drawNumber,warningMode?Clr_Red:Clr_PushedBlue);
    else {
        DrawLogic::ChangeColorModalRect(drawNumber,warningMode?Clr_Red:Clr_PushedBlue);
    }
}

void button_VR::Release(){
    isPressed=false;
    if (isSelected)
        if (!isModalB)
            DrawLogic::ChangeColorRect(drawNumber,Clr_SelectBlue);
        else {
            DrawLogic::ChangeColorModalRect(drawNumber,Clr_SelectBlue);
        }
    else
        if (!isModalB)
            DrawLogic::ChangeColorRect(drawNumber,warningMode?Clr_RedKey:Clr_LightBlue);
        else {
            DrawLogic::ChangeColorModalRect(drawNumber,warningMode?Clr_RedKey:Clr_LightBlue);
        }

}

void button_VR::setSelect(bool selState){
    isSelected=selState;
    if (isSelected) {
        if (!isModalB)
            DrawLogic::ChangeColorRect(drawNumber,warningMode?Clr_RedKey:Clr_SelectBlue);
        else {
            DrawLogic::ChangeColorModalRect(drawNumber,warningMode?Clr_RedKey:Clr_SelectBlue);
        }

    }
    else {
        if (!isModalB)
            DrawLogic::ChangeColorRect(drawNumber,warningMode?Clr_RedKey:Clr_LightBlue);
        else {
            DrawLogic::ChangeColorModalRect(drawNumber,warningMode?Clr_RedKey:Clr_LightBlue);
        }

    }
}
void button_VR::SetToWarningColor(){
    warningMode=true;
    if (!isModalB)
        DrawLogic::ChangeColorRect(drawNumber,Clr_RedKey);
    else {
        DrawLogic::ChangeColorModalRect(drawNumber,Clr_RedKey);
    }
}

void button_VR::SetToStateColor(){
    warningMode=false;
    if (!isModalB)
        DrawLogic::ChangeColorRect(drawNumber,Clr_LightBlue);
    else {
        DrawLogic::ChangeColorModalRect(drawNumber,Clr_LightBlue);
    }
}

void button_VR::DeleteButton(){
    if (hasSymbol) DrawLogic::DeleteTriangle(symbolNumber);
    resetMe();
    DrawLogic::DeleteString(stringNumber);
    DrawLogic::DeleteRect(drawNumber);
}
