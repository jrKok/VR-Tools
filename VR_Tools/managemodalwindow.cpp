#include "managemodalwindow.h"

 XPLMDataRef  ManageModalWindow::g_vr_dref;
 XPLMWindowID ManageModalWindow::myModalWindow(nullptr);
 int          ManageModalWindow::myWidth(0);
 int          ManageModalWindow::myHeight(0);
 rectangles  *ManageModalWindow::myRect(nullptr);
 DrawLogic   *ManageModalWindow::myDrawPad(nullptr);

ManageModalWindow::ManageModalWindow()

{

}

ManageModalWindow::~ManageModalWindow(){
if (myRect!=nullptr) delete myRect;
myRect= nullptr;
}

XPLMWindowID  ManageModalWindow::CreateModalWindow( void drawCB(XPLMWindowID,void*) ,char myColor,int width, int height){
    if (myDrawPad!=nullptr) return nullptr;
    myDrawPad=new DrawLogic();
    myDrawPad->Initiate();
    myDrawPad->SetNewSize(width,height);
    myDrawPad->SetBackGroundColor(Clr_BckgrdW);
    DrawLogic::EnableModalWindow();
    myWidth=width;
    myHeight=height;
    int offsetX(60),offsetY(70);
    int sLeft(0),sTop(0),sRight(0),sBottom(0);
    g_vr_dref = XPLMFindDataRef("sim/graphics/VR/enabled");
    const int vr_is_enabled = XPLMGetDatai(g_vr_dref);
    XPLMGetScreenBoundsGlobal(&sLeft, &sTop, &sRight, &sBottom);

    XPLMCreateWindow_t params;
     params.structSize = sizeof(params);
     params.visible = 1;
     params.drawWindowFunc       = drawCB;
     params.handleMouseClickFunc = dummy_mouse_handler;
     params.handleRightClickFunc = dummy_mouse_handler;
     params.handleMouseWheelFunc = dummy_wheel_handler;
     params.handleKeyFunc        = dummy_key_handler;
     params.handleCursorFunc     = dummy_cursor_status_handler;
     params.refcon               = nullptr;
     params.layer                    = xplm_WindowLayerFloatingWindows;
     params.decorateAsFloatingWindow = xplm_WindowDecorationNone;
     params.left   = sLeft + offsetX;
     params.bottom = sTop-offsetY-height;
     params.right  = sLeft + width+offsetX;
     params.top    = sTop -offsetY;

    myModalWindow = XPLMCreateWindowEx(&params);
    XPLMSetWindowPositioningMode(myModalWindow, vr_is_enabled>0?xplm_WindowVR:xplm_WindowPositionFree, -1);
    XPLMSetWindowResizingLimits(myModalWindow, myWidth, myHeight, myWidth+70, myHeight+30);

    myRect=new rectangles(true);//modal rectangle
    myRect->SetDimensions(static_cast<int>(width)+10,height);
    myRect->setColor(myColor);
    myRect->setVisibility(true);
    return myModalWindow;
}

XPLMWindowID ManageModalWindow::CreateMousedModalWindow(int mouseH(XPLMWindowID, int, int, int, void*),void drawCB(XPLMWindowID,void*),char myColor, int width, int height){
    if (myDrawPad!=nullptr) return nullptr;
    myDrawPad=new DrawLogic();
    myDrawPad->Initiate();
    myDrawPad->SetNewSize(width,height);
    myDrawPad->EnableModalWindow();
    myWidth=width;
    myHeight=height;
    int offsetX(40),offsetY(80);
    int sLeft(0),sTop(0),sRight(0),sBottom(0);
    g_vr_dref = XPLMFindDataRef("sim/graphics/VR/enabled");
    const int vr_is_enabled = XPLMGetDatai(g_vr_dref);
    XPLMGetScreenBoundsGlobal(&sLeft, &sTop, &sRight, &sBottom);

    XPLMCreateWindow_t params;
     params.structSize = sizeof(params);
     params.visible = 1;
     params.drawWindowFunc       = drawCB;
     params.handleMouseClickFunc = mouseH;
     params.handleRightClickFunc = dummy_mouse_handler;
     params.handleMouseWheelFunc = dummy_wheel_handler;
     params.handleKeyFunc        = dummy_key_handler;
     params.handleCursorFunc     = dummy_cursor_status_handler;
     params.refcon               = nullptr;
     params.layer                    = xplm_WindowLayerFloatingWindows;
     params.decorateAsFloatingWindow = xplm_WindowDecorationNone;
     params.left   = sLeft + offsetX;
     params.bottom = sTop-offsetY-height;
     params.right  = sLeft + width+offsetX;
     params.top    = sTop -offsetY;

    myModalWindow = XPLMCreateWindowEx(&params);
    XPLMSetWindowPositioningMode(myModalWindow, vr_is_enabled>0?xplm_WindowVR:xplm_WindowPositionFree, -1);
    XPLMSetWindowResizingLimits(myModalWindow, 10, 10, 2000, 1850);

    myRect=new rectangles(true);//modal rectangle
    myRect->SetDimensions(static_cast<int>(width)+10,height);
    myRect->setColor(myColor);
    myRect->setVisibility(true);
    return myModalWindow;
}

void ManageModalWindow::MakeTopWindow(){
    myDrawPad->ToUpperLevel();
    XPLMBringWindowToFront(myModalWindow);
}
void ManageModalWindow::DestroyModalWindow(){
    if (myModalWindow!=nullptr) XPLMDestroyWindow(myModalWindow);
    if (myRect!=nullptr) delete myRect;
    myRect=nullptr;
    myModalWindow=nullptr;
    delete myDrawPad;
    myDrawPad=nullptr;
    myWidth=0;
    myHeight=0;

}
void ManageModalWindow::ResizeModalWindow(int width, int height){

    if (XPLMWindowIsInVR(myModalWindow)==1)
        XPLMSetWindowGeometryVR(myModalWindow,width,height);
    else{
        int l(0),t(0),r(0),b(0);
        XPLMGetWindowGeometry(myModalWindow,&l,&t,&r,&b);
        XPLMSetWindowGeometry(myModalWindow,l,t,l+width,t-height);       
    }
    myRect->SetDimensions(width,height);
}

void ManageModalWindow::SetScreenCoords(int left, int bottom, int right, int top){

    DrawLogic::SetScreenOrigin(left,bottom,right, top);
}

int					ManageModalWindow::dummy_mouse_handler(XPLMWindowID , int, int, int, void *) { return 0; }
XPLMCursorStatus	ManageModalWindow::dummy_cursor_status_handler(XPLMWindowID, int, int, void *) { return xplm_CursorDefault; }
int					ManageModalWindow::dummy_wheel_handler(XPLMWindowID, int, int, int, int, void * ) { return 0; }
void                ManageModalWindow::dummy_key_handler(XPLMWindowID,char,XPLMKeyFlags,char,void*,int){}

void ManageModalWindow::PrintmyRectvisstatus(){
   myRect->PrintParameters();
}

XPLMWindowID ManageModalWindow::GetCurrentWindowId(){
    return myModalWindow;
}
