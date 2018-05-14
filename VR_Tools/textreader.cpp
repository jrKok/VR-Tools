#include "textreader.h"

TextReader::TextReader(): List_Box_With_ScrB(),
    fileName(""),
    strBuffADF(""),
    strBuffNav(""),
    strBuffCom(""),
    textFile(),
    fileExists(false),
    needsUTF8(false),
    hasNav(false),hasCom(false),hasADF(false),
    digits("0123456789"),
    freqADF(0),
    freqNAV(0),
    freqCOM(0.0f),
    fT(),
    filePath(),
    keepsize(0)

{

}

TextReader::TextReader(std::string fileN):List_Box_With_ScrB(),
    fileName(fileN),
    strBuffADF(""),
    strBuffNav(""),
    strBuffCom(""),
    fileExists(false),
    needsUTF8(false),
    hasNav(false),hasCom(false),hasADF(false),
    digits("0123456789"),
    freqADF(0),
    freqNAV(0),
    freqCOM(0),
    fT(),
    filePath(),
    keepsize(0)
{
    textFile.open(fileName,std::ifstream::in);
    if (textFile.is_open()){
        fileExists=true;
        textFile.close();
    }else{
        fileExists=false;
        fileName="";}

}

TextReader::~TextReader(){

}


bool TextReader::OpenFile(std::string name){ //sets textFile,FileName and FileExists
    fileName=name;
    textFile.open(fileName,std::ifstream::in);
    if (textFile.is_open()){
        fileExists=true;
        filePath=fileName;
        fT=fs::last_write_time(filePath);
        keepsize=std::experimental::filesystem::file_size(filePath);
    }else{
        fileExists=false;
        fileName="";}
    return fileExists;
}

std::string TextReader::GetFileName(){
    if (fileExists){
        std::string showName=fileName;
        char* fnm=XPLMExtractFileAndPath((char*)showName.c_str());
        std::string retS=fnm;
        return retS;}
   else return(std::string ("File not found "+fileName));
}

std::string TextReader::GetDirName(){
    if (fileExists){
        std::string showName=fileName;
        char* fnm=XPLMExtractFileAndPath((char*)showName.c_str());
        std::string retS=fnm;
        int fd=fileName.rfind(retS);
        if (fd>0){
           retS=fileName.substr(0,fd-1);
           return retS;}
        else{
           return("");
        }
    }
    else return(std::string ("File not found "+fileName));
}

bool TextReader::ReadFileToBuff(){ //defines MaxLineLength and reads all lines into buffer, breaks at spaces,"-","(",or")" or at maxLineLength

  if (!textFile.is_open()) {
      OpenFile(fileName);
      if (!textFile.is_open()) {
          std::string inputL="the file "+fileName+" couldn't be found";
          fileExists=false;
          AddLine(inputL);
        return false;}}
  clearText();
  stringOps ops;
  std::string inputL;
  while (getline(textFile,inputL)){
     inputL=ops.cleanOut(inputL,"\r");
     //inputL=ops.cleanOut(inputL,"\n");
     AddLine(inputL);
     }
  textFile.close();  
  if (needsUTF8) convertToUTF8();
  SetupforText();
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
                  if ((floatRead>=108.00) && (floatRead<=117.99)) {
                      freqNAV=(int)(floatRead*100.0f);
                      std::string conv=std::to_string(freqNAV);
                      strBuffNav=conv.substr(0,3)+"."+conv.substr(3,2);
                      hasNav=true;
                  }
                  if ((floatRead>=118.00)&&(floatRead<=139.99)){
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
