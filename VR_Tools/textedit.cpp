#include "textedit.h"
#include "drawlogic.h"

TextEdit::TextEdit(): TextReader(),
  hasToCommit(false),
  clickTime(0),
  tcursor(),
  ops(),
  localClip(),
  tempFile(""),
  tabchar('\t'),
  tabstring(1, tabchar)

{

}

void TextEdit::InitialiseEdit(){
    FilePointer::MakeBackups();
    tempFile=FilePointer::GetTempName();
    if (!FilePointer::OpenTempFileForRead(textFile)) DrawLogic::WriteDebug ("error in processing temporary file");
    else DrawLogic::WriteDebug("Editing file "+tempFile);
    textFile.close();
    hasToCommit=false;
}

void TextEdit::PrintATextLine(string inLine){
    box[0].setText(inLine);
}

void TextEdit::Commit(){
    if (hasToCommit){
        hasToCommit=false;
        if (FilePointer::OpenTempFileForWrite(textFile)){
            for (auto line : (*displayText)){
                textFile << line;
            }
            textFile.close();}
    }
}

bool TextEdit::Save(){
    Commit();
    FilePointer::SaveTemp();
    return true;
}


bool TextEdit::ReadFileToBuff(){
     Commit();
     if (FilePointer::OpenTempFileForRead(textFile)){
       clearText();
       stringOps ops;
       string inputL("");

       while (getline(textFile,inputL)){
           inputL=ops.cleanOut(inputL,"\r");//remove carriage return
           inputL+="\n";//but add line feed for saving, so to know where the original breaks were
          AddLine(inputL);
          }
       textFile.close();
       SetupforText();
       tcursor.Initiate(displayText,textOnly.GetLeft()+5,textOnly.GetBottom(),charHeight,pageHeightInL);
     } else {
         string inputL="the file "+tempFile+" couldn't be found";
         fileExists=false;
         AddLine(inputL);
         return false;}
     return true;
}

bool TextEdit::ProceedClick(int x, int y){

    //find location
    if (general.IsHere(x,y)){
        //which element
        int retV(0);
        ulong pgHgt=static_cast<ulong>(pageHeightInL);
        if (scrB.IsCommandForMe(x,y,retV)){//scrollBox ?
            DisplayAtLineN(scrB.GetPosFirstLine());
            needToContClick=true;//filterClick=false;//remove filter click when bug corrected
            return true;
        }
        for (ulong ln(0);ln<pgHgt;ln++){//TextLine ?
            if (box[ln].isHere(x,y)){
                clickTime=XPLMGetElapsedTime();
                currentIndxFP=indxFirstOnPg;
                currentIndx=box[ln].GetIndex();
                clickPosX=x;
                clickPosY=y;
                needToContClick=true;
                tcursor.FindPos(currentIndx,x);
                if (tcursor.HasCursor()) DrawLogic::SetCursorPosition(tcursor.PosToX(),box[ln].GetTextY()+2);
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
    //selection mode if click hold for more than 0.3 second (or less, to be determined
           if (!dragLines){
              if ((XPLMGetElapsedTime()-clickTime)>0.8f) dragLines=true;
           }
           else{
              for (auto bx:box){
                if (bx.isHere(x,y)){
                    tcursor.DragPos(bx.GetIndex()+indxFirstOnPg,x);
                    DisplayPage();}
                else{
                    if ((XPLMGetElapsedTime()-clickTime)>0.3f){//scroll the display at a rate of 3.3 lines/sec
                        if (y<textOnly.GetTop()) {
                            MoveDnNLines(1);
                            tcursor.MoveCursorUp();
                            DisplayPage();

                        }
                        if (y>textOnly.GetBottom()){
                            MoveUpNLines(1);
                            tcursor.MoveCursorDown();
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
        tcursor.EndOfDrag();
    }
    currentIndx=-1;
    currentIndxFP=-1;
    clickPosX=0;
    clickPosY=0;
    needToContClick=false;
    scrB.EndDrag();
    scrB.EndRepeat();
}

void TextEdit::DisplayPage(bool ntd){
    //Paint background
    indxLastOnPg=indxFirstOnPg+pageHeightInL-1;
    if (indxLastOnPg>=totalNbL) indxLastOnPg=totalNbL-1;
    tcursor.SetFirstLineOnPage(indxFirstOnPg);
    needDisplay=true;
    if (ntd){
        textOnly.UpdateMyTexture();
        //DrawSelectionRectangle

        //cast Variables to usable types
        ulong idx=static_cast<ulong>(indxFirstOnPg),
                uPageHeight=static_cast<ulong>(pageHeightInL),
                //uSelectedLine=static_cast<ulong>(lineSelected),
                uNumberOLs=static_cast<ulong>(totalNbL);
        //compute boundaries

        //copy & fill the box for displaying the text lines
        for (ulong ln(0);ln<uPageHeight;ln++){
            if (idx<uNumberOLs){
                box[ln].setText((*displayText)[idx]);
                box[ln].SetIndex(static_cast<int>(idx));
                if (tcursor.HasCursor()) {
                    if (box[ln].GetIndex()==tcursor.GetLine())
                        DrawLogic::SetCursorPosition(tcursor.PosToX(),box[ln].GetTextY()+2);
                }
                if (tcursor.HasSelection()){
                    int l(0),r(0);
                    if (tcursor.IsIndexInSelection(box[ln].GetIndex(),l,r)){

                        DrawLogic::PrintRectOnTexture(l,box[ln].GetBottom(),r,box[ln].GetTop(),Clr_TextSelectBlue);
                        box[ln].PrintStringOnLocalT();
                    }
                    else box[ln].PrintStringOnly();
                }
                else box[ln].PrintStringOnly();
            }
            else {
                box[ln].setText("");
            }
            idx++;
        }
        needDisplay=false;
    }
}

void TextEdit::DrawCursor(){
tcursor.DrawCursor();
}

void TextEdit::MoveCursorUp(){
    if (tcursor.HasCursor()){
        tcursor.MoveCursorUp();
        if (tcursor.GetLine()<indxFirstOnPg) MoveUpNLines(1);
    }
    if (tcursor.HasSelection()){
        tcursor.MoveSelectionUp();
        if (tcursor.GetSelectionStartLine()<indxFirstOnPg) MoveUpNLines(1);
    }
}

void TextEdit::MoveCursorDown(){
    if (tcursor.HasCursor()){
        tcursor.MoveCursorDown();
        if (tcursor.GetLine()>=indxLastOnPg) MoveDnNLines(1);
    }
    if (tcursor.HasSelection()){
        tcursor.MoveSelectionDown();
        if (tcursor.GetSelectionEndLine()>=indxLastOnPg) MoveDnNLines(1);
    }
}

void TextEdit::MoveCursorRight(){
    if (tcursor.HasCursor()) tcursor.MoveCursorRight();
    if (tcursor.HasSelection()) tcursor.MoveSelectionRight();
}

void TextEdit::MoveCursorLeft(){
    if (tcursor.HasCursor()) tcursor.MoveCursorLeft();
    if (tcursor.HasSelection()) tcursor.MoveSelectionLeft();
}

void TextEdit::MoveCursorBackTab(){
    if (tcursor.HasCursor()) tcursor.MoveCursorBackTab();
}

void TextEdit::FlipMoveSelectionEnd(){
    if (tcursor.HasSelection()) tcursor.FlipSelection();
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
    tcursor.InsertEmptyLine(in_line);
    totalNbL++;indxLastPage++;
}

void TextEdit::DeleteWholeLine(int in_line){
    displayText->erase(displayText->begin()+(in_line));
    tcursor.DeleteFromLines(in_line);
    totalNbL--;indxLastPage--;
}

void TextEdit::InsertTextInLine(int in_line,int charPos,string in_text){
    //low level function, puts text from paste or a char at the given position
    ulong l=static_cast<ulong>(in_line);
    string sText=(*displayText)[l];
    ulong bP=static_cast<ulong>(tcursor.FindPositionInNativeLine(sText,charPos));
    sText.insert(bP,in_text);
    (*displayText)[l]=sText;
}

void TextEdit::DeleteFromLine(int in_line,const int beginPos,const int endPos,bool keepLF){
    //low level function, deletes some text, caller will update display and read line again
    ulong l=static_cast<ulong>(in_line);
    string sText=(*displayText)[l];   
    ulong sTextLgth=sText.size();
    char lastchar=sText.back();
    ulong bP=static_cast<ulong>(tcursor.FindPositionInNativeLine(sText,beginPos));
    ulong eP=static_cast<ulong>(tcursor.FindPositionInNativeLine(sText,endPos));
    if (!keepLF&&endPos==tcursor.GetLastPositionOfLine(in_line))
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
    if (tcursor.HasCursor()) {
        cursorLine=static_cast<ulong>(tcursor.GetLine());
        cursorPos=tcursor.FindPositionInNativeLine((*displayText)[cursorLine],cursorPos);
    }
    bool hardEnd(false);
    do{
        retString+=(*displayText)[l];
        if (l<cursorLine){
            cursorPos+=static_cast<int>(retString.length());
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
    int startPS=tcursor.GetSelectionStartCharPos(),
        endPS  =tcursor.GetSelectionEndCharPos(),
        startL =tcursor.GetSelectionStartLine(),
        endL   =tcursor.GetSelectionEndLine();
    ulong uStart=static_cast<ulong>(startL);

    if (endL==startL) { //a part of only one line selected
        DeleteFromLine(startL,startPS,endPS,true);
    }

    else{               //selection spawns several lines
        for (int ln(endL);ln>=startL;ln--){
           if (ln==startL)//first line
               DeleteFromLine(ln,startPS,tcursor.GetLastPositionOfLine(ln),false);
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
                    tcursor.EraseCursor();
                    return;
                }
            }else{
                DeleteWholeLine(startL+1);
            }
        }
        //Merge first line of selection with last line before recalculating
        if ((uStart+1)<totalNbL){
            (*displayText)[uStart]=(*displayText)[uStart]+(*displayText)[uStart+1];
            DeleteWholeLine(startL+1);
        }
    }

    while (startL>=totalNbL) startL--;
    tcursor.SetCursorAt(startL,startPS);
}

void TextEdit::ReadLineAgain(int in_line){
    /* function called from all high level functions
     * makes display of edited text seamless
     * processes the edited line(s) again, from the starting line to the next LF char.
     * Follows cursor position.
     * Scrolls the display if necessary, synchronizes scrollbox
     * Synchronizing the display should be done by the caller
     * but this routine follows cursor position if cursor is in the lines proceeded.
     * This is an intermediate level function but does all the work.
     */
    hasToCommit=true;
    int last_line(0),cursorPos(0);
    ulong l=static_cast<ulong>(in_line);
    if (in_line>0) //go to find the first line to include in new processing
    {
        char lastchar=(*displayText)[l-1].back();
        if (lastchar!='\n'){
            l--;
            in_line--;}
    }

    int procSoFar(0),newline(0),newPos(0);
    if (tcursor.HasCursor()) {cursorPos=tcursor.GetPos();newline=tcursor.GetLine();}
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
            int leftStringsize=tcursor.GetLengthOfUTFString(leftString);
            if ((cursorPos>=procSoFar) && (cursorPos<(procSoFar+leftStringsize))) {
                newline=static_cast<int>(l);
                newPos=cursorPos-procSoFar;
                DrawLogic::WriteDebug(" newPos in iteration = "+std::to_string(newPos));

            }
            procSoFar+=leftStringsize;
            if (l<=uLastLine){
               (*displayText)[l]=leftString;
                  tcursor.MakeLinePosAgain(l,leftString);
            }
            else{
               InsertEmptyLine(static_cast<int>(l));
               (*displayText)[l]=leftString;
               tcursor.MakeLinePosAgain(l,leftString);
            }
            l++;
            sz=MeasureString(inputL);
        }
        //int inputLSize=tcursor.GetLengthOfUTFString(inputL);
        int inputLSize=static_cast<int>(inputL.size());
        if ((cursorPos>=procSoFar) && (cursorPos<=(procSoFar+inputLSize))) {
            newline=static_cast<int>(l);
            newPos=cursorPos-procSoFar;
        }

        procSoFar+=inputLSize;
        if (l<=uLastLine){
           (*displayText)[l]=inputL;
            tcursor.MakeLinePosAgain(l,inputL);
            l++;
        }
        else{
           InsertEmptyLine(static_cast<int>(l));
           (*displayText)[l]=inputL;
           tcursor.MakeLinePosAgain(l,inputL);
           l++;
        }
    }
    //delete lines if l<el
    while (l<=uLastLine){
          DeleteWholeLine(static_cast<int>(l));
          l++;
    }
    indxLastPage=totalNbL-pageHeightInL;
    if (indxLastPage<0) indxLastPage=0;
    scrB.Setup(heightPx,totalNbL,indxFirstOnPg,pageHeightInL,textOnly.GetWidth()+grlOffsetX,grlOffsetY);
    if (indxFirstOnPg>indxLastPage) indxFirstOnPg=indxLastPage;//I don't redefine indxFirstOnPage except if the display has shrunken
    indxLastOnPg=indxFirstOnPg+pageHeightInL;
    if (indxLastOnPg>=totalNbL) indxLastOnPg=totalNbL-1;

    if (tcursor.HasCursor()&(tcursor.GetLine()>=(in_line-1))) {
        string toConvert=(*displayText)[static_cast<ulong>(newline)].substr(0,static_cast<ulong>(newPos));
        int newP=tcursor.GetLengthOfUTFString(toConvert);
        tcursor.SetCursorAt(newline,newP);
        if (newline>=indxLastOnPg&&newline<totalNbL){
            indxFirstOnPg=newline-pageHeightInL+2;
            if (indxFirstOnPg<0) indxFirstOnPg=0;
            if (indxFirstOnPg>indxLastPage) indxFirstOnPg=indxLastPage;
            scrB.SetPosFirstLine(indxFirstOnPg);
        }
    }
}

/* The following are top level entry functions, called after a command is issued by the user
 *  they have to update the display, cursor position, leaves the file in
 *  a useable state. When one of these commands is issued, "save" is enabled and considered. Intermediate savings to a
 * buffer file will sometimes take place.
 */

void TextEdit::Cut(){
    if (tcursor.HasSelection()){
        int line(tcursor.GetSelectionStartLine()),pos(tcursor.GetSelectionStartCharPos());
        Copy();
        DeleteSelection();
        tcursor.SetCursorAt(line,pos);
        ReadLineAgain(line);
        DisplayPage();
    }
}

void TextEdit::Copy(){

    if (tcursor.HasSelection()){
        string copyString("");
        ulong bPos(static_cast<ulong>(tcursor.GetSelectionStartCharPos())),
              ePos(static_cast<ulong>(tcursor.GetSelectionEndCharPos())),
              bLine(static_cast<ulong>(tcursor.GetSelectionStartLine())),
              eLine(static_cast<ulong>(tcursor.GetSelectionEndLine()));

            if (bLine==eLine) {
                string sText=(*displayText)[bLine];
                int bp=tcursor.FindPositionInNativeLine(sText,static_cast<int>(bPos));
                int ep=tcursor.FindPositionInNativeLine(sText,static_cast<int>(ePos));
                copyString=sText.substr(bp,ep-bp);

            }else{
                for (ulong selLine(bLine);selLine<=eLine;selLine++){
                    if (selLine==bLine){
                        int bp=tcursor.FindPositionInNativeLine((*displayText)[selLine],static_cast<int>(bPos));
                        //copyString+=(*displayText)[selLine].substr(bp,(*displayText)[selLine].length()-bp);
                        copyString+=(*displayText)[selLine].substr(bp);
                     }
                     else{
                        if (selLine==eLine){
                            int ep=tcursor.FindPositionInNativeLine((*displayText)[selLine],static_cast<int>(ePos));
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

    string toPaste=tcursor.ReadLineToUTF(localClip.PullText());
        if (tcursor.HasSelection()&&toPaste!=""){
            int cLine=tcursor.GetSelectionStartLine();
            int cPos =tcursor.GetSelectionStartCharPos();
            DeleteSelection();
            if (cLine>=totalNbL){
                cLine--;
                if (cLine<0) {
                    cLine=0;
                }
                cPos=tcursor.GetLastPositionOfLine(cLine);
            }
            string fromClip=localClip.PullText();
            InsertTextInLine(cLine,cPos,toPaste);
            ReadLineAgain(cLine);
            DisplayPage();
        }
        if (tcursor.HasCursor()){
            int cLine=tcursor.GetLine();
            int cPos =tcursor.GetPos();
            InsertTextInLine(cLine,cPos,toPaste);
            int insL=tcursor.GetLengthOfUTFString(toPaste);
            DrawLogic::WriteDebug("Paste old pos "+std::to_string(cPos));
            cPos+=insL;
            DrawLogic::WriteDebug("PAste new pos "+std::to_string(cPos));
            tcursor.AddPositionsToLine(insL);
            tcursor.SetCursorAt(cLine,cPos);
            ReadLineAgain(cLine);
            DisplayPage();
        }
}

void TextEdit::InsertLetter(std::string fromKey){
    if (tcursor.HasSelection()){
        int lineS=tcursor.GetSelectionStartLine();
        DeleteSelection();
        if (!tcursor.HasCursor()){
            ReadLineAgain(lineS);
            DisplayPage();}
    }

    if (tcursor.HasCursor()){
    int cursorLine=tcursor.GetLine();
    int position=tcursor.GetPos();
    InsertTextInLine(cursorLine,position,fromKey);
    int insL=tcursor.GetLengthOfUTFString(fromKey);
    position+=insL;
    tcursor.AddPositionsToLine(insL);
    tcursor.SetCursorAt(cursorLine,position);
    ReadLineAgain(cursorLine);
    DisplayPage();
    }
}

void TextEdit::Backspace(){

    if (tcursor.HasCursor()){
       int cLine=tcursor.GetLine(),
           cPos=tcursor.GetPos();
       ulong uln=static_cast<ulong>(cLine);

       if (cLine==0&&cPos==0) return;

       if (cPos==0){//cursor at begin of line, results in cursor moving up, deleting last char of previous line (LF or other)

               int lastPosOfPrevLine=tcursor.GetLastPositionOfLine(static_cast<int>(uln-1));
               tcursor.SetCursorAt(cLine-1,lastPosOfPrevLine);

               string bufl=(*displayText)[uln],buf=(*displayText)[uln-1];
               if (buf.substr(buf.length()-1,1)=="\n")
                   buf=buf.substr(0,buf.length()-1);
               else{
                   buf=stringOps::RemoveLastUTFCharFromString(buf);
                   tcursor.MoveCursorLeft();
               }
               (*displayText)[uln-1]=buf+bufl;
               DeleteWholeLine(cLine);
               ReadLineAgain(cLine-1);
               DisplayPage();
        }
       else{

           DeleteFromLine(cLine,cPos-1,cPos,true);
           tcursor.MoveCursorLeft();
           ReadLineAgain(cLine);
           DisplayPage();
       }
    }
    if (tcursor.HasSelection()){
        int ln(tcursor.GetSelectionStartLine());
        DeleteSelection();
        if (tcursor.HasCursor())
            ReadLineAgain(ln);
        DisplayPage();
    }
}

void TextEdit::Suppress(){
    if (tcursor.HasCursor()){
        int ln(tcursor.GetLine()),pos(tcursor.GetPos());
        ulong uln=static_cast<ulong>(ln);
        if (pos>=tcursor.GetLastPositionOfLine(ln)) {
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
    if (tcursor.HasSelection()){
        int ln=tcursor.GetSelectionStartLine();
        DeleteSelection();
        if (tcursor.HasCursor()) ReadLineAgain(ln);
        DisplayPage();
    }
}

void TextEdit::Enter(){
    if (tcursor.HasSelection()){
        DeleteSelection();
    }
    if (tcursor.HasCursor()){
        int lineTag=tcursor.GetLine();
        int pos=tcursor.GetPos();
        InsertTextInLine(lineTag,pos,string("\n"));
        ReadLineAgain(lineTag);
        DisplayPage();
        if (lineTag<(totalNbL-1)){
            tcursor.SetCursorAt(lineTag+1,0);
            if (lineTag+1>indxLastOnPg) MoveDnNLines(1);
        }
    }
}
