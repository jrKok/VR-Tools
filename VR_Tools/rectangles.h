#ifndef RECTANGLES_H
#define RECTANGLES_H
#include <string>
#include <XPLMGraphics.h>
#include <point.h>
#include <vector>

class rectangles
{
public:
    rectangles();
    int top,left,bottom,right,in_top,in_left,height,width,offsetX,offsetY;
    bool isVisible;

  virtual  void recalculate();
  virtual  void recalculate(int lt,int tp);
    void setText(std::string);
    bool isHere(int,int);

    void resetMe();


};

#endif // RECTANGLES_H
