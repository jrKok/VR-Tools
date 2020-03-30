#ifndef RECTANGLES_H
#define RECTANGLES_H
#include <XPLMGraphics.h>
#include "XPLMUtilities.h"
#include "point.h"
#include <string>

using std::string;
using ulong=unsigned long long;

class rectangles
{
public:
    rectangles(bool drawable=true);
    rectangles(string name,bool drawable=true);
    rectangles(int in_top,
               int in_left,
               int in_bottom,
               int in_right,
              char in_Color,
              bool in_visibility);

  virtual     ~rectangles();
  virtual void setVisibility(bool in_Visibility);
  virtual void SetOrigin(int in_x,int in_y);
  virtual void Shift(int dx,int dy);

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
   ulong GetId()const;
    bool IsVisible()const;
    char GetColor()const;
    void PrintParameters();
    void UpdateMyTexture();
    void SetDebugName(string in_string);
  string GetDebugName();
    void SetNewDrawNumber(ulong in_Nb);
   ulong GetDrawNumber();

protected:
    int top,left,bottom,right,height,width;
    char my_currentColor;
    bool isDrawable,isVisible,isModal;
    ulong drawNumber;
    string dbgName;
};

#endif // RECTANGLES_H
