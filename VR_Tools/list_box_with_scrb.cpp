#include "list_box_with_scrb.h"
#include "fontman.h"
#include "drawlogic.h"

List_Box_With_ScrB::List_Box_With_ScrB(bool modal):
     inkColor(Clr_BlackInk),
     in_left(0),
     grlOffsetX(0),
     grlOffsetY(0),
     charHeight(0),
     maxStringWidth(0),
     pageHeightInL(0),   
     totalNbL(0),
     indxFirstOnPg(0),
     indxLastOnPg(0),
     indxLastPage(0),
     lineLength(0),
     lineSelected(-1),
     lastLineDeleted(-1),
     antepLineDeleted(-1),
     heightPx(0),//height of the usefull text zone in pixels
     widthPx(0), //width of the usefull text zone in pixels
     clickPosX(0),
     clickPosY(0),
     currentIndx(-1),
     currentIndxFP(-1),
     splitPolicy(0),
     hasHiddenLines(false),
     hasSelection(false),
     dragLines(false),
     delLines(false),
     needToContClick(false),
     canUndo(false),
     filterClick(true),
     canDelete(true),
     needDisplay(false),
     isModal(modal),
     scrB(modal),
     general("lists general",false),
     textOnly("lists textOnly",true),
     displayText(new std::vector<string>),
     box(),
     delStr1(""),
     delStr2("")
{
}

List_Box_With_ScrB::~List_Box_With_ScrB(){
    if (displayText) {
        displayText->clear();
        delete displayText;}
    displayText=nullptr;
}

void List_Box_With_ScrB::Setup (int hght,int larg,int in_offsetX,int in_offsetY){

    textOnly.SetVisibility(true);
    grlOffsetX=in_offsetX;
    grlOffsetY=in_offsetY;
    heightPx=hght;if (heightPx<120) heightPx=120;if (heightPx>900) heightPx=900;
    widthPx=larg;if (widthPx<230) widthPx=230;if (widthPx>1500) widthPx=1500;
    general.SetDimensions(widthPx,heightPx);
    general.SetOrigin(grlOffsetX,grlOffsetY);
    textOnly.SetDimensions(widthPx-15,heightPx);
    textOnly.SetOrigin(grlOffsetX,grlOffsetY);
    lineLength=textOnly.GetWidth()-25;
}

void List_Box_With_ScrB::SetupforText(){
    XPLMGetFontDimensions(xplmFont_Proportional,nullptr,&charHeight,nullptr);
    DeleteDisplayBox();
     pageHeightInL=int((heightPx-20)/(charHeight+2));
     for (int lg(0);lg<pageHeightInL;lg++){
         AddLineToDisplayBox(lg);
     }
    totalNbL=static_cast<int>(displayText->size());
    if (totalNbL==0){
        scrB.Setup(heightPx,totalNbL,indxFirstOnPg,pageHeightInL,textOnly.GetWidth()+grlOffsetX,grlOffsetY);
        AddLine("The file doesn't contain any text");
        totalNbL=1;
    }

    indxLastPage=totalNbL-pageHeightInL;
    if (indxLastPage<0) indxLastPage=0;
    scrB.Setup(heightPx,totalNbL,indxFirstOnPg,pageHeightInL,textOnly.GetWidth()+grlOffsetX,grlOffsetY);
    if (indxFirstOnPg>indxLastPage) indxFirstOnPg=indxLastPage;//I don't redefine indxFirstOnPage except if the display has shrunken
    indxLastOnPg=indxFirstOnPg+pageHeightInL;
    if (indxLastOnPg>=totalNbL) indxLastOnPg=totalNbL-1;
    delStr1="";
    delStr2="";
    lastLineDeleted=-1;
    antepLineDeleted=-1;
    canUndo=false;
    DisplayPage();
}

void List_Box_With_ScrB::DeleteDisplayBox(){
    for (auto &line:box)
        line.DeleteLine();
    box.clear();
}

void List_Box_With_ScrB::AddLineToDisplayBox(int lnNumber){
    TextLine nTL(isModal);
    nTL.SetDimensions(textOnly.GetWidth()-10,charHeight);
    nTL.SetOrigin(textOnly.GetLeft()+5,textOnly.GetTop()-15-lnNumber*(charHeight+2));
    nTL.SetIndex(lnNumber);
    nTL.SetTextColor(inkColor);
    nTL.SetBackGroundColor(textOnly.GetColor());
    box.push_back(nTL);
}

void List_Box_With_ScrB::AddLine(string in_Line){

    int sz=MeasureString(in_Line);
    if (sz>maxStringWidth) maxStringWidth=sz;
    //split line according to rules
    switch (splitPolicy){
    case BestSplitAtSpace:   {
        while (sz>lineLength){
            string leftString=strops.bestLeftSize(in_Line,lineLength);
            displayText->push_back(leftString);
            sz=MeasureString(in_Line);
            totalNbL++;
        }
        displayText->push_back(in_Line);
        totalNbL++;
        break;
    }

    case ForceSplitAtSize:{
        while (sz>lineLength){
            string leftString=strops.splitAtSize(in_Line,lineLength);
            displayText->push_back(leftString);
            sz=MeasureString(in_Line);
            totalNbL++;
        }
        displayText->push_back(in_Line);
        totalNbL++;
        break;
    }

    case TruncateKeepLeft:{
        string leftString=in_Line;
        if (sz>lineLength){
           leftString=strops.splitAtSize(in_Line,lineLength);
        }
        displayText->push_back(leftString);
        totalNbL++;
        break;
    }

    case DontTruncate:{//don't use, gives ugly results by overlaying lines
        displayText->push_back(in_Line);
        totalNbL++;
        break;
    }

    case TruncateKeepRight:{
        string rightStr=in_Line;
        if (sz>lineLength){
            rightStr=strops.splitRightAtSize(in_Line,lineLength);
        }
        displayText->push_back(rightStr);
        totalNbL++;
        break;
    }
    }

}

void List_Box_With_ScrB::SetBackGround(bool opt){
    needDisplay=true;
    if (textOnly.IsVisible()!=opt)
    {
        textOnly.SetVisibility(opt);
        if (opt)
        {
            for (auto &bx:box)
            {
                bx.SetBackGroundColor(textOnly.GetColor());
            }
        }
    }
}

void List_Box_With_ScrB::SetBckColor (char in_Color){
    textOnly.SetColor(in_Color);
    for (auto &bx:box)
    {
        bx.SetBackGroundColor(in_Color);
    }
    needDisplay=true;
}

void List_Box_With_ScrB::SetInkColor (char in_Color){
    needDisplay=true;
    inkColor=in_Color;
    textOnly.UpdateMyTexture();
    for (auto &bx:box)
    {
        if (bx.GetIndex()==lineSelected&&hasSelection){
            bx.SetTextColor(Clr_InkSelect);
        }
        else{
            bx.SetTextColor(in_Color);
        }
    }
}

bool List_Box_With_ScrB::ProceedClick(int x, int y){

    //find location
    if (general.IsHere(x,y)){
        //which element
        int retV(0);
        if (scrB.IsCommandForMe(x,y,retV)){//scrollBox ?
            DisplayAtLineN(scrB.GetPosFirstLine());
            needToContClick=true;//filterClick=false;//remove filter click when bug corrected
            return true;
        }
        for (int ln(0);ln<pageHeightInL;ln++){//TextLine ?
            if (box[ln].isHere(x,y)){

                currentIndxFP=indxFirstOnPg;
                currentIndx=ln+indxFirstOnPg;
                clickPosX=x;
                clickPosY=y;
                if (currentIndx>indxLastOnPg){
                    needToContClick=false;
                    return false;
                }
                else{
                    needToContClick=true;
                    return true;
                }
            }
        }
    }
    else needToContClick=false;
    return false;
}

void List_Box_With_ScrB::ProceedClickCont(int x, int y){
    if (needToContClick){//&filterClick){

        if (scrB.OngoingDrag()) {//process for scrB ?
            DisplayAtLineN(scrB.DragY(y));
            return;
        }
        if (scrB.OngoingRepeat()){
            scrB.ShouldRepeat();
            DisplayAtLineN(scrB.GetPosFirstLine());
            PrintMyText();
            return;
        }

       if (currentIndx!=-1){
          int diffY=y-clickPosY;
          if (dragLines){ //if dragLines is activated, it cannot become a delete command, so only look at vertical movement
             int nbLns=int(diffY/(charHeight+2));
             if (nbLns!=0) {
                DisplayAtLineN(currentIndxFP+nbLns);
                scrB.SetPosFirstLine(indxFirstOnPg);
                PrintMyText();
                return;
             }
          }else{ //determine if there is some significant movement along x or y
             int diffX=clickPosX-x;
             int nbLns=int(diffY/(charHeight+2));
             if (nbLns!=0){ //if y movement is significant drag becomes an irreversible drag along y axis
                dragLines=true;
                delLines=false;//you cannot both drag the display and delete a line
                DisplayAtLineN(currentIndxFP+nbLns);
                PrintMyText();
                return;
             }
             if ((diffX>30)|(diffX<-30)){ //only if movement along x is significant (a horizontal swipe) a delete will be generated
                 delLines=true;
             }
          }
       }
    }
}

void List_Box_With_ScrB::ProceedEndClick(){//Mouse Up

    if ((currentIndx>=0)&&(currentIndx<totalNbL)){//only do delete and select for valid data refs
        if (delLines) DeleteLine(currentIndx); //if the line has been swiped, regardless of selection, remove, can be undone
        if ((currentIndx>=0)&&(!dragLines)) SelectLine(currentIndx);
    }
    //whatever happened reset Variables for clickprocessing, then discharge scrB
    currentIndx=-1;
    currentIndxFP=-1;
    clickPosX=0;
    clickPosY=0;
    needToContClick=false;
    dragLines=false;
    delLines=false;
    scrB.EndDrag();
    scrB.EndRepeat();
}

void List_Box_With_ScrB::MoveUpNLines(int uL){
    DisplayAtLineN(indxFirstOnPg-uL);
    DisplayPage();
    scrB.SetPosFirstLine(indxFirstOnPg);
}
void List_Box_With_ScrB::MoveDnNLines(int dL){
    DisplayAtLineN(indxFirstOnPg+dL);
    DisplayPage();
    scrB.SetPosFirstLine(indxFirstOnPg);
}

void List_Box_With_ScrB::MoveUpPage(){
    MoveUpNLines(pageHeightInL);
}

void List_Box_With_ScrB::MoveDnPage(){
    MoveDnNLines(pageHeightInL);
}

void List_Box_With_ScrB::ScrollLineDn(){
    lineSelected++;
    if (lineSelected>=totalNbL) {lineSelected--;return;}
    if (lineSelected>=indxLastOnPg){MoveDnNLines(1);return;}
    DisplayPage();
}

void List_Box_With_ScrB::ScrollLineUp(){
    lineSelected--;
    if (lineSelected>=totalNbL) {lineSelected++;return;}
    if (lineSelected<=indxFirstOnPg){MoveUpNLines(1);return;}
    DisplayPage();
}

void List_Box_With_ScrB::DisplayAtLineN(int in_ln){//doesn't synchronize with scrB, because can be called to synchronize with scrB
    indxFirstOnPg=in_ln;
    if (in_ln<0) indxFirstOnPg=0;
    if (in_ln>indxLastPage) indxFirstOnPg=indxLastPage;
    indxLastOnPg=indxFirstOnPg+pageHeightInL;
    if ((lineSelected<indxLastOnPg)|(lineSelected>=indxLastOnPg)){
        lineSelected=-1;
        hasSelection=false;
    }
    DisplayPage();
}

void List_Box_With_ScrB::GoToLineN(int in_ln){//does synchronize with scrB, because always called without scrB being involved
    DisplayAtLineN(in_ln);
    scrB.SetPosFirstLine(indxFirstOnPg);
}

void List_Box_With_ScrB::GoToLastPage(){
    GoToLineN(indxLastPage);
}

void List_Box_With_ScrB::DisplayPage(bool htd){
//cast all variables
    indxLastOnPg=indxFirstOnPg+pageHeightInL-1;
    if (indxLastOnPg>=totalNbL) indxLastOnPg=totalNbL;
    needDisplay=true;
    if (htd){
        textOnly.UpdateMyTexture();
        ulong idx=static_cast<ulong>(indxFirstOnPg),
                uPageHeight=static_cast<ulong>(pageHeightInL),
                uSelectedLine=static_cast<ulong>(lineSelected),
                uNumberOLs=static_cast<ulong>(totalNbL);

        for (ulong ln(0);ln<uPageHeight;ln++){
            if (idx<uNumberOLs){
                box[ln].setText((*displayText)[idx]);
                box[ln].SetIndex(static_cast<int>(idx));
                box[ln].SetSelected(uSelectedLine==idx);
                box[ln].PrintStringOnly();
            }
            else {
                box[ln].setText("");
            }
            idx++;
        }
        needDisplay=false;
    }
}

bool List_Box_With_ScrB::HasHiddenLine(){
    return hasHiddenLines;
}

void List_Box_With_ScrB::SelectLine(int lnNb){
    if ((lnNb!=lastLineDeleted)&&(lnNb!=antepLineDeleted)){
       if ((lineSelected!=lnNb)&&(lnNb<=indxLastOnPg)&&(lnNb>=indxFirstOnPg)){
           hasSelection=true;
           lineSelected=lnNb;
           DisplayPage();
           return;
       }
    }
    hasSelection=false;
    lineSelected=-1;
    DisplayPage();
}

void List_Box_With_ScrB::SelectFirstLine(){
    SelectLine(indxFirstOnPg);
}

bool List_Box_With_ScrB::HasSelection(){
    return hasSelection;
}

int  List_Box_With_ScrB::SelectedLineNumber(){
    return lineSelected;
}

string List_Box_With_ScrB::StringSelected(){
    if (hasSelection) return (*displayText)[static_cast<ulong>(lineSelected)];
    return("");
}

void List_Box_With_ScrB::DeleteLine(int nbLn){
    int buffAPL=antepLineDeleted;
    if (lineSelected==nbLn) {
        hasSelection=false;
        lineSelected=-1;}
    antepLineDeleted=lastLineDeleted;
    lastLineDeleted=nbLn;
    delStr2=delStr1;
    ulong uNbLn=static_cast<ulong>(nbLn);
    delStr1=(*displayText)[uNbLn];
    (*displayText)[uNbLn]="";
    hasHiddenLines=true;
    canUndo=true;
    if (buffAPL>(-1)){
        //remove the string from the display, adjust variables
        if (lastLineDeleted>=buffAPL ) lastLineDeleted--;
        if (antepLineDeleted>=buffAPL) antepLineDeleted--;
        if (lineSelected>=buffAPL) lineSelected--;
        displayText->erase(displayText->begin()+buffAPL);
        totalNbL--;
        indxLastPage--; if (indxLastPage<0) indxLastPage=0;
        DisplayAtLineN(indxFirstOnPg);
        scrB.Setup(heightPx,totalNbL,indxFirstOnPg,pageHeightInL,textOnly.GetWidth()+grlOffsetX,grlOffsetY);
        scrB.SetPosFirstLine(indxFirstOnPg);
    }
    else DisplayPage();
}

void List_Box_With_ScrB::UndoDelete(){
    if (canUndo) {
        (*displayText)[static_cast<ulong>(lastLineDeleted)]=delStr1;
        delStr1=delStr2;
        delStr2="";
        lastLineDeleted=antepLineDeleted;
        antepLineDeleted=-1;
        if (lastLineDeleted==-1) canUndo=false;
        DisplayPage();
    }
}

bool List_Box_With_ScrB::CanUndo(){
    return canUndo;
}

void List_Box_With_ScrB::DeleteSelectedLine(){
    int lS=lineSelected;
    if (hasSelection&&(lS>=0)) {
        DeleteLine(lS);
        if ((*displayText)[static_cast<ulong>(lS)]!="")
            SelectLine(lS);
        else {
            if (lS<totalNbL-1) SelectLine(lS+1);
        }
    }
}

void List_Box_With_ScrB::SetWidth(int in_width){//the following only for resizing events generated by the user class
    //can't resize lines since this will need a renewed pass through initial text for line processing
    //so rebuild a text vector
    widthPx=in_width;if (widthPx<150) widthPx=150;if (widthPx>1200) widthPx=1200;
    Setup(heightPx,widthPx,grlOffsetX,grlOffsetY);
}
void List_Box_With_ScrB::SetHeightInLines (int in_Lines){
    heightPx=in_Lines*(charHeight+2)+20;
    Setup(heightPx,widthPx,grlOffsetX,grlOffsetY);
} //adjusts also pageHeightInL, adjust scrB accordingly

void List_Box_With_ScrB::SetHeightInPix(int in_pix){
    heightPx=in_pix; if (heightPx<120) heightPx=120;if (heightPx>900) heightPx=900;
    Setup(heightPx,widthPx,grlOffsetX,grlOffsetY);
} //adjusts also pageHeightInL if necessary, adjust scrB accordingly

void List_Box_With_ScrB::AdjustToWidth(){
    if (maxStringWidth<(textOnly.GetWidth()-25)){
        textOnly.SetDimensions(maxStringWidth+25,textOnly.GetHeight());
        lineLength=textOnly.GetWidth()-25;
        scrB.Setup(heightPx,totalNbL,indxFirstOnPg,pageHeightInL,textOnly.GetWidth()+grlOffsetX,grlOffsetY);
        scrB.SetPosFirstLine(indxFirstOnPg);
    }
}

void List_Box_With_ScrB::AdjustToHeight(){
    if (pageHeightInL>totalNbL){
        pageHeightInL=totalNbL;
        SetHeightInLines(pageHeightInL);
    }
} //effects if pageHeight> totalNbL

void List_Box_With_ScrB::ProportionateSizeToHeight(){
    widthPx=int(0.75*heightPx);
    if (widthPx<150) {widthPx=150;}
    if (widthPx>1200) {widthPx=1200;}
    Setup(heightPx,widthPx,grlOffsetX,grlOffsetY);
}//3/4 proportion

void List_Box_With_ScrB::ProportionateSizeToWidth(){
    heightPx=static_cast<int>(round(widthPx/0.75));
    Setup(heightPx,widthPx,grlOffsetX,grlOffsetY);
}//3/4 proportion

int  List_Box_With_ScrB::GetWidth(){
    return widthPx;
}
int  List_Box_With_ScrB::GetHeight(){
    return heightPx;
}
int  List_Box_With_ScrB::GetOffSetX(){
    return grlOffsetX;
}
int  List_Box_With_ScrB::GetOffSetY(){
    return grlOffsetY;
}
int  List_Box_With_ScrB::GetRight(){
    return general.GetRight();
}
int  List_Box_With_ScrB::GetTop(){
    return general.GetTop();
}

int  List_Box_With_ScrB::GetLeft(){
    return general.GetLeft();
}

int  List_Box_With_ScrB::GetBottom(){
    return general.GetBottom();
}
int  List_Box_With_ScrB::GetTextOnlysBottom(){
    return textOnly.GetBottom();
}
int  List_Box_With_ScrB::GetNumberOfLines(){
    return totalNbL;
}

int  List_Box_With_ScrB::GetMaxWidth(){
    return maxStringWidth;
}

void List_Box_With_ScrB::clearText(){
    maxStringWidth=0;
    totalNbL=0;
    displayText->clear();
    pageHeightInL=0;
    indxFirstOnPg=0;
    indxLastOnPg=0;
    indxLastPage=0;
    lineSelected=-1;
    lastLineDeleted=-1;
    antepLineDeleted=-1;
    hasSelection=false;
    scrB.Setup(heightPx,totalNbL,indxFirstOnPg,pageHeightInL,textOnly.GetWidth()+grlOffsetX,grlOffsetY);
    if (displayText!=nullptr)
    {delete displayText;
    displayText=nullptr;}
    displayText=new std::vector<string>;

} //has to adjust scrB also

int List_Box_With_ScrB::MeasureString(string str){
    return (fontMan::MeasureString(str,0));
}

void List_Box_With_ScrB::SetSplitPolicy(int splitP){
    splitPolicy=splitP;
}

void List_Box_With_ScrB::CanDelete(){

}

void List_Box_With_ScrB::DontDelete(){

}

void List_Box_With_ScrB::convertToUTF8(){
    for (auto &tln:(*displayText))
        tln=strops.ToUTF8(tln);
}

void List_Box_With_ScrB::PrintMyText(){
    for (auto lineInBox:box)
        lineInBox.PrintString();
}

char List_Box_With_ScrB::GetScrollBarsColorCode(){
    return scrB.GetColorCodeFromSymbol();
}

bool List_Box_With_ScrB::NeedsToDisplay(){
    return needDisplay;
}

void List_Box_With_ScrB::HideMyself(){
    DeleteDisplayBox();
    textOnly.SetVisibility(false);
    scrB.Setup(0,0,0,0,0,0);//force visibility false
}
