#include "advanced.h"
#include "managemodalwindow.h"
#include "drawlogic.h"
ulong advanced::selectedB(0);
int advanced::top(0);
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
    rectSit(true,true),rectBox(true,true),rectParam(true,true),rectButtons(true,true),
    sittingB(true,true),offsetB(true,true),rightShiftB(true,true),aftShiftB(true,true),heightB(true,true),widthB(true,true),rotB(true,true),tiltB(true,true),pitchB(true,true),
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
}

void advanced::MakeAdvancedDialog(Hotspot htsp, std::function<void()> cBck){
    left=0;
    top=0;
    endAlert=false;
    callBack=cBck;
    int wWidth=380,col1=140,lH=22,textBoxWidth=60,textBoxHeight=15,bH=15;;
    currentHsp=htsp;
    ManageModalWindow::CreateMousedModalWindow(MouseHandler,DrawMyself,Clr_LightGray,wWidth,450);


    rectSit.SetDimensions(wWidth,52);
    rectSit.SetOffsets(0,0);
    rectSit.setColor(Clr_LighterGray);
         rectBox.SetDimensions(wWidth,140);
         rectBox.SetOffsets(0,rectSit.GetOffsetY()+rectSit.GetHeight());
         rectBox.setColor(Clr_LightGray);
    rectParam.SetDimensions(wWidth,95);
    rectParam.SetOffsets(0,rectBox.GetOffsetY()+rectBox.GetHeight());
    rectParam.setColor(Clr_LighterGray);
      rectButtons.SetDimensions(wWidth,30);
      rectButtons.SetOffsets(0,rectParam.GetOffsetY()+rectParam.GetHeight());
      rectButtons.setColor(Clr_LightGray);

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

       sittingP.SetOffsets(col1-8-stringOps::StringSize(sittingT),rectSit.GetOffsetY()+lH+lH);
        offsetP.SetOffsets(col1-3-stringOps::StringSize(offsetT),rectBox.GetOffsetY()+lH+lH);
    rightShiftP.SetOffsets(col1-3-stringOps::StringSize(rightShiftT),offsetP.offsetY+lH);
      aftShiftP.SetOffsets(col1-3-stringOps::StringSize(aftShiftT),rightShiftP.offsetY+lH);
         widthP.SetOffsets(col1-3-stringOps::StringSize(widthT),aftShiftP.offsetY+lH);
        heightP.SetOffsets(col1-3-stringOps::StringSize(heightT),widthP.offsetY+lH);
           rotP.SetOffsets(col1-3-stringOps::StringSize(rotT),rectParam.GetOffsetY()+lH+20);
         pitchP.SetOffsets(col1-3-stringOps::StringSize(pitchT),rotP.offsetY+lH);
          tiltP.SetOffsets(col1-3-stringOps::StringSize(tiltT),pitchP.offsetY+lH);
        hsNameP.SetOffsets(col1-100,rectSit.GetOffsetY()+lH);
         hsBoxP.SetOffsets(col1-100,rectBox.GetOffsetY()+lH);
  hsAdditionalP.SetOffsets(col1-100,rectParam.GetOffsetY()+lH);

     sittingN=DrawLogic::AddModalString(sittingT,Clr_BlackInk,sittingP);
      offsetN=DrawLogic::AddModalString(offsetT,Clr_BlackInk,offsetP);
  rightShiftN=DrawLogic::AddModalString(rightShiftT,Clr_BlackInk,rightShiftP);
    aftShiftN=DrawLogic::AddModalString(aftShiftT,Clr_BlackInk,aftShiftP);
       widthN=DrawLogic::AddModalString(widthT,Clr_BlackInk,widthP);
      heightN=DrawLogic::AddModalString(heightT,Clr_BlackInk,heightP);
         rotN=DrawLogic::AddModalString(rotT,Clr_BlackInk,rotP);
       pitchN=DrawLogic::AddModalString(pitchT,Clr_BlackInk,pitchP);
        tiltN=DrawLogic::AddModalString(tiltT,Clr_BlackInk,tiltP);

      hsNameN=DrawLogic::AddModalString(nm,Clr_PushedBlue,hsNameP);
       hsBoxN=DrawLogic::AddModalString("Position of Binding Box relative to Hotspot :",Clr_PushedBlue,hsBoxP);
hsAdditionalN=DrawLogic::AddModalString("hotspot orientation : ",Clr_PushedBlue,hsAdditionalP);

     int corr=10;
     sittingB.SetDimensions(textBoxWidth+7,textBoxHeight);sittingB.SetOffsets(col1-10,sittingP.offsetY-corr);
      offsetB.SetDimensions(textBoxWidth,textBoxHeight);offsetB.SetOffsets(col1-3,offsetP.offsetY-10);
  rightShiftB.SetDimensions(textBoxWidth,textBoxHeight);rightShiftB.SetOffsets(col1-3,rightShiftP.offsetY-corr);
    aftShiftB.SetDimensions(textBoxWidth,textBoxHeight);aftShiftB.SetOffsets(col1-3,aftShiftP.offsetY-corr);
       widthB.SetDimensions(textBoxWidth,textBoxHeight);widthB.SetOffsets(col1-3,widthP.offsetY-corr);
      heightB.SetDimensions(textBoxWidth,textBoxHeight);heightB.SetOffsets(col1-3,heightP.offsetY-corr);
         rotB.SetDimensions(textBoxWidth-20,textBoxHeight);rotB.SetOffsets(col1-3,rotP.offsetY-corr);
       pitchB.SetDimensions(textBoxWidth-20,textBoxHeight);pitchB.SetOffsets(col1-3,pitchP.offsetY-corr);
        tiltB.SetDimensions(textBoxWidth-20,textBoxHeight);tiltB.SetOffsets(col1-3,tiltP.offsetY-corr);

        sittingB.setColor(Clr_White);
        offsetB.setColor(Clr_White);
        rightShiftB.setColor(Clr_White);
        aftShiftB.setColor(Clr_White);
        widthB.setColor(Clr_White);
        heightB.setColor(Clr_White);
        rotB.setColor(Clr_White);
        pitchB.setColor(Clr_White);
        tiltB.setColor(Clr_White);
       corr=10;
        sittingL.SetDimensions(textBoxWidth+7,textBoxHeight);sittingL.SetOffsets(col1-10,sittingB.GetOffsetY()+corr);
         offsetL.SetDimensions(textBoxWidth,textBoxHeight);offsetL.SetOffsets(col1-3,offsetB.GetOffsetY()+corr);
     rightShiftL.SetDimensions(textBoxWidth,textBoxHeight);rightShiftL.SetOffsets(col1-3,rightShiftB.GetOffsetY()+corr);
       aftShiftL.SetDimensions(textBoxWidth,textBoxHeight);aftShiftL.SetOffsets(col1-3,aftShiftB.GetOffsetY()+corr);
          widthL.SetDimensions(textBoxWidth,textBoxHeight);widthL.SetOffsets(col1-3,widthB.GetOffsetY()+corr);
         heightL.SetDimensions(textBoxWidth,textBoxHeight);heightL.SetOffsets(col1-3,heightB.GetOffsetY()+corr);
            rotL.SetDimensions(textBoxWidth-20,textBoxHeight);rotL.SetOffsets(col1-3,rotB.GetOffsetY()+corr);
          pitchL.SetDimensions(textBoxWidth-20,textBoxHeight);pitchL.SetOffsets(col1-3,pitchB.GetOffsetY()+corr);
           tiltL.SetDimensions(textBoxWidth-20,textBoxHeight);tiltL.SetOffsets(col1-3,tiltB.GetOffsetY()+corr);

           GetBBoxDescription();

           sittingL.setText(currentHsp.type);
           offsetL.setText(VRCReader::ConvertFloatToString(bBoxOffset));
           rightShiftL.setText(VRCReader::ConvertFloatToString(bBoxLatShift));
           aftShiftL.setText(VRCReader::ConvertFloatToString(bBoxAxShift));
           widthL.setText(VRCReader::ConvertFloatToString(bBoxWidth));
           heightL.setText(VRCReader::ConvertFloatToString(bBoxheight));
           rotL.setText(VRCReader::ConvertFloatToString(currentHsp.psi,0));
           pitchL.setText(VRCReader::ConvertFloatToString(currentHsp.the,0));
           tiltL.setText(VRCReader::ConvertFloatToString(currentHsp.phi,0));

        int oX=col1+6+textBoxWidth,bW=64;
        MakeModalButton("Cancel","Cancel",0,100,20,170,rectButtons.GetOffsetY()+5);
        MakeModalButton("SITTING","Sitting",1,bW,bH,oX,sittingB.GetOffsetY());
        MakeModalButton("STANDING","Standing",2,bW,bH,oX+bW+2,sittingB.GetOffsetY());
        MakeModalButton("+5cm","Raise50",3,bW,bH,oX,offsetB.GetOffsetY());
        MakeModalButton("-5cm","Lower50",4,bW,bH,oX+bW+2,offsetB.GetOffsetY());
        MakeModalButton("+2cm(R)","Right20",5,bW,bH,oX,rightShiftB.GetOffsetY());
        MakeModalButton("-2cm(L)","Left20",6,bW,bH,oX+bW+2,rightShiftB.GetOffsetY());
        MakeModalButton("-2cm(F)","For20",7,bW,bH,oX,aftShiftB.GetOffsetY());
        MakeModalButton("+2cm(B)","Aft20",8,bW,bH,oX+bW+2,aftShiftB.GetOffsetY());
        MakeModalButton("+5cm","Width20",9,bW,bH,oX,widthB.GetOffsetY());
        MakeModalButton("-5cm","Width-20",10,bW,bH,oX+bW+2,widthB.GetOffsetY());
        MakeModalButton("+10cm","Taller20",11,bW,bH,oX,heightB.GetOffsetY());
        MakeModalButton("-10cm","Shorter20",12,bW,bH,oX+bW+2,heightB.GetOffsetY());
        MakeModalButton("Forw","rot0",13,bW/2,bH,oX,rotB.GetOffsetY());
        MakeModalButton("Right","rot90",14,bW/2,bH,oX+bW/2+2,rotB.GetOffsetY());
        MakeModalButton("Back","rot180",15,bW/2,bH,oX+bW+4,rotB.GetOffsetY());
        MakeModalButton("Left","rot270",16,bW/2,bH,oX+3*bW/2+6,rotB.GetOffsetY());
        MakeModalButton("+5°","pitch+5",17,bW,bH,oX,pitchB.GetOffsetY());
        MakeModalButton("-5°","pitch-5",18,bW,bH,oX+bW+2,pitchB.GetOffsetY());
        MakeModalButton("+2°","tilt+2°",19,bW,bH,oX,tiltB.GetOffsetY());
        MakeModalButton("-2°","tilt-2°",20,bW,bH,oX+bW+2,tiltB.GetOffsetY());
        MakeModalButton("Commit","Ok",21,100,20,40,rectButtons.GetOffsetY()+5);

        int offcmX(195),offcmY(rectButtons.GetOffsetY()+30);
        MakeModalButton("m","m",22,40,20,offcmX,offcmY);
        MakeModalButton("cm","cm",23,40,20,offcmX,offcmY+22);
        MakeModalButton("mm","mm",24,40,20,offcmX,offcmY+44);
        MakeModalButton("ft","ft",25,40,20,offcmX,offcmY+66);
        MakeModalButton("inch","inch",26,40,20,offcmX,offcmY+88);

        forcursor.push_back("");

        numpad=new Keyboard(true);
        numpad->MakeKeyboard(100,rectButtons.GetOffsetY()+30,true);
        numpad->SetVisibility(true);
        ChangeCurrentUnit(0,22);

        //ManageModalWindow::ResizeModalWindow(wWidth,400);
}

void advanced::MakeModalButton(string name,string actionName,ulong myNumber,int width, int height, int offsetX, int offsetY){
    ModalButton thisButton;
    thisButton.SetDimensions(width,height);
    thisButton.SetOffsets(offsetX,offsetY);   
    thisButton.SetToStateColor();
    thisButton.setText(name);
    thisButton.setVisibility(true);
    Action newAction;
    newAction.ActionName=actionName;
    newAction.button=thisButton;
    newAction.number=myNumber;
    actionButtons.push_back(newAction);
}

void advanced::RecalculateDialog (){
    for (auto &bt:myself->actionButtons){
        bt.button.recalculate(left,top);
    }
     myself->rectBox.recalculate(left,top);
     myself->rectSit.recalculate(left,top);
     myself->rectParam.recalculate(left,top);
     myself->rectButtons.recalculate(left,top);
     myself->numpad->Recalculate(left,top);
     myself->sittingP.recalculate(left,top);
      myself->offsetP.recalculate(left,top);
  myself->rightShiftP.recalculate(left,top);
    myself->aftShiftP.recalculate(left,top);
       myself->widthP.recalculate(left,top);
      myself->heightP.recalculate(left,top);
         myself->rotP.recalculate(left,top);
       myself->pitchP.recalculate(left,top);
        myself->tiltP.recalculate(left,top);
       myself->hsBoxP.recalculate(left,top);
      myself->hsNameP.recalculate(left,top);
myself->hsAdditionalP.recalculate(left,top);

    myself->sittingB.recalculate(left,top);
     myself->offsetB.recalculate(left,top);
 myself->rightShiftB.recalculate(left,top);
   myself->aftShiftB.recalculate(left,top);
      myself->widthB.recalculate(left,top);
     myself->heightB.recalculate(left,top);
        myself->rotB.recalculate(left,top);
      myself->pitchB.recalculate(left,top);
       myself->tiltB.recalculate(left,top);

    myself->sittingL.recalculate(left,top);
     myself->offsetL.recalculate(left,top);
 myself->rightShiftL.recalculate(left,top);
   myself->aftShiftL.recalculate(left,top);
      myself->widthL.recalculate(left,top);
     myself->heightL.recalculate(left,top);
        myself->rotL.recalculate(left,top);
      myself->pitchL.recalculate(left,top);
       myself->tiltL.recalculate(left,top);

       DrawLogic::RelocateModalString(myself->sittingN,myself->sittingP);
       DrawLogic::RelocateModalString(myself->offsetN,myself->offsetP);
       DrawLogic::RelocateModalString(myself->rightShiftN,myself->rightShiftP);
       DrawLogic::RelocateModalString(myself->aftShiftN,myself->aftShiftP);
       DrawLogic::RelocateModalString(myself->widthN,myself->widthP);
       DrawLogic::RelocateModalString(myself->heightN,myself->heightP);
       DrawLogic::RelocateModalString(myself->rotN,myself->rotP);
       DrawLogic::RelocateModalString(myself->pitchN,myself->pitchP);
       DrawLogic::RelocateModalString(myself->tiltN,myself->tiltP);
       DrawLogic::RelocateModalString(myself->hsNameN,myself->hsNameP);
       DrawLogic::RelocateModalString(myself->hsBoxN,myself->hsBoxP);
       DrawLogic::RelocateModalString(myself->hsAdditionalN,myself->hsAdditionalP);

       ManageModalWindow::Recalculate(left,top);
       if (myself->activeLine!=nullptr){
           myself->cursor.Recalculate(myself->activeLine->GetX(),myself->activeLine->GetY());
       }
}

void advanced::DrawMyself(XPLMWindowID in_window_id, void * in_refcon){

    int lft(left),tp(top),right,bottom;
    XPLMGetWindowGeometry(in_window_id, &left, &top, &right, &bottom);
    if (lft!=left||tp!=top) RecalculateDialog();
    DrawLogic::DrawModalElements();
    if (myself->cursor.HasSelection()){
        int l,r;
        myself->cursor.IsIndexInSelection(0,l,r);
        myself->cursor.DrawRectangle(l,myself->activeLine->GetTop(),r,myself->activeLine->GetBottom());}
    DrawLogic::DrawModalStrings();
    if (myself->cursor.HasCursor()) myself->cursor.DrawCursor(myself->activeLine->GetY());
}

int advanced::MouseHandler(XPLMWindowID in_window_id, int x, int y, int is_down, void * unused){
    if (is_down==xplm_MouseDown){
        string selAct("");
        myself->mouseDrag=false;
        myself->epochClick=0;

        for (auto bt:myself->actionButtons){
            if (bt.button.isHere(x,y)){
                selAct=bt.ActionName;
                selectedB=bt.number;
                bt.button.Press();
                myself->ButtonAction(selAct,static_cast<ulong>(bt.number));
                return 1;
            }
        }
        if (myself->sittingB.isHere(x,y)){
            return 1;
        }
        if (myself->offsetB.isHere(x,y)){

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
        if (myself->rightShiftB.isHere(x,y)){
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
        if (myself->aftShiftB.isHere(x,y)){
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
        if (myself->widthB.isHere(x,y)){
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
        if (myself->heightB.isHere(x,y)){
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

        if (myself->rotB.isHere(x,y)){
            myself->SelectLine(&(myself->rotL),x,&myself->rot);
            myself->notallowed="-+.";
            myself->maxlength=3;
            myself->hasInterval=true;
            myself->minval=0;
            myself->maxval=359;
            myself->numberOfDigits=0;
            return 1;
        }

        if (myself->pitchB.isHere(x,y)){
            myself->SelectLine(&(myself->pitchL),x,&myself->pitch);
            myself->notallowed=".";
            myself->maxlength=3;
            myself->hasInterval=true;
            myself->minval=-90;
            myself->maxval=90;
            myself->numberOfDigits=0;
            return 1;
        }

        if (myself->tiltB.isHere(x,y)){
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
            myself->actionButtons[selectedB].button.Release();
            selectedB=0;
        }
        if (myself->endAlert) {myself->EndEdit();myself->callBack();return 1;}
    }
    return 1;
}

void advanced::ButtonAction(string actionName, ulong mbutton){
    int nod(1);
    if (currentUnit==0||currentUnit==3) nod=4;
    if (currentUnit==1||currentUnit==4) nod=2;
    actionSelected=actionName;
   if (actionName=="Cancel") endAlert=true;
   if (actionName=="Ok") {endAlert=true; Commit();}
   if (actionName=="Sitting"){sittingL.setText("SITTING");currentHsp.type="SITTING";}
   if (actionName=="Standing"){sittingL.setText(("STANDING"));currentHsp.type="STANDING";}
   if (actionName=="Raise50"){bBoxOffset+=gapincr;offsetL.setText(VRCReader::ConvertFloatToString(bBoxOffset,nod));}
   if (actionName=="Lower50"){bBoxOffset-=gapincr;offsetL.setText(VRCReader::ConvertFloatToString(bBoxOffset,nod));}
   if (actionName=="Right20"){bBoxLatShift+=latshiftincr;rightShiftL.setText(VRCReader::ConvertFloatToString(bBoxLatShift,nod));}
   if (actionName=="Left20"){bBoxLatShift-=latshiftincr;rightShiftL.setText(VRCReader::ConvertFloatToString(bBoxLatShift,nod));}
   if (actionName=="For20"){bBoxAxShift-=axshiftincr;aftShiftL.setText(VRCReader::ConvertFloatToString(bBoxAxShift,nod));}
   if (actionName=="Aft20"){bBoxAxShift+=axshiftincr;aftShiftL.setText(VRCReader::ConvertFloatToString(bBoxAxShift,nod));}
   if (actionName=="Width20"){bBoxWidth+=widthincr;widthL.setText(VRCReader::ConvertFloatToString(bBoxWidth,nod));}
   if (actionName=="Width-20"){bBoxWidth-=widthincr;if (bBoxWidth<0.1f) bBoxWidth=0.1f;widthL.setText(VRCReader::ConvertFloatToString(bBoxWidth,nod));}
   if (actionName=="Taller20"){bBoxheight+=heightincr;heightL.setText(VRCReader::ConvertFloatToString(bBoxheight,nod));}
   if (actionName=="Shorter20"){bBoxheight-=heightincr;if (bBoxheight<0.1f) bBoxheight=0.1f;heightL.setText(VRCReader::ConvertFloatToString(bBoxheight,nod));}
   if (actionName=="rot0")   {  rot=0;rotL.setText(VRCReader::ConvertFloatToString(rot,0));}
   if (actionName=="rot90")  { rot=90;rotL.setText(VRCReader::ConvertFloatToString(rot,0));}
   if (actionName=="rot180") {rot=180;rotL.setText(VRCReader::ConvertFloatToString(rot,0));}
   if (actionName=="rot270") {rot=270;rotL.setText(VRCReader::ConvertFloatToString(rot,0));}
   if (actionName=="pitch+5"){
       pitch+=pitchincr;
       if (pitch>90) pitch=90;
       pitchL.setText(VRCReader::ConvertFloatToString(pitch,0));}
   if (actionName=="pitch-5"){
       pitch-=pitchincr;
       if (pitch<-90) pitch=-90;
       pitchL.setText(VRCReader::ConvertFloatToString(pitch,0));}
   if (actionName=="tilt+2°"){tilt+=tiltincr;if (tilt>180) tilt=180;tiltL.setText(VRCReader::ConvertFloatToString(tilt,0));}
   if (actionName=="tilt-2°"){tilt-=tiltincr;if (tilt<-180) tilt=-180;tiltL.setText(VRCReader::ConvertFloatToString(tilt,0));}
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
        cursor.Initiate(&forcursor,activeLine->GetOffsetX(),activeLine->GetOffsetY(),10,1);
        cursor.SetCursorAt(0,0);
        cursor.Recalculate(activeLine->GetX(),activeLine->GetY());
    }
}

void advanced::CommitActiveLine(){
    if (activeLine!=nullptr){
        float val=VRCReader::ConvertStringToFloat(activeLine->GetText());
        if (hasInterval){
            if (val<minval) val=minval;
            if (val>maxval) val=maxval;
            *activeParameter=val;
            activeLine->setText(VRCReader::ConvertFloatToString(val,numberOfDigits));
        }
        else{
            *activeParameter=val;
            activeLine->setText(VRCReader::ConvertFloatToString(val,numberOfDigits));
        }
        cursor.EraseCursor();
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
        float toeval=VRCReader::ConvertStringToFloat(activeLine->GetText());
        if (hasInterval){
            if (toeval<minval) activeLine->setText(VRCReader::ConvertFloatToString(minval));
            if (toeval>maxval) activeLine->setText(VRCReader::ConvertFloatToString(maxval));
        }
        }
        if (keyName=="RIGHT") {MoveCursorRight();}
        if (keyName=="LEFT") {MoveCursorLeft();}

    }
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
        mb.button.setSelect(false);
    }
    actionButtons[in_button].button.setSelect(true);

    switch (new_unit){
    case 0:  {//meters
        gapincr=0.05f;
        widthincr=0.05f;
        heightincr=0.1f;
        latshiftincr=0.02f;
        axshiftincr=0.02f;
        //actionbuttons 3 à 12
        actionButtons[3].button.setText("+5cm");
        actionButtons[4].button.setText("-5cm");
        actionButtons[5].button.setText("+2cm(R)");
        actionButtons[6].button.setText("-2cm(L)");
        actionButtons[7].button.setText("-2cm(F)");
        actionButtons[8].button.setText("+2cm(B)");
        actionButtons[9].button.setText("+5cm");
        actionButtons[10].button.setText("-5cm");
        actionButtons[11].button.setText("+10cm");
        actionButtons[12].button.setText("-10cm");
        if (numberOfDigits>0) numberOfDigits=4;
        offsetL.setText(VRCReader::ConvertFloatToString(bBoxOffset));
        rightShiftL.setText(VRCReader::ConvertFloatToString(bBoxLatShift));
        aftShiftL.setText(VRCReader::ConvertFloatToString(bBoxAxShift));
        widthL.setText(VRCReader::ConvertFloatToString(bBoxWidth));
        heightL.setText(VRCReader::ConvertFloatToString(bBoxheight));
        break;
    }
    case 1:  {//centimeters
        gapincr=5;
        widthincr=5;
        heightincr=10;
        latshiftincr=2;
        axshiftincr=2;
        //actionbuttons 3 à 12
        actionButtons[3].button.setText("+5cm");
        actionButtons[4].button.setText("-5cm");
        actionButtons[5].button.setText("+2cm(R)");
        actionButtons[6].button.setText("-2cm(L)");
        actionButtons[7].button.setText("-2cm(F)");
        actionButtons[8].button.setText("+2cm(B)");
        actionButtons[9].button.setText("+5cm");
        actionButtons[10].button.setText("-5cm");
        actionButtons[11].button.setText("+10cm");
        actionButtons[12].button.setText("-10cm");
        if (numberOfDigits>0) numberOfDigits=2;
        offsetL.setText(VRCReader::ConvertFloatToString(bBoxOffset,2));
        rightShiftL.setText(VRCReader::ConvertFloatToString(bBoxLatShift,2));
        aftShiftL.setText(VRCReader::ConvertFloatToString(bBoxAxShift,2));
        widthL.setText(VRCReader::ConvertFloatToString(bBoxWidth,2));
        heightL.setText(VRCReader::ConvertFloatToString(bBoxheight,2));
        break;
    }
    case 2:  {//millimeters
        gapincr=50;
        widthincr=50;
        heightincr=100;
        latshiftincr=20;
        axshiftincr=20;
        //actionbuttons 3 à 12
        actionButtons[3].button.setText("+5cm");
        actionButtons[4].button.setText("-5cm");
        actionButtons[5].button.setText("+2cm(R)");
        actionButtons[6].button.setText("-2cm(L)");
        actionButtons[7].button.setText("-2cm(F)");
        actionButtons[8].button.setText("+2cm(B)");
        actionButtons[9].button.setText("+5cm");
        actionButtons[10].button.setText("-5cm");
        actionButtons[11].button.setText("+10cm");
        actionButtons[12].button.setText("-10cm");
        if (numberOfDigits>0) numberOfDigits=1;
        offsetL.setText(VRCReader::ConvertFloatToString(bBoxOffset,1));
        rightShiftL.setText(VRCReader::ConvertFloatToString(bBoxLatShift,1));
        aftShiftL.setText(VRCReader::ConvertFloatToString(bBoxAxShift,1));
        widthL.setText(VRCReader::ConvertFloatToString(bBoxWidth,1));
        heightL.setText(VRCReader::ConvertFloatToString(bBoxheight,1));
        break;
    }
    case 3:   {//feet
        gapincr     =2*0.0833333f;
        widthincr   =2*0.0833333f;
        heightincr  =4*0.0833333f;
        latshiftincr=0.0833333f;
        axshiftincr =0.0833333f;
        //actionbuttons 3 à 12
        actionButtons[3].button.setText("+2i");
        actionButtons[4].button.setText("-2i");
        actionButtons[5].button.setText("+1i(R)");
        actionButtons[6].button.setText("-1i(L)");
        actionButtons[7].button.setText("-1i(F)");
        actionButtons[8].button.setText("+1i(B)");
        actionButtons[9].button.setText("+2i");
        actionButtons[10].button.setText("-2i");
        actionButtons[11].button.setText("+4i");
        actionButtons[12].button.setText("-4i");
        if (numberOfDigits>0) numberOfDigits=4;
        offsetL.setText(VRCReader::ConvertFloatToString(bBoxOffset,4));
        rightShiftL.setText(VRCReader::ConvertFloatToString(bBoxLatShift,4));
        aftShiftL.setText(VRCReader::ConvertFloatToString(bBoxAxShift,4));
        widthL.setText(VRCReader::ConvertFloatToString(bBoxWidth,4));
        heightL.setText(VRCReader::ConvertFloatToString(bBoxheight,4));
        break;
    }
    case 4:  {//inches
        gapincr     =2;
        widthincr   =2;
        heightincr  =4;
        latshiftincr=1;
        axshiftincr =1;
        //actionbuttons 3 à 12
        actionButtons[3].button.setText("+2i");
        actionButtons[4].button.setText("-2i");
        actionButtons[5].button.setText("+1i(R)");
        actionButtons[6].button.setText("-1i(L)");
        actionButtons[7].button.setText("-1i(F)");
        actionButtons[8].button.setText("+1i(B)");
        actionButtons[9].button.setText("+2i");
        actionButtons[10].button.setText("-2i");
        actionButtons[11].button.setText("+4i");
        actionButtons[12].button.setText("-4i");
        if (numberOfDigits>0) numberOfDigits=2;
        offsetL.setText(VRCReader::ConvertFloatToString(bBoxOffset,2));
        rightShiftL.setText(VRCReader::ConvertFloatToString(bBoxLatShift,2));
        aftShiftL.setText(VRCReader::ConvertFloatToString(bBoxAxShift,2));
        widthL.setText(VRCReader::ConvertFloatToString(bBoxWidth,2));
        heightL.setText(VRCReader::ConvertFloatToString(bBoxheight,2));
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

