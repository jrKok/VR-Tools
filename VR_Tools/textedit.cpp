#include "textedit.h"

TextEdit::TextEdit(): TextReader(),
  clickTime(0),
  cursor(),
  ops(),
  localClip()

{

}

bool TextEdit::Save(){
    bool success(true);
    if (!textFile.is_open()){
        textFile.open(fileName,std::ofstream::out);
        if (textFile.is_open()){
            for (auto line : (*displayText)){
                textFile << line;
            }
            textFile.close();}
        else success=false;}
    else success=false;
    return success;
}

void TextEdit::Recalculate(int in_lft,int in_tp){
    List_Box_With_ScrB::Recalculate(in_lft,in_tp);
    cursor.Recalculate(box[0].x,box[0].y);
}


bool TextEdit::ReadFileToBuff(){
     if (fileName!=""){
       if (!textFile.is_open()) {
           OpenFile();
           if (!textFile.is_open()) {
               string inputL="the file "+fileName+" couldn't be found";
               fileExists=false;
               AddLine(inputL);
             return false;}}

       clearText();
       stringOps ops;
       string inputL(""),CR("\r"),LF("\n");

       while (getline(textFile,inputL)){
           inputL=ops.cleanOut(inputL,CR);//remove carriage return
           inputL+=LF;//but add line feed for saving, so to know where the original breaks were
          AddLine(cursor.ReadLineToUTF(inputL));
          }
       textFile.close();
       SetupforText();
       cursor.Initiate(displayText,grlOffsetX,grlOffsetY,charHeight,pageHeightInL);
     }
     return true;
}

bool TextEdit::ProceedClick(int x, int y){

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
                clickTime=XPLMGetElapsedTime();
                currentIndxFP=indxFirstOnPg;
                currentIndx=ln+indxFirstOnPg;
                clickPosX=x;
                clickPosY=y;
                needToContClick=true;
                cursor.FindPos(currentIndx,x);
                return true;
            }
        }
    }
    else needToContClick=false;
    return false;
}

void TextEdit::ProceedClickCont(int x, int y){

    if (needToContClick){

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
    //selection mode if click hold for more than a second (or less, to be determined
           if (!dragLines){
              if ((XPLMGetElapsedTime()-clickTime)>0.3f) dragLines=true;
           }
           else{
              for (auto bx:box){
                if (bx.isHere(x,y)){
                    cursor.DragPos(bx.index+indxFirstOnPg,x);}
                else{
                    if ((XPLMGetElapsedTime()-clickTime)>0.3f){
                        if (y<textOnly.top) {
                            MoveDnNLines(1);
                            cursor.MoveCursorUp();
                            DisplayPage();

                        }
                        if (y>textOnly.bottom){
                            MoveUpNLines(1);
                            cursor.MoveCursorDown();
                            DisplayPage();
                        }
                        clickTime=XPLMGetElapsedTime();
                    }
                }

              }
           }
       }
    }
}

void TextEdit::ProceedEndClick(){//Mouse Up

    if (dragLines) {
        cursor.EndOfDrag();
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

void TextEdit::DisplayPage(){
    List_Box_With_ScrB::DisplayPage();
    cursor.SetIndxFirstPage(indxFirstOnPg);

}

void TextEdit::DrawMySelf(){
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
    int l,r;
    for (auto bx:box){
         if (cursor.IsIndexInSelection(bx.index,l,r)) cursor.DrawRectangle(l,bx.top,r,bx.bottom);
         if (cursor.HasCursor()&&((bx.index+indxFirstOnPg)==cursor.GetLine())) cursor.DrawCursor(bx.y+3);
         XPLMDrawString(ink,bx.x,bx.y+1,(char*)bx.textOfLine.c_str(),NULL,xplmFont_Proportional);}

    scrB.DrawMySelf();
}

void TextEdit::MoveCursorUp(){
    if (cursor.HasCursor()){
        cursor.MoveCursorUp();
        if (cursor.GetLine()<indxFirstOnPg) MoveUpNLines(1);
    }
    if (cursor.HasSelection()){
        cursor.MoveSelectionUp();
        if (cursor.GetSelectionStartLine()<indxFirstOnPg) MoveUpNLines(1);
    }

}

void TextEdit::MoveCursorDown(){
    if (cursor.HasCursor()){
        cursor.MoveCursorDown();
        if (cursor.GetLine()>indxLastOnPg) MoveDnNLines(1);
    }
    if (cursor.HasSelection()){
        cursor.MoveSelectionDown();
        if (cursor.GetSelectionEndLine()>indxLastOnPg) MoveDnNLines(1);
    }

}

void TextEdit::MoveCursorRight(){
    if (cursor.HasCursor()) cursor.MoveCursorRight();
    if (cursor.HasSelection()) cursor.MoveSelectionRight();
}

void TextEdit::MoveCursorLeft(){
    if (cursor.HasCursor()) cursor.MoveCursorLeft();
    if (cursor.HasSelection()) cursor.MoveSelectionLeft();
}

void TextEdit::FlipMoveSelectionEnd(){
    if (cursor.HasSelection()) cursor.FlipSelection();
}

/* The following are low and middle level functions for editing the text file
 * they can be called frrom different commands and perform basic operations in
 * the file. They don't update the display, but track internal descriptive
 * variables like number of lines, page indicators
 */

void TextEdit::UpdateBox(int thisLine){
    if (thisLine>=indxFirstOnPg&&thisLine<indxLastOnPg){
        box[thisLine-indxFirstOnPg].setText((*displayText)[thisLine]);
    }
}

void TextEdit::InsertEmptyLine(int in_line){
    if (in_line<totalNbL)
        displayText->insert(displayText->begin()+in_line,string(""));
    else
        displayText->push_back(string(""));
    cursor.InsertEmptyLine(in_line);
    totalNbL++;indxLastPage++;
}

void TextEdit::DeleteWholeLine(int in_line){
    displayText->erase(displayText->begin()+(in_line));
    cursor.DeleteFromLines(in_line);
    totalNbL--;indxLastPage--;
}



void TextEdit::InsertTextInLine(int in_line,int charPos,string in_text){
    //low level function, puts text from paste or a char at the given position
    ulong l=static_cast<ulong>(in_line);
    string sText=(*displayText)[l];
    ulong bP=static_cast<ulong>(cursor.FindPositionInNativeLine(sText,charPos));
    sText.insert(bP,in_text);
    (*displayText)[l]=sText;
}

void TextEdit::DeleteFromLine(int in_line,const int beginPos,const int endPos,bool keepLF){
    //low level function, deletes some text, caller will update display and read line again
    ulong l=static_cast<ulong>(in_line);
    string sText=(*displayText)[l];   
    ulong sTextLgth=sText.size();
    char lastchar=sText.back();
    ulong bP=static_cast<ulong>(cursor.FindPositionInNativeLine(sText,beginPos));
    ulong eP=static_cast<ulong>(cursor.FindPositionInNativeLine(sText,endPos));
    if (!keepLF&&endPos==cursor.GetLastPositionOfLine(in_line))
        eP=sText.length();
    sText.erase(bP,eP-bP);
    if (keepLF&&(lastchar=='\n')&&(eP==sTextLgth)) sText+="\n";
    if (sText=="")
        DeleteWholeLine(in_line);
    else
        (*displayText)[l]=sText;
}

string TextEdit::GetLineToBreak(int in_line,int &last_line,int &cursorPos){
    string LF("\n"),retString("");
    ulong cursorLine(0);
    ulong l=static_cast<ulong>(in_line);
    if (cursor.HasCursor()) {
        cursorLine=static_cast<ulong>(cursor.GetLine());
        cursorPos=cursor.FindPositionInNativeLine((*displayText)[cursorLine],cursorPos);
    }
    bool hardEnd(false);
    do{
        retString+=(*displayText)[l];
        if (l<cursorLine){
            cursorPos+=retString.length();
        }
        if (LF==(*displayText)[l].substr(((*displayText)[l].length()-1),1)) hardEnd=true;
        else l++;
        if (l>=static_cast<ulong>(totalNbL)) hardEnd=true;
    }while (!hardEnd);
    last_line=static_cast<int>(l);
    return retString;
}

void TextEdit:: DeleteSelection(){
    /* Deletes the selection
     * for Cut, backspace, type a char when selection is on...
     * thatfore update of display must be done by caller
     * however this is an intermediate level function
     */
    //Retrieve Cursor Positions
    int startPS=cursor.GetSelectionStartCharPos(),
        endPS  =cursor.GetSelectionEndCharPos(),
        startL =cursor.GetSelectionStartLine(),
        endL   =cursor.GetSelectionEndLine();
    ulong uStart=static_cast<ulong>(startL);

    if (endL==startL) { //a part of only one line selected
        DeleteFromLine(startL,startPS,endPS,true);
    }

    else{               //selection spawns several lines
        for (int ln(endL);ln>=startL;ln--){
           if (ln==startL)//first line
               DeleteFromLine(ln,startPS,cursor.GetLastPositionOfLine(ln),false);
           else{
               if (ln==endL) //last line
                   DeleteFromLine(ln,0,endPS,true);
              else DeleteWholeLine(ln); //line in between
           }
         }

        if (((startL+1)<totalNbL)&((*displayText)[uStart+1]=="")){
            if (startL+1==(totalNbL-1)) {
                DeleteWholeLine(startL+1);
                if ((*displayText)[uStart]==""){
                    if (indxFirstOnPg>0&&startL==indxFirstOnPg) indxFirstOnPg--;
                    DeleteWholeLine(startL);
                    cursor.EraseCursor();
                    return;
                }
            }else{
                DeleteWholeLine(startL+1);
            }
        }
        (*displayText)[uStart]=(*displayText)[uStart]+(*displayText)[uStart+1];
        DeleteWholeLine(startL+1);
    }

    while (startL>=totalNbL) startL--;
    cursor.SetCursorAt(startL,startPS);
}

void TextEdit::ReadLineAgain(int in_line){
    /* function called from all high level functions
     * makes display of edited text seamless
     * read the edited line(s) again, from the starting line to the next LF char
     * synchronizing the display should be done by the caller
     * but follows cursor position if cursor is in the lines proceeded
     * intermediate level function
     */
    int last_line(0),cursorPos(0);
    ulong l=static_cast<ulong>(in_line);
    if (in_line>0)
    {
        char lastchar=(*displayText)[l-1].back();
        if (lastchar!='\n')
        {
            l--;
            in_line--;
        }
    }
    int procSoFar(0),newline(0),newPos(0);
    if (cursor.HasCursor()) {cursorPos=cursor.GetPos();newline=cursor.GetLine();}
    string toWorkOn=GetLineToBreak(in_line,last_line,cursorPos);
    if (last_line>=totalNbL) last_line=totalNbL-1;
    ulong uLastLine=static_cast<ulong>(last_line);
    std::istringstream iss;
    iss.str(toWorkOn);
    string inputL(""),CR("\r"),LF("\n");

    while (getline(iss,inputL)){
        strops.cleanOut(inputL,CR);
        inputL+=LF;
        int sz=MeasureString(inputL);
    //split line according to bestSplitAtSpace rule

        while (sz>lineLength){
            std::string leftString=strops.bestLeftSize(inputL,lineLength);
            int leftStringsize=static_cast<int>(leftString.length());
            if ((cursorPos>=procSoFar) && (cursorPos<(procSoFar+leftStringsize))) {
                newline=static_cast<int>(l);
                newPos=cursorPos-procSoFar;
            }
            procSoFar+=leftStringsize;
            if (l<=uLastLine){
               (*displayText)[l]=leftString;
                  cursor.MakeLinePosAgain(l,leftString);
            }
            else{
               InsertEmptyLine(static_cast<int>(l));
               (*displayText)[l]=leftString;
               cursor.MakeLinePosAgain(l,leftString);
            }
            l++;
            sz=MeasureString(inputL);
        }

        if ((cursorPos>=procSoFar) && (cursorPos<(procSoFar+sz))) {
            newline=static_cast<int>(l);
            newPos=cursorPos-procSoFar;
        }
        if (l<=uLastLine){
           (*displayText)[l]=inputL;
            cursor.MakeLinePosAgain(l,inputL);
            l++;
        }
        else{
           InsertEmptyLine(static_cast<int>(l));
           (*displayText)[l]=inputL;
           cursor.MakeLinePosAgain(l,inputL);
           l++;
        }
    }
    //delete lines if l<el
    while (l<=uLastLine){
          DeleteWholeLine(static_cast<int>(l));
          l++;
    }

    if (cursor.HasCursor()&(cursor.GetLine()>=(in_line-1))) {
        string toConvert=(*displayText)[static_cast<ulong>(newline)].substr(0,static_cast<ulong>(newPos));
        int newP=cursor.GetLengthOfUTFString(toConvert);
        cursor.SetCursorAt(newline,newP);
    }
}

/* The following are top level entry functions, called after a command is issued by the user
 *  they have to update the display, cursor position, leave the file in
 *  a useable state
 */

void TextEdit::Cut(){
    if (cursor.HasSelection()){
        int line(cursor.GetSelectionStartLine()),pos(cursor.GetSelectionStartCharPos());
        Copy();
        DeleteSelection();
        DisplayPage();
        cursor.SetCursorAt(line,pos);
    }
}

void TextEdit::Copy(){

    if (cursor.HasSelection()){
        string copyString("");
        ulong bPos(static_cast<ulong>(cursor.GetSelectionStartCharPos())),
              ePos(static_cast<ulong>(cursor.GetSelectionEndCharPos())),
              bLine(static_cast<ulong>(cursor.GetSelectionStartLine())),
              eLine(static_cast<ulong>(cursor.GetSelectionEndLine()));

            if (bLine==eLine) {
                string sText=(*displayText)[bLine];
                int bp=cursor.FindPositionInNativeLine(sText,static_cast<int>(bPos));
                int ep=cursor.FindPositionInNativeLine(sText,static_cast<int>(ePos));
                copyString=sText.substr(bp,ep-bp);

            }else{
                for (ulong selLine(bLine);selLine<=eLine;selLine++){
                    if (selLine==bLine){
                        int bp=cursor.FindPositionInNativeLine((*displayText)[selLine],static_cast<int>(bPos));
                        copyString+=(*displayText)[selLine].substr(bp,(*displayText)[selLine].length()-bp);
                     }
                     else{
                        if (selLine==eLine){
                            int ep=cursor.FindPositionInNativeLine((*displayText)[selLine],static_cast<int>(ePos));
                            copyString+=(*displayText)[selLine].substr(0,ep);
                            }
                        else copyString+=(*displayText)[selLine];
                        }
                 }
            }
            localClip.PushText(copyString);
        }

}

void TextEdit::Paste(){
    if (localClip.HasContent()){
        string toPaste=cursor.ReadLineToUTF(localClip.PullText());
        if (cursor.HasSelection()){
            int cLine=cursor.GetSelectionStartLine();
            int cPos =cursor.GetSelectionStartCharPos();
            DeleteSelection();
            if (cLine>=totalNbL){
                cLine--;
                if (cLine<0) {
                    cLine=0;
                }
                cPos=cursor.GetLastPositionOfLine(cLine);
            }
            string fromClip=localClip.PullText();
            InsertTextInLine(cLine,cPos,toPaste);
            ReadLineAgain(cLine);
            DisplayPage();
        }
        if (cursor.HasCursor()){
            int cLine=cursor.GetLine();
            int cPos =cursor.GetPos();
            InsertTextInLine(cLine,cPos,toPaste);
            ReadLineAgain(cLine);
            DisplayPage();
        }
    }
}

void TextEdit::InsertLetter(std::string fromKey){
    if (cursor.HasSelection()){
        int lineS=cursor.GetSelectionStartLine();
        DeleteSelection();
        if (!cursor.HasCursor()) ReadLineAgain(lineS);
    }
    if (cursor.HasCursor()){
    int cursorLine=cursor.GetLine();
    int position=cursor.GetPos();
    InsertTextInLine(cursorLine,position,fromKey);
    int insL=cursor.GetLengthOfUTFString(fromKey);
    position+=insL;
    cursor.AddPositionsToLine(insL);
    cursor.SetCursorAt(cursorLine,position);
    ReadLineAgain(cursorLine);
    DisplayPage();
    }
}

void TextEdit::Backspace(){
    if (cursor.HasSelection()){
        DeleteSelection();
        if (cursor.HasCursor()) {
            int ln(cursor.GetSelectionStartLine());
            ReadLineAgain(ln);
        }
        DisplayPage();
        return;
    }
    if (cursor.HasCursor()){
       int cLine=cursor.GetLine(),
           cPos=cursor.GetPos();
       ulong uln=static_cast<ulong>(cLine);

       if (cLine==0&&cPos==0) return;

       if (cPos==0){//cursor at begin of line, results in cursor moving up, deleting last char of previous line (LF or other)

               int lastPosOfPrevLine=cursor.GetLastPositionOfLine(static_cast<int>(uln-1));
               cursor.SetCursorAt(cLine-1,lastPosOfPrevLine);
               cursor.MoveCursorLeft();
               string bufl=(*displayText)[uln],buf=(*displayText)[uln-1];
               buf=stringOps::RemoveLastUTFCharFromString(buf);
               (*displayText)[uln-1]=buf+bufl;
               DeleteWholeLine(cLine);
               ReadLineAgain(cLine-1);
               DisplayPage();
        }

       else{

           DeleteFromLine(cLine,cPos-1,cPos,true);
           cursor.MoveCursorLeft();
           ReadLineAgain(cLine);
           DisplayPage();
       }
    }
}

void TextEdit::Suppress(){
    if (cursor.HasCursor()){
        int ln(cursor.GetLine()),pos(cursor.GetPos());
        ulong uln=static_cast<ulong>(ln);
        if (pos>=cursor.GetLastPositionOfLine(ln)) {
            //delete the eventual LF, else delete first character from next line
            if (ln<(totalNbL-1)){
                char lastchar=(*displayText)[uln].back();
                if (lastchar=='\n'){
                    (*displayText)[uln]=(*displayText)[uln].substr(0,(*displayText)[uln].length()-1);
                    (*displayText)[uln]+=(*displayText)[uln+1];
                }else{
                    (*displayText)[uln]+=(*displayText)[uln+1].substr(1,(*displayText)[uln].length()-1);
                }
                DeleteWholeLine(ln+1);
            }
            else return;
        }else{
            DeleteFromLine(ln,pos,pos+1,false);
        }
        ReadLineAgain(ln);
        DisplayPage();
        return;
    }
    if (cursor.HasSelection()){
        int ln=cursor.GetSelectionStartLine();
        DeleteSelection();
        if (cursor.HasCursor()) ReadLineAgain(ln);
        DisplayPage();
    }
}

void TextEdit::Enter(){
    if (cursor.HasSelection()){
        DeleteSelection();
    }
    if (cursor.HasCursor()){
        int lineTag=cursor.GetLine();
        int pos=cursor.GetPos();
        InsertTextInLine(lineTag,pos,string("\n"));
        ReadLineAgain(lineTag);
        DisplayPage();
        if (lineTag<(totalNbL-1)){
            cursor.SetCursorAt(lineTag+1,0);
        }
    }
}
