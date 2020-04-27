#ifndef SHOWDIR_H
#define SHOWDIR_H
#include <string>
#include <vector>
#include <XPLMUtilities.h>
#include <XPLMGraphics.h>
#include <rectangles.h>
#include <textline.h>
#include <point.h>
#include <inisettings.h>
#include "filenamepicker.h"
#include "dirnamereader.h"
#include "button_vr.h"
#include "drawlogic.h"

using std::string;

class ShowDir
{
public:


    ShowDir(DrawLogic *newPad);
    ~ShowDir();

    void MakeButton();
    void MakeLine();
    void SetupDirWindow(int left,int top);
    void ActivateWindow();
    void Update();
    void RecolorFirstLines();
    void DrawDirWindow(XPLMWindowID g_FileWindow);
    int  processMouseDn(int,int);
    void processMouseDrag(int,int);
    int  processMouseUp(int,int);
    void SelectDir();
    void SetDirToSearch(string dir);
    void SelectFileLine();
    void UpdateDirInfo();
    int  MeasureString(string);
    void CloseDirWindow();
    string GetSelectedFile();
    string GetSelectedPath();
    int  GetRight();
    int  GetTop();
    int  GetBottom();
    int  GetLeft();
    int  GetWidth();
    int  GetHeight();
    void SetWindowId(XPLMWindowID inw);


private:
    DrawLogic  *myDrawPad;
    rectangles general,sliderBox;
    enum { button_ok=0,button_Cancel=1,button_SelDir=2,button_All=3,button_txt=4};
    enum {windowTitle = 0,dirTitle=1, fileTitle=2,DirSelected=3,FileSelected=4,FilePicker=5,DirReader=6};

    int in_top,in_left,newT,newL,nR,nB;
    int buttonPressed, SectionPressed,charHeight,hghDisp,wWidth,wHeight;//section pr
    bool waitForMouseUp;
    dirNameReader dirN;
    FileNamePicker fileN;
    string fileSelected, filePathSelected;
    XPLMWindowID myWindow;
    std::vector<TextLine*> displayLines;//DirTitle,FileTitle,CurrentDirectory,Directory selected,FileSelected
    std::vector<button_VR*> buttons;
};

#endif // SHOWDIR_H
