#include "textreader.h"
#include "drawlogic.h"

TextReader::TextReader(): List_Box_With_ScrB(false),
    freqADF(0),
    freqNAV(0),
    freqCOM(0.0f),
    fileName(""),
    strBuffADF(""),
    strBuffNav(""),
    strBuffCom(""),
    digits("0123456789"),
    textFile(),
    fileExists(false),
    needsUTF8(false),
    streamOn(false),
    hasNav(false),hasCom(false),hasADF(false),
    //fT(),
    filePath(),
    keepsize(0)

{
    fileName=FilePointer::GetCurrentFile();
}

TextReader::~TextReader(){
}

void TextReader::PointToFile(){
    fileName=FilePointer::GetCurrentFile();
    DrawLogic::WriteDebug("TextReader pointed to file "+fileName);
}

bool TextReader::OpenFile(){ //sets textFile,FileName and FileExists

    if (FilePointer::OpenCurrentFile(textFile)){
        fileExists=true;
        filePath=fileName;
        //fT=last_write_time(filePath);
        DrawLogic::WriteDebug("TextReader going to read size for "+filePath.string());
        keepsize=FilePointer::GetSizeOfCurrentFile();
    }else{
        DrawLogic::WriteDebug("Couldn't open file "+fileName);
        fileExists=false;
        fileName="";}
    return fileExists;
}

std::string TextReader::GetFileName(){
    if (fileExists){
        return FilePointer::GetCurrentFileName();}
   else return(string ("File not found "+fileName));
}

std::string TextReader::GetStemFileName(){//returns the file Name without Extension
    return FilePointer::GetCurrentNameSansExt();
}

std::string TextReader::GetDirName(){
    if (fileExists){
        return FilePointer::GetCurrentDirName();
        }
    else return(string ("Directory not found "+FilePointer::GetCurrentDirName()));
}

bool TextReader::ReadFileToBuff(){ //defines MaxLineLength and reads all lines into buffer, breaks at spaces,"-","(",or")" or at maxLineLength
if (fileName!=""){
  if (!FilePointer::OpenCurrentFile(textFile)) {
          std::string inputL="the file "+fileName+" couldn't be found";
          fileExists=false;
          AddLine(inputL);
        return true;}
  clearText();
  //stringOps ops;
  std::string inputL;
  filePos=0;
  while (getline(textFile,inputL)){
     AddLine(inputL);
     filePos++;
     }
  textFile.close();  
  SetupforText();}
  return true;
}

void TextReader::AppendToStream(){
    //Check if file is bigger than before
    auto currentSize=FilePointer::GetSizeOfCurrentFile();
    DrawLogic::WriteDebug("before comparison, new size, old size ",filePos,currentSize,keepsize);
    if (currentSize!=keepsize){
        bool bigger(currentSize>keepsize);
        if (!OpenFile()) return;
        //Read from last entry point
        string inputL;
        DrawLogic::WriteDebug("number of lines in file, new size, old size ",filePos,currentSize,keepsize);
        if (bigger){
            DrawLogic::WriteDebug("number of lines in file is and size ",filePos,currentSize);
            for (int line(0);line<filePos;line++) getline(textFile,inputL); //skip lines already read
            while (getline(textFile,inputL)){
                AddLine(inputL);
                filePos++;
            }
            textFile.close();
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
            keepsize=currentSize;
        }
        else DoReloadNotStream();
    }
    GoToLastPage();
}

void TextReader::ShowAll(){
    ReadFileToBuff();
    hasSelection=false;
    hasHiddenLines=false;
}

bool TextReader::Reload(){
/*Reloads the file but keeps firstLineOnPage if possible
 * restores this position or if >lastLineOnPage will be reset to that value*/
    if (streamOn){
        AppendToStream();
        return true;
    }
    return (DoReloadNotStream());

}

bool TextReader::DoReloadNotStream(){
    int posOfL(indxFirstOnPg);
    bool lastPage(false);
    if (indxFirstOnPg==indxLastPage) lastPage=true;
    auto oldSize=keepsize;
    ShowAll();
    if (lastPage){ GoToLineN(indxLastPage);}
    else GoToLineN(posOfL);
    auto currentSize=FilePointer::GetSizeOfCurrentFile();
    if (currentSize!=oldSize){
       keepsize=currentSize;
       return true;}
    return false;
}

bool TextReader::ReloadIfSizeChanged(){
/*Reloads the file but keeps firstLineOnPage if possible
 * restores this position or if >lastLineOnPage will be reset to that value*/
    if (streamOn){
        AppendToStream();
        return true;
    }
    bool changed(false);
    auto currentSize=FilePointer::GetSizeOfCurrentFile();

    if (currentSize!=keepsize){
        changed=true;
        keepsize=currentSize;
        bool lastPage=false;
        if (indxFirstOnPg==indxLastPage) lastPage=true;
        int posOfL=indxFirstOnPg;
        ShowAll();
        if (lastPage) posOfL=indxLastPage;
        GoToLineN(posOfL); }
    return changed;
}

void TextReader::closeReader(){
    ResetFrequencies();
    clearText();
    hasSelection=false;hasHiddenLines=false;
}
bool TextReader::HasNav(){
    return hasNav;
}
bool TextReader::HasCom(){
    return hasCom;
}
bool TextReader::HasADF(){
    return hasADF;
}

void TextReader::CheckForFrequencies(){
    if (HasSelection()){
    float floatRead;
    int intRead;
    size_t pos;
    std::string remnant=StringSelected();
    //ResetFrequencies();
    while (remnant.size()>0){
        if (isADigit(remnant.substr(0,1))){
              intRead=int(std::stof(remnant,&pos));
              if ((intRead>=200)&&(intRead<=1800)) {
                  freqADF=intRead;
                  strBuffADF=std::to_string(freqADF);
                  hasADF=true;
              }
              else {
                  floatRead=std::stof(remnant,&pos);
                  if ((floatRead>=108.00f) && (floatRead<=117.99f)) {
                      freqNAV=static_cast<int>(floatRead*100.0f);
                      std::string conv=std::to_string(freqNAV);
                      strBuffNav=conv.substr(0,3)+"."+conv.substr(3,2);
                      hasNav=true;
                  }
                  if ((floatRead>=118.00f)&&(floatRead<=139.99f)){
                      freqCOM=floatRead*100.0f;
                      std::string conv=std::to_string(freqCOM*10);
                      strBuffCom=conv.substr(0,3)+"."+conv.substr(3,3);
                      hasCom=true;
                  }
              }
              if (pos==0) pos=1;
              remnant=remnant.substr(pos);
        }
        else {remnant=remnant.substr(1);}
    }
    }
}

bool TextReader::isADigit(std::string test){
    if (digits.find(test)!=std::string::npos) return (true);
    else return (false);
}



void TextReader::ResetFrequencies(){
     hasNav=false;hasCom=false;hasADF=false;
     freqADF=0;
     freqNAV=0;
     freqCOM=0;
     strBuffADF="";
     strBuffNav="";
     strBuffCom="";
}

void TextReader::SetNeedsUTF(bool utf)
{
    needsUTF8=utf;
    if (needsUTF8) {
        convertToUTF8();
        DisplayPage();
}
}

void   TextReader::SetStreamingMode(bool in_stream){
    streamOn=in_stream;
}
bool   TextReader::GetStreamingMode(){
    return streamOn;
}

std::string TextReader::GetNavStr(){
    return strBuffNav;
}
std::string TextReader::GetComStr(){
    return strBuffCom;
}
std::string TextReader::GetADFStr(){
    return strBuffADF;
}
