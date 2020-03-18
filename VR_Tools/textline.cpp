#include "textline.h"
#include "drawlogic.h"

TextLine::TextLine() :
    bindingBox(true),
    textOfLine(""),
    index(0),
    textX(0),textY(0),textOx(0),textOy(0),
    mySize(0),
    isSelected(false),
    isModal(false),
    textColor(Clr_BlackInk),
    backGroundcol(Clr_PaperWhite),
    ops()
{
    point pt;
    pt.SetCoords(0,0);
    myStringNumber=DrawLogic::AddString("",textColor,backGroundcol,pt);
}

TextLine::TextLine(bool modal) :
    textOfLine(""),
    index(0),
    textX(0),textY(0),textOx(1),textOy(1),
    mySize(0),
    isSelected(false),
    isModal(modal),
    textColor(Clr_BlackInk),
    backGroundcol(Clr_PaperWhite),
    ops()
{
    point pt;
    pt.SetCoords(0,0);
    myStringNumber=DrawLogic::AddString("",textColor,backGroundcol,pt);
}



void TextLine::DeleteLine(){
    DrawLogic::DeleteString(myStringNumber);
    myStringNumber=-1;
}

void TextLine::setText (const string &in_Str){
    textOfLine=ops.cleanOut(in_Str,"\r");
    mySize=static_cast<int>(textOfLine.length());
    DrawLogic::ChangeString(myStringNumber,textOfLine);
}

string TextLine::GetText(){
    return textOfLine;
}

void TextLine::PrintString(){//Erases existint String by drawing the binding box, then prints string
    bindingBox.UpdateMyTexture();
    DrawLogic::PrintString(myStringNumber);
}

void TextLine::PrintStringOnly(){//doesn't erase string at all if the zone was already wiped clean
    DrawLogic::PrintString(myStringNumber);
}

bool TextLine::isHere(int x_in, int y_in){
    if (bindingBox.isHere(x_in,y_in)) return true;
    else return false;
}

void TextLine::SetTextColor(char in_clr){
     textColor=in_clr;
     DrawLogic::ChangeColorString(myStringNumber,in_clr);
     bindingBox.UpdateMyTexture();
     DrawLogic::PrintString(myStringNumber);
}

void TextLine::SetBackGroundColor(char in_clr){
    backGroundcol=in_clr;
    DrawLogic::ChangeBckGrdColorString(myStringNumber,in_clr);
    bindingBox.setColor(in_clr);
}

void   TextLine::SetOrigin(int in_x,int in_y){
    bindingBox.SetOrigin(in_x,in_y);
    textX=in_x+textOx;
    textY=in_y+textOy;
    point p(textX,textY);
    DrawLogic::RelocateString(myStringNumber,p);
}

void   TextLine::SetDimensions(int in_W, int in_H){
    bindingBox.SetDimensions(in_W,in_H);
}

void   TextLine::SetTextXY (int in_x, int in_y){
    textOx=in_x;textOy=in_y;
    textX=bindingBox.GetLeft()+textOx;
    textY=bindingBox.GetBottom()+textOy;
    point p(textX,textY);
    DrawLogic::RelocateString(myStringNumber,p);
}

void   TextLine::SetIndex(int idx){
    index=idx;
}

void   TextLine::SetSelected(bool sel){
    if (isSelected!=sel){
        isSelected=sel;
        if (isSelected){
            DrawLogic::ChangeColorString(myStringNumber,Clr_InkSelect);
        }
        else{
            DrawLogic::ChangeColorString(myStringNumber,textColor);
        }
    }
}

void  TextLine::SetVisibility(bool in_vis){DrawLogic::SetVisibilityString(myStringNumber,in_vis);}
bool  TextLine::GetVisibility(){return DrawLogic::GetVisibilityString(myStringNumber);}
int   TextLine::GetTextX(){return textX;}
int   TextLine::GetTextY(){ return textY;}
int   TextLine::GetTop(){return bindingBox.GetTop();}
int   TextLine::GetLeft(){return bindingBox.GetLeft();}
int   TextLine::GetBottom(){return bindingBox.GetBottom();}
int   TextLine::GetRight(){return bindingBox.GetRight();}
int   TextLine::GetIndex(){return index;}
int   TextLine::GetWidth(){return bindingBox.GetWidth();}
int   TextLine::GetHeight(){return bindingBox.GetHeight();}
int   TextLine::GetStringSize(){return mySize;}
bool  TextLine::GetSelected(){return isSelected;}
