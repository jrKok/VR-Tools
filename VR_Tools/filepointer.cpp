#include "filepointer.h"
#include "layout.h"
#include "inisettings.h"

string FilePointer::currentFileName("fn");
string FilePointer::currentDirName("dn");
string FilePointer::DirSeparator("/");
string FilePointer::currentTemp("");
string FilePointer::currentBackup("");
string FilePointer::currentPlaneDir("");
FileStack FilePointer::fStack;

FilePointer::FilePointer()
{

}

void     FilePointer::Initiate(){
    fStack.Initiate();
    if (fStack.HasStack()){
        SetCompleteFileName(fStack.GetCurrentActive());
    }else {
        //Go to find file
        currentDirName=IniSettings::GetDir();
        currentFileName=IniSettings::GetFile();
        if (!ExistsName()){
             currentFileName="flightnotes";
             currentDirName="Output/textfiles";
            if (!ExistsName()){
                currentDirName="Resources/plugins/VR_Tools/textfiles";
                if (!ExistsName()){
                    currentDirName="";
                    currentFileName="";
                }
            }
        }
}
}

bool     FilePointer::ExistsName(){
    path p(currentDirName+"/"+currentFileName);
    return (exists(p));
}

void     FilePointer::AddFile(string dir,string file){
    currentDirName=dir;
    currentFileName=file;
    path p=dir+"/"+file;
    if (exists(p))
    fStack.AddToStack(p.string(),false);
}

string   FilePointer::GetCurrentFile(){
    return currentDirName+"/"+currentFileName;
}

string   FilePointer::GetNext(){
    if (fStack.HasStack()) SetCompleteFileName(fStack.NextActive());
    return GetCurrentFileName();
}

string   FilePointer::GetPrevious(){
    if (fStack.HasStack()) SetCompleteFileName(fStack.PreviousActive());
    return GetCurrentFileName();
}

string FilePointer::GetCurrentFileName(){

    return currentFileName;
}

void FilePointer::SetCurrentFileName(string in_Name){
    currentFileName=in_Name;
}

void FilePointer::SetCompleteFileName(string in_String){
    path p=in_String;
    currentFileName=p.filename().string();
    currentDirName=p.parent_path().string();

}

void   FilePointer::SetCurrentDirName(string in_Dir){
    if (in_Dir=="") {
        currentDirName="";
        return;
    }
    if (in_Dir.substr(in_Dir.length()-1,1)==DirSeparator) in_Dir=in_Dir.substr(0,in_Dir.length()-1);
    currentDirName=in_Dir;
}
string FilePointer::GetCurrentDirName(){
    return currentDirName;
}

void FilePointer::FindCurrentPlaneDir(){
    char plName[512],plDir[1024];
    string planeDir("");
    XPLMGetNthAircraftModel(0,plName,plDir);
    string planePath=plDir;
    if (planePath!=""){
        path p=planePath;
        planeDir=p.parent_path().string();
    }
    FilePointer::currentPlaneDir=planeDir;

}

string FilePointer::GetCurrentPlaneDir(){
    return currentPlaneDir;
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
        itr++;
    }
    currentBackup=p.string();
    itr=1;
    p=nudeRootName+".tmp";
    while (exists(p)){
        p=path(nudeRootName+std::to_string(itr)+".tmp");
        itr++;
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
    if (currentBackup!=""){
        path pB(currentBackup);
        if (is_regular_file(pB))
            remove(pB);
        currentBackup="";
    }

    if (currentTemp!=""){
        path pT(currentTemp);
        if (is_regular_file((pT)))
            remove(pT);
        currentTemp="";
    }
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

string FilePointer::GetCurrentNameSansExt(){
    path p=currentFileName;
    string toRet=p.stem().string();
    return toRet;
}
