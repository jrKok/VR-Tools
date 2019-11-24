#ifndef TEXTREADER_H
#define TEXTREADER_H
#include <string>
#include <chrono>
#include <stringops.h>
#include <fstream>
#include <vector>
#include <inisettings.h>
#include "VR_Tools_global.h"
#include "list_box_with_scrb.h"

class TextReader:public List_Box_With_ScrB
{
public:
    TextReader();
    ~TextReader();

    int freqADF,freqNAV;
    float freqCOM;

    bool   OpenFile(); //sets textFile and FileExists
    void   PointToFile();
    std::string GetFileName();
    std::string GetDirName();
    virtual bool   ReadFileToBuff(); //defines MaxLineLength and reads all lines into buffer, breaks at spaces,"-","(",or")" or at maxLineLength
    bool   HasNav();
    bool   HasCom();
    bool   HasADF();
    bool   isADigit(std::string test);
    void   ShowAll();
    bool   Reload();
    bool   ReloadIfSizeChanged();
    void   closeReader();
    void   CheckForFrequencies();
    void   WriteDebug(std::string message);
    void   SetNeedsUTF(bool utf);
    std::string GetNavStr();
    std::string GetComStr();
    std::string GetADFStr();


protected:
    std::string fileName;
    std::string strBuffADF,strBuffNav,strBuffCom;
    std::string digits;
    std::fstream textFile;
    bool fileExists,needsUTF8;
    bool hasNav,hasCom,hasADF;
    file_time_type fT;
    path filePath;
    std::uintmax_t keepsize;



    void ResetFrequencies();

};

#endif // TEXTREADER_H
