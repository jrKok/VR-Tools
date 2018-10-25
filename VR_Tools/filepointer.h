#ifndef FILEPOINTER_H
#define FILEPOINTER_H

#include <string>
#include <VR_Tools_global.h>
#include <filesystem>


using std::string;
using namespace std::experimental::filesystem;


class FilePointer
{
public:
    FilePointer();

static string   DirSeparator;
static void     SetCurrentFileName(string in_Name);
static string   GetCurrentFileName();
static void     SetCurrentDirName(string in_Dir);
static string   GetCurrentDirName();
static void     MakeBackups();
static void     ReleaseBackups();
static void     RestoreBackup();
static void     SaveTemp();
static string   GetTempName();
static string   GetBackupName();

    private:
    static string currentFileName,currentDirName,currentBackup,currentTemp;

};

#endif // FILEPOINTER_H