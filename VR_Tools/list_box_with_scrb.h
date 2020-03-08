#ifndef LIST_BOX_WITH_SCRB_H
#define LIST_BOX_WITH_SCRB_H

#include <string>
#include <stringops.h>
#include <fstream>
#include <vector>
#include "VR_Tools_global.h"
#include <XPLMDisplay.h>
#include <XPLMUtilities.h>
#include <XPLMProcessing.h>
#include <rectangles.h>
#include <textline.h>
#include <scrollbars.h>
#include <stringops.h>

#if IBM
    #include <windows.h>
#endif
#if LIN
    #include <cmath>
    #include <ctgmath>
#endif

/*This abstract class sets up a list box for displaying lines of text held
 * in a vector. Only ShowAll has to be implemented by the subclass. The Pageheight=number of Lines displayed, can be set either directly
 * or calculated for a given height in pixels. Cannot go over 1200*900, nor be smaller than 120hx150l.
 * The size can be proportionate l/H=3/4 or entirely customized.
 * The class is meant to be subclassed by individual text readers or text makers.
 * It has several modes to deal with long lines : splitting at space, splitting at point, truncating. It processes lines to fit in a
 * given size.
 * text lines can be deleted
 * A title for the list can be set (a pure virtual function)
 * It uses the classes for rectangles and lines.
 * It processes clicks, and recalculates its position when necessary*/

using std::string;

class List_Box_With_ScrB
{
public:
    List_Box_With_ScrB(bool modal=false);
    virtual ~List_Box_With_ScrB();
    void Setup (int hght,int larg,int in_offsetX,int in_offsetY);
    void SetupforText();
    void AddLine(string in_Line);
    virtual bool ProceedClick(int x, int y);
    virtual void ProceedClickCont(int x, int y);
    virtual void ProceedEndClick();
    void Recalculate(int in_lft,int in_tp);
    void MoveUpNLines(int uL);
    void MoveDnNLines(int dL);
    void MoveUpPage();
    void MoveDnPage();
    void ScrollLineDn();
    void ScrollLineUp();
    void DisplayAtLineN(int in_ln);
    void GoToLineN(int in_ln);
    void GoToLastPage();
    bool HasHiddenLine();
    void SelectLine(int lnNb);
    void SelectFirstLine();
    bool HasSelection();
    int  SelectedLineNumber();
  string StringSelected();
    void DeleteLine(int nbLn);
    void DeleteSelectedLine();
    void UndoDelete();
    void SetWidth(int in_width); //the following only for resizing events generated by the user class
    void SetHeightInLines (int in_Lines); //adjusts also pageHeightInL, adjust scrB accordingly
    void SetHeightInPix(int in_pix); //adjusts also pageHeightInL if necessary, adjust scrB accordingly
    int  GetWidth();
    int  GetHeight();
    int  GetOffSetX();
    int  GetOffSetY();
    int  GetTop();
    int  GetLeft();
    int  GetRight();
    int  GetBottom();
    int  GetNumberOfLines();
    int  GetMaxWidth();
    void AdjustToWidth();
    void AdjustToHeight(); //effects if pageHeight> totalNbL
    void ProportionateSizeToHeight();//3/4 proportion
    void ProportionateSizeToWidth();//3/4 proportion
    void SetBckColor (char in_Color);
    void SetInkColor (char in_Color);
    void clearText(); //has to adjust scrB also
    void SetSplitPolicy(int splitP);
    void CanDelete();
    bool CanUndo();
    void DeleteDisplayBox();
    void AddLineToDisplayBox(int lnNumber);
    void DontDelete();
    void convertToUTF8();
    void SetBackGround(bool opt);
    void PrintMyText();
    virtual void ShowAll()=0;

protected:
    int in_bottom,in_left;
    int grlOffsetX,grlOffsetY;
    int charHeight,maxStringWidth;
    int pageHeightInL,totalNbL,indxFirstOnPg,indxLastOnPg,indxLastPage,lineLength;
    int lineSelected;
    int lastLineDeleted,antepLineDeleted;
    int heightPx,widthPx;
    int clickPosX, clickPosY, currentIndx,currentIndxFP;//for dragging, currentIndx registers the line clicked when initiating drag
    int splitPolicy;
    bool hasHiddenLines,hasSelection,dragLines,delLines,needToContClick,canUndo,filterClick,canDelete;
    bool drawBackground;
    bool isModal;
    ScrollBars scrB;
    rectangles general,textOnly;
    std::vector<string> *displayText;
    std::vector<TextLine> box;
    stringOps strops;
    string delStr1,delStr2;

    virtual void DisplayPage();
    int  MeasureString(string str);
    void WriteDebug(string message);
};

#endif // LIST_BOX_WITH_SCRB_H
