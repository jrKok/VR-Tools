#include "temporarywindow.h"
#include "drawlogic.h"
#include "managemodalwindow.h"
#include "fontman.h"
/* the temporarywindow class makes a floating window which appears only for a few seconds
 * with a simple text alert
 * */

float temporaryWindow::duration(0);
float temporaryWindow::timeStamp(0);
XPLMWindowID temporaryWindow::myXPWindow(nullptr);
int temporaryWindow::myStringNumber(0);
int temporaryWindow::width(0);
int temporaryWindow::height(0);

temporaryWindow::temporaryWindow()
{

}

void temporaryWindow::DrawMyself(XPLMWindowID in_window_id, void *){
    //measures time
    ManageModalWindow::ConstrainGeometry();
    float tm=XPLMGetElapsedTime();
    DrawLogic::RenderContent();
    if ((tm-timeStamp)>duration){
        StopAlert();}
    //else calls StopAlert
}

void temporaryWindow::ShowAlert(string in_String,float time_to_show){
    duration=time_to_show;   
    width=fontMan::MeasureString(in_String)+15;
    height=50;
    myXPWindow = ManageModalWindow::CreateModalWindow(temporaryWindow::DrawMyself,Clr_Gray,width,height);
    point p(5,5);
    myStringNumber=DrawLogic::AddString(in_String,Clr_Amber,Clr_Gray,p);
    DrawLogic::UpdateTexture();
    timeStamp=XPLMGetElapsedTime();
}

void temporaryWindow::StopAlert(){
    ManageModalWindow::DestroyModalWindow();
    myStringNumber=0;
    duration=0;
    timeStamp=0;
}

