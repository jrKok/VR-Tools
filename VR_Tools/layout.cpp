#include <layout.h>


Layout::Layout() :
  gTop(0),gBottom(0),gLeft(0),gRight(0),//Used for initial calculations and coordinates,global coordinates
  wTop(0),wBottom(0),wLeft(0),wRight(0),wWidth(0),wHeight(0),minWidth(0),maxWidth(0),minHeight(0),maxHeight(0),
  charHeight(0),charWidth(0),textPointX(0),textPointY(0),//Used for initial setup and resize instructions
  t(0),b(0),l(0),r(0),//input from draw instruction mostly
  textHeight(470),textWidth(355),colWidth(30),idxSelected(-1),nButtons(0),
  tFileReader(new TextReader("log.txt")), //tFileReader(new TextReader("Output\\textfiles\\flightnotes.txt")),
  generalR(),tButtons(),
  myWindow(NULL),
  epoch(0),
  continueClick(false),buttonClick(false),autoReload(false),saveAuto(false),
  clickresult(-1),vrWidth(0),vrHeight(0)




//Constructor

{
    nav1on       = XPLMFindDataRef("sim/cockpit2/radios/actuators/nav1_power");
    nav2on       = XPLMFindDataRef("sim/cockpit2/radios/actuators/nav2_power");
    com1on       = XPLMFindDataRef("sim/cockpit2/radios/actuators/com1_power");
    com2on       = XPLMFindDataRef("sim/cockpit2/radios/actuators/com2_power");
    adf1on       = XPLMFindDataRef("sim/cockpit2/radios/actuators/adf1_power"); //returns an int 0 for Off 2 for ON
    adf2on       = XPLMFindDataRef("sim/cockpit2/radios/actuators/adf2_power");
    nav1freq     = XPLMFindDataRef("sim/cockpit2/radios/actuators/nav1_standby_frequency_hz");
    nav2freq     = XPLMFindDataRef("sim/cockpit2/radios/actuators/nav2_standby_frequency_hz");
    com1freq     = XPLMFindDataRef("sim/cockpit2/radios/actuators/com1_standby_frequency_hz");
    com2freq     = XPLMFindDataRef("sim/cockpit2/radios/actuators/com2_standby_frequency_hz");
    com1freqk    = XPLMFindDataRef("sim/cockpit2/radios/actuators/com1_standby_frequency_khz");
    com2freqk    = XPLMFindDataRef("sim/cockpit2/radios/actuators/com2_standby_frequency_khz");
    adf1freq     = XPLMFindDataRef("sim/cockpit2/radios/actuators/adf1_standby_frequency_hz");
    adf2freq     = XPLMFindDataRef("sim/cockpit2/radios/actuators/adf2_standby_frequency_hz");
    XPLMGetFontDimensions(xplmFont_Proportional,NULL,&charHeight,NULL);
    minWidth=260+colWidth;//can vary if there are no buttons
    maxWidth=1510+colWidth;

    }

//Destructor

Layout::~Layout(){
    delete tFileReader;
}

//Helpers

void Layout::WriteDebug(std::string message){
    std::string in_String="VR Tools : " +message+"\n";
    XPLMDebugString((char*)in_String.c_str());
}

//Initialisers

bool Layout::initiate(){
    resize();
    wLeft=gLeft+150;
    wRight=wLeft+wWidth;
    wTop=gTop-100;//moreover this will force a new recalculation at next draw call
    wBottom=wTop-wHeight;
    defineButtons();
    nButtons=tButtons.size();
    tFileReader->GoToLastPage();
    LaunchCommand(B_Auto);
    return true;
}

void Layout::resize(){//calculate offsets; areas of rectangles}

    tFileReader->Setup(textHeight,textWidth,colWidth,charHeight+15);
    tFileReader->ReadFileToBuff();
    generalR.width=textWidth+colWidth+10;//10 bx right margin
    generalR.height=textHeight+tFileReader->GetOffSetY()+35;//offsetY upper margin and 35lower margin
    wWidth=generalR.width;
    wHeight=generalR.height;
    generalR.offsetX=0;
    generalR.offsetY=0;
    minHeight=tFileReader->GetOffSetY()+155;
    maxHeight=tFileReader->GetOffSetY()+935;
}

void Layout::newSize(){
 int oldW=vrWidth,oldH=vrHeight;
 XPLMGetWindowGeometryVR(myWindow,&vrWidth,&vrHeight);
 if ((oldW!=vrWidth)|(oldH!=vrHeight)){
     int deltaW(0),deltaH(0);
     deltaW=vrWidth-oldW;
     deltaH=vrHeight-oldH;
     textWidth=textWidth+deltaW;
     textHeight=textHeight+deltaH;
     if (textWidth<230) textWidth=230;
     if (textWidth>1500) textWidth=1500;
     if (textHeight<120) textHeight=120;
     if (textHeight>900) textHeight=900;
     resize();
 }
}

void Layout::defineButtons(){
    int middle=tFileReader->GetOffSetY()+(int)(tFileReader->GetHeight()/2);
    button_VR emptyBn;
    for (int box(0);box <17;box++){tButtons.push_back(emptyBn);}//create 16 buttons
  /*  B_Load_File=0*/
    tButtons[B_Load_File].isVisible=true;
    tButtons[B_Load_File].height=16;
    tButtons[B_Load_File].width=90;
    tButtons[B_Load_File].offsetX=5;
    tButtons[B_Load_File].offsetY=4;
    tButtons[B_Load_File].setText("Load");

         //B_Page_Up=1;
    tButtons[B_Reload].isVisible=true;
    tButtons[B_Reload].height=16;
    tButtons[B_Reload].width=90;
    tButtons[B_Reload].offsetX=100;
    tButtons[B_Reload].offsetY=4;
    tButtons[B_Reload].setText("Reload");

         //B_Line_Up=2;
    tButtons[B_Auto].isVisible=true;
    tButtons[B_Auto].height=16;
    tButtons[B_Auto].width=90;
    tButtons[B_Auto].offsetX=195;
    tButtons[B_Auto].offsetY=4;
    tButtons[B_Auto].setText("Auto Reload");

         //B_Toggle_Line=3;
    tButtons[B_Toggle_Line].isVisible=true;
    tButtons[B_Toggle_Line].height=15;
    tButtons[B_Toggle_Line].width=20;
    tButtons[B_Toggle_Line].offsetX=5;
    tButtons[B_Toggle_Line].offsetY=middle-13;
    tButtons[B_Toggle_Line].setText("Del");

         //B_Line_Down=4;
    tButtons[B_Line_Down].isVisible=true;
    tButtons[B_Line_Down].height=16;
    tButtons[B_Line_Down].width=60;
    tButtons[B_Line_Down].offsetX=290;
    tButtons[B_Line_Down].offsetY=4;
    tButtons[B_Line_Down].setText("To UTF8");

        // B_Page_Down=5;
    tButtons[B_Page_Down].isVisible=true;
    tButtons[B_Page_Down].height=20;
    tButtons[B_Page_Down].width=20;
    tButtons[B_Page_Down].offsetX=27;
    tButtons[B_Page_Down].offsetY=middle+7+20+3;
    tButtons[B_Page_Down].setText("PgDn");//\u2C4 bas\CB85

         //B_Longer_Lines=6;
    tButtons[B_Longer_Lines].isVisible=true;
    tButtons[B_Longer_Lines].height=15;
    tButtons[B_Longer_Lines].width=20;
    tButtons[B_Longer_Lines].offsetX=colWidth+5;
    tButtons[B_Longer_Lines].offsetY=generalR.height-31;
    tButtons[B_Longer_Lines].setText("wider");//\u2C4 bas\CB85

        // B_Smaller_Lines=7;
    tButtons[B_Smaller_Lines].isVisible=true;
    tButtons[B_Smaller_Lines].height=15;
    tButtons[B_Smaller_Lines].width=20;
    tButtons[B_Smaller_Lines].offsetX=colWidth+47;
    tButtons[B_Smaller_Lines].offsetY=generalR.height-31;
    tButtons[B_Smaller_Lines].setText("narrower");//\u2C4 bas\CB85

         //B_More_Lines=8;
    tButtons[B_More_Lines].isVisible=true;
    tButtons[B_More_Lines].height=15;
    tButtons[B_More_Lines].width=20;
    tButtons[B_More_Lines].offsetX=colWidth+142;
    tButtons[B_More_Lines].offsetY=generalR.height-31;
    tButtons[B_More_Lines].setText("+Lines");//\u2C4 bas\CB85

        // B_Less_Lines=9;
    tButtons[B_Less_Lines].isVisible=true;
    tButtons[B_Less_Lines].height=15;
    tButtons[B_Less_Lines].width=20;
    tButtons[B_Less_Lines].offsetX=colWidth+190;
    tButtons[B_Less_Lines].offsetY=generalR.height-31;
    tButtons[B_Less_Lines].setText("-Lines");//\u2C4 bas\CB85

         //B_NAV1 = 10;
    tButtons[B_NAV1].isVisible=true;
    tButtons[B_NAV1].height=20;
    tButtons[B_NAV1].width=35;
    tButtons[B_NAV1].offsetX=colWidth;
    tButtons[B_NAV1].offsetY=generalR.height-26;
    tButtons[B_NAV1].setText("Nav1");//\u2C4 bas\CB85

        // B_NAV2 = 11;
    tButtons[B_NAV2].isVisible=true;
    tButtons[B_NAV2].height=20;
    tButtons[B_NAV2].width=35;
    tButtons[B_NAV2].offsetX=tButtons[B_NAV1].offsetX+38;
    tButtons[B_NAV2].offsetY=tButtons[B_NAV1].offsetY;
    tButtons[B_NAV2].setText("Nav2");//\u2C4 bas\CB85

         //B_COM1 = 12;
    tButtons[B_COM1].isVisible=true;
    tButtons[B_COM1].height=20;
    tButtons[B_COM1].width=35;
    tButtons[B_COM1].offsetX=tButtons[B_NAV2].offsetX+38;
    tButtons[B_COM1].offsetY=tButtons[B_NAV1].offsetY;
    tButtons[B_COM1].setText("com1");

        // B_COM2 = 13;
    tButtons[B_COM2].isVisible=true;
    tButtons[B_COM2].height=20;
    tButtons[B_COM2].width=35;
    tButtons[B_COM2].offsetX=tButtons[B_COM1].offsetX+38;
    tButtons[B_COM2].offsetY=tButtons[B_NAV1].offsetY;
    tButtons[B_COM2].setText("com2");

        // B_ADF1 = 14;
    tButtons[B_ADF1].isVisible=true;
    tButtons[B_ADF1].height=20;
    tButtons[B_ADF1].width=35;
    tButtons[B_ADF1].offsetX=tButtons[B_COM2].offsetX+38;
    tButtons[B_ADF1].offsetY=tButtons[B_NAV1].offsetY;
    tButtons[B_ADF1].setText("Adf1");

        // B_ADF2 = 15*/
    tButtons[B_ADF2].isVisible=true;
    tButtons[B_ADF2].height=20;
    tButtons[B_ADF2].width=35;
    tButtons[B_ADF2].offsetX=tButtons[B_ADF1].offsetX+38;
    tButtons[B_ADF2].offsetY=tButtons[B_NAV1].offsetY;
    tButtons[B_ADF2].setText("Adf2");

    //B_Show_All=16
    tButtons[B_Show_All].isVisible=true;
    tButtons[B_Show_All].height=17;
    tButtons[B_Show_All].width=20;
    tButtons[B_Show_All].offsetX=5;
    tButtons[B_Show_All].offsetY=middle+5;
    tButtons[B_Show_All].setText("All");

}

//members, functions for running the class

void Layout::recalculate(){ //gets called at every draw callback so used to process other frequent checks
    newSize();

    if (autoReload){
        float timePt=XPLMGetElapsedTime();
        if ((timePt-epoch)>=1.0f) {
            epoch=timePt;
            tFileReader->Reload();
        }
    }

    if ((t!=wTop)|(l!=wLeft)){
        wTop=t;
        wLeft=l;
        generalR.in_left=l;
        generalR.in_top=t;
        generalR.recalculate();
        tFileReader->Recalculate(l,t);
        for (int I(0);I<nButtons;I++){
            tButtons[I].in_top=t;
            tButtons[I].in_left=l;
            tButtons[I].recalculate();
        }
}
}

void Layout::CheckButtonsVisibility(){
    tButtons[B_Smaller_Lines].isVisible=false;
    tButtons[B_Longer_Lines].isVisible=false;
    tButtons[B_More_Lines].isVisible=false;
    tButtons[B_Less_Lines].isVisible=false;
    tButtons[B_Page_Down].isVisible=false;

    tButtons[B_Line_Down].isVisible=true;//button UTF8
    tButtons[B_Load_File].isVisible=true;
    tButtons[B_Reload].isVisible=!autoReload;
    tButtons[B_Auto].isVisible=true;
    if (tFileReader->HasSelection()) tButtons[B_Toggle_Line].isVisible=true;
                                   else tButtons[B_Toggle_Line].isVisible=false;
    if (tFileReader->HasHiddenLine())   tButtons[B_Show_All].isVisible=true;
                                   else tButtons[B_Show_All].isVisible=false;
    if (tFileReader->HasNav()) { tButtons[B_NAV1].isVisible=true;
                                 tButtons[B_NAV2].isVisible=true;}
                           else {tButtons[B_NAV1].isVisible=false;
                                 tButtons[B_NAV2].isVisible=false;}
    if (tFileReader->HasCom()) { tButtons[B_COM1].isVisible=true;
                                 tButtons[B_COM2].isVisible=true;}
                           else {tButtons[B_COM1].isVisible=false;
                                 tButtons[B_COM2].isVisible=false;}
    if (tFileReader->HasADF()) { tButtons[B_ADF1].isVisible=true;
                                 tButtons[B_ADF2].isVisible=true;}
                           else {tButtons[B_ADF1].isVisible=false;
                                 tButtons[B_ADF2].isVisible=false;}
    int on(0);
    on=XPLMGetDatai(nav1on); if (on==0) tButtons[B_NAV1].isVisible=false;
    on=XPLMGetDatai(nav2on); if (on==0) tButtons[B_NAV2].isVisible=false;
    on=XPLMGetDatai(com1on); if (on==0) tButtons[B_COM1].isVisible=false;
    on=XPLMGetDatai(com2on); if (on==0) tButtons[B_COM2].isVisible=false;
    on=XPLMGetDatai(adf1on); if (on==0) tButtons[B_ADF1].isVisible=false;
    on=XPLMGetDatai(adf2on); if (on==0) tButtons[B_ADF2].isVisible=false;
}
int Layout::rightRectangle(int idx){
    return (tButtons[idx].right);
}
int Layout::leftRectangle(int idx){
    return (tButtons[idx].left);
}
int Layout::topRectangle(int idx){
    return (tButtons[idx].top);
}
int Layout::bottomRectangle(int idx){
    return (tButtons[idx].bottom);
}


//process a mouse or controller click

void Layout::findClick(int mX, int mY){
    saveAuto=autoReload;
    autoReload=false;
    if (tFileReader->ProceedClick(mX,mY)){//Click in the text or scrB ?
        CheckButtonsVisibility();
        continueClick=true;
        return;
      }

    for (int I(0);I<nButtons;I++){       //Click on a button ?
      if (tButtons[I].isHere(mX,mY)){
           buttonClick=true;
           clickresult=I;
           tButtons[I].Press();
           return;
           }
    }

}

void Layout::HandleMouseKeepDown(int mX,int mY){
    if (continueClick) tFileReader->ProceedClickCont(mX,mY);
}

int Layout::HandleMouseUp(int mX,int mY){
    autoReload=saveAuto;
    epoch=XPLMGetElapsedTime();
    int retVal=-1;
    if (continueClick){
        continueClick=false;
        buttonClick=false;
        clickresult=-1;
        tFileReader->ProceedEndClick();
        tFileReader->CheckForFrequencies();
        CheckButtonsVisibility();}

    if (buttonClick){
        tButtons[clickresult].Release();
        if (clickresult==B_Load_File) retVal=2;
        else LaunchCommand(clickresult);
        continueClick=false;
        buttonClick=false;
        clickresult=-1;
    }
    return retVal;
}
//The following can also be used by the plugin handlers to launch a custom command
//to implement functionnality by pressing physical commands, keys, menus ...
//enumeration to be found in show_fps_test_global.h

void Layout::LaunchCommand(int refCommand){
    switch(refCommand){
         case B_Reload:{
             tFileReader->Reload();
         break;}

         case B_Page_Down:{
             tFileReader->MoveDnPage();
         break;}

         case B_Auto:{
             autoReload=!autoReload;
             if (autoReload) {
                 //tButtons[B_Auto].setText("manual Reload");
                 tButtons[B_Auto].setSelect(true);
                 epoch=XPLMGetElapsedTime();
             }else{
                 //tButtons[B_Auto].setText("auto Reload");
                 tButtons[B_Auto].setSelect(false);
             }
         break;}

         case B_Line_Down:{
             tFileReader->SetNeedsUTF(true);
         break;}

         case B_Toggle_Line:{
             tFileReader->DeleteSelectedLine();
         break;}

         case B_Show_All:{
             tFileReader->ShowAll();
             tFileReader->SetupforText();
         break;}

         case B_NAV1:{
             XPLMSetDatai(nav1freq,tFileReader->freqNAV);
         break;}

         case B_NAV2:{
             XPLMSetDatai(nav2freq,tFileReader->freqNAV);
         break;}

         case B_COM1:{
             int frac(0);
             float val=tFileReader->freqCOM;
             XPLMSetDatai(com1freq,(int)val);
             val-=(float)XPLMGetDatai(com1freq);
             if (val!=0){
                 frac=XPLMGetDatai(com1freqk);
                 frac+=(int)(val*10.0);
                 XPLMSetDatai(com1freqk,frac);
             }
         break;}
         case B_COM2:{
             int frac(0);
             float val=tFileReader->freqCOM;
             XPLMSetDatai(com2freq,(int)val);
             val-=(float)XPLMGetDatai(com2freq);
             if (val!=0){
                 frac=XPLMGetDatai(com2freqk);
                 frac+=(int)(val*10.0);
                 XPLMSetDatai(com2freqk,frac);
             }
         break;}
         case B_ADF1:{
             XPLMSetDatai(adf1freq,tFileReader->freqADF);
         break;}
         case B_ADF2:{
             XPLMSetDatai(adf2freq,tFileReader->freqADF);
         break;}

         }
    CheckButtonsVisibility();
}

void Layout::ClosegWindow(){
    tFileReader->closeReader();
    tButtons.clear();
    gTop=0;gBottom=0;gLeft=0;gRight=0;
    wTop=0;wBottom=0;wLeft=0;wRight=0;
    wWidth=0;wHeight=0;
    charWidth=0;
    textPointX=0;textPointY=0;
    t=0;b=0;l=0;r=0;
    colWidth=30;idxSelected=-1;nButtons=0;
    //I keep charHeight,textHeight and textWidth,
}

void Layout::SetNewFile(std::string newFilePath){
    tFileReader->OpenFile(newFilePath);
    tFileReader->ShowAll();
}

std::string Layout::GetFileName(){
    return tFileReader->GetFileName();
}

void Layout::SetWindowHandler(XPLMWindowID thisW){
    myWindow=thisW;
    XPLMGetWindowGeometryVR(myWindow,&vrWidth,&vrHeight);
}

bool Layout::isWindowInVR(){
    return (XPLMWindowIsInVR(myWindow));
}
