#include "filepointer.h"

string FilePointer::currentFileName("fn");
string FilePointer::currentDirName("dn");
string FilePointer::DirSeparator("\\");

FilePointer::FilePointer()
{

}
string FilePointer::GetCurrentFileName(){

    return currentFileName;
}

void FilePointer::SetCurrentFileName(string in_Name){
    currentFileName=in_Name;

}

void   FilePointer::SetCurrentDirName(string in_Dir){
    if (in_Dir.substr(in_Dir.length()-1,1)==DirSeparator) in_Dir=in_Dir.substr(0,in_Dir.length()-1);
    currentDirName=in_Dir;
}
string FilePointer::GetCurrentDirName(){
    return currentDirName;
}
