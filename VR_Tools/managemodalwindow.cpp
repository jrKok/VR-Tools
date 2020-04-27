#include "managemodalwindow.h"
#include "opcenter.h"

 XPLMDataRef        ManageModalWindow::g_vr_dref;
 XPLMWindowID       ManageModalWindow::myModalWindow(nullptr);
 int                ManageModalWindow::myWidth(0);
 int                ManageModalWindow::myHeight(0);
 int                ManageModalWindow::myLeft(0);
 int                ManageModalWindow::myBottom(0);
 rectangles        *ManageModalWindow::myRect(nullptr);
 ManageModalWindow *ManageModalWindow::myself(nullptr);


ManageModalWindow::ManageModalWindow(DrawLogic *dp)

{
myself=this;
myDrawPad=dp;
}

ManageModalWindow::~ManageModalWindow(){
if (myRect!=nullptr) delete myRect;
myRect= nullptr;
if (myDrawPad!=nullptr) delete myDrawPad;
myDrawPad= nullptr;
}


XPLMWindowID  ManageModalWindow::CreateModalWindow( void drawCB(XPLMWindowID,void*) ,char myColor,int width, int height){
  return  CreateMousedModalWindow(dummy_mouse_handler,drawCB,myColor,width,height);
}

XPLMWindowID ManageModalWindow::CreateMousedModalWindow(int mouseH(XPLMWindowID, int, int, int, void*),void drawCB(XPLMWindowID,void*),char myColor, int width, int height){
    myself->myDrawPad->Initiate();
    myself->myDrawPad->SetNewSize(width,height);
    myself->myDrawPad->SetId("Modal Window");
    myself->myDrawPad->SetBackGroundColor(myColor);
    OpCenter::SetModalWindow(true);
    myWidth=width;
    myHeight=height;
    int offsetX(40),offsetY(100);
    int sLeft(0),sTop(height),sRight(width),sBottom(0);
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
     params.bottom = sBottom+offsetY;
     params.right  = sLeft+myWidth+offsetX;
     params.top    = sBottom+myHeight+offsetY;

    myModalWindow = XPLMCreateWindowEx(&params);
    myself->myDrawPad->SetWindowHandle(myModalWindow);
    XPLMSetWindowPositioningMode(myModalWindow, vr_is_enabled>0?xplm_WindowVR:xplm_WindowPositionFree, -1);
    XPLMSetWindowResizingLimits(myModalWindow, myWidth, myHeight, myWidth+70, myHeight+30);

    myRect=new rectangles("modalW generalR",false);
    myself->myDrawPad->AddAsFirstRectangle(myRect);
    myRect->SetDimensions(width,height);
    myRect->setColor(myColor);
    myRect->setVisibility(true);
    return myModalWindow;
}

void ManageModalWindow::MakeTopWindow(){
    myself->myDrawPad->ToUpperLevel();
    //XPLMBringWindowToFront(myModalWindow);
}
void ManageModalWindow::DestroyModalWindow(){
    OpCenter::SetModalWindow(false);
    if (myModalWindow!=nullptr) XPLMDestroyWindow(myModalWindow);
    if (myRect!=nullptr) delete myRect;
    myRect=nullptr;
    myModalWindow=nullptr;
    myself->myDrawPad->FlushContent();
    myself->myDrawPad->CloseWindow();
    myWidth=0;
    myHeight=0;

}
void ManageModalWindow::ResizeModalWindow(int width, int height){
    myWidth=width;myHeight=height;
    XPLMSetWindowResizingLimits(myModalWindow, myWidth, myHeight, myWidth+70, myHeight+30);
    if (XPLMWindowIsInVR(myModalWindow)==1)
        XPLMSetWindowGeometryVR(myModalWindow,width,height);
    else{
        int l(0),t(0),r(0),b(0);
        XPLMGetWindowGeometry(myModalWindow,&l,&t,&r,&b);
        XPLMSetWindowGeometry(myModalWindow,l,t,l+width,t-height);       
    }

    myRect->SetDimensions(width,height);
    myself->myDrawPad->SetNewSize(width,height);
}

void ManageModalWindow::ConstrainGeometry(){
    if (myModalWindow){
        int top(0),right(0);
        myself->myDrawPad->ToUpperLevel();
        int mw(0),mh(0);
        XPLMGetWindowGeometry(myModalWindow, &myLeft, &top, &right, &myBottom);
        {
            if (XPLMWindowIsInVR(myModalWindow)){
                XPLMGetWindowGeometryVR(myModalWindow,&mw,&mh);
            }
            else {
                if ((right-myLeft)>myWidth||(top-myBottom)>myHeight){
                   XPLMSetWindowGeometry(myModalWindow,myLeft,myBottom+myHeight,myLeft+myWidth,myBottom);
                   XPLMGetWindowGeometry(myModalWindow, &myLeft, &top, &right, &myBottom);
                   mw=right-myLeft;
                   mh=top-myBottom;
                }
            }
        }
        myself->myDrawPad->UpdateDrawPad(mw,mh,myLeft,myBottom,right,top);
    }
}

int ManageModalWindow::GetLeft(){
    return myLeft;
}

int ManageModalWindow::GetBottom(){
    return myBottom;
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

void ManageModalWindow::ReactivateDrawPad(){
    myself->myDrawPad->Initiate();
}

void ManageModalWindow::HideMyRect(){
    myRect->setVisibility(false);
}

void ManageModalWindow::DebugRects(){
    myself->myDrawPad->PrintRectanglesAssessment();
}
