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
#include "boxed_button.h"
#include "textline.h"
#include "VR_Tools_global.h"
#include "linedialog.h"
#include "vrcreader.h"
#include "drawlogic.h"
#include "background.h"

using std::vector;
using std::string;

enum{B_Open=0,
     B_Refresh=1,
     B_Stream=2,
     B_Toggle=3,
     B_UTF8=4,
     B_Undo=5,
     B_Edit=6,
     B_Save=7,
     B_Next_File=8,
     B_Prev_File=9,
     B_NAV1 = 10,
     B_NAV2 = 11,
     B_COM1 = 12,
     B_COM2 = 13,
     B_ADF1 = 14,
     B_ADF2 = 15,
     B_Show_All=16,
     B_Hide=17,
     B_NextLine=18,
     B_PrevLine=19,
     B_FirstLine=20,
     B_Close=21
     };

class TextReader;

class Layout
{
public:
    Layout(DrawLogic *newPad);
    virtual ~Layout();

            void Begin();
    virtual bool initiate();
    virtual bool resize();
            void resizeVRWindow();
            void DrawBackground();
    virtual bool newSize(int wth,int hgt);
    virtual void Update();
            void RelocateButtons(int middle);
            void recalculate(float cT);
    virtual void FitToFile();
            void defineButtons();
            void MakeButton(int btnCmd,bool visible,string in_Label,int width,int height,int oX, int oY);
            void MakeBoxedButton(int number, bool visible,string in_Label,int width,int height,int oX, int oY,int fontSize);
            void MakeHeader(std::string in_String);
    virtual void findClick(int mX, int mY);
    virtual void HandleMouseKeepDown(int mX,int mY);
    virtual int  HandleMouseUp(int mX,int mY);
    virtual void CheckButtonsVisibility();
    virtual void ClosegWindow();
    virtual void LaunchCommand(int refCommand);
    virtual string GetFileName();
            void StopStreaming();
            void UpdateFrequencies();
            string GetDirName();
            void SetWindowHandle(XPLMWindowID thisW);
            bool isWindowInVR();
    virtual void DrawTextW(XPLMWindowID);
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
        rectangles generalR,titleR,decoR;
        Background * bckg;
       TextReader *tFileReader;

std::map<int,button_VR*>    tButtons;
std::map<int,Boxed_Button*> tBButtons;

      XPLMDataRef nav1on,nav2on,com1on,com2on,adf1on,adf2on;
      XPLMDataRef nav1freq,nav2freq,com1freq,com2freq,adf1freq,adf2freq,com1freqk,com2freqk;
      XPLMDataRef dref_SunPitch,g_FPS;
   XPLMCommandRef screenShot;
     XPLMWindowID myWindow;

         TextLine fNav,fCom,fAdf,lFPS,lTitle;
           string charSep;
            float epoch,fpsTag;
            bool  editMode,openAtStart,goToLastPage;
            bool  continueClick,buttonClick,autoReload,saveAuto,canUTF,autoReloadOnSize,useBackGround;
            bool  flash,flashWhenChange,noResize,fitSizeToFile,keepLastFile,keepSize,enableDelete,enableFreqs,showFPS;
            bool  must_print,text_visible;
            int   clickresult, splitLinePolicy;
            int   bottom,upperMargin,lowerMargin;
            int   dayPart;//0 : day, 1 : dusk, 2 : night
            float currentFPS;
            float beginFlash;

};

#endif // LAYOUT_H
