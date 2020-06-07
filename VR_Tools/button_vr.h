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
    button_VR(string label,bool modal=false);
    ~button_VR();

virtual void setVisibility(bool vis);
virtual void SetOrigin(int in_x, int in_y);
virtual void Shift(int dx,int dy);
virtual void ReDrawButton();
    void AddText();
    void LocateText();
    void LocateTextFixedSize();
    void SetTextOffsets(int oX,int oY);
    int  GetTextX();
    int  GetTextY();
    void addPoint(int,int);
    void setText(const std::string &);
    void setTextFixedSize(const std::string &in_String);
    void setTextColor(char to_Clr);
    void setButtonColor(char to_Clr);
    void setStateColor (char to_Clr);
    void setSelectedColor(char to_Clr);
    void setActivatedColor(char to_Clr);
    void resetMe();
    void Press();
    void Release();
    void setSelect(bool selState);
    void SetToWarningColor();
    void SetToStateColor();
    void SetOffsetY(int in_oY);
    void SetFontSize(int in_S);

    char GetStringColorCode();

protected:

    char state_color,pressed_color,activated_color,text_color;
    bool isSelected,isPressed,hasSymbol,warningMode,fixedTextPos;
    std::string buttonText;
    std::vector<point> symbol;
    bool isModalB;
    int  offsetTextX, offsetTextY,numberPoints,fixedTextX,fixedTextY;
    int symbolNumber,stringNumber,font_Size;
    point stringLocation;

};

#endif // BUTTON_VR_H
