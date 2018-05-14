#ifndef INISETTINGS_H
#define INISETTINGS_H
#include <string>
#include <string.h>
#include <vector>
#include <locale>
#include <filesystem>
#include <fstream>
#include <stringops.h>
#include <XPLMPlanes.h>
#include <VR_Tools_global.h>
/*
* ini options :
*
* Presets : dev console
* start options
*  open_window_at_start : bool
*  autoreload_at_open : bool
* reload_on_file_size : bool
* reload_period_sec : int
*  flash_on_text_changed :bool
*  last_page_at_open : bool
* display :
*  width             : int
*  height            : int
*  fit_to_file       : bool
*  fixed_dimensions  : bool
*  keep_size         : bool
*  background        : bool
*
* file
*  default_directory : string
*  default_file      : string
*  keep_last_file    : bool
*  trim line option  : truncate line, split at space, split at max larg
* look into the nav/com/adf availability issue
*function :
*  can_delete        : bool
*  can_read_freqs    : bool
*Datarefs displayed
*/

class IniSettings
{
public:
    IniSettings();
    //build a vector of datarefs
    void ReadIniFile();
    void WriteIniFile();
    void LogInstruction(std::string lf,std::string rg,std::string ct);

    bool GetOptStart();
    bool GetOptReload();
    bool GetOptUTF();
    bool GetOptFlash();
    bool GetOptLastPg();
    int  GetWidth();
    int  GetHeight();
    bool GetOptFit();
    bool GetOptFixed();
    bool GetOptKeepSize();
    bool GetOptBckG();
    std::string GetDir();
    std::string GetFile();
    bool GetOptLastFile();
    int  GetOptTrim();
    bool GetOptDelete();
    bool GetOptFreqs();
    bool GetOptFPS();
    bool GetOptReloadProc();
    int  GetReloadPeriod();

    void SetOptStart(bool opt);
    void SetOptReload(bool opt);
    void SetOptUTF(bool opt);
    void SetOptFlash(bool opt);
    void SetOptLastPg(bool opt);
    void SetWidth(int opt);
    void SetHeight(int opt);
    void SetOptFit(bool opt);
    void SetOptFixed(bool opt);
    void SetOptKeepSize(bool opt);
    void SetOptBckG(bool opt);
    void SetDir (std::string opt);
    void SetFile(std::string opt);
    void SetOptLastFile(bool opt);
    void SetOptTrim(int opt);
    void SetOptDelete(bool opt);
    void SetOptFreqs(bool opt);
    void SetOptFPS(bool opt);
    std::string CodeTrimOption(int in_option);
    void SetOptReloadProc(bool opt);
    void SetReloadPeriod(int opt);
    void WriteOption(std::string optionName, int opt);
    void WriteOption(std::string optionName, bool opt);
    void WriteOption(std::string optionName, std::string opt);
    int  FindOption(std::string optionName);

private:
    void WriteDebug(std::string message);
    stringOps ops;
    std::string iniFileName,iniDirectory,completeName;
    std::fstream iniFile;
    bool openWdwAtStart;
    bool autoReload;
    bool periodicReload;
    int  reloadPeriod;
    bool autoUTF8;
    bool changeClrOnTxtChg;
    bool moveLastPgOnStart;
    int width,height;
    bool fitToFile;
    bool noResize;
    bool keepSize;
    bool noBckground;
    std::string directory;
    std::string file;
    bool keepLastFile;
    int trimLineOption;
    bool deleteEnable;
    bool navsSetEnable;
    bool displayFPS;
    std::vector <std::string> leftH,rightH,comment;

};

#endif // INISETTINGS_H
