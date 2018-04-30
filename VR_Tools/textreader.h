#ifndef TEXTREADER_H
#define TEXTREADER_H
#include <string>
#include <chrono>
#include <stringops.h>
#include <fstream>
#include <filesystem>
#include <vector>
#include "VR_Tools_global.h"
#include "list_box_with_scrb.h"
namespace fs=std::experimental::filesystem;



class TextReader:public List_Box_With_ScrB
{
public:
    TextReader();
    TextReader(std::string name);
    ~TextReader();

    int freqADF,freqNAV;
    float freqCOM;

    bool   OpenFile(std::string name); //sets textFile and FileExists
    std::string GetFileName();
    bool   ReadFileToBuff(); //defines MaxLineLength and reads all lines into buffer, breaks at spaces,"-","(",or")" or at maxLineLength
    bool   HasNav();
    bool   HasCom();
    bool   HasADF();
    bool   isADigit(std::string test);
    void   ShowAll();
    void   Reload();
    void   closeReader();
    void   CheckForFrequencies();
    void   WriteDebug(std::string message);
    void   SetNeedsUTF(bool utf);

private:
    std::string fileName;
    std::ifstream textFile;
    bool fileExists,needsUTF8;
    bool hasNav,hasCom,hasADF;
    std::string digits;
    fs::file_time_type fT;
    fs::path filePath;


    void ResetFrequencies();

};

#endif // TEXTREADER_H
