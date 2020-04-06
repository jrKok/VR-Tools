#include "textline.h"
#include "drawlogic.h"

TextLine::TextLine() :
    bbl(0),
    bbb(0),
    bbr(0),
    bbt(0),
    textOfLine(""),
    width(0),
    height(12),
    index(0),
    textX(0),textY(0),textOx(2),textOy(2),
    mySize(0),
    isSelected(false),
    isModal(false),
    isVisible(true),
    textColor(Clr_BlackInk),
    backGroundcol(Clr_PaperWhite),
    ops()
{
    point pt;
    pt.SetCoords(0,0);
    myStringNumber=DrawLogic::AddString("",textColor,backGroundcol,pt);
}

TextLine::TextLine(bool modal) :
    bbl(0),
    bbb(0),
    bbr(0),
    bbt(0),
    textOfLine(""),
    width(0),
    height(12),
    index(0),
    textX(0),textY(0),textOx(2),textOy(2),
    mySize(0),
    isSelected(false),
    isModal(modal),
    isVisible(true),
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

void TextLine::setTextAndUpdate(const string &in_Str){
    textOfLine=ops.cleanOut(in_Str,"\r");
    mySize=static_cast<int>(textOfLine.length());
    DrawLogic::ChangeString(myStringNumber,textOfLine);
    PrintString();
}
string TextLine::GetText(){
    return textOfLine;
}

void TextLine::PrintString(){//Erases existint String by drawing the binding box, then prints string
    DrawLogic::PrintRectOnTexture(bbl,bbb,bbr,bbt,backGroundcol);
    DrawLogic::PrintString(myStringNumber);
}

void TextLine::PrintStringOnly(){//doesn't erase string at all if the zone was already wiped clean
    DrawLogic::PrintString(myStringNumber);
}

void TextLine::PrintStringOnLocalT(){
    DrawLogic::PrintStringOnLocalT(myStringNumber);
}

void TextLine::PrintBox(){
    DrawLogic::PrintRectOnTexture(bbl,bbb,bbr,bbt,backGroundcol);
}

bool TextLine::isHere(int x_in, int y_in){
    if ((x_in>=bbl)&&(x_in<=bbr)&&(y_in<=bbt)&&(y_in>=bbb)&&isVisible) return true;
    else return false;
}

void TextLine::SetTextColor(char in_clr){
     textColor=in_clr;
     DrawLogic::ChangeColorString(myStringNumber,in_clr);
     PrintString();
}

void TextLine::SetBackGroundColor(char in_clr){
    backGroundcol=in_clr;
    DrawLogic::ChangeBckGrdColorString(myStringNumber,in_clr);
}

void   TextLine::SetOrigin(int in_x,int in_y){
    bbl=in_x;
    bbb=in_y;
    bbr=bbl+width;
    bbt=bbb+height;
    textX=in_x+textOx;
    textY=in_y+textOy;
    point p(textX,textY);
    DrawLogic::RelocateString(myStringNumber,p);
}

void   TextLine::SetDimensions(int in_W, int in_H){
    width=in_W;
    height=in_H;
    bbr=bbl+width;
    bbt=bbb+height;
}

void   TextLine::SetTextXY (int in_x, int in_y){
    textOx=in_x;textOy=in_y;
    textX=bbl+textOx;
    textY=bbb+textOy;
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

void  TextLine::SetVisibility(bool in_vis){
    isVisible=in_vis;
    DrawLogic::SetVisibilityString(myStringNumber,in_vis);
    if (isVisible) PrintString();
    }

bool  TextLine::GetVisibility(){return isVisible;}
int   TextLine::GetTextX(){return textX;}
int   TextLine::GetTextY(){ return textY;}
int   TextLine::GetTop(){return bbt;}
int   TextLine::GetLeft(){return bbl;}
int   TextLine::GetBottom(){return bbb;}
int   TextLine::GetRight(){return bbr;}
int   TextLine::GetIndex(){return index;}
int   TextLine::GetWidth(){return width;}
int   TextLine::GetHeight(){return height;}
int   TextLine::GetStringSize(){return mySize;}
bool  TextLine::GetSelected(){return isSelected;}
