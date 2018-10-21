#ifndef RECTANGLES_H
#define RECTANGLES_H
#include <string>
#include <XPLMGraphics.h>
#include "XPLMUtilities.h"
#include <point.h>
#include <vector>

class rectangles
{
public:
    rectangles();
    virtual ~rectangles();
    int top,left,bottom,right,in_top,in_left,height,width,offsetX,offsetY;
    bool isVisible;

  virtual  void recalculate();
  virtual  void recalculate(int lt,int tp);
    void setText(std::string);
    bool isHere(int,int);

    void resetMe();
    void WriteDebug(std::string message);


};

#endif // RECTANGLES_H
