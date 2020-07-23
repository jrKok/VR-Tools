#ifndef FILEPOINTER_H
#define FILEPOINTER_H

#include <string>
#include <sstream>
#include "VR_Tools_global.h"
#include "XPLMPlanes.h"
#include "filestack.h"

class Layout;


using std::string;


class FilePointer
{
public:
    FilePointer();

static bool     Initiate();
static bool     CheckDirExists(string dName);
static bool     CheckFileExists(string Name);
static void     SetCurrentFileName(string in_Name);
static string   GetCurrentFileName();
static string   GetCurrentNameSansExt();
static bool     ExistsName();
static void     SetCompleteFileName(string in_Name);
static void     SetCurrentDirName(string in_Dir);
static string   GetCurrentDirName();
static bool     FindCurrentPlaneDir();
static string   GetCurrentPlaneDir();
static void     MakeBackups();
static void     ReleaseBackups();
static void     RestoreBackup();
static void     SaveTemp();
static string   GetTempName();
static string   GetBackupName();
static void     AddFile(string dir,string file);
static string   GetCurrentFile();
static string   GetNext();
static string   GetPrevious();
static bool     OpenCurrentFile(std::fstream &oFile);
static bool     OpenTempFileForRead(std::fstream &oFile);
static bool     OpenTempFileForWrite(std::fstream &oFile);
static bool     MakeVRMirrorForCurrentPlane();
static bool     HasMirror();
static void     CopyVRFromMirror();
static std::uintmax_t GetSizeOfCurrentFile();

static string   DirSeparator;

private:
    static string currentFileName,currentDirName,currentBackup,currentTemp,currentPlaneDir,currentPlaneName;
    static string xPlaneDir,vrToolsDir,mirrorDir;
    static FileStack fStack;

};

#endif // FILEPOINTER_H
