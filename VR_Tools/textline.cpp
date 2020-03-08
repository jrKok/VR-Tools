#include "textline.h"
#include "drawlogic.h"

TextLine::TextLine() :
    textOfLine(""),
    index(0),
    textX(0),textY(0),textOx(0),textOy(0),
    width(0),height(0),
    top(0),bottom(0),left(0),right(0),
    mySize(0),
    isSelected(false),
    isModal(false),
    ops()
{

    point pt;
    pt.SetCoords(0,0);
    myStringNumber=DrawLogic::AddString("",Clr_BlackInk,pt);
}

TextLine::TextLine(bool modal) :
    textOfLine(""),
    index(0),
    textX(0),textY(0),textOx(0),textOy(0),
    width(0),height(0),
    top(0),bottom(0),left(0),right(0),
    mySize(0),
    isSelected(false),
    isModal(modal),
    textColor(Clr_BlackInk),
    ops()
{
    point pt;
    pt.SetCoords(0,0);
    myStringNumber=DrawLogic::AddString("",textColor,pt);
}

void TextLine::recalculate(){
    right=left+width;
    top=bottom+height;
    textX=left+textOx;
    textY=bottom+textOy;

    point p;
    p.SetCoords(textX,textY);
    DrawLogic::RelocateString(myStringNumber,p);

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

void TextLine::PrintString(){
    DrawLogic::PrintString(myStringNumber);
}
bool TextLine::isHere(int x_in, int y_in){
    if ((x_in>=left)&(x_in<=right)&(y_in<=top)&(y_in>=bottom)) return true;
    else return false;
}

void TextLine::SetTextColor(char in_clr){
        textColor=in_clr;
        DrawLogic::ChangeColorString(myStringNumber,in_clr);
        DrawLogic::PrintString(myStringNumber);
}


void   TextLine::SetOrigin(int in_x,int in_y){
        left=in_x;
        bottom=in_y;
        recalculate();
}

void   TextLine::SetDimensions(int in_W, int in_H){
    width=in_W;
    height=in_H;
}
void   TextLine::SetTextXY (int in_x, int in_y){
    textOx=in_x;textOy=in_y;
    recalculate();
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
int   TextLine::GetTop(){return top;}
int   TextLine::GetLeft(){return left;}
int   TextLine::GetBottom(){return bottom;}
int   TextLine::GetRight(){return right;}
int   TextLine::GetIndex(){return index;}
int   TextLine::GetWidth(){return width;}
int   TextLine::GetHeight(){return height;}
int   TextLine::GetStringSize(){return mySize;}
bool  TextLine::GetSelected(){return isSelected;}
