#include "button_vr.h"
#include "drawlogic.h"
#include "fontman.h"

button_VR::button_VR(bool modal) : rectangles(true),

    state_color(Clr_LightBlue),
    pressed_color(Clr_PushedBlue),
    activated_color(Clr_SelectBlue),
    text_color(Clr_White),
    isSelected(false),
    isPressed(false),
    hasSymbol(false),
    warningMode(false),
    fixedTextPos(false),
    buttonText(""),
    symbol(),
    isModalB(modal),
    offsetTextX(0),
    offsetTextY(0),
    numberPoints(0),
    fixedTextX(0),
    fixedTextY(0),
    symbolNumber(0),
    stringNumber(-1),
    font_Size(0)

{
   stringLocation.SetCoords(0,0);
}

button_VR::button_VR(string label, bool modal) : rectangles(label,true),

    state_color(Clr_LightBlue),
    pressed_color(Clr_PushedBlue),
    activated_color(Clr_SelectBlue),
    text_color(Clr_White),
    isSelected(false),
    isPressed(false),
    hasSymbol(false),
    warningMode(false),
    fixedTextPos(false),
    buttonText(""),
    symbol(),
    isModalB(modal),
    offsetTextX(0),
    offsetTextY(0),
    numberPoints(0),
    fixedTextX(0),
    fixedTextY(0),
    symbolNumber(0),
    stringNumber(-1),
    font_Size(0)

{
   stringLocation.SetCoords(0,0);
}

button_VR::~button_VR(){
 if (stringNumber) DrawLogic::DeleteString(stringNumber);
}

void button_VR::AddText(){
        stringNumber=DrawLogic::AddString("",Clr_White,Clr_LightBlue,stringLocation,0);
}

void button_VR::SetOrigin(int in_x, int in_y){
    if (in_y!=-1) bottom=in_y;
    if (in_x!=-1) left=in_x;
    right=left+width;
    top=bottom+height;
    LocateText();
    //ReDrawButton();
}
void button_VR::SetFontSize(int in_S){
    font_Size=in_S;
    DrawLogic::ChangeFontSize(stringNumber,in_S);
}
void button_VR::setText(const std::string &in_String){
    if (stringNumber==-1) AddText();
    buttonText=in_String;
    DrawLogic::ChangeString(stringNumber,in_String);
    DrawLogic::ChangeColorString(stringNumber,text_color);
    LocateText();
    ReDrawButton();
}

void button_VR::LocateText(){
    int tWidth=fontMan::MeasureString(buttonText,font_Size);
    int tHeight=fontMan::GetFontSize(font_Size);
    if (fixedTextPos){
        offsetTextX=left+fixedTextX;
        offsetTextY=bottom+fixedTextY;
    }else{
        offsetTextY=(height/2)-(tHeight/2)+bottom;
        if (tWidth>=(width-4)){
            width=tWidth+4;
            right=left+width;}
        offsetTextX=(width/2)-(tWidth/2)+left;
    }
    if (offsetTextX<left) offsetTextX=left;
    stringLocation.SetCoords(offsetTextX,offsetTextY);
    DrawLogic::RelocateString(stringNumber,stringLocation);
    //ReDrawButton();
}

void button_VR::SetTextOffsets(int oX,int oY){
    fixedTextPos=true;
    fixedTextX=oX;
    fixedTextY=oY;
    LocateText();
    ReDrawButton();
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
        DrawLogic::ChangeColorString(stringNumber,text_color);


    //calculates Offsets for text
    int tWidth=fontMan::MeasureString(in_String,font_Size);
    int tHeight=fontMan::GetFontSize(font_Size);
    offsetTextY=bottom+(height/2)+(tHeight/2);
    offsetTextX=left+(width/2)-(tWidth/2);
}

void button_VR::setTextColor(char to_Clr){
     text_color=to_Clr;
     DrawLogic::ChangeColorString(stringNumber,to_Clr);
}

void button_VR::setButtonColor(char to_Clr){  
        my_currentColor=to_Clr;
        state_color=to_Clr;
        DrawLogic::ChangeBckGrdColorString(stringNumber,to_Clr);
        ReDrawButton();
}

void button_VR::setStateColor(char to_Clr){
    state_color=to_Clr;
}

void button_VR::setSelectedColor(char to_Clr){
    pressed_color=to_Clr;
}

void button_VR::setActivatedColor(char to_Clr){
    activated_color=to_Clr;
}

void button_VR::resetMe(){
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
    my_currentColor=warningMode?Clr_Red:pressed_color;
    DrawLogic::ChangeBckGrdColorString(stringNumber,my_currentColor);
    ReDrawButton();
}

void button_VR::Release(){
    isPressed=false;
    if (isSelected)
    {
        my_currentColor=activated_color;
        DrawLogic::ChangeBckGrdColorString(stringNumber,my_currentColor);
        ReDrawButton();
    }
    else
    {
        my_currentColor=warningMode?Clr_RedKey:state_color;
        DrawLogic::ChangeBckGrdColorString(stringNumber,my_currentColor);
        ReDrawButton();
    }

}

void button_VR::setSelect(bool selState){
    isSelected=selState;
    if (isSelected)
    {
        my_currentColor=activated_color;
    }
    else
    {
        my_currentColor=warningMode?Clr_RedKey:state_color;
    }
    DrawLogic::ChangeBckGrdColorString(stringNumber,my_currentColor);

    ReDrawButton();
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
    my_currentColor=state_color;
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
