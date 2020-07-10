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

void temporaryWindow::DrawMyself(XPLMWindowID, void *){
    ManageModalWindow::MakeTopWindow();
    DrawLogic::RenderContent();

    float tm=XPLMGetElapsedTime();
    if ((tm-timeStamp)>duration){
        StopAlert();}
}

void temporaryWindow::ShowAlert(string in_String,float time_to_show){
    duration=time_to_show;   
    width=fontMan::MeasureString(in_String)+20;
    height=50;
    myXPWindow = ManageModalWindow::CreateModalWindow(temporaryWindow::DrawMyself,Clr_Gray,width,height);
    point p(5,20);
    myStringNumber=DrawLogic::AddString(in_String,Clr_Amber,Clr_Gray,p);
    DrawLogic::UpdateTexture();
    timeStamp=XPLMGetElapsedTime();
    ManageModalWindow::ConstrainGeometry();
}

void temporaryWindow::StopAlert(){
    ManageModalWindow::DestroyModalWindow();
    myStringNumber=0;
    duration=0;
    timeStamp=0;
}

