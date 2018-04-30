#ifndef SCROLLBARS_H
#define SCROLLBARS_H
#include <string>
#include "rectangles.h"
#include "button_vr.h"
#include "VR_Tools_global.h"
#include <XPLMDisplay.h>
#include <XPLMUtilities.h>
#include <XPLMProcessing.h>
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


class ScrollBars
{
public:
    ScrollBars();
    bool IsCommandForMe(int x, int y, int& retVal);
    bool IsVisible();
    void BeginDrag(int,int);
    int  DragY(int);
    bool OngoingDrag();
    int  GetPosFirstLine();
    void EndDrag();
    void Setup(int height, int totLines, int firstLine, int linesInPage, int offX, int offY);
    void SetPosFirstLine(int firstLine);
    void Refresh(int height, int totLines, int firstLine, int linesOnPage);
    void Recalculate(int left, int top);
    void SetVisibility(bool iV);
    void LineUpNLines(int nL);
    void LineDownNLines (int nL);
    void UpPage();
    void DownPage();
    void DrawMySelf();
    void WriteDebug(std::string message);
    void BeginRepeat(int cmd);
    bool ShouldRepeat();
    void EndRepeat();
    bool OngoingRepeat();

private:
    int totalLines,page,numberfirstLine,limitLastLine;
    int totalheight,topCore,lowCore,freeCore,heightOfCore,heightOfLift,posOfLift;
    int offsetX, offsetY,y0,dragPosY, cmdToRepeat;
    float linesPPx, epoch,waitForRepeat;
    bool isVisible, drag, repeatCmd;
    button_VR commandUp,commandDown,lift;
    rectangles general,core;
    float colorWhite[3],colorGray[3],colorlightGray[3];

};

#endif // SCROLLBARS_H
