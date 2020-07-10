#ifndef TEXTLINE_H
#define TEXTLINE_H

#include <string>
#include <stringops.h>
#include <XPLMGraphics.h>
#include <rectangles.h>

/*Very simple widget which holds a line of text,
 * coordinates for a rectangle around that text.
 * Doesn't use rectangles to avoid registration in
 * Drawlogic, manages its own drawing when needed
 */

using std::string;
class TextLine
{
public:
    TextLine();
    TextLine(bool modal);

    void   DeleteLine();
    void   setText (const string &in_Str);
    string GetText();
    void   setTextAndUpdate(const string &in_Str);
    void   PrintBox();
    void   PrintString();
    void   PrintStringOnly();
    void   PrintStringOnLocalT();
    bool   isHere(int,int);
    void   SetTextColor(char in_clr);
    void   SetBackGroundColor(char in_clr);
    void   SetOrigin(int in_x,int in_y);
    void   SetDimensions(int in_W, int in_H);
    void   SetTextXY (int in_x, int in_y);
    void   SetIndex(int idx);
    void   SetSelected(bool sel);
    void   SetVisibility(bool in_vis);
    void   SetFontSize(int fs);

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
    int    GetStringWidth();

private:
    int        bbl,bbb,bbr,bbt;
    int        font_Size;
    string     textOfLine;
    int        myStringNumber;
    int        width,height;
    int        index,textX,textY,textOx,textOy;
    int        mySize;
    bool       isSelected, isModal,isVisible;
    char       textColor,backGroundcol;
    stringOps ops;
};

#endif // TEXTLINE_H
