#ifndef BUTTON_VR_H
#define BUTTON_VR_H
#include <string>
#include <vector>
#include "rectangles.h"
#include <XPLMDisplay.h>
#include "VR_Tools_global.h"


#if IBM
    #include <windows.h>
#endif

class button_VR:public rectangles
{
public:
    button_VR(bool modal=false);
    bool isSelected,isPressed,hasSymbol,warningMode;
    //char* buttonText;
    std::string buttonText;
    std::vector<point> symbol;

virtual void recalculate();
virtual void setVisibility(bool vis);
    void AddText();
    void SetTextOffsets(int oX,int oY);
    int  GetTextOffsetX();
    int  GetTextOffsetY();
    void addPoint(int,int);
    void setText(const std::string &);
    void setTextFixedSize(const std::string &in_String);
    void setTextColor(char to_Clr);
    void setSymbolColor(char to_Clr);
    void setButtonColor(char to_Clr);
    void resetMe();
    void Press();
    void Release();
    void setSelect(bool selState);
    void SetToWarningColor();
    void SetToStateColor();
    void DeleteButton();
    void SetOffsetY(int in_oY);

protected:
    bool isModalB;
    int  offsetTextX, offsetTextY,numberPoints;
    int symbolNumber,stringNumber;
    point stringLocation;

};

#endif // BUTTON_VR_H
