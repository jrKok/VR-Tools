#ifndef TEXTEDIT_H
#define TEXTEDIT_H
#include <textreader.h>
#include <sstream>
#include <stringops.h>
#include <cursor.h>
#include <localclipboard.h>
using std::string;

typedef long long unsigned ulong;

class TextEdit : public TextReader

{
public:
    TextEdit();

    bool Save();
    void Recalculate(int in_lft,int in_tp);
    bool ProceedClick(int x, int y);
    void ProceedClickCont(int x, int y);
    void ProceedEndClick();
    bool ReadFileToBuff();
    void DisplayPage();
    void DrawMySelf();
    void UpdateBox(int thisLine);
    void MoveCursorUp();
    void MoveCursorDown();
    void MoveCursorRight();
    void MoveCursorLeft();
    void FlipMoveSelectionEnd();
    //Editing the text file :
    void InsertLetter(std::string fromKey);
    void InsertEmptyLine(int in_line);
    void DeleteWholeLine(int in_line);
    void DeleteSelection();
    void InsertTextInLine(int in_line,int charPos,string in_text);
    void DeleteFromLine(int in_line, const int beginPos, const int endPos, bool keepLF);
  string GetLineToBreak(int in_line, int &last_line, int &cursorPos);
    void ReadLineAgain(int in_line);
    void Cut();
    void Copy();
    void Paste();
    void Backspace();
    void Suppress();
    void Enter();


private:
    float clickTime;
    cursor cursor;
    stringOps ops;
    LocalClipBoard localClip;
};

#endif // TEXTEDIT_H
