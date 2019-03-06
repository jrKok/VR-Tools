#include "textline.h"
#include "drawlogic.h"

TextLine::TextLine() :
    textOfLine(""),
    index(0),
    x(0),y(0),offSetX(0),offSetY(0),in_top(0),in_left(0),width(0),height(0),top(0),
    bottom(0),left(0),right(0),
    mySize(0),
    isSelected(false),
    isModal(false),
    ops()
{

    point pt;
    pt.myX=0;pt.myY=0;
    myStringNumber=DrawLogic::AddString("",Clr_BlackInk,pt);
}

TextLine::TextLine(bool modal) :
    textOfLine(""),
    index(0),
    x(0),y(0),offSetX(0),offSetY(0),in_top(0),in_left(0),width(0),height(0),top(0),
    bottom(0),left(0),right(0),
    mySize(0),
    isSelected(false),
    isModal(modal),
    ops()
{
    point pt;
    pt.myX=0;pt.myY=0;
    if (isModal)
        myStringNumber=DrawLogic::AddModalString("",Clr_BlackInk,pt);
    else
        myStringNumber=DrawLogic::AddString("",Clr_BlackInk,pt);
}

void TextLine::recalculate(){
    x=in_left+offSetX;
    y=in_top-offSetY-1;
    top=y+height;
    right=x+width+5;
    bottom=y-1;
    left=x-1;
    point p;
    p.myX=x;
    p.myY=y;
    if (isModal)
        DrawLogic::RelocateModalString(myStringNumber,p);
    else
        DrawLogic::RelocateString(myStringNumber,p);

}

void TextLine::recalculate(int in_l,int in_t){
    in_left=in_l;
    in_top=in_t;
    x=in_left+offSetX;
    y=in_top-offSetY-1;
    top=y+height;
    right=x+width+5;
    bottom=y-2;
    left=x-1;
    point p;
    p.myX=x;
    p.myY=y;
    if (isModal)
        DrawLogic::RelocateModalString(myStringNumber,p);
    else
        DrawLogic::RelocateString(myStringNumber,p);
}

void TextLine::DeleteLine(){
    DrawLogic::DeleteString(myStringNumber);
    myStringNumber=-1;
}

void TextLine::setText (const string &in_Str){
    textOfLine=ops.cleanOut(in_Str,"\r");
    mySize=static_cast<int>(textOfLine.length());
    if (isModal)
       DrawLogic::ChangeModalString(myStringNumber,textOfLine);
    else
       DrawLogic::ChangeString(myStringNumber,textOfLine);

}

string TextLine::GetText(){
    return textOfLine;
}

bool TextLine::isHere(int x_in, int y_in){
    if ((x_in>=left)&(x_in<=right)&(y_in<=top)&(y_in>=bottom)) return true;
    else return false;
}

void TextLine::SetTextColor(char in_clr){
    if (isModal)
        DrawLogic::ChangeColorModalString(myStringNumber,in_clr);
    else
        DrawLogic::ChangeColorString(myStringNumber,in_clr);
}


void   TextLine::SetOrigin(int in_l,int in_t){
    in_top=in_t;in_left=in_l;
}
void   TextLine::SetOffsets(int oX,int oY){
    offSetX=oX;offSetY=oY;
}
void   TextLine::SetDimensions(int in_W, int in_H){
    width=in_W;
    height=in_H;
}
void   TextLine::SetXY (int in_x, int in_y){
    x=in_x;y=in_y;
}
void   TextLine::SetIndex(int idx){
    index=idx;
}

void   TextLine::SetSelected(bool sel){
    isSelected=sel;
    if (isSelected){
        if (isModal)
            DrawLogic::ChangeColorModalString(myStringNumber,Clr_InkSelect);
        else
            DrawLogic::ChangeColorString(myStringNumber,Clr_InkSelect);
    }
}

void  TextLine::SetVisibility(bool in_vis){DrawLogic::SetVisibilityString(myStringNumber,in_vis);}
bool  TextLine::GetVisibility(){return DrawLogic::GetVisibilityString(myStringNumber);}
int   TextLine::GetX(){return x;}
int   TextLine::GetY(){ return y;}
int   TextLine::GetOffsetX(){return offSetX;}
int   TextLine::GetOffsetY(){return offSetY;}
int   TextLine::GetOffsetXRect(){return(offSetX-1);}
int   TextLine::GetOffsetYRect(){return (offSetY-height+1);}
int   TextLine::GetTop(){return top;}
int   TextLine::GetLeft(){return left;}
int   TextLine::GetBottom(){return bottom;}
int   TextLine::GetRight(){return right;}
int   TextLine::GetIndex(){return index;}
int   TextLine::GetWidth(){return width;}
int   TextLine::GetHeight(){return height;}
int   TextLine::GetStringSize(){return mySize;}
bool  TextLine::GetSelected(){return isSelected;}
