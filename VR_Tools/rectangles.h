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
    rectangles(bool drawable=true);
    rectangles(int in_top,
               int in_left,
               int in_bottom,
               int in_right,
              char in_Color,
              bool in_visibility);

  virtual     ~rectangles();
  virtual void setVisibility(bool in_Visibility);
  virtual void SetOrigin(int in_x,int in_y);

    void setColor(char in_Color);
    bool isHere(int,int);
    void resetMe();
    void WriteDebug(std::string message);
    void SetDimensions(int in_width, int in_height);
    void SetAngles(int in_left, int in_bottom, int in_right, int in_top);
    int  GetTop()const ;
    int  GetBottom()const;
    int  GetLeft()const;
    int  GetRight()const;
    int  GetWidth()const;
    int  GetHeight()const;
    int  GetId()const;
    bool IsVisible()const;
    char GetColor()const;
    void PrintParameters();
    void Shift(int dx,int dy);
    void UpdateMyTexture();

protected:
    int top,left,bottom,right,height,width;
    char my_currentColor;
    bool isVisible,isModal;
    int drawNumber;
};

#endif // RECTANGLES_H
