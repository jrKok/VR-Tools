#include "filepointer.h"
#include "layout.h"
#include "inisettings.h"

string FilePointer::currentFileName("fn");
string FilePointer::currentDirName("dn");
string FilePointer::DirSeparator("/");
string FilePointer::currentTemp("");
string FilePointer::currentBackup("");
string FilePointer::currentPlaneDir("");
string FilePointer::currentPlaneName("");
string FilePointer::xPlaneDir("");
string FilePointer::vrToolsDir("");
string FilePointer::mirrorDir("");
FileStack FilePointer::fStack;

FilePointer::FilePointer()
{

}

bool     FilePointer::Initiate(){
    char xpld[1024];
    bool integrity(true);
    XPLMGetSystemPath(xpld);
    xPlaneDir=xpld;
    vrToolsDir=xPlaneDir+"Resources/plugins/VR_Tools/";

    //integrity of VR Tools directory
    DrawLogic::WriteDebug("going to check integrity");
    integrity = CheckDirExists("resources/"); if (integrity) DrawLogic::WriteDebug("resources exists as Dir");
    integrity = CheckDirExists("resources/keyboards/"); if (integrity) DrawLogic::WriteDebug("resource/keyboards exists as Dir");
    integrity = CheckFileExists("resources/kneeboard.txt"); if (integrity) DrawLogic::WriteDebug("kneeboard exists as file");
    integrity = CheckFileExists("resources/keyboards/USKB.cfg"); if (integrity) DrawLogic::WriteDebug("USKB exists as file");
    integrity = CheckFileExists("resources/keyboards/numpad.cfg"); if (integrity) DrawLogic::WriteDebug("numpad exists as file");
    integrity = CheckFileExists("resources/keyboards/numpad2.cfg"); if (integrity) DrawLogic::WriteDebug("numpad2 exists as file");
    if (integrity) {
        mirrorDir=vrToolsDir+"resources/mirrors/";
        path p(mirrorDir);
        if (!exists(p)||!is_directory(p)) {
            DrawLogic::WriteDebug("FilePointer : attempt to create mirror directory");
            create_directory(p);
            if (!exists(p)){
                DrawLogic::WriteDebug("FilePointer : cannot create mirror directory");
                mirrorDir="";}

        }

        fStack.Initiate();
        if (fStack.HasStack()){
            SetCompleteFileName(fStack.GetCurrentActive());
        }else {
            //Go to find file
            currentDirName=IniSettings::GetDir();
            currentFileName=IniSettings::GetFile();
            if (!ExistsName()){
                currentFileName="flightnotes.txt";
                currentDirName="Output/textfiles";
                if (!ExistsName()){
                    currentDirName="Resources/plugins/VR_Tools/textfiles";
                    if (!ExistsName()){
                        currentDirName="";
                        currentFileName="";
                    }
                }
            }
            DrawLogic::WriteDebug("filePointer : finally got dir and nam "+currentDirName+" "+currentFileName);
        }
    }
    return integrity;
}

bool FilePointer::CheckDirExists(string dName){
    string dirToCheck=vrToolsDir+dName;
    DrawLogic::WriteDebug("test presence of "+dirToCheck);
    path p(dirToCheck);
    return (is_directory(p)&&exists(p));
}

bool FilePointer::CheckFileExists(string fName){
    string fileToCheck=vrToolsDir+fName;
    path p(fileToCheck);
    return (is_regular_file(p)&&exists(p));
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
    if (currentDirName==""&&currentFileName=="") return ("");
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

bool FilePointer::FindCurrentPlaneDir(){
    bool hasPlane(false);
    char plName[512],plDir[1024];
    string planeDir("");
    XPLMGetNthAircraftModel(0,plName,plDir);
    string planePath=plDir;
    if (planePath!=""){
        path p=planePath;
        planeDir=p.parent_path().string();
        currentPlaneDir=planeDir;
        currentPlaneName=plName;
        if (currentPlaneName!="") currentPlaneName=currentPlaneName.erase(currentPlaneName.size()-4,4);//erase trailing.acf
        DrawLogic::WriteDebug("Filepointer plane Directory and Name : "+currentPlaneDir+" "+currentPlaneName);
        if (currentPlaneDir!=""&&currentPlaneName!="") hasPlane=true;
    }
    return hasPlane;
}

string FilePointer::GetCurrentPlaneDir(){
    return currentPlaneDir;
}

bool FilePointer::MakeVRMirrorForCurrentPlane(){
    //point or create directory
    string mirrorPDir=mirrorDir+currentPlaneName+"/";
    path pMirror=mirrorPDir;
    if (!exists(pMirror)||!is_directory(pMirror)){
        create_directory(pMirror);
    }
    if (exists(pMirror)){
        path pMP=mirrorPDir+currentPlaneName+"_vrconfig.txt";
        path pVR=VRCReader::GetVRConfig();
        DrawLogic::WriteDebug("FilePointer : should copy to mirror "+pMP.string());
        if (is_regular_file(pVR))
            copy_file(pVR,pMP,copy_options::overwrite_existing);
        return true;
    }
    else return false;

}

bool FilePointer::HasMirror(){
    string mirrorPDir=mirrorDir+currentPlaneName+"/";
    path pMirror=mirrorPDir+currentPlaneName+"_vrconfig.txt";
    return (exists(pMirror)&&is_regular_file(pMirror));

}

void FilePointer::CopyVRFromMirror(){
    string mirrorPDir=mirrorDir+currentPlaneName+"/";
    path pMirror=mirrorPDir+currentPlaneName+"_vrconfig.txt";
    path pVR=VRCReader::GetVRConfig();
    if (is_regular_file(pMirror))
        copy_file(pMirror,pVR,copy_options::overwrite_existing);
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

bool FilePointer::OpenTempFileForRead(std::fstream &oFile){
    if (!oFile.is_open()){
        string fname=currentTemp;
        if (fname=="/") return false;
        if (currentTemp=="") return false;
        path p(fname);
        if (std::experimental::filesystem::is_directory(p)) return false;
        oFile.open(p,std::ifstream::in);
        return (oFile.is_open());
    }
    else return true;
}

bool FilePointer::OpenTempFileForWrite(std::fstream &oFile){
    if (!oFile.is_open()){
        string fname=currentTemp;
        if (fname=="/") return false;
        if (currentTemp=="") return false;
        path p(fname);
        if (std::experimental::filesystem::is_directory(p)) return false;
        oFile.open(p,std::ofstream::out);
        return (oFile.is_open());
    }
    else return true;
}

bool FilePointer::OpenCurrentFile(std::fstream &oFile){
    if (!oFile.is_open()){
        string fname=currentDirName+"/"+currentFileName;
        if (fname=="/") return false;
        if (currentFileName=="") return false;
        path p(fname);
        if (std::experimental::filesystem::is_directory(p)) return false;
        oFile.open(p,std::ifstream::in);
        return (oFile.is_open());
    }
    else return true;
}


std::uintmax_t FilePointer::GetSizeOfCurrentFile(){
    string fname=currentDirName+"/"+currentFileName;
    if (fname=="/") return 0;
    if (currentFileName=="") return 0;
    path p(fname);
    if (std::experimental::filesystem::is_regular_file(p)) return std::experimental::filesystem::file_size(p);
    else return 0;
}
