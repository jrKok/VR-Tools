#include "list_box_with_scrb.h"

List_Box_With_ScrB::List_Box_With_ScrB():
     in_top(0),
     in_left(0),
     grlOffsetX(0),
     grlOffsetY(0),
     charHeight(0),
     maxStringWidth(0),
     pageHeightInL(0),
     lineLength(0),
     totalNbL(0),
     indxFirstOnPg(0),
     indxLastOnPg(0),
     indxLastPage(0),
     lineSelected(-1),
     lastLineDeleted(-1),
     antepLineDeleted(-1),
     heightPx(0),
     widthPx(0),
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
     drawBackground(true),
     backgrd{0.9922f,0.9608f,0.9098f},
     ink{0.005f,0.005f,0.005f},
     inkSelect{0.9f,0.05f,0.4f},
     displayText(new std::vector<std::string>),
     delStr1(""),
     delStr2("")

{

}

void List_Box_With_ScrB::WriteDebug(std::string message){
    std::string in_String="VR Tools : " +message+"\n";
    XPLMDebugString((char*)in_String.c_str());
}

List_Box_With_ScrB::~List_Box_With_ScrB(){
    delete displayText;
    displayText=nullptr;
}

void List_Box_With_ScrB::Setup (int hght,int larg,int in_offsetX,int in_offsetY){
//calculate sizes
//input width is total width, scrollboxes are going to be substracted from this
    grlOffsetX=in_offsetX;
    grlOffsetY=in_offsetY;
    heightPx=hght;if (heightPx<120) heightPx=120;if (heightPx>900) heightPx=900;
    widthPx=larg;if (widthPx<230) widthPx=230;if (widthPx>1500) widthPx=1500;
    general.width=widthPx;
    general.height=heightPx;
    general.offsetX=in_offsetX;
    general.offsetY=in_offsetY;
    textOnly.width=widthPx-15;//(-15 for scrollbar)
    lineLength=textOnly.width-25;
    textOnly.height=heightPx;
    textOnly.offsetX=in_offsetX;
    textOnly.offsetY=in_offsetY;

//calculate text params
    XPLMGetFontDimensions(xplmFont_Proportional,NULL,&charHeight,NULL);


//setup lines

    box.clear();
    pageHeightInL=int((heightPx-20)/(charHeight+2));
    TextLine nTL;
    nTL.height=charHeight;
    nTL.width=textOnly.width-10; //5 margin left, 5 right
    nTL.offSetX=grlOffsetX+5;
    for (int lg(0);lg<pageHeightInL;lg++){
        nTL.index=lg;
        nTL.offSetY=grlOffsetY+10+charHeight*lg+lg*2;
        box.push_back(nTL);
    }
//setup scrB

}

void List_Box_With_ScrB::SetupforText(){
    pageHeightInL=int((heightPx-20)/(charHeight+2));

    totalNbL=displayText->size();
    if (totalNbL==0){
        scrB.SetVisibility(false);
        AddLine("The file doesn't contain any text");
        totalNbL=1;
    }

    indxLastPage=totalNbL-pageHeightInL;if (indxLastPage<0) indxLastPage=0;
    scrB.Setup(heightPx,totalNbL,indxFirstOnPg,pageHeightInL,textOnly.width+grlOffsetX,grlOffsetY);
    if (indxFirstOnPg>indxLastPage) indxFirstOnPg=indxLastPage;//I don't redefine indxFirstOnPage except if the display has shrunken
    indxLastOnPg=indxFirstOnPg+pageHeightInL;
    if (indxLastOnPg>totalNbL) indxLastOnPg=totalNbL;
    DisplayPage();
    Recalculate(in_left,in_top);
}

void List_Box_With_ScrB::AddLine(std::string in_Line){

    int sz=MeasureString(in_Line);
    if (sz>maxStringWidth) maxStringWidth=sz;
    //split line according to rules
    switch (splitPolicy){
    case BestSplitAtSpace:   {
        while (sz>lineLength){
            std::string leftString=strops.bestLeftSize(in_Line,lineLength);
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
            std::string leftString=strops.splitAtSize(in_Line,lineLength);
            displayText->push_back(leftString);
            sz=MeasureString(in_Line);
            totalNbL++;
        }
        displayText->push_back(in_Line);
        totalNbL++;
        break;
    }

    case TruncateKeepLeft:{
        std::string leftString=in_Line;
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
        std::string rightStr=in_Line;
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
    drawBackground=opt;
}

void List_Box_With_ScrB::SetBckColor (float in_col[3]){
    backgrd[0]=in_col[0];
    backgrd[1]=in_col[1];
    backgrd[2]=in_col[2];
}

void List_Box_With_ScrB::SetInkColor (float in_col[3]){
    ink[0]=in_col[0];
    ink[1]=in_col[1];
    ink[2]=in_col[2];
}

void List_Box_With_ScrB::DrawMySelf(){
    if (drawBackground){
    glColor3fv(backgrd);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glBegin(GL_TRIANGLE_FAN);
    {
        glVertex2i(textOnly.left, textOnly.top);
        glVertex2i(textOnly.right, textOnly.top);
        glVertex2i(textOnly.right, textOnly.bottom);
        glVertex2i(textOnly.left, textOnly.bottom);
    }
    glEnd();
    }
    for (auto bx:box){
        if (bx.isSelected)
           XPLMDrawString(inkSelect,bx.x,bx.y,(char*)bx.textOfLine.c_str(),NULL,xplmFont_Proportional);
        else
           XPLMDrawString(ink,bx.x,bx.y,(char*)bx.textOfLine.c_str(),NULL,xplmFont_Proportional);
        }
    scrB.DrawMySelf();
}

bool List_Box_With_ScrB::ProceedClick(int x, int y){

    //find location
    if (general.isHere(x,y)){
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
                needToContClick=true;//filterClick=false;//remove filter click when bug corrected
                return true;
            }
        }
    }
    else needToContClick=false;
    return false;
}

void List_Box_With_ScrB::ProceedClickCont(int x, int y){
   // filterClick=!filterClick;//toggle filter Click, first position returned is Ok, second is not, alternating, discard this when bug corrected
    if (needToContClick){//&filterClick){

        if (scrB.OngoingDrag()) {//process for scrB ?
            DisplayAtLineN(scrB.DragY(y));
            return;
        }
        if (scrB.OngoingRepeat()){
            scrB.ShouldRepeat();
            DisplayAtLineN(scrB.GetPosFirstLine());
            return;
        }

    if (currentIndx!=-1){
    int diffY=y-clickPosY;
    if (dragLines){ //if dragLines is activated, it cannot become a delete command, so only look at vertical movement
        int nbLns=int(diffY/(charHeight+2));
        if (nbLns!=0) {
            DisplayAtLineN(currentIndxFP+nbLns);
            scrB.SetPosFirstLine(indxFirstOnPg);
            return;
        }
    }else{ //determine if there is some significant movement along x or y
        int diffX=clickPosX-x;
        int nbLns=int(diffY/(charHeight+2));
        if (nbLns!=0){ //if y movement is significant drag becomes an irreversible drag along y axis
            dragLines=true;
            delLines=false;//you cannot both drag the display and delete a line
            DisplayAtLineN(currentIndxFP+nbLns);
            //scrB.SetPosFirstLine(indxFirstOnPg);
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
    if (scrB.OngoingDrag()) scrB.EndDrag();
    if (scrB.OngoingRepeat()) scrB.EndRepeat();
}

void List_Box_With_ScrB::Recalculate(int in_lft, int in_tp){
    in_top=in_tp;
    in_left=in_lft;
    general.recalculate(in_lft,in_tp);
    textOnly.recalculate(in_lft,in_tp);
    scrB.Recalculate(in_lft,in_tp);
    for (int lg(0);lg<pageHeightInL;lg++){
        box[lg].recalculate(in_left,in_top);
    }
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
    if (lineSelected<indxLastOnPg|lineSelected>=indxLastOnPg){
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

void List_Box_With_ScrB::DisplayPage(){
    for (int ln(0);ln<pageHeightInL;ln++){
        int idx=indxFirstOnPg+ln;
        if (idx<totalNbL){
        box[ln].setText((*displayText)[idx]);
        if (lineSelected==idx) box[ln].isSelected=true;
        else box[ln].isSelected=false;
        }
        else box[ln].textOfLine="";
    }
}

bool List_Box_With_ScrB::HasHiddenLine(){
    return hasHiddenLines;
}

void List_Box_With_ScrB::SelectLine(int lnNb){
    if ((lnNb!=lastLineDeleted)&&(lnNb!=antepLineDeleted)){
       if ((lineSelected!=lnNb)&&(lnNb<indxLastOnPg)&&(lnNb>=indxFirstOnPg)){
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

std::string List_Box_With_ScrB::StringSelected(){
    if (hasSelection) return (*displayText)[lineSelected];
    return("");
}

void List_Box_With_ScrB::DeleteLine(int nbLn){
    int buffAPL=antepLineDeleted;
    if (lineSelected==nbLn) {
        hasSelection=false;
        lineSelected=-1;};
    antepLineDeleted=lastLineDeleted;
    lastLineDeleted=nbLn;
    delStr2=delStr1;
    delStr1=(*displayText)[nbLn];
    (*displayText)[nbLn]="";
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
        scrB.Setup(heightPx,totalNbL,indxFirstOnPg,pageHeightInL,textOnly.width+grlOffsetX,grlOffsetY);
    }
    else DisplayPage();
}

void List_Box_With_ScrB::UndoDelete(){
    if (canUndo) {
        (*displayText)[lastLineDeleted]=delStr1;
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
if (hasSelection&&(lineSelected>=0)) DeleteLine(lineSelected);
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
    if (maxStringWidth<(textOnly.width-25)){
        textOnly.width=maxStringWidth+25;
        lineLength=textOnly.width-25;
        scrB.Setup(heightPx,totalNbL,indxFirstOnPg,pageHeightInL,textOnly.width+grlOffsetX,grlOffsetY);
        scrB.SetPosFirstLine(indxFirstOnPg);
        Recalculate(in_left,in_top);
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
    if (widthPx<150) widthPx=150;if (widthPx>1200) widthPx=1200;
    Setup(heightPx,widthPx,grlOffsetX,grlOffsetY);
}//3/4 proportion

void List_Box_With_ScrB::ProportionateSizeToWidth(){
    heightPx=widthPx/0.75;
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
    return general.right;
}
int  List_Box_With_ScrB::GetBottom(){
    return general.bottom;
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
    scrB.SetVisibility(false);
    if (displayText!=nullptr)
    {delete displayText;
    displayText=nullptr;}
    displayText=new std::vector<std::string>;

} //has to adjust scrB also

int List_Box_With_ScrB::MeasureString(std::string str){
    int retVal;
    retVal=(int)XPLMMeasureString(xplmFont_Proportional,(char*)str.c_str(),str.size());
    return retVal;
}

void List_Box_With_ScrB::SetSplitPolicy(int splitP){
    splitPolicy=splitP;
}

void List_Box_With_ScrB::CanDelete(){

}

void List_Box_With_ScrB::DontDelete(){

}
void List_Box_With_ScrB::convertToUTF8(){
    for (int itr(0);itr<totalNbL;itr++){
        (*displayText)[itr]=strops.ToUTF8((*displayText)[itr]);
    }
}
