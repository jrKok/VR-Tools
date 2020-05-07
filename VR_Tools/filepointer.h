#ifndef FILEPOINTER_H
#define FILEPOINTER_H

#include <string>
#include "VR_Tools_global.h"
#include "XPLMPlanes.h"

class Layout;


using std::string;


class FilePointer
{
public:
    FilePointer();

static string   DirSeparator;
static void     SetCurrentFileName(string in_Name);
static string   GetCurrentFileName();
static string   GetCurrentNameSansExt();
static void     SetCompleteFileName(string in_Name);
static void     SetCurrentDirName(string in_Dir);
static string   GetCurrentDirName();
static void     FindCurrentPlaneDir();
static string   GetCurrentPlaneDir();
static void     MakeBackups();
static void     ReleaseBackups();
static void     RestoreBackup();
static void     SaveTemp();
static string   GetTempName();
static string   GetBackupName();

    private:
    static string currentFileName,currentDirName,currentBackup,currentTemp,currentPlaneDir;

};

#endif // FILEPOINTER_H
