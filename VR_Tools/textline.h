#ifndef TEXTLINE_H
#define TEXTLINE_H

#include <string>
#include <stringops.h>
#include <XPLMGraphics.h>

using std::string;
class TextLine
{
public:
    TextLine();
    TextLine(bool modal);

    void   recalculate();
    void   recalculate(int in_l,int in_t);
    void   DeleteLine();
    void   setText (const string &in_Str);
    string GetText();
    bool   isHere(int,int);
    void   SetTextColor(char in_clr);
    void   SetOrigin(int in_x,int in_y);
    void   SetOffsets(int oX,int oY);
    void   SetDimensions(int in_W, int in_H);
    void   SetXY (int in_x, int in_y);
    void   SetIndex(int idx);
    void   SetSelected(bool sel);
    void   SetVisibility(bool in_vis);
    bool   GetVisibility();
    int    GetX();
    int    GetY();
    int    GetOffsetX();
    int    GetOffsetY();
    int    GetOffsetYRect();
    int    GetOffsetXRect();
    int    GetTop();
    int    GetLeft();
    int    GetBottom();
    int    GetRight();
    int    GetIndex();
    int    GetWidth();
    int    GetHeight();
    int    GetStringSize();
    bool   GetSelected();

private:
    string textOfLine;
    int    myStringNumber;
    int    index,x,y,offSetX,offSetY,in_top,in_left,width,height,top,bottom,left,right;
    int    mySize;
    bool   isSelected, isModal;
    char   textColor;
    stringOps ops;
};

#endif // TEXTLINE_H
