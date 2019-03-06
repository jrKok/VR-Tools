#ifndef RECTANGLES_H
#define RECTANGLES_H
#include <string>
#include <XPLMGraphics.h>
#include "XPLMUtilities.h"
#include <point.h>
#include <vector>
#include <array>


class rectangles
{
public:
    rectangles(bool drawable=true,bool ismodal=false);
    rectangles(int in_top, int in_left, int in_bottom, int in_right, char in_Color, bool in_visibility);

  virtual void setVisibility(bool in_Visibility);
  virtual      ~rectangles();
  virtual  void recalculate();
  virtual  void recalculate(int lt,int tp);

    void setColor(char in_Color);
    void setText(std::string);
    bool isHere(int,int);
    void resetMe();
    void WriteDebug(std::string message);

    void SetOrigin(int in_x,int in_y);
    void SetOffsets(int ox, int oy);
    void SetDimensions(int in_width, int in_height);
    void SetAngles(int in_left,int in_top, int in_right,int in_bottom);
    int  GetTop()const ;
    int  GetBottom()const;
    int  GetLeft()const;
    int  GetRight()const;
    int  GetWidth()const;
    int  GetHeight()const;
    int  GetOffsetX()const;
    int  GetOffsetY()const;
    void PrintParameters();
    void PrintModalParams();
    void SetAsModalRect0();

protected:
    int top,left,bottom,right,in_top,in_left,height,width,offsetX,offsetY;
    bool isVisible,isModal;
    int drawNumber;
};

#endif // RECTANGLES_H
