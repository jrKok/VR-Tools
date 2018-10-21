#ifndef CURSOR_H
#define CURSOR_H
/*
 * This class maintains a cursor.
 * It can determine a pos in the std::string sense from an X in pixels given a line
 * It can manage a selection ; if hasSelection=true line/charpos is at the beginning of the selection lineEnd/charEnd is the end of the selection
 *
 * chLenght contains X Plane's length of the UTF 8 character. Only 2 byte UTF 8 characters are taken into account.
 * The cursor function makes UTF 8 from the input, if invalid UTF8 considers everything as ANSI.
 */

#include <string>
#include <stringops.h>
#include <fstream>
#include <vector>
#include "VR_Tools_global.h"
#include <XPLMDisplay.h>
#include <XPLMUtilities.h>
#include <XPLMProcessing.h>
#include <layout.h>

class Layout;

using std::string;
using intVector=std::vector<int>;
using strVector=std::vector<std::string>;

typedef long long unsigned ulong;
class cursor

{
public:
    cursor(); //will make chLength
    ~cursor();
    void Initiate(const strVector *inLines, int offsetXLine, int offsetYLine, int lnHeight, int boxlines); //makes chPos, table of absolute positions of characters WRT beginning offsetX of lines
    void MakeLinePos(const std::string &thisline);
    void MakeLinePosAgain(const ulong line,const string &thisline);
     int FindPositionInNativeLine(const string &in_string, int in_pos);
    void Recalculate(int oX,int oY);
    void SetIndxFirstPage(int indx);
    void FindClosestXPosition(int inLine, int inX);
    int  GetLengthOfUTFString(string inLine);
    void FindPos(int inLine,int inX); //gets a line number and a pixel number, converts to a position, sets line and charPos hasSelection=false
    void DragPos (int inLine, int inX);// informed a drag occurs, tracks lineEnd and charEnd, modifies them if necessary hasSelection=true
    void EndOfDrag();
    int  GetPos(); //returns charPos
    int  GetLine();//returns line
    int  GetSelectionStartLine();
    int  GetSelectionEndLine(); //returns lineEnd last line if hasSelection
    int  GetSelectionStartCharPos();
    int  GetSelectionEndCharPos();
    int  GetLastPositionOfLine(int in_line);
    void AddPositionsToLine(int nbOfP);
    void SetCursorAt(int in_line, int in_pos);
    bool HasCursor();
    bool HasSelection();//returns hasSelection
    bool IsIndexInSelection(int idx, int &left, int &right);
    void FlipSelection();
    bool IsMoveSelectionEnd();
    void MoveCursorRight();
    void MoveCursorLeft();
    void MoveCursorUp();
    void MoveCursorDown();
    void MoveSelectionUp();
    void MoveSelectionDown();
    void MoveSelectionRight();
    void MoveSelectionLeft();
    void SwapEndsOfSelection();
    void FindClosestPosition(const int &ln, int &cX, int &pos);
    void MakeCursorFromSelectionStart();
    int  PosToX();
    int  LineToY(int firstLine);
    int  EndPosToX();
    int  EndLineToY(int firstLine);
    void DrawRectangle(int left, int top, int right, int bottom);
    void DrawCursor(int cursorY);
    string ReadLineToUTF(string inLine);
    unsigned int UTFint(unsigned char in_ANSIchar);
    void DeleteFromLines(int in_line);
    void InsertEmptyLine(int in_line);
    void StopSelection();
    void EraseCursor();

private:


    void MakeStart();
    void CursorFromCurrent();
    void SelectionRegular();
    void SelectionInverted();

    float timePoint;
    unsigned char testChar;
    int nbOfLines,line,charPos,cursorX;
    int startCharPos,startCursorX,startLine;
    int selectionEndLine,selectionEndCharPos,selectionEndCursorX;
    int selectionStartLine,selectionStartCharPos,selectionStartCursorX,currentLine,currentPos,currentX;
    int offX,offY,firstLineDisplayed,lineHeight,nbBoxLines;//the x and y offsets for computing PosToX and LineToY()
    int indexFirstOnPage,indexLastOnPage;
    bool hasCursor,hasSelection,validUTF,beginUTF,cursorOn,moveSelectionEnd;
    intVector chPos,chLength;
    std::vector<intVector> posLines;
    float ink[3],lightblue[3];
    const char * cursorChar;
    stringOps ops;

};

#endif // CURSOR_H
