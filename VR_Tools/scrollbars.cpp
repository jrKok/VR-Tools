#include "scrollbars.h"

/* this class implements a scroll bar with 2 commands, a lift or thumb cursor,
 * based on text widgets,
 * with a base unit of 1 line
 * processes coordinates of a click
 * ready for drag commands
*/

ScrollBars::ScrollBars():
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
    linesPPx(1.0f),
    epoch(0.0f),
    waitForRepeat(0.7f),
    cmdToRepeat(0),
    isVisible(false),
    drag(false),
    repeatCmd(false),
    commandUp(),
    commandDown(),
    lift(),
    general(),
    core(),
    colorWhite{0.950f,0.950f,0.950f},
    colorGray{0.600f,0.600f,0.600f},
    colorlightGray{0.750f,0.750f,0.750f}
{

}
void ScrollBars::WriteDebug(std::string message){
    std::string in_String="VR Tools : " +message+"\n";
    XPLMDebugString((char*)in_String.c_str());
}

void ScrollBars::Setup(int height,int totLines, int firstLine, int linesInPage,int offX, int offY){
    //define fixed elements
    commandUp.resetMe();
    commandDown.resetMe();
    totalheight=height;
    totalLines=totLines;
    page=linesInPage;
    limitLastLine=totalLines-page;
    heightOfCore=totalheight-30;

    general.height=height;
    general.width=15;
    general.offsetX=offX;
    general.offsetY=offY;

    topCore=offY+15;

    commandUp.offsetX=offX;
    commandUp.offsetY=offY;
    commandUp.height=15;
    commandUp.width=15;
    commandUp.hasSymbol=true;
    commandUp.buttonText="";
    commandUp.setColor(colorlightGray);
    commandUp.addPoint(offX+7,offY+2);
    commandUp.addPoint(offX+13,offY+13);
    commandUp.addPoint(offX+2,offY+13);

    commandDown.offsetX=offX;
    commandDown.offsetY=offY+totalheight-15;
    commandDown.height=15;
    commandDown.width=15;
    commandDown.hasSymbol=true;
    commandDown.buttonText="";
    commandDown.setColor(colorlightGray);
    commandDown.addPoint(offX+2,commandDown.offsetY+2);
    commandDown.addPoint(offX+13,commandDown.offsetY+2);
    commandDown.addPoint(offX+7,commandDown.offsetY+13);

    core.height=heightOfCore;
    core.width=15;
    core.offsetX=offX;
    core.offsetY=topCore;

    //define mobile element (lift, thumb or what you like)
    lift.width=15;
    lift.offsetX=offX;

    if (totalLines<=page) {
        lift.height=heightOfCore;
        heightOfLift=heightOfCore;
        this->isVisible=false;
    }else{
    this->isVisible=true;
    lowCore=topCore+heightOfCore-15;
    freeCore=lowCore-topCore;
    if (lowCore<(limitLastLine)){
        lift.height=15;
        freeCore=lowCore-topCore;
        linesPPx=(float)(limitLastLine)/(float)freeCore;
    }
    else{
        lowCore=topCore+limitLastLine;
        freeCore=lowCore-topCore;
        lift.height=heightOfCore-limitLastLine;
        linesPPx=1;
    }}
    SetPosFirstLine(firstLine);
    //compute lift height and params, its initial position at the top
    //compute linesPPx
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
                if (y>=lift.top){
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
    y0=lift.offsetY;
    drag=true;
}

int  ScrollBars::DragY(int y){
    if (drag) {
        int diff=dragPosY-y; //get variation of position
        int dy=y0+diff; //convert into variation of offset
        //limit variation of offset into bounds
        if (dy<=topCore) {
            numberfirstLine=0;
            lift.offsetY=topCore;
            lift.recalculate();
            return numberfirstLine;
            }
        if (dy>=lowCore) {
            numberfirstLine=limitLastLine;
            lift.offsetY=lowCore;
            lift.recalculate();
            return numberfirstLine;}
        //if in bounds convert pixels to line number
        numberfirstLine=(dy-topCore)*linesPPx;
        lift.offsetY=dy;
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
            waitForRepeat=0.2;
            return true;
        }
        if (cmdToRepeat==B_Line_Down){
            LineDownNLines(1);
            waitForRepeat=0.2;
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
    waitForRepeat=0.7;
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
        lift.offsetY=topCore;
        lift.recalculate();
    }else{
    if (firstLine>=limitLastLine){
        numberfirstLine=limitLastLine;
        lift.offsetY=lowCore;
        lift.recalculate();}
    else{
        if (linesPPx==1){
            numberfirstLine=firstLine;
            lift.offsetY=topCore+numberfirstLine;
            lift.recalculate();
        }else{
            numberfirstLine=firstLine;
            lift.offsetY=topCore+(int)(numberfirstLine*freeCore/limitLastLine);
            lift.recalculate();
        }
    }}
}

int ScrollBars::GetPosFirstLine(){
    return numberfirstLine;
}

void ScrollBars::Refresh(int height, int totLines, int firstLine, int linesOnPage){
    //core of computation
}

void ScrollBars::Recalculate(int left, int top){
    general.in_left=left;    general.in_top=top;    general.recalculate();
    core.in_left=left;       core.in_top=top;       core.recalculate();
    lift.in_left=left;       lift.in_top=top;       lift.recalculate();
    commandUp.in_left=left;  commandUp.in_top=top;  commandUp.recalculate();
    commandDown.in_left=left;commandDown.in_top=top;commandDown.recalculate();
}
void ScrollBars::SetVisibility(bool iV){
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



void ScrollBars::DrawMySelf(){
    if (isVisible){
    //draw UpperButton
    commandUp.drawButton();
    //draw Core
    glColor3fv(colorGray);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glBegin(GL_TRIANGLE_FAN);
      {
      glVertex2i(core.left, core.top);
      glVertex2i(core.right, core.top);
      glVertex2i(core.right, core.bottom);
      glVertex2i(core.left, core.bottom);
      }
    glEnd();

    //draw Lift
    glColor3fv(colorWhite);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glBegin(GL_TRIANGLE_FAN);
      {
      glVertex2i(lift.left, lift.top);
      glVertex2i(lift.right,lift.top);
      glVertex2i(lift.right, lift.bottom);
      glVertex2i(lift.left, lift.bottom);
      }
    glEnd();
    //draw DownButton
    commandDown.drawButton();
   }
}
