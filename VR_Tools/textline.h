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

    string textOfLine;
    int    index,x,y,offSetX,offSetY,in_top,in_left,width,height,top,bottom,left,right;
    int    mySize;
    bool   isSelected;

    void   recalculate();
    void   recalculate(int in_l,int in_t);
    void   setText (const string &in_Str);
    string GetText();
    bool   isHere(int,int);
    void   SetTextColor(float textcolr[]);
    void   DrawTextLine();
private:
    float     textColor[3];
    stringOps ops;
};

#endif // TEXTLINE_H
