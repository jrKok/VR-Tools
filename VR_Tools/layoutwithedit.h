#ifndef LAYOUTWITHEDIT_H
#define LAYOUTWITHEDIT_H

#include <layout.h>
#include <keyboard.h>
#include <textedit.h>
#include <temporarywindow.h>
#include "alert3buttons.h"

class Keyboard;
class TextEdit;
class OpCenter;

class LayoutWithEdit : public Layout
{
public:
    LayoutWithEdit(DrawLogic *newPad,OpCenter *opc );
    ~LayoutWithEdit();
    void StartEdit();
    void BeginEdit();
    void ReLabelButtons();
    void EndEdit();
    void ReActivateWindow();
    void Update();
    //specific member functions for editing purposes
    void ToggleKeyboard();
    void SetSpecialKey(bool spec);
    void UsePhysicalKeyboard();
    void UnfocusPhysicalKeyboard();
    void SetTextToShow(string in_text);

    //reimplemented member functions
    bool initiate();
    bool resize();
    bool newSize(int wth,int hgt);
    void FitToFile();
    void recalculate();
    void DrawTextW(XPLMWindowID);
    void DrawNoResize(XPLMWindowID g_textWindow);
    void findClick(int mX, int mY);
    void HandleMouseKeepDown(int mX,int mY);
    int  HandleMouseUp(int, int);
    void LaunchCommand(int refCommand);
    void QuitCommand();
    static void HandleAlertResult();
    void ProcessKeyPress(std::string keyName,std::string in_String);
    void ContinueKeyPress();
    void EndKeyPress();
    void CheckButtonsVisibility();
    std::string GetFileName();
    void SetNewFile(std::string newFilePath);
    void ToggleFPS();
    void ClosegWindow();
    void EndEditMode();


private:
    OpCenter *myCenter;
    rectangles keybR;
    Keyboard keyb;
    bool showKeyb,keyPressed,specialKey,utfKey,hasToSave,physK;
    char firstChar,secondChar;
    std::string fName,textToShow;
    std::unique_ptr<TextEdit> tEdFileReader;
    const char   tabchar;
    const string tabstring;
    Alert3Buttons quitWoSave;
    static LayoutWithEdit *myself;
};

#endif // LAYOUTWITHEDIT_H
