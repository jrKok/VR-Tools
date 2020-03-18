#ifndef TEXTLINE_H
#define TEXTLINE_H

#include <string>
#include <stringops.h>
#include <XPLMGraphics.h>
#include <rectangles.h>

using std::string;
class TextLine
{
public:
    TextLine();
    TextLine(bool modal);

    void   DeleteLine();
    void   setText (const string &in_Str);
    string GetText();
    void   PrintString();
    void   PrintStringOnly();
    bool   isHere(int,int);
    void   SetTextColor(char in_clr);
    void   SetBackGroundColor(char in_clr);
    void   SetOrigin(int in_x,int in_y);
    void   SetDimensions(int in_W, int in_H);
    void   SetTextXY (int in_x, int in_y);
    void   SetIndex(int idx);
    void   SetSelected(bool sel);
    void   SetVisibility(bool in_vis);
    bool   GetVisibility();

    int    GetTextX();
    int    GetTextY();
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
    rectangles bindingBox;
    string     textOfLine;
    int        myStringNumber;
    int        index,textX,textY,textOx,textOy;
    int        mySize;
    bool       isSelected, isModal;
    char       textColor,backGroundcol;
    stringOps ops;
};

#endif // TEXTLINE_H
