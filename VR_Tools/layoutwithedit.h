#ifndef LAYOUTWITHEDIT_H
#define LAYOUTWITHEDIT_H

#include <layout.h>
#include <keyboard.h>
#include <textedit.h>

class Keyboard;
class TextEdit;

class LayoutWithEdit : public Layout
{
public:
    LayoutWithEdit();
    //reimplemented member functions
    void BeginEdit();
    bool initiate();
    void resize();
    bool newSize(int wth,int hgt);
    void FitToFile();
    void ReLabelButtons();
    void recalculate();
    void DrawTextW(XPLMWindowID g_textWindow);
    void DrawNoResize(XPLMWindowID g_textWindow);
    void findClick(int mX, int mY);
    void HandleMouseKeepDown(int mX,int mY);
    int  HandleMouseUp(int mX,int mY);
    void PhysicalKeyPressed(char in_char,XPLMKeyFlags flag,char in_VK);
    void LaunchCommand(int refCommand);
    void ProcessKeyPress(std::string keyName,std::string in_String);
    void ContinueKeyPress();
    void EndKeyPress();
    void CheckButtonsVisibility();
    std::string GetFileName();
    void SetNewFile(std::string newFilePath);
    void EndEdit();
    void ClosegWindow();
    void ToggleKeyboard();
    void UsePhysicalKeyboard();
    //specific member functions for editing purposes
private:
    Keyboard keyb;
    bool showKeyb,keyPressed,specialKey,utfKey,editMode;
    char firstChar,secondChar;
    std::string fName;
    TextEdit* tEdFileReader;
};

#endif // LAYOUTWITHEDIT_H
