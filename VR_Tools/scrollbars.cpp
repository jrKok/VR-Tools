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
    topCore(0),lowCore(0),
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
    general(0,0,0,0,Clr_White,false),
    core(0,0,0,0,Clr_White,false),
    lift(0,0,0,0,Clr_White,false),
    commandUp(modal),
    commandDown(modal)

{}

void ScrollBars::WriteDebug(std::string message){
    std::string in_String="VR Tools : " +message+"\n";
    XPLMDebugString((char*)in_String.c_str());
}

void ScrollBars::Setup(int height,int totLines, int firstLine, int linesInPage,int offX, int offY){
    //define fixed elements
   if (totLines>0&&totalLines>=page){

       this->isVisible=true;
       commandUp.resetMe();
       commandDown.resetMe();
       totalheight=height;
       totalLines=totLines;
       page=linesInPage;
       limitLastLine=totalLines-page;
       heightOfCore=totalheight-30;

       general.SetDimensions(15,height);
       general.SetOrigin(offX,offY);
       core.SetDimensions(15,heightOfCore);
       core.SetOrigin(offX,offY+15);

       commandUp.SetOrigin(offX, offY+totalheight-15);
       commandUp.SetDimensions(15,15);
       commandUp.setText("\xE2\x96\xB2");//UP pointing e296b2 try also e28fb6
       commandUp.setButtonColor(Clr_LightGray);
       commandUp.setTextColor(Clr_BlackInk);

       commandDown.SetOrigin(offX,offY);
       commandDown.SetDimensions(15,15);
       commandDown.setText("\xE2\x96\xBC");//e296bc trys also e28fb7
       commandDown.setButtonColor(Clr_LightGray);
       commandDown.setTextColor(Clr_BlackInk);

       lowCore=offY+15;
       topCore=lowCore+heightOfCore-15;

       if (topCore<limitLastLine){//if there are more lines then vertical pixels in core then proportionnalise the core
          heightOfLift=15;
          linesPPx=static_cast<float>(limitLastLine)/static_cast<float>(topCore);
        }
        else{
          topCore=lowCore+limitLastLine;
          lift.SetDimensions(15,heightOfCore-limitLastLine);
          heightOfLift=heightOfCore-limitLastLine;
          linesPPx=1;
        }

        lift.SetOrigin(offX,15);
        SetPosFirstLine(firstLine);
        core.setColor(Clr_Gray);
        lift.setColor(Clr_White);
        SetVisibility(isVisible);
   }
   else{//no lines are to be displayed, no initialisation occured
       this->isVisible=false;

   }
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
                BeginDrag(y);
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

void ScrollBars::BeginDrag(int y){
    dragPosY=y;
    y0=lift.GetBottom();
    drag=true;
}

int  ScrollBars::DragY(int y){
    if (drag) {
        int diff=y-dragPosY; //get variation of position
        int dy=y0+diff; //convert into variation of offset
        //limit variation of offset into bounds
        if (dy>=topCore) {
            numberfirstLine=0;
            lift.SetOrigin(lift.GetLeft(),topCore);
            core.UpdateMyTexture();
            lift.UpdateMyTexture();
            return numberfirstLine;
            }
        if (dy<=lowCore) {
            numberfirstLine=limitLastLine;
            lift.SetOrigin(lift.GetLeft(),lowCore);
            core.UpdateMyTexture();
            lift.UpdateMyTexture();
            return numberfirstLine;}
        //if in bounds convert pixels to line number
        numberfirstLine=(topCore-dy)*static_cast<int>(linesPPx);
        lift.SetOrigin(lift.GetLeft(),dy);
        core.UpdateMyTexture();
        lift.UpdateMyTexture();
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
    //Lift bottom flucuates between core.top and (core.low)
    //This function calculates position of lift WRT firstLine
    //with special positions of firstLine=0 and firstLine between possible position of last line and total number of lines

    if (firstLine<=0) {
        numberfirstLine=0;
        lift.SetOrigin(lift.GetLeft(),topCore);
        core.UpdateMyTexture();
        lift.UpdateMyTexture();
    }else{
    if (firstLine>=limitLastLine){
        numberfirstLine=limitLastLine;
        lift.SetOrigin(lift.GetLeft(),lowCore);
        core.UpdateMyTexture();
        lift.UpdateMyTexture();}
    else{
        if (linesPPx==1.0f){
            numberfirstLine=firstLine;
            lift.SetOrigin(lift.GetLeft(),topCore-numberfirstLine);
            core.UpdateMyTexture();
            lift.UpdateMyTexture();
        }else{
            numberfirstLine=firstLine;
            lift.SetOrigin(lift.GetLeft(),topCore-numberfirstLine*(topCore-lowCore)/limitLastLine);
        }
    }}
}

int ScrollBars::GetPosFirstLine(){
    return numberfirstLine;
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

