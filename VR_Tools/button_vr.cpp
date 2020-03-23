#include "button_vr.h"
#include "drawlogic.h"

button_VR::button_VR(bool modal) : rectangles(true),

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
   stringLocation.SetCoords(0,0);
}

button_VR::button_VR(string label, bool modal) : rectangles(label,true),

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
   stringLocation.SetCoords(0,0);
}
void button_VR::AddText(){
        stringNumber=DrawLogic::AddString("",Clr_White,Clr_LightBlue,stringLocation);
}

void button_VR::SetOrigin(int in_x, int in_y){
    if (in_y!=-1) bottom=in_y;
    if (in_x!=-1) left=in_x;
    right=left+width;
    top=bottom+height;
    LocateText();
}

void button_VR::setText(const std::string &in_String){
    if (stringNumber==-1) AddText();
    buttonText=in_String;
    DrawLogic::ChangeString(stringNumber,in_String);
    DrawLogic::ChangeColorString(stringNumber,Clr_White);
    LocateText();
}

void button_VR::LocateText(){
    int tWidth=static_cast<int>(XPLMMeasureString(xplmFont_Proportional,(char*)buttonText.c_str(),buttonText.length()));
    int tHeight(0);
    XPLMGetFontDimensions(xplmFont_Proportional,nullptr,&tHeight,nullptr);
    offsetTextY=(height/2)-(tHeight/2)+bottom;
    if (tWidth>=(width-4)){
        width=tWidth+4;
        right=left+width;}
    offsetTextX=(width/2)-(tWidth/2)+left;
    stringLocation.SetCoords(offsetTextX,offsetTextY);
    DrawLogic::RelocateString(stringNumber,stringLocation);
}

void button_VR::SetTextOffsets(int oX,int oY){
    offsetTextX=oX;
    offsetTextY=oY;
}

void button_VR::SetOffsetY(int in_oY){
    bottom=in_oY-height;
}

int  button_VR::GetTextX(){return offsetTextX;}
int  button_VR::GetTextY(){return offsetTextY;}

void button_VR::setTextFixedSize(const std::string &in_String){
    if (stringNumber==-1) AddText();
    buttonText=in_String;

        DrawLogic::ChangeString(stringNumber,in_String);
        DrawLogic::ChangeColorString(stringNumber,Clr_White);


    //calculates Offsets for text
    int tWidth=static_cast<int>(XPLMMeasureString(xplmFont_Proportional,(char*)in_String.c_str(),in_String.length()));
    int tHeight(0);
    XPLMGetFontDimensions(xplmFont_Proportional,nullptr,&tHeight,nullptr);
    offsetTextY=bottom+(height/2)+(tHeight/2);
    offsetTextX=left+(width/2)-(tWidth/2);
}

void button_VR::setTextColor(char to_Clr){
        DrawLogic::ChangeColorString(stringNumber,to_Clr);
}

void button_VR::setButtonColor(char to_Clr){  
        my_currentColor=to_Clr;
        DrawLogic::ChangeBckGrdColorString(stringNumber,to_Clr);
        ReDrawButton();
}

void button_VR::resetMe(){
    symbol.clear();
    isVisible=false;
    isSelected=false;
    buttonText="";
    numberPoints=0;
    top=0;left=0;bottom=0;right=0;height=0;width=0;
    stringLocation.SetCoords(0,0); offsetTextX=0; offsetTextY=0;
    DrawLogic::RelocateString(stringNumber,stringLocation);

}

void button_VR::setVisibility(bool vis){
    if (vis!=isVisible){
        isVisible=vis;
        DrawLogic::SetVisibilityString(stringNumber,vis);
        if (!isVisible)
           {DrawLogic::HideRectangle(drawNumber);
            DrawLogic::SetVisibilityString(stringNumber,false);}
        else {
            DrawLogic::SetVisibilityString(stringNumber,true);
            ReDrawButton();
        }
    }
}

void button_VR::Press(){
    isPressed=true;
    my_currentColor=warningMode?Clr_Red:Clr_PushedBlue;
    DrawLogic::ChangeBckGrdColorString(stringNumber,my_currentColor);
    ReDrawButton();
}

void button_VR::Release(){
    isPressed=false;
    if (isSelected)
    {
        my_currentColor=Clr_SelectBlue;
        DrawLogic::ChangeBckGrdColorString(stringNumber,my_currentColor);
        ReDrawButton();
    }
    else
    {
        my_currentColor=warningMode?Clr_RedKey:Clr_LightBlue;
        DrawLogic::ChangeBckGrdColorString(stringNumber,my_currentColor);
        ReDrawButton();
    }

}

void button_VR::setSelect(bool selState){
    isSelected=selState;
    if (isSelected)
    {
        my_currentColor=Clr_SelectBlue;
        DrawLogic::ChangeBckGrdColorString(stringNumber,my_currentColor);
        ReDrawButton();
    }
    else
    {
        my_currentColor=warningMode?Clr_RedKey:Clr_LightBlue;
        DrawLogic::ChangeBckGrdColorString(stringNumber,my_currentColor);
        ReDrawButton();
    }
}

void button_VR::SetToWarningColor(){
    warningMode=true;
    my_currentColor=Clr_RedKey;
    DrawLogic::ChangeBckGrdColorString(stringNumber,my_currentColor);
    ReDrawButton();
}

void button_VR::SetToStateColor(){
    warningMode=false;
    isSelected=false;
    isPressed=false;
    my_currentColor=Clr_LightBlue;
    DrawLogic::ChangeBckGrdColorString(stringNumber,my_currentColor);
    ReDrawButton();
}

void button_VR::ReDrawButton(){
    DrawLogic::UpdateRectangle(drawNumber);
    DrawLogic::PrintString(stringNumber);
}
char button_VR::GetStringColorCode(){
    return DrawLogic::GetColorCodeString(stringNumber);
}

void button_VR::Shift(int dx, int dy){
    rectangles::Shift(dx,dy);
    LocateText();
}
