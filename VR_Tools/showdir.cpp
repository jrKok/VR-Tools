#include "showdir.h"



ShowDir::ShowDir(DrawLogic *newPad) :
    myDrawPad(newPad),
    general(true),
    leftR(true),
    rightR(true),
    slider(true),
    in_top(0),in_left(0),newT(0),newL(0),nR(0),nB(0),
    buttonPressed(-1), SectionPressed(-1),charHeight(0),hghDisp(190),
    waitForMouseUp(false),
    dirN(),
    fileN(),
    fileSelected(""),filePathSelected(""),
    displayLines(),
    buttons()

{  //Constructor

} //End Constructor

//Helper functions

void ShowDir::WriteDebug(string message){
    string in_String="VR Tools : " +message+"\n";
    XPLMDebugString((char*)in_String.c_str());
}

void ShowDir::MakeButton(){
    button_VR button;
    button.setVisibility(true);
    button.SetToStateColor();
    button.setTextColor(Clr_White);
    buttons.push_back(button);
}

void ShowDir::MakeLine(){
    TextLine line;
    line.SetTextColor(Clr_White);
    displayLines.push_back(line);
}

//class members/functions

void ShowDir::SetupDirWindow(int left, int top){
    myDrawPad->Initiate();
    FilePointer::FindCurrentPlaneDir();
    if (displayLines.size()==0){
        for (int Cpt(0);Cpt<7;Cpt++)  MakeLine();
        for (int b(0);b<5;b++) MakeButton();
        XPLMGetFontDimensions(xplmFont_Proportional,nullptr,&charHeight,nullptr);
        general.setVisibility(true);
        general.setColor(Clr_Gray);
        dirN.Setup(hghDisp,300,10,40);
        fileN.Setup(hghDisp,280,320,40);
        dirN.SetBckColor(Clr_PaperWhite);
        fileN.SetBckColor(Clr_PaperWhite);

        dirN.SetupDirectoryReader();
        fileN.SetDirToRead(dirN.GetActualPathName());

        in_top=top;
        in_left=left;

        //windowTitle = 0,dirTitle=1, fileTitle=2,DirSelected=3,FileSelected=4,FilePicker=5,DirReader=6
        displayLines[windowTitle].setText("Text files (.txt) in : .."+dirN.GetActualDirName());
        displayLines[windowTitle].SetDimensions(280,charHeight+2);
        displayLines[windowTitle].SetOrigin(320,13);

        int TitleSize=MeasureString(displayLines[dirTitle].GetText());
        int fileTitleSize=MeasureString(displayLines[fileTitle].GetText());
        int dirSelectedSize=MeasureString(displayLines[DirSelected].GetText());
        int fileSelectedSize=MeasureString(displayLines[FileSelected].GetText());

        displayLines[dirTitle].setText("Choose a directory :");
        displayLines[dirTitle].SetDimensions(TitleSize,charHeight+2);
        displayLines[dirTitle].SetOrigin(10,27);

        displayLines[fileTitle].setText("Click the file to display :");
        displayLines[fileTitle].SetDimensions(fileTitleSize,charHeight+2);
        displayLines[fileTitle].SetOrigin(320,27);

        displayLines[DirSelected].setText("Directory Selected :");
        displayLines[DirSelected].SetDimensions(dirSelectedSize,charHeight+2);
        displayLines[DirSelected].SetOrigin(10,hghDisp+50);

        displayLines[FileSelected].setText("File Selected :");
        displayLines[FileSelected].SetDimensions(fileSelectedSize,charHeight+2);
        displayLines[FileSelected].SetOrigin(320,displayLines[DirSelected].GetBottom());

        displayLines[FilePicker].setText("");
        displayLines[FilePicker].SetDimensions(200,charHeight+6);
        displayLines[FilePicker].SetTextColor(Clr_BlackInk);
        displayLines[FilePicker].SetOrigin(320,displayLines[FileSelected].GetBottom()+charHeight+12);

        displayLines[DirReader].setText("");
        displayLines[DirReader].SetDimensions(280,charHeight+6);
        displayLines[DirReader].SetTextColor(Clr_BlackInk);
        displayLines[DirReader].SetOrigin(10,displayLines[FilePicker].GetBottom());

        //button_ok=0,button_Cancel=1,button_SelDir=2,button_All=3,button_txt=4

        buttons[button_ok].SetDimensions(30,20);
        buttons[button_ok].SetOrigin(365,displayLines[FilePicker].GetBottom()+charHeight+2);
        buttons[button_ok].setText("OK");
        buttons[button_ok].setVisibility(false);

        buttons[button_Cancel].SetDimensions(40,20);
        buttons[button_Cancel].SetOrigin(290,buttons[button_ok].GetBottom());
        buttons[button_Cancel].setText("Cancel");
        buttons[button_Cancel].setVisibility(true);

        buttons[button_SelDir].SetDimensions(40,20);
        buttons[button_SelDir].SetOrigin(120,buttons[button_ok].GetBottom());
        buttons[button_SelDir].setText("Select");
        buttons[button_SelDir].setVisibility(false);

        buttons[button_All].SetDimensions(30,20);
        buttons[button_All].SetOrigin(500,15);
        buttons[button_All].setText("*.*");
        buttons[button_All].setVisibility(true);

        buttons[button_txt].SetDimensions(30,20);
        buttons[button_txt].SetOrigin(buttons[button_All].GetLeft()+30,buttons[button_All].GetBottom());
        buttons[button_txt].setText(".txt");
        buttons[button_txt].setVisibility(false);

        slider.SetOrigin(buttons[button_All].GetLeft(),buttons[button_All].GetBottom()-1);
        slider.SetDimensions(62,22);
        slider.setColor(Clr_White);
        slider.setVisibility(true);

        general.SetOrigin(in_left+100,in_top-100);
        general.SetOrigin(0,0);
        general.SetDimensions(fileN.GetLeft()+330,buttons[button_ok].GetBottom()+30);

        dirN.ShowAll();
        fileN.ShowAll();

        displayLines[FilePicker].recalculate();
        leftR.SetAngles(displayLines[FilePicker].GetLeft()-2,displayLines[FilePicker].GetTop()-2,displayLines[FilePicker].GetRight()+2,displayLines[FilePicker].GetBottom()-3);
        leftR.SetOrigin(displayLines[FilePicker].GetLeft(),displayLines[FilePicker].GetBottom()+2);
        leftR.setColor(Clr_PaperWhite);
        leftR.setVisibility(true);
        displayLines[DirReader].recalculate();
        rightR.SetAngles(displayLines[DirReader].GetLeft()-2,displayLines[DirReader].GetTop()-2,displayLines[DirReader].GetRight()+2,displayLines[DirReader].GetBottom()-3);
        rightR.SetOrigin(displayLines[DirReader].GetLeft(),displayLines[DirReader].GetBottom()+2);
        rightR.setColor(Clr_PaperWhite);
        rightR.setVisibility(true);
        myDrawPad->SetNewSize(general.GetWidth(),general.GetHeight());
        DrawLogic::UpdateTexture();
    }
    else{
        dirN.ShowAll();
        fileN.ShowAll();
    }
}


void ShowDir::DrawDirWindow(XPLMWindowID g_FileWindow){
    XPLMGetWindowGeometry(g_FileWindow, &newL, &newT, &nR, &nB);
    DrawLogic::DrawContent();
}

int  ShowDir::processMouseDn(int x,int y){
    SectionPressed=-1;
    if (dirN.ProceedClick(x,y)) {SectionPressed=DirReader; return DirReader;}
    if (fileN.ProceedClick(x,y)) {SectionPressed=FilePicker; return FilePicker;}
    for (ulong b(0);b<5;b++) {
        if (buttons[b].isHere(x,y)) {
            buttonPressed=static_cast<int>(b);
        }
    }
    return buttonPressed;
}

void ShowDir::processMouseDrag(int x,int y){
    if (SectionPressed==DirReader){
        dirN.ProceedClickCont(x,y);
        fileN.SetInkColor(Clr_BlackInk);
        dirN.SetInkColor(Clr_BlackInk);
        dirN.ColorFirstLines();
        return;
    }
    if (SectionPressed==FilePicker){
        fileN.ProceedClickCont(x,y);
        fileN.SetInkColor(Clr_BlackInk);
        dirN.SetInkColor(Clr_BlackInk);
        dirN.ColorFirstLines();
        return;
    }
}

int  ShowDir::processMouseUp(int x,int y){
    int retVal=-1;
    if (buttonPressed>-1) {
        retVal=buttonPressed;
        switch (buttonPressed){
        case button_ok:{
            SelectFileLine();
            FilePointer::SetCurrentFileName(displayLines[FilePicker].GetText());
            FilePointer::SetCurrentDirName(dirN.GetActualPathName());
            break;
        }
        case button_Cancel:{
            fileSelected="";
            filePathSelected="";
            break;
        }
        case button_SelDir:{
            SelectDir();
            break;
        }
        case button_All:{
            if (fileN.GetTxtOption()) {
                fileN.ToggleAllTxt();
                fileN.ShowAll();
                buttons[button_All].setVisibility(false);
                buttons[button_txt].setVisibility(true);
                UpdateDirInfo();
            }
            break;
        }
        case button_txt:{
            if (!fileN.GetTxtOption()) {
                fileN.ToggleAllTxt();
                fileN.ShowAll();
                buttons[button_All].setVisibility(true);
                buttons[button_txt].setVisibility(false);
                UpdateDirInfo();
            }
            break;
        }
        }
    }
    if (SectionPressed>-1) {
        if (SectionPressed==FilePicker) {
            fileN.ProceedEndClick();
            if (fileN.HasSelection()){
                displayLines[FilePicker].setText(fileN.StringSelected());
                buttons[button_ok].setVisibility(true);}
        }
        if (SectionPressed==DirReader) {
            dirN.ProceedEndClick();
            if (dirN.HasSelection()){
                displayLines[DirReader].setText(dirN.StringSelected());
                buttons[button_SelDir].setVisibility(true);}
        }
    }

    buttonPressed=-1;
    SectionPressed=-1;
    fileN.SetInkColor(Clr_BlackInk);
    dirN.SetInkColor(Clr_BlackInk);
    dirN.ColorFirstLines();
    return retVal;
}

void ShowDir::SelectDir(){
    dirN.ReadSelectedDir();
    string current=dirN.GetActualPathName();
    fileN.SetDirToRead(current);
    buttons[button_ok].setVisibility(false);
    buttons[button_SelDir].setVisibility(false);
    UpdateDirInfo();
}

void ShowDir::SetDirToSearch(string dir){
    dirN.SetDirectory(dir);
    fileN.SetDirToRead(dir);
}

void ShowDir::UpdateDirInfo(){
    string info("");
    info = (fileN.GetTxtOption()?"Text files (.txt) in ":"All files (*.*) in ");
    info = info+dirN.GetActualDirName();
    displayLines[windowTitle].setText(info);
}

int  ShowDir::MeasureString(string in_String){
    float mySize=XPLMMeasureString(xplmFont_Proportional,DrawLogic::ToC(in_String),static_cast<int>(in_String.size()));
    int retSize=static_cast<int>(mySize);
    return retSize;
}

int  ShowDir::GetRight(){return general.GetRight();}
int  ShowDir::GetTop(){return general.GetTop();}
int  ShowDir::GetBottom(){return general.GetBottom();}
int  ShowDir::GetLeft(){return general.GetLeft();}
int  ShowDir::GetWidth(){return general.GetWidth();}
int  ShowDir::GetHeight(){return general.GetHeight();}

void ShowDir::CloseDirWindow(){
}

void ShowDir::SelectFileLine(){
    fileSelected=fileN.GetSelectedName();
    filePathSelected=fileN.GetCompleteSelectedFName();
}

string ShowDir::GetSelectedFile(){
    return fileSelected;
}
string ShowDir::GetSelectedPath(){
    return filePathSelected;
}
