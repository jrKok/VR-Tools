#include "textline.h"

TextLine::TextLine() :
    textOfLine(""),
    index(0),
    x(0),y(0),offSetX(0),offSetY(0),in_top(0),in_left(0),width(0),height(0),top(0),right(0),
    isSelected(false),
    mySize(0)
{
}


void TextLine::recalculate(){
    x=in_left+offSetX;
    y=in_top-offSetY;
    top=y+height;
    right=x+width;

}

void TextLine::recalculate(int in_l,int in_t){
    in_left=in_l;
    in_top=in_t;
    x=in_left+offSetX;
    y=in_top-offSetY;
    top=y+height;
    right=x+width;
}
void TextLine::setText (std::string in_Str){
    textOfLine=in_Str;
    index=-1;
    mySize=in_Str.length();

}

bool TextLine::isHere(int x_in, int y_in){
    if ((x_in>=x)&(x_in<=right)&(y_in<=top)&(y_in>=y)) return true;
    else return false;
}

void TextLine::SetTextColor(float textcolr[]){
    textColor[0]=textcolr[0];
    textColor[1]=textcolr[1];
    textColor[2]=textcolr[2];
}

void TextLine::DrawTextLine(){
    XPLMDrawString(textColor,x,y,(char*)textOfLine.c_str(),NULL,xplmFont_Proportional);
}
