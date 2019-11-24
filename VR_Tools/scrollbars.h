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

class ScrollBars
{
public:
    ScrollBars(bool modal=false);
    bool IsCommandForMe(int x, int y, int& retVal);
    bool IsVisible();
    void BeginDrag(int,int);
    int  DragY(int);
    bool OngoingDrag();
    int  GetPosFirstLine();
    void EndDrag();
    void Setup(int height, int totLines, int firstLine, int linesInPage, int offX, int offY);
    void SetPosFirstLine(int firstLine);
    void Recalculate(int left, int top);
    void SetVisibility(bool iV);
    void LineUpNLines(int nL);
    void LineDownNLines (int nL);
    void UpPage();
    void DownPage();
    void WriteDebug(std::string message);
    void BeginRepeat(int cmd);
    bool ShouldRepeat();
    void EndRepeat();
    bool OngoingRepeat();

    enum{
        B_Line_Up=1,
        B_Line_Down=2,
        B_Page_Up=3,
        B_Page_Down=4
        };
private:
    int totalLines,page,numberfirstLine,limitLastLine;
    int totalheight,topCore,lowCore,freeCore,heightOfCore,heightOfLift,posOfLift;
    int offsetX, offsetY,y0,dragPosY, cmdToRepeat;
    float linesPPx, epoch,waitForRepeat;
    bool isVisible, drag, repeatCmd;
    rectangles general,core;
    button_VR commandUp,commandDown,lift;

};

#endif // SCROLLBARS_H
