#ifndef FILEPOINTER_H
#define FILEPOINTER_H

#include <string>
#include <VR_Tools_global.h>


using std::string;

class FilePointer
{
public:
    FilePointer();

static string DirSeparator;
static void   SetCurrentFileName(string in_Name);
static string GetCurrentFileName();
static void   SetCurrentDirName(string in_Dir);
static string GetCurrentDirName();

    private:
    static string currentFileName,currentDirName;

};

#endif // FILEPOINTER_H
