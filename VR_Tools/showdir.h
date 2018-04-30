#ifndef SHOWDIR_H
#define SHOWDIR_H
#include <string>
#include <vector>
#include <XPLMUtilities.h>
#include <XPLMGraphics.h>
#include <rectangles.h>
#include <textline.h>
#include <point.h>
#include <filesystem>
#include "filenamepicker.h"
#include "dirnamereader.h"
#include "button_vr.h"

class ShowDir
{
public:


    ShowDir();
    rectangles general;

    void WriteDebug(std::string message);
    void SetupDirWindow(int left,int top);
    void RecalculateDirWindow();
    void DrawDirWindow(XPLMWindowID g_FileWindow);
    int  processMouseDn(int,int);
    void processMouseDrag(int,int);
    int  processMouseUp(int,int);
    void SelectDir();
    void SelectFileLine();
    void UpdateDirInfo();
    int  MeasureString(std::string);
    void CloseDirWindow();
    std::string GetSelectedFile();
    std::string GetSelectedPath();
private:

    enum { button_ok=0,button_Cancel=1,button_SelDir=2,button_All=3,button_txt=4};
    enum {windowTitle = 0,dirTitle=1, fileTitle=2,DirSelected=3,FileSelected=4,FilePicker=5,DirReader=6};

    int in_top,in_left,newT,newL,nR,nB;
    int buttonPressed, SectionPressed,charHeight,hghDisp;//section pr
    bool waitForMouseUp;
    float ink[3],background[3],backgroundS[3],bckgW[3];
    dirNameReader dirN;
    FileNamePicker fileN;
    std::string fileSelected, filePathSelected;

    std::vector<TextLine> displayLines;//DirTitle,FileTitle,CurrentDirectory,Directory selected,FileSelected
    std::vector<button_VR> buttons;
};

#endif // SHOWDIR_H
