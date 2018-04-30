#ifndef TEXTLINE_H
#define TEXTLINE_H

#include <string>

class TextLine
{
public:
    TextLine();
    std::string textOfLine;
    int index,x,y,offSetX,offSetY,in_top,in_left,width,height,top,right;
    int mySize;
    bool isSelected;
    void recalculate();
    void recalculate(int in_l,int in_t);
    void setText (std::string in_Str);
    bool isHere(int,int);
};

#endif // TEXTLINE_H
