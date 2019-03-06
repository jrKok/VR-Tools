#include "temporarywindow.h"
#include "drawlogic.h"
#include "managemodalwindow.h"

float temporaryWindow::duration(0);
float temporaryWindow::timeStamp(0);
XPLMWindowID temporaryWindow::myXPWindow(nullptr);
int temporaryWindow::myStringNumber(0);
int temporaryWindow::top(0);
int temporaryWindow::right(0);
int temporaryWindow::left(0);
int temporaryWindow::bottom(0);

temporaryWindow::temporaryWindow()
{

}

void temporaryWindow::DrawMyself(XPLMWindowID in_window_id, void * in_refcon){
    //measures time
    int lft(left),tp(top);
    XPLMGetWindowGeometry(myXPWindow, &left, &top, &right, &bottom);
    if (lft!=left||tp!=top) recalculate();
    float tm=XPLMGetElapsedTime();
    if ((tm-timeStamp)<duration){
        DrawLogic::DrawModalElements();
        DrawLogic::DrawModalStrings();
    }
    else StopAlert();
    //else calls StopAlert
}

void temporaryWindow::recalculate(){
    point p;
    p.myX=left+5;
    p.myY=top-20;
    DrawLogic::RelocateModalString(myStringNumber,p);
    ManageModalWindow::Recalculate(left,top);
}

void temporaryWindow::ShowAlert(string in_String,float time_to_show){

    duration=time_to_show;
    point p;
    p.myX=5;
    p.myY=5;
    myStringNumber=DrawLogic::AddModalString(in_String,Clr_Amber,p);
    float width=XPLMMeasureString(xplmFont_Proportional,in_String.c_str(),static_cast<int>(in_String.size()));
    myXPWindow = ManageModalWindow::CreateModalWindow(DrawMyself,Clr_Gray,static_cast<int>(width)+10,50);
    timeStamp=XPLMGetElapsedTime();
}

void temporaryWindow::StopAlert(){
    ManageModalWindow::DestroyModalWindow();
    myStringNumber=0;
    duration=0;
    timeStamp=0;
    left=0;
    right=0;
    top=0;
    bottom=0;


}

