#include "filepointer.h"

string FilePointer::currentFileName("fn");
string FilePointer::currentDirName("dn");
string FilePointer::DirSeparator("\\");
string FilePointer::currentTemp("");
string FilePointer::currentBackup("");

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

void FilePointer::MakeBackups(){
    string rootName=currentDirName+DirSeparator+currentFileName;
    path p=rootName;
    string ext=p.extension().string();
    string nudeRootName =rootName.substr(0,rootName.find_last_not_of(ext)+1);
    p=nudeRootName+".bck";
    int itr(1);
    while (exists(p)){
        p=path(nudeRootName+std::to_string(itr)+".bck");
    }
    currentBackup=p.string();
    itr=1;
    p=nudeRootName+".tmp";
    while (exists(p)){
        p=path(nudeRootName+std::to_string(itr)+".tmp");
    }
    currentTemp=p.string();
    path pC(rootName), pB(currentBackup), pT(currentTemp);
    if (is_regular_file((pC)))
    {
        copy_file (pC,pB,copy_options::overwrite_existing);
        copy_file (pC,pT,copy_options::overwrite_existing);
    }
}

void FilePointer::ReleaseBackups(){
    path pB(currentBackup), pT(currentTemp);
    if (is_regular_file(pB)&&is_regular_file(pT))
    {
        remove(pB);
        remove(pT);
    }
    currentBackup="";
    currentTemp="";
}
void FilePointer::RestoreBackup(){
    string rootName=currentDirName+DirSeparator+currentFileName;
    path pC(rootName), pB(currentBackup),pT(currentTemp);
    if (is_regular_file(pB)&&is_regular_file(pC)){
        copy_file (pB,pC,copy_options::overwrite_existing);
        copy_file (pB,pT,copy_options::overwrite_existing);
    }
}

void FilePointer::SaveTemp(){
    string rootName=currentDirName+DirSeparator+currentFileName;
    path pC(rootName), pT(currentTemp);
    if (is_regular_file(pT)&&is_regular_file(pC))
        copy_file (pT,pC,copy_options::overwrite_existing);
}

string FilePointer::GetTempName(){
    return currentTemp;
}

string FilePointer::GetBackupName(){
    return currentBackup;
}

