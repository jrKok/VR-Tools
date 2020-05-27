#ifndef FILESTACK_H
#define FILESTACK_H

#include "globals.h"
#include <fstream>
//#if IBM
//   #include <filesystem>
//#endif
#if LIN
#include <experimental/filesystem>
#endif

 using namespace std::experimental::filesystem;


struct FileItem{
    string FileName;
    bool   IsStream;
};

class FileStack
{
public:
    FileStack();

    void   Initiate();
    void   ReadStackFromDisk();
    void   WriteStackToDisk();
    void   AddToStack(string in_FileName,bool is_Streaming);
    string GetCurrentActive();
    string NextActive();
    string PreviousActive();
    void   RemoveFromStack(int number);
    void   DeleteStack();
    bool   HasStack();
    bool   IsCurrentStream();
    string StackFileName();
    void   DebugStack(string step);

private:

    string stackFileName;
    const int maxFiles_InStack=5;
    map<int,FileItem> files;
    bool hasStack;



};

#endif // FILESTACK_H
