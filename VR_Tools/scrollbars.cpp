#include "scrollbars.h"

/* this class implements a scroll bar with 2 commands, a lift or thumb cursor,
 * based on text widgets,
 * with a base unit of 1 line
 * processes coordinates of a click
 * ready for drag commands
*/

ScrollBars::ScrollBars(bool modal):
    totalLines(0),
    page(0),
    numberfirstLine(0),limitLastLine(0),
    totalheight(0),
    topCore(0),lowCore(0),freeCore(0),
    heightOfCore(0),
    heightOfLift(0),
    posOfLift(0),
    offsetX(0),
    offsetY(0),
    y0(0),
    dragPosY(0),
    cmdToRepeat(0),
    linesPPx(1.0f),
    epoch(0.0f),
    waitForRepeat(0.7f),
    isVisible(false),
    drag(false),
    repeatCmd(false),
    general(true,modal),
    core(true,modal),
    commandUp(modal),
    commandDown(modal),
    lift(modal)
{}

void ScrollBars::WriteDebug(std::string message){
    std::string in_String="VR Tools : " +message+"\n";
    XPLMDebugString((char*)in_String.c_str());
}

void ScrollBars::Setup(int height,int totLines, int firstLine, int linesInPage,int offX, int offY){
    //define fixed elements
   if (totLines>0){
    commandUp.resetMe();
    commandDown.resetMe();
    totalheight=height;
    totalLines=totLines;
    page=linesInPage;
    limitLastLine=totalLines-page;
    heightOfCore=totalheight-30;
    general.SetDimensions(15,height);
    general.SetOffsets(offX,offY);
    topCore=offY+15;

    commandUp.SetOffsets(offX,offY);
    commandUp.SetDimensions(15,15);
    commandUp.setText("");
    commandUp.addPoint(offX+7,offY+2);
    commandUp.addPoint(offX+13,offY+13);
    commandUp.addPoint(offX+2,offY+13);
    commandUp.setButtonColor(Clr_LightGray);

    commandDown.SetOffsets(offX,offY+totalheight-15);
    commandDown.SetDimensions(15,15);
    commandDown.setText("");
    commandDown.setButtonColor(Clr_LightGray);
    commandDown.addPoint(offX+2,commandDown.GetOffsetY()+2);
    commandDown.addPoint(offX+13,commandDown.GetOffsetY()+2);
    commandDown.addPoint(offX+7,commandDown.GetOffsetY()+13);

    core.SetDimensions(15,heightOfCore);
    core.SetOffsets(offX,topCore);
    lift.SetOffsets(offX,offY);

    if (totalLines<=page) {
        lift.SetDimensions(15,heightOfCore);
        heightOfLift=heightOfCore;
        this->isVisible=false;
    }else{
    this->isVisible=true;
    lowCore=topCore+heightOfCore-15;
    freeCore=lowCore-topCore;
    if (lowCore<(limitLastLine)){
        lift.SetDimensions(15,15);
        heightOfLift=15;
        freeCore=lowCore-topCore;
        linesPPx=(float)(limitLastLine)/(float)freeCore;
    }
    else{
        lowCore=topCore+limitLastLine;
        lift.SetDimensions(15,heightOfCore-limitLastLine);
        freeCore=lowCore-topCore;
        heightOfLift=heightOfCore-limitLastLine;
        linesPPx=1;
    }}
    SetPosFirstLine(firstLine);
   }
   else{
       this->isVisible=false;
   }
    core.setColor(Clr_Gray);
    lift.setButtonColor(Clr_White);
    SetVisibility(isVisible);
}

bool ScrollBars::IsCommandForMe(int x, int y, int & retVal){
    if (general.isHere(x,y)){
        if (commandUp.isHere(x,y)) {
            retVal=LineUp;
            LineUpNLines (1);
            BeginRepeat(B_Line_Up);
            return true;
        }
        if (commandDown.isHere(x,y)){
            retVal=LineDown;
            LineDownNLines(1);
            BeginRepeat(B_Line_Down);
            return true;
        }
        if (core.isHere(x,y)){
            if (lift.isHere(x,y)){
                retVal=y;
                BeginDrag(x,y);
                return true;
            }
            else {
                if (y>=lift.GetTop()){
                    retVal=PageUp;
                    UpPage();
                    BeginRepeat(B_Page_Up);
                    return true;
                }else{
                    retVal=PageDown;
                    DownPage();
                    BeginRepeat(B_Page_Down);
                    return true;
                }
            }
        }
    }
    return false;
}

bool ScrollBars::IsVisible(){

    return isVisible;
}

void ScrollBars::BeginDrag(int x,int y){
    dragPosY=y;
    y0=lift.GetOffsetY();
    drag=true;
}

int  ScrollBars::DragY(int y){
    if (drag) {
        int diff=dragPosY-y; //get variation of position
        int dy=y0+diff; //convert into variation of offset
        //limit variation of offset into bounds
        if (dy<=topCore) {
            numberfirstLine=0;
            lift.SetOffsetY(topCore);
            lift.recalculate();
            return numberfirstLine;
            }
        if (dy>=lowCore) {
            numberfirstLine=limitLastLine;
            lift.SetOffsetY(lowCore);
            lift.recalculate();
            return numberfirstLine;}
        //if in bounds convert pixels to line number
        numberfirstLine=(dy-topCore)*static_cast<int>(linesPPx);
        lift.SetOffsetY(dy);
        lift.recalculate();
        return numberfirstLine;
        }
    return -1;//nothing happened
}

bool ScrollBars::OngoingDrag(){
    return drag;
}

void ScrollBars::EndDrag(){
    drag=false;dragPosY=0;
    y0=0;
}

void ScrollBars::BeginRepeat(int cmd){
    cmdToRepeat=cmd;
    repeatCmd=true;
    epoch=XPLMGetElapsedTime();
}

bool ScrollBars::ShouldRepeat(){
    float now=XPLMGetElapsedTime();
    if ((now-epoch)>waitForRepeat){
        epoch=now;
        if (cmdToRepeat==B_Line_Up){
            LineUpNLines(1);
            waitForRepeat=0.2f;
            return true;
        }
        if (cmdToRepeat==B_Line_Down){
            LineDownNLines(1);
            waitForRepeat=0.2f;
            return true;
        }
        if (cmdToRepeat==B_Page_Down){
            DownPage();
            return true;
        }
        if (cmdToRepeat==B_Page_Up){
            UpPage();
            return true;
        }

    }
    return false;
}

void ScrollBars::EndRepeat(){
    repeatCmd=false;
    cmdToRepeat=-1;
    epoch=0;
    waitForRepeat=0.7f;
}

bool ScrollBars::OngoingRepeat(){
    return repeatCmd;
}


void ScrollBars::SetPosFirstLine(int firstLine){
    //Name FirstLine means first Line displayed, should not be greater than totalLines-page
    //Lift top flucuates between core.top and (core.low)
    //This function calculates position of lift WRT firstLine
    //with special positions of firstLine=0 and firstLine between possible position of last line and total number of lines

    if (firstLine<=0) {
        numberfirstLine=0;
        lift.SetOffsetY(topCore);
        lift.recalculate();
    }else{
    if (firstLine>=limitLastLine){
        numberfirstLine=limitLastLine;
        lift.SetOffsetY(lowCore);
        lift.recalculate();}
    else{
        if (linesPPx==1.0f){
            numberfirstLine=firstLine;
            lift.SetOffsetY(topCore+numberfirstLine);

            lift.recalculate();
        }else{
            numberfirstLine=firstLine;
            lift.SetOffsetY(topCore+(numberfirstLine*freeCore/limitLastLine));
            lift.recalculate();
        }
    }}
}

int ScrollBars::GetPosFirstLine(){
    return numberfirstLine;
}

void ScrollBars::Recalculate(int left, int top){
    general.SetOrigin(left,top);
    core.SetOrigin(left,top);
    lift.SetOrigin(left,top);
    commandUp.SetOrigin(left,top);
    commandDown.SetOrigin(left,top);
    general.recalculate();
    core.recalculate();
    lift.recalculate();
    commandUp.recalculate();
    commandDown.recalculate();
}

void ScrollBars::SetVisibility(bool iV){
    general.setVisibility(iV);
    lift.setVisibility(iV);
    core.setVisibility(iV);
    commandDown.setVisibility(iV);
    commandUp.setVisibility(iV);
    isVisible=iV;

}

void ScrollBars::LineUpNLines(int nL){
    numberfirstLine-=nL;
    if (numberfirstLine<0) numberfirstLine=0;
    SetPosFirstLine(numberfirstLine);
}

void ScrollBars::LineDownNLines (int nL){
    numberfirstLine+=nL;
    if (numberfirstLine>limitLastLine) numberfirstLine=limitLastLine;
    SetPosFirstLine((numberfirstLine));
}
void ScrollBars::UpPage(){
    LineUpNLines(page);
}
void ScrollBars::DownPage(){
    LineDownNLines(page);
}

