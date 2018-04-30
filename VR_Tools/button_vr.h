#ifndef BUTTON_VR_H
#define BUTTON_VR_H
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


class button_VR:public rectangles
{
public:
    button_VR();
    bool isSelected,isPressed,hasSymbol;
    int posTextX,posTextY, offsetTextX, offsetTextY,numberPoints;
    //char* buttonText;
    std::string buttonText;
    std::vector<point> symbol;

virtual    void recalculate();
    void addPoint(int,int);
    void setText(std::string);
    void drawButton();
    void setColor(float custColor[3]);
    void resetMe();
    void Press();
    void Release();
    void setSelect(bool selState);

private:
    float myColor[3],darkgray[3],colorPressed[3],colorSelected[3],niceWhite[3];
};

#endif // BUTTON_VR_H
