#ifndef MODALBUTTON_H
#define MODALBUTTON_H

#include <string>
#include <vector>
#include "rectangles.h"
#include <XPLMDisplay.h>


#if IBM
    #include <windows.h>
#endif
#if LIN
    #include <GL/gl.h>
#elif __GNUC__
    #include <OpenGL/gl.h>
#else
    #include <GL/gl.h>
#endif
using ulong=unsigned long long;

class ModalButton:public rectangles
{
public:
    ModalButton();
    bool isSelected,isPressed,hasSymbol,warningMode;
    //char* buttonText;
    std::string buttonText;

virtual void recalculate();
virtual void setVisibility(bool vis);
virtual void recalculate(int in_X,int in_Y);
    void AddText();
    void SetTextOffsets(int oX,int oY);
    int  GetTextOffsetX();
    int  GetTextOffsetY();
    void setText(const std::string &);
    void setTextFixedSize(const std::string &in_String);
    void setTextColor(char to_Clr);
    void setButtonColor(char to_Clr);
    void resetMe();
    void Press();
    void Release();
    void setSelect(bool selState);
    void SetToStateColor();
    void SetOffsetY(int in_oY);

protected:
    int  offsetTextX, offsetTextY,numberPoints;
    int symbolNumber,stringNumber;
    point stringLocation;

};


#endif // MODALBUTTON_H
