#ifndef LAYOUT_H
#define LAYOUT_H
#include <string>
#include <vector>
#include <locale>
#include "XPLMDisplay.h"    // for window creation and manipulation
#include "XPLMGraphics.h"   // for window drawing
#include "XPLMDataAccess.h" // for the VR dataref
#include "XPLMPlugin.h"     // for XPLM_MSG_SCENERY_LOADED message
#include "XPLMUtilities.h"
#include "inisettings.h"
#include "textreader.h"
#include "rectangles.h"
#include "button_vr.h"
#include "textline.h"
#include "VR_Tools_global.h"
#include "textreader.h"

class TextReader;

class Layout
{
public:
    Layout();
    virtual ~Layout();
    int gTop,gBottom,gLeft,gRight;//Used for initial calculations and coordinates,global coordinates
    int wTop,wBottom,wLeft,wRight,wWidth,wHeight,minWidth,maxWidth,minHeight,maxHeight;
    int charHeight,charWidth,textPointX,textPointY;//Used for initial setup and resize instructions
    int t,b,l,r;//input from draw instruction mostly
    int textHeight,textWidth,colWidth,idxSelected,nButtons;
    float reloadPeriod;
    TextReader *tFileReader;
    rectangles generalR;
    std::vector<button_VR> tButtons;

     static void WriteDebug(std::string message);
     static void InitCycleDebug(int nbs);
     static void WriteDebugInCycles(std::string message);
            void Begin();
    virtual bool initiate();
    virtual void resize();
            void resizeVRWindow();
    virtual bool newSize(int wth,int hgt);
            void RelocateButtons(int middle);
            void recalculate(float cT);
    virtual void FitToFile();
             int rightRectangle(int idx);
             int leftRectangle(int idx);
             int topRectangle(int idx);
             int bottomRectangle(int idx);
            void defineButtons();
    virtual void findClick(int mX, int mY);
    virtual void HandleMouseKeepDown(int mX,int mY);
    virtual int  HandleMouseUp(int mX,int mY);
    virtual void CheckButtonsVisibility();
    virtual void ClosegWindow();
    virtual void LaunchCommand(int refCommand);
    virtual std::string GetFileName();
            std::string GetDirName();
            void SetWindowHandle(XPLMWindowID thisW);
            bool isWindowInVR();
    virtual void DrawTextW(XPLMWindowID g_textWindow);
    virtual void DrawNoResize(XPLMWindowID g_textWindow);
            bool OpenWindowAtStart();
            void DoFlash();
            bool GetResizeOption();
            void ToggleFPS();
            void KeepFile();
            void KeepCurrentSize();
            static int cycle;

protected:
    XPLMDataRef nav1on,nav2on,com1on,com2on,adf1on,adf2on;
    XPLMDataRef nav1freq,nav2freq,com1freq,com2freq,adf1freq,adf2freq,com1freqk,com2freqk;
    XPLMDataRef dref_SunPitch,g_FPS;
    XPLMWindowID myWindow;
    IniSettings iniParams;
    TextLine fNav,fCom,fAdf,lFPS;
    std::string charSep;
    float epoch,fpsTag;
    bool  openAtStart,goToLastPage;
    bool  continueClick,buttonClick,autoReload,saveAuto,canUTF,autoReloadOnSize,useBackGround;
    bool  flash,flashWhenChange,noResize,fitSizeToFile,keepLastFile,keepSize,enableDelete,enableFreqs,showFPS;
    int   clickresult, splitLinePolicy;
    int   vrWidth,vrHeight,upperMargin,lowerMargin;
    float currentFPS;
    float lightGray[3];
    float cyan[3];
    float green[3];
    float gray[3];
    float lightergray[3];
    float paperWhitesomber[3];
    //float red[]= {0.90f,0.0f,0.0f};
    float darkgray[3];
    float blackpaper[3];
    float colorPaper[3]; //Duron paper white
    float colorInk[3];
    float amberInk[3];
    float beginFlash;

};

#endif // LAYOUT_H
