#include "filenamepicker.h"

FileNamePicker::FileNamePicker():List_Box_With_ScrB(),
    currentDir(""),
    systemCharSep(""),
    txtOnly(true),
    directoryEmpty(false)
{
 SetSplitPolicy(DontTruncate);
 systemCharSep=XPLMGetDirectorySeparator();
}

void FileNamePicker::SetDirToRead(std::string in_dirPath){
    currentDir=in_dirPath;
    ShowAll();
}

void FileNamePicker::ToggleAllTxt(){
    txtOnly= !txtOnly;
}

bool FileNamePicker::GetTxtOption(){
    return txtOnly;
}

void FileNamePicker::ShowAll(){
    clearText();

    for (auto & p : std::experimental::filesystem::directory_iterator(currentDir)){
        //std::experimental::filesystem::file_status sts=status(p.path().string());

        if (std::experimental::filesystem::is_regular_file(std::experimental::filesystem::status(p.path().string()))) {
            std::string fName=p.path().filename().string();
            std::string ext=p.path().extension().string();
            if (txtOnly){
                if (ext==".txt")
                   {AddLine(fName);}
            }
            else AddLine(fName);
        }}
    if (totalNbL==0) {
        if (txtOnly)
            AddLine("no text files found");
        else AddLine("no files found");
        directoryEmpty=true;
    }else{
        directoryEmpty=false;
    }
    SetupforText();
}

std::string FileNamePicker::GetCompleteSelectedFName(){
    return (currentDir+systemCharSep+StringSelected());
}

std::string FileNamePicker::GetSelectedName(){
    return(StringSelected());
}
