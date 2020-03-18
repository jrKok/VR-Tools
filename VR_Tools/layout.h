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
#include "textreader.h"
#include "rectangles.h"
#include "button_vr.h"
#include "textline.h"
#include "VR_Tools_global.h"
#include "linedialog.h"
#include "vrcreader.h"
#include "drawlogic.h"

using std::vector;
using std::string;

class TextReader;

class Layout
{
public:
    Layout(DrawLogic *newPad);
    virtual ~Layout();


     static void WriteDebug(string message);
     static void InitCycleDebug(int nbs);
     static void WriteDebugInCycles(string message);
            void Begin();
    virtual bool initiate();
    virtual bool resize();
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
            void MakeButton(bool visible,string in_Label,int width,int height,int oX, int oY);
            virtual void findClick(int mX, int mY);
    virtual void HandleMouseKeepDown(int mX,int mY);
    virtual int  HandleMouseUp(int mX,int mY);
    virtual void CheckButtonsVisibility();
    virtual void ClosegWindow();
    virtual void LaunchCommand(int refCommand);
    virtual string GetFileName();
            string GetDirName();
            void SetWindowHandle(XPLMWindowID thisW);
            bool isWindowInVR();
    virtual void DrawTextW(XPLMWindowID g_textWindow);
            bool OpenWindowAtStart();
            void DoFlash();
            bool GetResizeOption();
    virtual void ToggleFPS();
            void KeepFile();
            void KeepCurrentSize();
    static  void HandleDialog();

       DrawLogic *myDrawPad;
       //int       gTop,gLeft,gRight,gBottom;

       static int cycle;
              int wTop,wBottom,wLeft,wRight;
              int wWidth,wHeight,minWidth,maxWidth,minHeight,maxHeight;

protected:

 LineDialog        myDialog;
 int               charHeight,charWidth,textPointX,textPointY;//Used for initial setup and resize instructions
 int               t,b,l,r;//input from draw instruction mostly
               int textHeight,textWidth,colWidth,idxSelected,nButtons;
             float reloadPeriod;
        rectangles generalR;
       TextReader *tFileReader;

vector<button_VR*> tButtons;


      XPLMDataRef nav1on,nav2on,com1on,com2on,adf1on,adf2on;
      XPLMDataRef nav1freq,nav2freq,com1freq,com2freq,adf1freq,adf2freq,com1freqk,com2freqk;
      XPLMDataRef dref_SunPitch,g_FPS;
   XPLMCommandRef screenShot;
     XPLMWindowID myWindow;

         TextLine fNav,fCom,fAdf,lFPS;
           string charSep;
            float epoch,fpsTag;
            bool  openAtStart,goToLastPage;
            bool  continueClick,buttonClick,autoReload,saveAuto,canUTF,autoReloadOnSize,useBackGround;
            bool  flash,flashWhenChange,noResize,fitSizeToFile,keepLastFile,keepSize,enableDelete,enableFreqs,showFPS;
            int   clickresult, splitLinePolicy;
            int   vrWidth,vrHeight,upperMargin,lowerMargin;
            int   dayPart;//0 : day, 1 : dusk, 2 : night
            float currentFPS;
            float beginFlash;

};

#endif // LAYOUT_H
