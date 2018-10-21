#include "cursor.h"

cursor::cursor():
  timePoint(0),
  testChar(0), 
  nbOfLines(0),
  line(0),
  charPos(0),
  cursorX(0),
  startCharPos(0),
  startCursorX(0),
  startLine(0),
  selectionEndLine(0),
  selectionEndCharPos(0),
  selectionEndCursorX(0),
  selectionStartLine(0),
  selectionStartCharPos(0),
  selectionStartCursorX(0),
  currentLine(0),
  currentPos(0),
  currentX(0),
  offX(0),
  offY(0),
  firstLineDisplayed(0),
  nbBoxLines(0),
  indexFirstOnPage(0),
  indexLastOnPage(0),
  hasCursor(false),
  hasSelection(false),
  validUTF(true),
  beginUTF(true),
  cursorOn(true),
  moveSelectionEnd(true),
  chPos(),
  chLength(),
  posLines(),
  ink{0.005f,0.000f,0.000f},
  lightblue{0.600f,0.960f,1.0f},
  ops()


{
//will make chLength, table containing character lengths
    cursorChar="|";
    int nulval(0);
    for (int tg(0);tg<=0xdfc0;tg++) chLength.push_back(nulval);
    unsigned char* tst=new unsigned char[3];
    float lgth(0);
    tst[1]='0';
    for (unsigned char chr(0x20);chr<=0x7E;chr++){
        tst[0]=chr;
        lgth=XPLMMeasureString(xplmFont_Proportional,reinterpret_cast<char*>(tst),1);
        ulong tag=static_cast<ulong>(chr);
        chLength[tag]=static_cast<int>(lgth);
    }
    tst[2]='\0';
    for (unsigned char ch1(0xC2);ch1<=0xdf;ch1++){
        tst[0]=ch1;
        for (unsigned char ch2(0x80);ch2<=0xBF;ch2++){
            tst[1]=ch2;
            lgth=XPLMMeasureString(xplmFont_Proportional,reinterpret_cast<char*>(tst),1);
            ulong tag=ch1*0x100+ch2;
            chLength[tag]=static_cast<int>(lgth);
        }
        Layout::InitCycleDebug(100);
    }
}

cursor::~cursor(){

}

void cursor::Initiate(const strVector *inLines,int offsetXLine, int offsetYLine,int lnHeight,int boxlines){
    //makes chPos, table of absolute positions of characters WRT beginning offsetX of lines
    EraseCursor();
    posLines.clear();
    offX=offsetXLine;//will have to be updated when recalculate occurs in reader
    offY=offsetYLine;//id
    lineHeight=lnHeight;
    nbOfLines=inLines->size();
    nbBoxLines=boxlines;

    for (string line:(*inLines)){//examine each line
        MakeLinePos(line);
        posLines.push_back(chPos);
    }//file finished
}

void cursor::MakeLinePos(const string &thisline){
    int position(0);
    unsigned char keep(0);
    unsigned int tag;
    chPos.clear();
    chPos.push_back(position);
    for (unsigned char ansiC:thisline){//going to examine every char of the text in the line
        if (ansiC!='\n')
        {
            if (ansiC<=0x7F) {//first case, simple char
                if (!beginUTF){//but if a putative UTF first character has been read, we are invalid UTF...
                    beginUTF=true;
                    tag=UTFint(keep);
                    position=position+chLength[tag];
                    chPos.push_back(position);
                    position=position+chLength[ansiC];
                    chPos.push_back(position);
                }
                position=position+chLength[ansiC];//simple character
                chPos.push_back(position);
            }
            else{
                if (beginUTF){//look at first potential UTF char
                    if (ansiC>=0xC2 && ansiC<=0xDF) {//if range Ok, keep the char for further investigation
                        beginUTF=false;keep=ansiC;
                    }
                    else {//invalid since out of range
                        tag=UTFint(ansiC);
                        position=position+chLength[tag];
                        chPos.push_back(position);
                    }
                }
                else{//a valid UTF first char has been read
                    beginUTF=true;
                    if (ansiC>=0x80 && ansiC<=0xBF){ // range for second char OK ?
                        tag=keep*0x100+ansiC;
                        position=position+chLength[tag];
                        chPos.push_back(position);
                    }
                    else{//again not valid range for second character
                        beginUTF=true;
                        tag=UTFint(keep);
                        position=position+chLength[tag];
                        chPos.push_back(position);
                        tag=UTFint(ansiC);//because ansiC>127 otherwise would have already been captured as a non valid second UTF char
                        position=position+chLength[tag];
                        chPos.push_back(position);
                    }
                }
            }
        }
    }//line finished
}

void cursor::MakeLinePosAgain(const ulong line,const string &thisline){
    MakeLinePos(thisline);
    posLines[line]=chPos;
}

int cursor::FindPositionInNativeLine(const string &in_string,int in_pos){
    int posInString(0),posInDisplayedString(0);
    if (in_pos==0) return 0;
    bool afterFirstUtfChar(false);
    for (unsigned char ansiC:in_string){
        if (ansiC<=0x7F){
            posInString++;
            posInDisplayedString++;
        }
        else {
            if (afterFirstUtfChar){
                posInDisplayedString++;
                posInString++;
                afterFirstUtfChar=false;
            }
            else{
                afterFirstUtfChar=true;
                posInString++;
            }
        }
        if (posInDisplayedString==in_pos) break;
    }
    return posInString;
}

int  cursor::GetLengthOfUTFString(string inLine){
    if (inLine=="") return 0;
    int length(0);
    bool afterFirstUtfChar(false);
    for (unsigned char ansiC:inLine){
        if (ansiC<=0x7F){
            length++;
        }
        else {
            if (afterFirstUtfChar){
                length++;
                afterFirstUtfChar=false;
            }
            else{
                afterFirstUtfChar=true;
            }
        }
    }
    return length;
}

void cursor::Recalculate(int oX,int oY){
    int oldOffX=offX;
    offX=oX;
    offY=oY+1;
    if (hasCursor){cursorX=cursorX-oldOffX+oX;}
    if (hasSelection){
        selectionEndCursorX=selectionEndCursorX-oldOffX+oX;
        selectionStartCursorX=selectionStartCursorX-oldOffX+oX;
    }
}

void cursor::SetIndxFirstPage(int indx){
    indexFirstOnPage=indx;
    indexLastOnPage=indx+nbOfLines-1;
}

void cursor::MakeStart(){
    startCharPos=charPos;
    startCursorX=cursorX;
    startLine=line;
}

void cursor::CursorFromCurrent(){
    line=currentLine;
    charPos=currentPos;
    cursorX=currentX;
    hasCursor=true;
    hasSelection=false;
}

void cursor::SelectionRegular(){
    selectionStartLine=startLine;
    selectionStartCharPos=startCharPos;
    selectionStartCursorX=startCursorX;
    selectionEndLine=currentLine;
    selectionEndCharPos=currentPos;
    selectionEndCursorX=currentX;
}

void cursor::SelectionInverted(){
    selectionEndLine=startLine;
    selectionEndCharPos=startCharPos;
    selectionEndCursorX=startCursorX;
    selectionStartLine=currentLine;
    selectionStartCharPos=currentPos;
    selectionStartCursorX=currentX;
}

void cursor::FindClosestXPosition(int inLine, int inX){

    ulong l(0);
    ulong lineNb=static_cast<ulong>(inLine);
    bool notFound(true);
    ulong maxPos=static_cast<ulong>(GetLastPositionOfLine(inLine));

    currentLine=inLine;
    do{

        if (inX<(posLines[lineNb][l]+offX+2) ){//a slightly offset detection for having a detection not too close to next char
            notFound=false;
            currentX=posLines[lineNb][l]+offX-1;//-1 for displaying cursor really between 2 chars
        }
        else {
            l++;
        }
    }
    while (notFound&&(l<=maxPos));

    if (l>maxPos) l=maxPos;
    if (inX>posLines[lineNb][l]) currentX=posLines[lineNb][l]+offX-1;
    currentPos=static_cast<int>(l);
}

void cursor::FindPos(int inLine,int inX){
  //the basic cursor positionning
  //gets a line number and a pixel number, converts to a position, sets line and charPos hasSelection=false
    StopSelection();
    currentX=offX-1;
    if (inLine<nbOfLines){
        FindClosestXPosition(inLine,inX);
        CursorFromCurrent();
        MakeStart();
    }
    else
       { EraseCursor();}
}

void cursor::DragPos (int inLine, int inX){
    // informed a drag occurs, tracks selectionEndLine and selectionEndCharPos, modifies them if necessary hasSelection=true
    //doesn't know about scrolling, should be taken care of by textedit then setindxFirstPage here

    if (inLine<nbOfLines){
        FindClosestXPosition(inLine,inX);
           // compute the selection parameters
        if ((currentLine==startLine)&&(currentPos==startCharPos)){
            CursorFromCurrent();
        }
        else{
            hasSelection=true;
            hasCursor=false;
            if (currentLine<startLine){
                SelectionInverted();
                return;
            }
            if (currentLine==startLine){
                if (currentX<startCursorX) SelectionInverted();
                else SelectionRegular();
                return;
            }
            else SelectionRegular();
        }
    }
}

void cursor::EndOfDrag(){
    currentX=0;currentLine=0;currentPos=0;

    if (hasCursor) {
        StopSelection();
        return;
    }
}

void cursor::FlipSelection(){
    moveSelectionEnd=!moveSelectionEnd;
}

bool cursor::IsMoveSelectionEnd(){
    return moveSelectionEnd;
}

void cursor::MoveCursorRight(){
    ulong ln=static_cast<ulong>(line);
    ulong maxPos=posLines[ln].size()-1;
    if (static_cast<ulong>(charPos)<maxPos){
        charPos++;
        cursorX=posLines[ln][static_cast<ulong>(charPos)]+offX-1;
    }
}

void cursor::MoveCursorLeft(){
    ulong ln=static_cast<ulong>(line);
    if (charPos>0){
        charPos--;
        cursorX=posLines[ln][static_cast<ulong>(charPos)]+offX-1;
    }
}

void cursor::MoveCursorUp(){
    if (!hasSelection&&hasCursor){
        if (line>0){
            line--;
            FindClosestPosition(line,cursorX,charPos);
        }
    }
}

void cursor::MoveCursorDown(){
    if (!hasSelection&&hasCursor){
        if (line<(nbOfLines-1)){
            line++;
            FindClosestPosition(line,cursorX,charPos);
        }
    }
}

void cursor::MoveSelectionUp(){
//Move selected end or begin of selection up one line if possible
    if (hasSelection){
        if (moveSelectionEnd){
            int diff=selectionEndLine-selectionStartLine;
            if (diff>1){
                selectionEndLine--;
                FindClosestPosition(selectionEndLine,selectionEndCursorX,selectionEndCharPos);
            }
            if ((diff==1)&&(selectionEndCursorX>selectionStartCursorX)){
                    selectionEndLine--;
                    FindClosestPosition(selectionEndLine,selectionEndCursorX,selectionEndCharPos);
                    if (selectionEndCharPos==selectionStartCharPos){
                        line=selectionStartLine;
                        charPos=selectionStartCharPos;
                        cursorX=selectionStartCursorX;
                        hasCursor=true;
                        StopSelection();
                    }
            }
        }
        else{
            if (selectionStartLine>0)
            {
                selectionStartLine--;
                FindClosestPosition(selectionStartLine,selectionStartCursorX,selectionStartCharPos);
            }
        }
    }
}

void cursor::MoveSelectionDown(){
    if (hasSelection){
        if (moveSelectionEnd)
        {
            if (selectionEndLine<(nbOfLines-1))
            {
                selectionEndLine++;
                FindClosestPosition(selectionEndLine,selectionEndCursorX,selectionEndCharPos);
            }
        }else
        {
            int diff=selectionEndLine-selectionStartLine;
            if (diff>1)
            {
                selectionStartLine++;
                FindClosestPosition(selectionStartLine,selectionStartCursorX,selectionStartCharPos);
            }
            if ((diff==1)&&(selectionEndCursorX>selectionStartCursorX)){
                selectionStartLine++;
                FindClosestPosition(selectionStartLine,selectionStartCursorX,selectionStartCharPos);
                if (selectionEndCharPos==selectionStartCharPos){
                    line=selectionStartLine;
                    charPos=selectionStartCharPos;
                    cursorX=selectionStartCursorX;
                    hasCursor=true;
                    StopSelection();
                }
            }
        }
    }
}

void cursor::MoveSelectionRight(){
    int limitStart=static_cast<int>(posLines[selectionStartLine].size());
    int limitEnd=static_cast<int>(posLines[selectionEndLine].size());
    if (hasSelection){

            if (moveSelectionEnd){
                if (selectionEndCharPos<(limitEnd-1))
                {
                    selectionEndCharPos++;
                    selectionEndCursorX=posLines[selectionEndLine][static_cast<ulong >(selectionEndCharPos)]+offX-1;
                }
            }else{
                if (selectionEndLine==selectionStartLine)
                {
                    if (selectionStartCharPos<(selectionEndCharPos-1)){
                        selectionStartCharPos++;
                        selectionStartCursorX=posLines[selectionStartLine][static_cast<ulong >(selectionStartCharPos)]+offX-1;
                    }
                }
                else{
                if (selectionStartCharPos<(limitStart-1))
                {
                    selectionStartCharPos++;
                    selectionStartCursorX=posLines[selectionStartLine][static_cast<ulong >(selectionStartCharPos)]+offX-1;
                }
            }
        }
    }
}

void cursor::MoveSelectionLeft(){
    if (hasSelection){
            if (!moveSelectionEnd){
                if (selectionStartCharPos>0)
                {
                    selectionStartCharPos--;
                    selectionStartCursorX=posLines[selectionStartLine][static_cast<ulong >(selectionStartCharPos)]+offX-1;
                }
            }
            else{
                if (selectionStartLine==selectionEndLine){
                   if (selectionEndCharPos>(selectionStartCharPos+1))
                   {
                       selectionEndCharPos--;
                       selectionEndCursorX=posLines[selectionEndLine][static_cast<ulong >(selectionEndCharPos)]+offX-1;
                   }else{
                       if (selectionEndCharPos>0)
                       {
                          selectionEndCharPos--;
                          selectionEndCursorX=posLines[selectionEndLine][static_cast<ulong >(selectionEndCharPos)]+offX-1;
                       }
                   }
                }
            }
    }
}

void cursor::SwapEndsOfSelection(){
    moveSelectionEnd=!moveSelectionEnd;
}

void cursor::FindClosestPosition(const int &ln,int &cX, int &pos){
    //looks for closest position to pos on ln
    cX=cX-offX+1;
    const ulong currentLine=static_cast<ulong >(ln);
    int foundLength(10000),foundPos(0);
    ulong nb=posLines[currentLine].size();
    for (ulong position(0);position<nb;position++){
        int diff=abs(posLines[currentLine][position]-cX);
        if (diff<foundLength) {
            foundPos=static_cast<int>(position);
            foundLength=diff;
        }
    }
    pos=foundPos;
    cX=posLines[currentLine][static_cast<ulong >(foundPos)]+offX-1;

}

void cursor::MakeCursorFromSelectionStart(){
    cursorX=selectionStartCursorX;
    line=selectionStartLine;
    charPos=selectionStartCharPos;
    StopSelection();
}

int  cursor::GetPos(){//position of cursor on line
    return charPos;
}

int  cursor::GetLine(){//position of cursor in line number
    return line;
}

int  cursor::GetSelectionStartLine(){
    return selectionStartLine;
}

int  cursor::GetSelectionEndLine(){// last line selected
    return selectionEndLine;
}

int  cursor::GetSelectionStartCharPos(){
    return selectionStartCharPos;
}

int  cursor::GetSelectionEndCharPos(){
    return selectionEndCharPos;
}

int  cursor::GetLastPositionOfLine(int in_line){
      ulong ln=static_cast<ulong>(in_line);
      ulong maxPos=posLines[ln].size();
      if (maxPos>0) maxPos--;
      return static_cast<int>(maxPos);
}
void cursor::AddPositionsToLine(int nbOfP){
    if (hasCursor){
        for (int I(0);I<nbOfP;I++)
            posLines[static_cast<ulong>(line)].push_back(0);
    }
}

void cursor::SetCursorAt(int in_line, int in_pos){
    EraseCursor();//then Check if values are valid
    if (in_line>=nbOfLines) return;
    if (in_pos==-1)
         charPos=GetLastPositionOfLine(in_line);
    else charPos=in_pos;
    if (charPos>GetLastPositionOfLine(in_line)) {
        charPos=0;
        return;
    }
    hasCursor=true;//if values were valid, make the cursor
    line=in_line;
    ulong uln=static_cast<ulong>(in_line),upos=static_cast<ulong>(in_pos);
    cursorX=posLines[uln][upos]+offX-1;
}

bool cursor::HasCursor(){
    return hasCursor;
}

bool cursor::HasSelection(){
    return hasSelection;
}

bool cursor::IsIndexInSelection(int id,int &left,int &right){
//gets a line number, checks if it is visible in the text window, if yes checks if it is part of an eventual selection
    if (hasSelection){
        int idx=id+indexFirstOnPage;
        if (idx<indexFirstOnPage) return false;
        if (idx>indexLastOnPage) return false;
        if (selectionEndLine==selectionStartLine){
            if (idx==selectionStartLine){//selection on one line, the index received is this line
            left=selectionStartCursorX;
            right=selectionEndCursorX;
            return true;}
            else return false;
        }
        else{
            if (idx==selectionStartLine){
                left=selectionStartCursorX;
                right=offX+posLines[idx][posLines[idx].size()-1];
                return true;
            }
            if (idx==selectionEndLine){
                left=offX;
                right=selectionEndCursorX;
                return true;
            }
            if (idx>selectionStartLine&&idx<selectionEndLine){
                left=offX;
                right=offX+posLines[idx][posLines[idx].size()-1];
                return true;
            }
        }
        return false;
    }else return false;
}

void cursor::StopSelection(){
    hasSelection=false;
    selectionEndLine=0; selectionEndCharPos=0;selectionEndCursorX=0;
    selectionStartCursorX=0;selectionStartCharPos=0;selectionStartLine=0;
}

void cursor::EraseCursor(){
    StopSelection();
    hasCursor=false;
    line=0;charPos=0;cursorX=0;
}

int  cursor::PosToX(){//returns the position of the cursor in pixels
    return cursorX;
}

int  cursor::LineToY(int firstLine){
    int boxLine=(line<firstLine?0:line-firstLine);
    return (offY-(boxLine)*(lineHeight+2));
}

int  cursor::EndPosToX(){
    return selectionEndCursorX;
}

int  cursor::EndLineToY(int firstLine){
    int yL=offY-(selectionEndLine-firstLine)*(lineHeight+2);
    return yL;
}

void cursor::DrawRectangle(int left, int top, int right, int bottom){

    glColor3fv(lightblue);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glBegin(GL_TRIANGLE_FAN);
   {
     glVertex2i(left, top);
     glVertex2i(right, top);
     glVertex2i(right, bottom);
     glVertex2i(left, bottom);
   }
   glEnd();

}

void cursor::DrawCursor(int cursorY){
//draws a blinking cursor, textedit checks if the cursor is at the right line
    if (hasCursor&&!hasSelection){
    float now=XPLMGetElapsedTime();
    if (cursorOn&&(now-timePoint)>1.0f){//if cursor shown >1s, dont show it
        timePoint=now;
        cursorOn=false;
    }
    if (!cursorOn&&(now-timePoint)>0.5f){//if cursor is hidden 0,5s show it
        timePoint=now;
        cursorOn=true;
    }
    if (cursorOn){
        string crsr=string(cursorChar);
        XPLMDrawString(ink,cursorX,cursorY,(char*)cursorChar,nullptr,xplmFont_Proportional);
    }
   }
}

string cursor::ReadLineToUTF(string inLine){
    //gets a string, if it finds a char not UTF8, supposes it is ANSI and converts to UTF
    string retString("");
    unsigned char keep(0);

    for (unsigned char ansiC:inLine){//going to examine every char of the text in the line

        if (ansiC<=0x7F) {//first case, simple char
            if (!beginUTF){//but if a putative UTF first character has been read, we are invalid UTF...
                beginUTF=true;
                string toMod(1,keep);
                retString+=ops.ToUTF8(toMod);
                retString.append(1,ansiC);
            }
            retString.append(1,ansiC);
        }
        else{
            if (beginUTF){//look at first potential UTF char
               if (ansiC>=0xC2 && ansiC<=0xDF) {//if range Ok, keep the char for further investigation
                  beginUTF=false;keep=ansiC;
               }
               else{//invalid since out of range
                  string toMod(1,ansiC);
                  retString+=ops.ToUTF8(toMod);
               }
             }
             else{//a valid UTF first char has been read
                beginUTF=true;
                if (ansiC>=0x80 && ansiC<=0xBF){ // range for second char OK ?
                    retString.append(1,keep);
                    retString.append(1,ansiC);

                }
                else {//again not valid range for second character
                    string toModkeep(1,keep);
                    retString+=ops.ToUTF8(toModkeep);
                    string toMod(1,ansiC);
                    retString+=ops.ToUTF8(toMod);
                    }
            }
        }
        }
    return  retString;
}

unsigned int  cursor::UTFint(unsigned char in_ANSIchar){
    unsigned int retInt;
    retInt= (unsigned char)((0xC0 | (in_ANSIchar >> 6)));
    retInt=retInt*0x100 + (unsigned char)(0x80 | (in_ANSIchar & 0x3F));
    return retInt;
}

void cursor::DeleteFromLines(int in_line){
    posLines.erase(posLines.begin()+in_line);
    nbOfLines--;
}

void cursor::InsertEmptyLine(int in_line){
    if (in_line<nbOfLines)
    {
        posLines.insert(posLines.begin()+in_line,intVector());
    }
    else{
        posLines.push_back(intVector());
    }
    nbOfLines++;
}
