#include "textreader.h"

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
    hasNav(false),hasCom(false),hasADF(false),
    fT(),
    filePath(),
    keepsize(0)

{
    fileName=FilePointer::GetCurrentDirName()+FilePointer::DirSeparator+FilePointer::GetCurrentFileName();
}

TextReader::~TextReader(){

}

void TextReader::PointToFile(){
    fileName=FilePointer::GetCurrentDirName()+FilePointer::DirSeparator+FilePointer::GetCurrentFileName();
}

bool TextReader::OpenFile(){ //sets textFile,FileName and FileExists

    textFile.open(fileName,std::ifstream::in);
    if (textFile.is_open()){
        fileExists=true;
        filePath=fileName;
        fT=last_write_time(filePath);
        keepsize=std::experimental::filesystem::file_size(filePath);
    }else{
        List_Box_With_ScrB::WriteDebug("Couldn't open file "+fileName);
        fileExists=false;
        fileName="";}
    return fileExists;
}

std::string TextReader::GetFileName(){
    if (fileExists){
        return FilePointer::GetCurrentFileName();}
   else return(std::string ("File not found "+fileName));
}

std::string TextReader::GetDirName(){
    if (fileExists){
        return FilePointer::GetCurrentDirName();
        }
    else return(std::string ("Directory not found "+FilePointer::GetCurrentDirName()));
}

bool TextReader::ReadFileToBuff(){ //defines MaxLineLength and reads all lines into buffer, breaks at spaces,"-","(",or")" or at maxLineLength
if (fileName!=""){
  if (!textFile.is_open()) {
      OpenFile();
      if (!textFile.is_open()) {
          std::string inputL="the file "+fileName+" couldn't be found";
          fileExists=false;
          AddLine(inputL);
        return false;}}
  clearText();
  stringOps ops;
  std::string inputL;
  while (getline(textFile,inputL)){
     AddLine(inputL);
     }
  textFile.close();  
  if (needsUTF8) convertToUTF8();
  SetupforText();}
  return true;
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

void TextReader::ShowAll(){
    ReadFileToBuff();
    hasSelection=false;
    hasHiddenLines=false;
}

bool TextReader::Reload(){
/*Reloads the file but keeps firstLineOnPage if possible
 * restores this position or if >lastLineOnPage will be reset to that value*/
    int posOfL=indxFirstOnPg;
    bool lastPage=false;
    if (indxFirstOnPg==indxLastPage) lastPage=true;
    auto oldSize=keepsize;
    ShowAll();
    if (lastPage){ GoToLineN(indxLastPage);}
    else GoToLineN(posOfL);
    auto currentSize=std::experimental::filesystem::file_size(filePath);
    if (currentSize!=oldSize){
       keepsize=currentSize;
       return true;}
    return false;
}

bool TextReader::ReloadIfSizeChanged(){
/*Reloads the file but keeps firstLineOnPage if possible
 * restores this position or if >lastLineOnPage will be reset to that value*/
    bool changed(false);
    if (fileExists){
    auto currentSize=std::experimental::filesystem::file_size(filePath);

    if (currentSize!=keepsize){
        changed=true;
        keepsize=currentSize;
        bool lastPage=false;
        if (indxFirstOnPg==indxLastPage) lastPage=true;
        int posOfL=indxFirstOnPg;
        ShowAll();
        if (lastPage) posOfL=indxLastPage;
        GoToLineN(posOfL); }
    }
    return changed;
}

void TextReader::closeReader(){
    ResetFrequencies();
    clearText();
    hasSelection=false;hasHiddenLines=false;
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

std::string TextReader::GetNavStr(){
    return strBuffNav;
}
std::string TextReader::GetComStr(){
    return strBuffCom;
}
std::string TextReader::GetADFStr(){
    return strBuffADF;
}
