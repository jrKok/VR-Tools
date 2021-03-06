#include "scrollbars.h"
#include "drawlogic.h"

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
    general(false),
    core(0,0,0,0,Clr_Gray,false),
    lift(0,0,0,0,Clr_White,false),
    commandUp("scroll up",modal),
    commandDown("scroll down",modal)

{}

void ScrollBars::Setup(int height,int totLines, int firstLine, int linesInPage,int offX, int offY){

   totalheight=height;
   totalLines=totLines;
   page=linesInPage;
   core.SetDebugName("scrollb core");
   lift.SetDebugName("scrollb lift");

   if ((totLines>0)&&(totalLines>=page)){
       commandUp.resetMe();
       commandDown.resetMe();
       limitLastLine=totalLines-page;
       heightOfCore=totalheight-30;

       general.SetDimensions(15,height);
       general.SetOrigin(offX,offY);
       core.SetDimensions(15,heightOfCore);
       core.SetOrigin(offX,offY+15);

       commandUp.SetOrigin(offX, offY+totalheight-15);
       commandUp.SetDimensions(15,15);
       commandUp.setText("\xE2\x96\xB2");//UP pointing e296b2 try also e28fb6
       commandUp.SetTextOffsets(4,4);
       commandUp.setButtonColor(Clr_LightGray);
       commandUp.setTextColor(Clr_BlackInk);

       commandDown.SetOrigin(offX,offY);
       commandDown.SetDimensions(15,15);
       commandDown.setText("\xE2\x96\xBC");//e296bc try also e28fb7
       commandDown.SetTextOffsets(4,5);
       commandDown.setButtonColor(Clr_LightGray);
       commandDown.setTextColor(Clr_BlackInk);

       lowCore=offY+15;
       topCore=lowCore+heightOfCore-15;

       if ((heightOfCore-15)<limitLastLine){//if there are more lines then vertical pixels in core then proportionnalise the core
          heightOfLift=15;
          lift.SetDimensions(15,15);
          linesPPx=static_cast<float>(limitLastLine)/static_cast<float>(heightOfCore-15);
        }
        else{
          topCore=lowCore+limitLastLine;
          lift.SetDimensions(15,heightOfCore-limitLastLine);
          heightOfLift=heightOfCore-limitLastLine;
          linesPPx=1;
        }

        lift.SetOrigin(offX,15);
        SetPosFirstLine(firstLine);
        SetVisibility(true);
   }
   else{//no extra lines are to be displayed, no initialisation occured
       SetVisibility(false);
   }
}

bool ScrollBars::IsCommandForMe(int x, int y, int & retVal){
    if (general.IsHere(x,y)){
        if (commandUp.IsHere(x,y)) {
            retVal=LineUp;
            LineUpNLines (1);
            BeginRepeat(B_Line_Up);
            return true;
        }
        if (commandDown.IsHere(x,y)){
            retVal=LineDown;
            LineDownNLines(1);
            BeginRepeat(B_Line_Down);
            return true;
        }
        if (core.IsHere(x,y)){
            if (lift.IsHere(x,y)){
                retVal=numberfirstLine;
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

int  ScrollBars::DragY(int in_y){
    if (drag) {
        int dy=dragPosY-in_y; //get variation of position
        int newY=y0-dy; //convert into new lift bottom position
        //limit variation of offset into bounds
        if (newY>=topCore) {
            numberfirstLine=0;
            lift.SetOrigin(lift.GetLeft(),topCore);
            core.UpdateMyTexture();
            lift.UpdateMyTexture();
            return numberfirstLine;
            }
        if (newY<=lowCore) {
            numberfirstLine=limitLastLine;
            lift.SetOrigin(lift.GetLeft(),lowCore);
            core.UpdateMyTexture();
            lift.UpdateMyTexture();
            return numberfirstLine;}
        //if in bounds convert pixels to line number
        numberfirstLine=static_cast<int>((topCore-newY)*linesPPx);
        lift.SetOrigin(lift.GetLeft(),newY);
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
    //Lift bottom flucuates between topcore and lowcore
    //This function calculates position of lift WRT firstLine
    //with special positions of firstLine=0 and firstLine between possible position of last line and total number of lines

   if (firstLine<=0) {
        numberfirstLine=0;
        lift.SetOrigin(lift.GetLeft(),topCore);
   }else{
      if (firstLine>=limitLastLine){
         numberfirstLine=limitLastLine;
         lift.SetOrigin(lift.GetLeft(),lowCore);
      }
      else{
         if (linesPPx==1.0f){
            numberfirstLine=firstLine;
            lift.SetOrigin(lift.GetLeft(),topCore-numberfirstLine);
         }else{
            numberfirstLine=firstLine;
            lift.SetOrigin(lift.GetLeft(),topCore-static_cast<int>((numberfirstLine)/linesPPx));
         }
      }
   }
   core.UpdateMyTexture();
   lift.UpdateMyTexture();
}

int ScrollBars::GetPosFirstLine(){
    return numberfirstLine;
}

void ScrollBars::SetVisibility(bool iV){
    general.SetVisibility(iV);
    core.SetVisibility(iV);
    lift.SetVisibility(iV);
    commandDown.setVisibility(iV);
    commandUp.setVisibility(iV);
    isVisible=iV;
    if (isVisible){
        core.UpdateMyTexture();
        lift.UpdateMyTexture();
    }
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

char ScrollBars::GetColorCodeFromSymbol(){
    return commandUp.GetStringColorCode();
}
