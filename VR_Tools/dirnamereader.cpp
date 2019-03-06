#include "dirnamereader.h"


dirNameReader::dirNameReader():List_Box_With_ScrB(),
    parentPath(""),
    actualPath(""),
    actualDirName(""),
    systemCharSep(""),
    sysDir(""),shortCutBck(".."),shortCutXP("X Plane"),ShortCutPlug("VR Tools"),ShortCutF("Text Files"),
    ShortCutPlane("Current Plane"),
    nbSpecialLines(5)

{
    SetSplitPolicy(DontTruncate);
}

void   dirNameReader:: SetupDirectoryReader(){
    char sysPath[512];
    XPLMGetSystemPath(sysPath);
    sysDir=sysPath;//conversion from char[]
    sysDir=sysDir.substr(0,(sysDir.size()-1));
    systemCharSep=XPLMGetDirectorySeparator();
    actualPath=sysDir+systemCharSep+FilePointer::GetCurrentDirName();
    actualPath=FilePointer::GetCurrentDirName();
    GetDirNameFromPath();
    GetParentPathFromPath();
}

void dirNameReader::SetDirectory(std::string dirN){
actualPath=dirN;
GetDirNameFromPath();
GetParentPathFromPath();
ShowAll();
}

std::string dirNameReader::GetParentPath(){
    return parentPath;
}

std::string dirNameReader::GetActualDirName(){
    return actualDirName;
}

std::string dirNameReader::GetActualPathName(){
    return actualPath;
}

void dirNameReader::ReadRootDir(){
    if ((parentPath!="")&&(parentPath!="C:\\")&&(parentPath!="C:")){
        actualPath=parentPath;
        actualDirName=GetDirNameFromPath();
        parentPath=GetParentPathFromPath();
        ShowAll();
    }
    else ShowAll();
}

void dirNameReader::ShowAll(){
    clearText();
    AddLine(shortCutBck);
    AddLine(shortCutXP);
    AddLine(ShortCutF);
    AddLine(ShortCutPlug);
    AddLine(ShortCutPlane);
    for (auto & p : std::experimental::filesystem::directory_iterator(actualPath)){
        if (std::experimental::filesystem::is_directory(std::experimental::filesystem::status(p.path().string()))){
            std::string fName=p.path().filename().string();
            AddLine(fName);
        }
        }
    SetupforText();
    SetInkColor(Clr_BlackInk);
    ColorFirstLines();
}

void dirNameReader::ReadSelectedDir(){
    std::string name=StringSelected();
    if (SelectedLineNumber()<nbSpecialLines){
       if (name==shortCutBck) {
           ReadRootDir();
           return;}
       if (name==shortCutXP) {
           SetDirectory(sysDir);
           return;
       }
       if (name==ShortCutF){
           SetDirectory(sysDir+"\\Output\\textfiles");
           return;
       }
       if (name==ShortCutPlug){
           SetDirectory(sysDir+"\\Resources\\plugins\\VR_Tools");
           WriteDebug(actualPath);
           return;
      }
       if (name==ShortCutPlane){
           SetDirectory(FilePointer::GetCurrentPlaneDir());
           WriteDebug(actualPath);
       }
    }else{
       parentPath=actualPath;
       actualPath=actualPath+systemCharSep+StringSelected();
       actualDirName=StringSelected();
       ShowAll();}
}

std::string dirNameReader::GetDirNameFromPath(){
    std::experimental::filesystem::path pth=actualPath;
    actualDirName=pth.filename().string();
    return actualDirName;
}
std::string dirNameReader::GetParentPathFromPath(){
    std::experimental::filesystem::path pth=actualPath;
    parentPath=pth.parent_path().string();
    return parentPath;
}

std::string dirNameReader::GetSelectedDirPath(){
    return (actualPath+systemCharSep+StringSelected());
}

void dirNameReader::ColorFirstLines(){
    if (indxFirstOnPg<nbSpecialLines){
        int idx(indxFirstOnPg);
        ulong bIdx(0);
        while (idx<nbSpecialLines&&idx<=indxLastOnPg){
            if (!box[bIdx].GetSelected())
                box[bIdx].SetTextColor(Clr_Green);
            bIdx++;
            idx++;
        }
    }
}
