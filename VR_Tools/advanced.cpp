#include "advanced.h"
#include "managemodalwindow.h"
#include "drawlogic.h"
ulong advanced::selectedB(0);
int advanced::bottom(0);
int advanced::left(0);
advanced *advanced::myself(nullptr);

advanced::advanced():
    endAlert(false),
    callBack(),
    currentHsp(),
    actionButtons(),
    activeParameter(nullptr),
    bBoxOffset(0),bBoxLatShift(0),bBoxAxShift(0),bBoxheight(0),bBoxWidth(0),
    rot(0),tilt(0),pitch(0),
    sittingL(true),offsetL(true),rightShiftL(true),aftShiftL(true),heightL(true),widthL(true),rotL(true),tiltL(true),pitchL(true),
    activeLine(nullptr),
    sittingT(""),offsetT(""),rightShiftT(""),aftShiftT(""),heightT(""),widthT(""),rotT(""),tiltT(""),pitchT(""),
    sittingN(0),offsetN(0),rightShiftN(0),aftShiftN(0),heightN(0),widthN(0),rotN(0),tiltN(0),pitchN(0),
    hsNameN(0),hsBoxN(0),hsAdditionalN(0),
    rectSit(true),rectBox(true),rectParam(true),rectButtons(true),
    sittingP(),offsetP(),rightShiftP(),aftShiftP(),heightP(),widthP(),rotP(),tiltP(),pitchP(),
    hsNameP(),hsBoxP(),hsAdditionalP(),
    actionSelected(""),
    cursor(),
    forcursor(),
    numpad(nullptr),
    mouseDrag(false),
    epochClick(0),
    notallowed(""),
    maxlength(0),
    hasInterval(false),
    minval(0),
    maxval(0),
    currentUnit(0), //0 : m, 1: cm, 2 : mm, 3 : ft, 4 : inch
    numberOfDigits(0),
    gapincr(0.05f),
    widthincr(0.05f),
    heightincr(0.1f),
    latshiftincr(0.02f),
    axshiftincr(0.02f),
    rotincr(90.0f),
    tiltincr(5.0f),
    pitchincr(5.0f)

{
myself=this;
}
advanced::~advanced(){
    if (numpad != nullptr) delete numpad;
    for (auto  ab: actionButtons){
        delete (*ab).button;
        delete ab;
    }
}

void advanced::MakeAdvancedDialog(Hotspot htsp, std::function<void()> cBck){
    left=0;
    bottom=0;
    endAlert=false;
    callBack=cBck;
    int wWidth=380,col1=140,lH=22,textBoxWidth=60,textBoxHeight=15,bH=15;
    currentHsp=htsp;
    ManageModalWindow::CreateMousedModalWindow(MouseHandler,DrawMyself,Clr_LightGray,wWidth,450);

    rectButtons.SetDimensions(wWidth,163);
    rectButtons.SetOrigin(0,0);
    rectButtons.SetColor(Clr_LighterGray);

    rectParam.SetDimensions(wWidth,95);
    rectParam.SetOrigin(0,rectButtons.GetTop());
    rectParam.SetColor(Clr_LightGray);

    rectBox.SetDimensions(wWidth,140);
    rectBox.SetOrigin(0,rectParam.GetTop());
    rectBox.SetColor(Clr_LighterGray);

    rectSit.SetDimensions(wWidth,52);
    rectSit.SetOrigin(0,rectBox.GetTop());
    rectSit.SetColor(Clr_LightGray);


//T for text
       sittingT="sitting/Standing :";
        offsetT="gap between box-spot :";
    rightShiftT="lateral shift :";
      aftShiftT="axial shift :";
         widthT="diameter :";
        heightT="height :";
           rotT="rotation (PSI) :";
         pitchT="Pitch (THE) :";
          tiltT="Tilt (PHI) : ";
      string nm="Advanced Parameters for Hotspot : "+currentHsp.name;

      //P for points of origin

       sittingP.SetCoords(col1-8-stringOps::StringSize(sittingT),rectSit.GetBottom()+2);
        heightP.SetCoords(col1-3-stringOps::StringSize(heightT),rectBox.GetBottom()+2);
         widthP.SetCoords(col1-3-stringOps::StringSize(widthT),heightP.GetY()+lH);
      aftShiftP.SetCoords(col1-3-stringOps::StringSize(aftShiftT),widthP.GetY()+lH);
    rightShiftP.SetCoords(col1-3-stringOps::StringSize(rightShiftT),aftShiftP.GetY()+lH);
        offsetP.SetCoords(col1-3-stringOps::StringSize(offsetT),rightShiftP.GetY()+lH);

          tiltP.SetCoords(col1-3-stringOps::StringSize(tiltT),rectParam.GetBottom()+2);
          pitchP.SetCoords(col1-3-stringOps::StringSize(pitchT),tiltP.GetY()+lH);
           rotP.SetCoords(col1-3-stringOps::StringSize(rotT),pitchP.GetY()+lH);

        hsNameP.SetCoords(col1-100,rectSit.GetBottom()+lH+4);
         hsBoxP.SetCoords(col1-100,offsetP.GetY()+lH);
  hsAdditionalP.SetCoords(col1-100,rotP.GetY()+lH);

//N for Names, in fact Labels for the textlines, getting their drawing numbers
     sittingN=DrawLogic::AddString(sittingT,Clr_BlackInk,Clr_LightGray,sittingP);
      offsetN=DrawLogic::AddString(offsetT,Clr_BlackInk,Clr_LightGray,offsetP);
  rightShiftN=DrawLogic::AddString(rightShiftT,Clr_BlackInk,Clr_LightGray,rightShiftP);
    aftShiftN=DrawLogic::AddString(aftShiftT,Clr_BlackInk,Clr_LightGray,aftShiftP);
       widthN=DrawLogic::AddString(widthT,Clr_BlackInk,Clr_LightGray,widthP);
      heightN=DrawLogic::AddString(heightT,Clr_BlackInk,Clr_LightGray,heightP);
         rotN=DrawLogic::AddString(rotT,Clr_BlackInk,Clr_LightGray,rotP);
       pitchN=DrawLogic::AddString(pitchT,Clr_BlackInk,Clr_LightGray,pitchP);
        tiltN=DrawLogic::AddString(tiltT,Clr_BlackInk,Clr_LightGray,tiltP);

      hsNameN=DrawLogic::AddString(nm,Clr_PushedBlue,Clr_LightGray,hsNameP);
       hsBoxN=DrawLogic::AddString("Position of Binding Box relative to Hotspot :",Clr_PushedBlue,Clr_LightGray,hsBoxP);
hsAdditionalN=DrawLogic::AddString("hotspot orientation : ",Clr_PushedBlue,Clr_LightGray,hsAdditionalP);

 // L for Text Lines, define also their boxes.

           GetBBoxDescription();

           MakeLine(sittingL,col1-10,sittingP.GetY(),textBoxWidth+7,textBoxHeight,currentHsp.type);
           MakeLine(offsetL,col1-3,offsetP.GetY(),textBoxWidth,textBoxHeight,stringOps::ConvertFloatToString(bBoxOffset));
           MakeLine(rightShiftL,col1-3,rightShiftP.GetY(),textBoxWidth,textBoxHeight,stringOps::ConvertFloatToString(bBoxLatShift));
           MakeLine(aftShiftL,col1-3,aftShiftP.GetY(),textBoxWidth,textBoxHeight,stringOps::ConvertFloatToString(bBoxAxShift));
           MakeLine(widthL,col1-3,widthP.GetY(),textBoxWidth,textBoxHeight,stringOps::ConvertFloatToString(bBoxWidth));
           MakeLine(heightL,col1-3,heightP.GetY(),textBoxWidth,textBoxHeight,stringOps::ConvertFloatToString(bBoxheight));
           MakeLine(rotL,col1-3,rotP.GetY(),textBoxWidth-20,textBoxHeight,stringOps::ConvertFloatToString(currentHsp.psi,0));
           MakeLine(pitchL,col1-3,pitchP.GetY(),textBoxWidth-20,textBoxHeight,stringOps::ConvertFloatToString(currentHsp.the,0));
           MakeLine(tiltL,col1-3,tiltP.GetY(),textBoxWidth-20,textBoxHeight,stringOps::ConvertFloatToString(currentHsp.phi,0));

        int oX=col1+6+textBoxWidth,bW=64;

        MakeModalButton("Cancel","Cancel",0,100,20,170,rectButtons.GetTop()-25);
        MakeModalButton("SITTING","Sitting",1,bW,bH,oX,sittingL.GetBottom());
        MakeModalButton("STANDING","Standing",2,bW,bH,oX+bW+2,sittingL.GetBottom());
        MakeModalButton("+5cm","Raise50",3,bW,bH,oX,offsetL.GetBottom());
        MakeModalButton("-5cm","Lower50",4,bW,bH,oX+bW+2,offsetL.GetBottom());
        MakeModalButton("+2cm(R)","Right20",5,bW,bH,oX,rightShiftL.GetBottom());
        MakeModalButton("-2cm(L)","Left20",6,bW,bH,oX+bW+2,rightShiftL.GetBottom());
        MakeModalButton("-2cm(F)","For20",7,bW,bH,oX,aftShiftL.GetBottom());
        MakeModalButton("+2cm(B)","Aft20",8,bW,bH,oX+bW+2,aftShiftL.GetBottom());
        MakeModalButton("+5cm","Width20",9,bW,bH,oX,widthL.GetBottom());
        MakeModalButton("-5cm","Width-20",10,bW,bH,oX+bW+2,widthL.GetBottom());
        MakeModalButton("+10cm","Taller20",11,bW,bH,oX,heightL.GetBottom());
        MakeModalButton("-10cm","Shorter20",12,bW,bH,oX+bW+2,heightL.GetBottom());
        MakeModalButton("Forw","rot0",13,bW/2,bH,oX,rotL.GetBottom());
        MakeModalButton("Right","rot90",14,bW/2,bH,oX+bW/2+2,rotL.GetBottom());
        MakeModalButton("Back","rot180",15,bW/2,bH,oX+bW+4,rotL.GetBottom());
        MakeModalButton("Left","rot270",16,bW/2,bH,oX+3*bW/2+6,rotL.GetBottom());
        MakeModalButton("+5°","pitch+5",17,bW,bH,oX,pitchL.GetBottom());
        MakeModalButton("-5°","pitch-5",18,bW,bH,oX+bW+2,pitchL.GetBottom());
        MakeModalButton("+2°","tilt+2°",19,bW,bH,oX,tiltL.GetBottom());
        MakeModalButton("-2°","tilt-2°",20,bW,bH,oX+bW+2,tiltL.GetBottom());
        MakeModalButton("Commit","Ok",21,100,20,40,rectButtons.GetTop()-25);

        // make numpad and its associated buttons

        numpad=new Keyboard(true);
        numpad->MakeKeyboard(100,rectButtons.GetBottom()+10,true);
        numpad->SetVisibility(true);

        numpad->Relocate((wWidth/2)-(numpad->MyWidth()/2),numpad->MyBottom());

        int offcmX(numpad->MyLeft()+numpad->MyWidth()+2),offcmY(numpad->MyBottom());
        MakeModalButton("m","m",22,40,20,offcmX,offcmY);
        MakeModalButton("cm","cm",23,40,20,offcmX,offcmY+22);
        MakeModalButton("mm","mm",24,40,20,offcmX,offcmY+44);
        MakeModalButton("ft","ft",25,40,20,offcmX,offcmY+66);
        MakeModalButton("inch","inch",26,40,20,offcmX,offcmY+88);
        forcursor.push_back("");

        ChangeCurrentUnit(0,22);
        DrawLogic::UpdateTexture();
        PrintLines();
        //ManageModalWindow::ResizeModalWindow(wWidth,400);
}

void advanced::MakeModalButton(string name,string actionName,ulong myNumber,int width, int height, int offsetX, int offsetY){
    DrawLogic::WriteDebug("advanced::MakeModalButton got to make "+name);
    button_VR *thisButton(new button_VR(actionName,false));
    thisButton->SetDimensions(width,height);
    thisButton->SetOrigin(offsetX,offsetY);
    thisButton->SetToStateColor();
    thisButton->setText(name);
    thisButton->SetDebugName(name);
    thisButton->setVisibility(true);
    Action *newAction=new (Action);
    newAction->ActionName=actionName;
    newAction->button=thisButton;
    newAction->number=myNumber;
    actionButtons.push_back(newAction);
    DrawLogic::WriteDebug("advanced::MakeModalButton made "+name);
}

void advanced::MakeLine(TextLine &in_Line, int in_x, int in_y, int in_width, int in_height, string in_text){
    in_Line.SetOrigin(in_x,in_y);
    in_Line.SetDimensions(in_width,in_height);
    in_Line.SetBackGroundColor(Clr_White);
    in_Line.setText(in_text);
    in_Line.PrintString();
}

void advanced::PrintLines(){
    sittingL.PrintString();
    offsetL.PrintString();
    rightShiftL.PrintString();
    aftShiftL.PrintString();
    widthL.PrintString();
    heightL.PrintString();
    rotL.PrintString();
    pitchL.PrintString();
    tiltL.PrintString();
}
void advanced::DrawMyself(XPLMWindowID, void *){

    ManageModalWindow::ConstrainGeometry();
    DrawLogic::RenderContent();
    if (myself->cursor.HasCursor()) myself->cursor.DrawCursor();//myself.activeLine->GetTextY()
}

int advanced::MouseHandler(XPLMWindowID, int in_x, int in_y, int is_down, void *){
    ManageModalWindow::MakeTopWindow();
    int x(in_x-ManageModalWindow::GetLeft()),y(in_y-ManageModalWindow::GetBottom());
    if (is_down==xplm_MouseDown){
        string selAct("");
        myself->mouseDrag=false;
        myself->epochClick=0;

        for (auto bt:myself->actionButtons){
            if (bt->button->IsHere(x,y)){
                selAct=bt->ActionName;
                selectedB=bt->number;
                bt->button->Press();
                myself->ButtonAction(selAct,static_cast<ulong>(bt->number));
                return 1;
            }
        }
        if (myself->sittingL.isHere(x,y)){
            return 1;
        }
        if (myself->offsetL.isHere(x,y)){

            myself->SelectLine(&(myself->offsetL),x,&myself->bBoxOffset);
            myself->notallowed="";
            myself->maxlength=7;
            myself->hasInterval=false;
            myself->minval=0;
            myself->maxval=0;
            if (myself->currentUnit==0||myself->currentUnit==3) myself->numberOfDigits=4;
            if (myself->currentUnit==1||myself->currentUnit==4) myself->numberOfDigits=2;
            return 1;
        }
        if (myself->rightShiftL.isHere(x,y)){
            myself->SelectLine(&(myself->rightShiftL),x,&myself->bBoxLatShift);
            myself->notallowed="";
            myself->maxlength=7;
            myself->hasInterval=false;
            myself->minval=0;
            myself->maxval=0;
            if (myself->currentUnit==0||myself->currentUnit==3) myself->numberOfDigits=4;
            if (myself->currentUnit==1||myself->currentUnit==4) myself->numberOfDigits=2;
            return 1;
        }
        if (myself->aftShiftL.isHere(x,y)){
            myself->SelectLine(&(myself->aftShiftL),x,&myself->bBoxAxShift);
            myself->notallowed="";
            myself->maxlength=7;
            myself->hasInterval=false;
            myself->minval=0;
            myself->maxval=0;
            if (myself->currentUnit==0||myself->currentUnit==3) myself->numberOfDigits=4;
            if (myself->currentUnit==1||myself->currentUnit==4) myself->numberOfDigits=2;
            return 1;
        }
        if (myself->widthL.isHere(x,y)){
            myself->SelectLine(&(myself->widthL),x,&myself->bBoxWidth);
            myself->notallowed="-+";
            myself->maxlength=6;
            myself->hasInterval=false;
            myself->minval=0;
            myself->maxval=0;
            if (myself->currentUnit==0||myself->currentUnit==3) myself->numberOfDigits=4;
            if (myself->currentUnit==1||myself->currentUnit==4) myself->numberOfDigits=2;
            return 1;
        }
        if (myself->heightL.isHere(x,y)){
            myself->SelectLine(&(myself->heightL),x,&myself->bBoxheight);
            myself->notallowed="-+";
            myself->maxlength=6;
            myself->hasInterval=false;
            myself->minval=0;
            myself->maxval=0;
            if (myself->currentUnit==0||myself->currentUnit==3) myself->numberOfDigits=4;
            if (myself->currentUnit==1||myself->currentUnit==4) myself->numberOfDigits=2;
            return 1;
        }

        if (myself->rotL.isHere(x,y)){
            myself->SelectLine(&(myself->rotL),x,&myself->rot);
            myself->notallowed="-+.";
            myself->maxlength=3;
            myself->hasInterval=true;
            myself->minval=0;
            myself->maxval=359;
            myself->numberOfDigits=0;
            return 1;
        }

        if (myself->pitchL.isHere(x,y)){
            myself->SelectLine(&(myself->pitchL),x,&myself->pitch);
            myself->notallowed=".";
            myself->maxlength=3;
            myself->hasInterval=true;
            myself->minval=-90;
            myself->maxval=90;
            myself->numberOfDigits=0;
            return 1;
        }

        if (myself->tiltL.isHere(x,y)){
            myself->SelectLine(&(myself->tiltL),x,&myself->tilt);
            myself->notallowed=".";
            myself->maxlength=3;
            myself->hasInterval=true;
            myself->minval=-180;
            myself->maxval=180;
            myself->numberOfDigits=0;
            return 1;
        }
        bool special;
        std::string keyName,keyVal;
        if (myself->numpad->PressKey(x,y,special,keyName,keyVal)){
            if (myself->numpad->IsKeyPressed()) myself->ProcessKeyPress(special,keyName,keyVal);
        }
    }
 if (is_down==xplm_MouseDrag){
        if (myself->epochClick>0){
            float now=XPLMGetElapsedTime();
            if (now-myself->epochClick>0.3f) myself->mouseDrag=true;
            if (myself->mouseDrag){
                myself->cursor.DragPos(0,x);
            }
        }
    }

 if (is_down==xplm_MouseUp){
        if (myself->mouseDrag) {
            myself->cursor.EndOfDrag();
            myself->epochClick=0;
        }
        myself->numpad->ReleaseCurrentKey();

        if (selectedB>0) {
            myself->actionButtons[selectedB]->button->Release();
            selectedB=0;
        }
        if (myself->endAlert) {
            myself->EndEdit();
            myself->callBack();
            return 1;}
    }
 if (myself->cursor.HasCursor()&&myself->activeLine!=nullptr) DrawLogic::SetCursorPosition(myself->cursor.PosToX(),myself->activeLine->GetTextY()+2);
 if (myself->cursor.HasSelection()&&myself->activeLine!=nullptr) {
     int l(0),r(0);
     if (myself->cursor.IsIndexInSelection(0,l,r)){
         myself->activeLine->PrintBox();
         DrawLogic::PrintRectOnTexture(l,myself->activeLine->GetBottom(),r,myself->activeLine->GetTop(),Clr_TextSelectBlue);
         myself->activeLine->PrintStringOnLocalT();

     }
 }
    return 1;
}

void advanced::ButtonAction(string actionName, ulong mbutton){
    DrawLogic::WriteDebug("advanced ButtonAction received "+actionName);
    ulong nod(1);
    if (currentUnit==0||currentUnit==3) nod=4;
    if (currentUnit==1||currentUnit==4) nod=2;
    actionSelected=actionName;
    if (actionName=="Cancel"){ endAlert=true;DrawLogic::WriteDebug("advanced buttonaction cancelr selected");}
   if (actionName=="Ok") {endAlert=true; Commit();}
   if (actionName=="Sitting"){sittingL.setTextAndUpdate("SITTING");currentHsp.type="SITTING";}
   if (actionName=="Standing"){sittingL.setTextAndUpdate(("STANDING"));currentHsp.type="STANDING";}
   if (actionName=="Raise50"){bBoxOffset+=gapincr;offsetL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxOffset,nod));}
   if (actionName=="Lower50"){bBoxOffset-=gapincr;offsetL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxOffset,nod));}
   if (actionName=="Right20"){bBoxLatShift+=latshiftincr;rightShiftL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxLatShift,nod));}
   if (actionName=="Left20"){bBoxLatShift-=latshiftincr;rightShiftL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxLatShift,nod));}
   if (actionName=="For20"){bBoxAxShift-=axshiftincr;aftShiftL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxAxShift,nod));}
   if (actionName=="Aft20"){bBoxAxShift+=axshiftincr;aftShiftL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxAxShift,nod));}
   if (actionName=="Width20"){bBoxWidth+=widthincr;widthL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxWidth,nod));}
   if (actionName=="Width-20"){bBoxWidth-=widthincr;if (bBoxWidth<0.1f) bBoxWidth=0.1f;widthL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxWidth,nod));}
   if (actionName=="Taller20"){bBoxheight+=heightincr;heightL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxheight,nod));}
   if (actionName=="Shorter20"){bBoxheight-=heightincr;if (bBoxheight<0.1f) bBoxheight=0.1f;heightL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxheight,nod));}
   if (actionName=="rot0")   {  rot=0;rotL.setTextAndUpdate(stringOps::ConvertFloatToString(rot,0));}
   if (actionName=="rot90")  { rot=90;rotL.setTextAndUpdate(stringOps::ConvertFloatToString(rot,0));}
   if (actionName=="rot180") {rot=180;rotL.setTextAndUpdate(stringOps::ConvertFloatToString(rot,0));}
   if (actionName=="rot270") {rot=270;rotL.setTextAndUpdate(stringOps::ConvertFloatToString(rot,0));}
   if (actionName=="pitch+5"){
       pitch+=pitchincr;
       if (pitch>90) pitch=90;
       pitchL.setTextAndUpdate(stringOps::ConvertFloatToString(pitch,0));}
   if (actionName=="pitch-5"){
       pitch-=pitchincr;
       if (pitch<-90) pitch=-90;
       pitchL.setTextAndUpdate(stringOps::ConvertFloatToString(pitch,0));}
   if (actionName=="tilt+2°"){tilt+=tiltincr;if (tilt>180) tilt=180;tiltL.setTextAndUpdate(stringOps::ConvertFloatToString(tilt,0));}
   if (actionName=="tilt-2°"){tilt-=tiltincr;if (tilt<-180) tilt=-180;tiltL.setTextAndUpdate(stringOps::ConvertFloatToString(tilt,0));}
   if (actionName=="Ok"){Commit();}
   if (actionName=="m") {ChangeCurrentUnit(0,mbutton);}
   if (actionName=="cm") {ChangeCurrentUnit(1,mbutton);}
   if (actionName=="mm") {ChangeCurrentUnit(2,mbutton);}
   if (actionName=="ft") {ChangeCurrentUnit(3,mbutton);}
   if (actionName=="inch") {ChangeCurrentUnit(4,mbutton);}


}

void advanced::SelectLine(TextLine *in_line, int x, float *in_param){
    if(activeLine==in_line){
        epochClick=XPLMGetElapsedTime();
        cursor.FindPos(0,x);
    }
    else {
        CommitActiveLine();
        activeLine=in_line;
        activeParameter=in_param;
        forcursor[0]=activeLine->GetText();
        cursor.Initiate(&forcursor,activeLine->GetTextX(),activeLine->GetTextY(),10,1);
        cursor.SetCursorAt(0,0);
    }
}

void advanced::CommitActiveLine(){
    if (activeLine!=nullptr){
        float val=stringOps::ConvertStringToFloat(activeLine->GetText());
        if (hasInterval){
            if (val<minval) val=minval;
            if (val>maxval) val=maxval;
            *activeParameter=val;
            activeLine->setText(stringOps::ConvertFloatToString(val,numberOfDigits));
        }
        else{
            *activeParameter=val;
            activeLine->setText(stringOps::ConvertFloatToString(val,numberOfDigits));
        }
        cursor.EraseCursor();
        activeLine->PrintString();
        activeLine=nullptr;
        activeParameter=nullptr;
    }
}

void advanced::ProcessKeyPress(bool special,string keyName,string in_String){
    if (!special){
        if (cursor.HasSelection()) {
            int currentPos=cursor.GetSelectionStartCharPos();
            EraseSelection();
            cursor.SetCursorAt(0,currentPos);
        }
        if (cursor.HasCursor()&&(notallowed.find(in_String)==std::string::npos)){
            int cP=cursor.GetPos();
            string oldLine=activeLine->GetText();
            if (oldLine.size()<maxlength){
                ulong sP=static_cast<ulong>(cursor.FindPositionInNativeLine(oldLine,cP));
                if ((in_String=="+"||in_String=="-")&&sP>0) return;
                oldLine.insert(sP,in_String);
                activeLine->setText(oldLine);
                int insL=cursor.GetLengthOfUTFString(in_String);
                cP+=insL;
                cursor.AddPositionsToLine(insL);
                cursor.MakeLinePosAgain(0,oldLine);
                cursor.SetCursorAt(0,cP);
            }
        }
    }
    else{
        if (keyName=="BckSpc") {Backspace();}
        if (keyName=="Enter") {
            cursor.EraseCursor();
        float toeval=stringOps::ConvertStringToFloat(activeLine->GetText());
        if (hasInterval){
            if (toeval<minval) activeLine->setText(stringOps::ConvertFloatToString(minval));
            if (toeval>maxval) activeLine->setText(stringOps::ConvertFloatToString(maxval));
        }
        }
        if (keyName=="RIGHT") {MoveCursorRight();}
        if (keyName=="LEFT") {MoveCursorLeft();}

    }
    activeLine->PrintString();
}
void advanced::Backspace(){
    if (cursor.HasCursor()){
        int cP=cursor.GetPos();
        if (cP>0){
            EraseFromLine(cP-1,cP);
        }
        cursor.SetCursorAt(0,cP-1);
    }
    if (cursor.HasSelection()) {
        int cP=cursor.GetSelectionStartCharPos();
        EraseSelection();
        cursor.SetCursorAt(0,cP);
    }
}

void advanced::MoveCursorLeft(){
    if (cursor.HasSelection()){
        int startPos=cursor.GetSelectionStartCharPos();
        cursor.SetCursorAt(0,startPos);
    }
    if (cursor.HasCursor()) cursor.MoveCursorLeft();
}

void advanced::MoveCursorRight(){
    if (cursor.HasSelection()){
        int endPos=cursor.GetSelectionEndCharPos();
        cursor.SetCursorAt(0,endPos);
    }
    if (cursor.HasCursor()) cursor.MoveCursorRight();
}

void advanced::EraseFromLine(int begin, int end){
    string userLine=activeLine->GetText();
    ulong sP=static_cast<ulong>(cursor.FindPositionInNativeLine(userLine,begin));
    ulong eP=static_cast<ulong>(cursor.FindPositionInNativeLine(userLine,end));
    userLine.erase(sP,eP-sP);
    activeLine->setText(userLine);
    cursor.EraseCursor();
    cursor.MakeLinePosAgain(0,userLine);
    cursor.SetCursorAt(0,begin);
}

void advanced::EraseSelection(){
    if (cursor.HasSelection()){
        int startPos=cursor.GetSelectionStartCharPos();
        int endPos=cursor.GetSelectionEndCharPos();
        EraseFromLine(startPos,endPos);

    }
}

void advanced::GetBBoxDescription(){
    bBoxheight=currentHsp.AABBmaxY-currentHsp.AABBminY;
    bBoxWidth=max((currentHsp.AABBmaxX-currentHsp.AABBminX),(currentHsp.AABBmaxZ-currentHsp.AABBminZ));
    float span=currentHsp.AABBmaxX-currentHsp.AABBminX;
    span=span/2.0f;
    bBoxLatShift=currentHsp.AABBmaxX-span-currentHsp.PresetX;
    span=currentHsp.AABBmaxZ-currentHsp.AABBminZ;
    span=span/2.0f;
    bBoxAxShift=currentHsp.AABBmaxZ-span-currentHsp.PresetZ;
    bBoxOffset=currentHsp.AABBmaxY-currentHsp.PresetY;
}


void advanced::EndEdit(){
    CommitActiveLine();
    delete numpad;
    numpad=nullptr;
    DrawLogic::WriteDebug("advanced endedit going to destroy modal window");
   ManageModalWindow::DestroyModalWindow();
}

float advanced::ConvertMeterToUnit(int in_unit, float in_value){
    switch (in_unit){
    case 0:  return in_value;
    case 1: return (in_value*100);
    case 2: return (in_value*1000);
    case 3: return (in_value*3.28084f);
    case 4: return (in_value*39.37007874f);
    }
    return 0;
}

float advanced::ConvertUnitToMeter(int in_unit, float in_value){
    switch (in_unit){
    case 0:  return in_value;
    case 1: return (in_value*0.01f);
    case 2: return (in_value*0.001f);
    case 3: return (in_value*0.3048f);
    case 4: return (in_value*0.0254f);
    }
    return 0;
}

void advanced::ChangeCurrentUnit(int new_unit, ulong in_button){
    CommitActiveLine();
    bBoxOffset=  ConvertMeterToUnit(new_unit,ConvertUnitToMeter(currentUnit,bBoxOffset));
    bBoxWidth=   ConvertMeterToUnit(new_unit,ConvertUnitToMeter(currentUnit,bBoxWidth));
    bBoxheight=  ConvertMeterToUnit(new_unit,ConvertUnitToMeter(currentUnit,bBoxheight));
    bBoxLatShift=ConvertMeterToUnit(new_unit,ConvertUnitToMeter(currentUnit,bBoxLatShift));
    bBoxAxShift= ConvertMeterToUnit(new_unit,ConvertUnitToMeter(currentUnit,bBoxAxShift));
    currentUnit=new_unit;

    for (auto &mb:actionButtons){
        mb->button->setSelect(false);
    }
    actionButtons[in_button]->button->setSelect(true);
    switch (new_unit){
    case 0:  {//meters
        gapincr=0.05f;
        widthincr=0.05f;
        heightincr=0.1f;
        latshiftincr=0.02f;
        axshiftincr=0.02f;
        //actionbuttons 3 à 12
        actionButtons[3]->button->setText("+5cm");
        actionButtons[4]->button->setText("-5cm");
        actionButtons[5]->button->setText("+2cm(R)");
        actionButtons[6]->button->setText("-2cm(L)");
        actionButtons[7]->button->setText("-2cm(F)");
        actionButtons[8]->button->setText("+2cm(B)");
        actionButtons[9]->button->setText("+5cm");
        actionButtons[10]->button->setText("-5cm");
        actionButtons[11]->button->setText("+10cm");
        actionButtons[12]->button->setText("-10cm");
        if (numberOfDigits>0) numberOfDigits=4;
        offsetL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxOffset));
        rightShiftL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxLatShift));
        aftShiftL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxAxShift));
        widthL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxWidth));
        heightL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxheight));
        break;
    }
    case 1:  {//centimeters
        gapincr=5;
        widthincr=5;
        heightincr=10;
        latshiftincr=2;
        axshiftincr=2;
        //actionbuttons 3 à 12
        actionButtons[3]->button->setText("+5cm");
        actionButtons[4]->button->setText("-5cm");
        actionButtons[5]->button->setText("+2cm(R)");
        actionButtons[6]->button->setText("-2cm(L)");
        actionButtons[7]->button->setText("-2cm(F)");
        actionButtons[8]->button->setText("+2cm(B)");
        actionButtons[9]->button->setText("+5cm");
        actionButtons[10]->button->setText("-5cm");
        actionButtons[11]->button->setText("+10cm");
        actionButtons[12]->button->setText("-10cm");
        if (numberOfDigits>0) numberOfDigits=2;
        offsetL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxOffset,2));
        rightShiftL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxLatShift,2));
        aftShiftL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxAxShift,2));
        widthL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxWidth,2));
        heightL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxheight,2));
        break;
    }
    case 2:  {//millimeters
        gapincr=50;
        widthincr=50;
        heightincr=100;
        latshiftincr=20;
        axshiftincr=20;
        //actionbuttons 3 à 12
        actionButtons[3]->button->setText("+5cm");
        actionButtons[4]->button->setText("-5cm");
        actionButtons[5]->button->setText("+2cm(R)");
        actionButtons[6]->button->setText("-2cm(L)");
        actionButtons[7]->button->setText("-2cm(F)");
        actionButtons[8]->button->setText("+2cm(B)");
        actionButtons[9]->button->setText("+5cm");
        actionButtons[10]->button->setText("-5cm");
        actionButtons[11]->button->setText("+10cm");
        actionButtons[12]->button->setText("-10cm");
        if (numberOfDigits>0) numberOfDigits=1;
        offsetL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxOffset,1));
        rightShiftL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxLatShift,1));
        aftShiftL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxAxShift,1));
        widthL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxWidth,1));
        heightL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxheight,1));
        break;
    }
    case 3:   {//feet
        gapincr     =2*0.0833333f;
        widthincr   =2*0.0833333f;
        heightincr  =4*0.0833333f;
        latshiftincr=0.0833333f;
        axshiftincr =0.0833333f;
        //actionbuttons 3 à 12
        actionButtons[3]->button->setText("+2i");
        actionButtons[4]->button->setText("-2i");
        actionButtons[5]->button->setText("+1i(R)");
        actionButtons[6]->button->setText("-1i(L)");
        actionButtons[7]->button->setText("-1i(F)");
        actionButtons[8]->button->setText("+1i(B)");
        actionButtons[9]->button->setText("+2i");
        actionButtons[10]->button->setText("-2i");
        actionButtons[11]->button->setText("+4i");
        actionButtons[12]->button->setText("-4i");
        if (numberOfDigits>0) numberOfDigits=4;
        offsetL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxOffset,4));
        rightShiftL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxLatShift,4));
        aftShiftL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxAxShift,4));
        widthL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxWidth,4));
        heightL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxheight,4));
        break;
    }
    case 4:  {//inches
        gapincr     =2;
        widthincr   =2;
        heightincr  =4;
        latshiftincr=1;
        axshiftincr =1;
        //actionbuttons 3 à 12
        actionButtons[3]->button->setText("+2i");
        actionButtons[4]->button->setText("-2i");
        actionButtons[5]->button->setText("+1i(R)");
        actionButtons[6]->button->setText("-1i(L)");
        actionButtons[7]->button->setText("-1i(F)");
        actionButtons[8]->button->setText("+1i(B)");
        actionButtons[9]->button->setText("+2i");
        actionButtons[10]->button->setText("-2i");
        actionButtons[11]->button->setText("+4i");
        actionButtons[12]->button->setText("-4i");
        if (numberOfDigits>0) numberOfDigits=2;
        offsetL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxOffset,2));
        rightShiftL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxLatShift,2));
        aftShiftL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxAxShift,2));
        widthL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxWidth,2));
        heightL.setTextAndUpdate(stringOps::ConvertFloatToString(bBoxheight,2));
        break;
    }
    }
}

void advanced::Commit(){

    actionSelected="Commit";
    CommitActiveLine();
    currentHsp.type=sittingL.GetText();
    currentHsp.AABBmaxX=ConvertUnitToMeter(currentUnit,bBoxWidth)/2.0f+ConvertUnitToMeter(currentUnit,bBoxLatShift)+currentHsp.PresetX;
    currentHsp.AABBminX=currentHsp.AABBmaxX-ConvertUnitToMeter(currentUnit,bBoxWidth);
    currentHsp.AABBmaxY=currentHsp.PresetY+ConvertUnitToMeter(currentUnit,bBoxOffset);
    currentHsp.AABBminY=currentHsp.AABBmaxY-ConvertUnitToMeter(currentUnit,bBoxheight);
    currentHsp.AABBminZ=currentHsp.PresetZ-(ConvertUnitToMeter(currentUnit,bBoxWidth))/2+ConvertUnitToMeter(currentUnit,bBoxAxShift);
    currentHsp.AABBmaxZ=currentHsp.AABBminZ+ConvertUnitToMeter(currentUnit,bBoxWidth);
    currentHsp.psi=rot;
    currentHsp.the=tilt;
    currentHsp.phi=pitch;
}

Hotspot advanced::GetHotspot(){return currentHsp;}

bool advanced::IsToCommit(){return (actionSelected=="Commit");}

