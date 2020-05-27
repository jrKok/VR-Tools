#include "filestack.h"
#include "drawlogic.h"

FileStack::FileStack() :
    stackFileName(""),
    files(),
    hasStack(false)
{

}

void   FileStack::Initiate(){

    stackFileName="Resources/plugins/VR_Tools/resources/filestack.ini";
    ReadStackFromDisk();
}

void   FileStack::ReadStackFromDisk(){
    std::fstream wFile;
    wFile.open(stackFileName,std::fstream::in);
    if (wFile.is_open()){
        string in_Line;
        int tag=1;
        while (std::getline(wFile,in_Line)){
            if (in_Line[0]=='"'){
                in_Line=in_Line.erase(0,1);
                string newName("");
                for (auto c:in_Line){
                    if (c!='"') newName=newName+c;
                    else break;
                }
                string last=in_Line.substr(in_Line.find_last_of('"')+1);
                if (tag<=maxFiles_InStack){
                    path p(newName);
                    if (exists(p)&&!is_directory(p)&&newName!=""){
                        FileItem it;
                        it.FileName=newName;
                        it.IsStream=(last=="true"?true:false);
                        files[tag]=it;
                        tag++;
                    }
                }
            }
        }
    }
    else hasStack=false;
    if (files.size()) hasStack=true;
}

void   FileStack::WriteStackToDisk(){
    std::fstream wFile;
    wFile.open(stackFileName,std::fstream::out);
    if (files.size()){
        for (auto file:files){
            string line=(file.second.IsStream?"true":"false");
            line="\""+file.second.FileName+"\""+line+"\n";
            wFile<<line;
        }
    }else{
        wFile<<"empty\n";
    }
    wFile.close();
}

void   FileStack::AddToStack(string in_FileName,bool is_Streaming){
    FileItem toAdd;

    toAdd.FileName=in_FileName;
    toAdd.IsStream=is_Streaming;
    bool found(false);
    for (auto file:files){
        if (file.second.FileName==in_FileName) found=true;
    }
    if (!found){
        int sz=static_cast<int>(files.size());
        if (sz>0)
        {
            int dp=(sz<maxFiles_InStack?sz+1:maxFiles_InStack);
            for (int index(dp);index>1;index--){
                files[index]=files[index-1];
            }
        }
        files[1]=toAdd;
        WriteStackToDisk();
    }
    hasStack=true;
}

string FileStack::GetCurrentActive(){
    return files[1].FileName;
}

string FileStack::NextActive(){
    FileItem first(files[1]);
    int number=static_cast<int>(files.size());
    if (number>1){
        for (int index(1);index<number;index++){
            files[index]=files[index+1];
        }
    }
    files[number]=first;
    WriteStackToDisk();
    return GetCurrentActive();
}

string FileStack::PreviousActive(){
    int maxN=static_cast<int>(files.size());
    FileItem last=files[maxN];
    if (maxN>1){
        for (int index(maxN);index>1;index--){
            files[index]=files[index-1];
        }
    }
    files[1]=last;
    WriteStackToDisk();
    return GetCurrentActive();
}

void   FileStack::RemoveFromStack(int number){
    int maxN=static_cast<int>(files.size());
    if (number<maxN) {
        for (int index(number);index<maxN;index++){
            files[index]=files[index+1];
        }
    }
    files.erase(maxN);
    WriteStackToDisk();
    if (!files.size()) hasStack=false;
}

void   FileStack::DeleteStack(){
    hasStack=false;
    files.clear();
    WriteStackToDisk();
}

bool   FileStack::HasStack(){
    return hasStack;
}

bool   FileStack::IsCurrentStream(){
    if (hasStack) {
        return files[1].IsStream;
    }
    return false;
}

string FileStack::StackFileName(){
    return stackFileName;
}

void FileStack::DebugStack(string step){
    DrawLogic::WriteDebug("Debug stack going to show elements of file stack step "+step+" :");
    for (auto it:files){
        DrawLogic::WriteDebug("File Name "+it.second.FileName);
    }
}
