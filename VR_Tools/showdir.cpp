#include "showdir.h"
#include "fontman.h"


ShowDir::ShowDir(DrawLogic *newPad) :
    myDrawPad(newPad),
    background(true),
    general(true),
    sliderBox(true),
    in_top(0),in_left(0),newT(0),newL(0),nR(0),nB(0),
    buttonPressed(-1), SectionPressed(-1),charHeight(0),hghDisp(190),
    waitForMouseUp(false),
    dirN(),
    fileN(),
    fileSelected(""),filePathSelected(""),
    myWindow(nullptr),
    displayLines(),
    buttons()

{  //Constructor

} //End Constructor

ShowDir::~ShowDir(){
    myDrawPad->ToUpperLevel();
    for (auto bt:buttons) {
        delete bt;
        bt=nullptr;
    }
    buttons.clear();
    for (auto ln:displayLines){
        delete ln;
        ln=nullptr;
    }
    displayLines.clear();
}

//Helper functions

void ShowDir::MakeButton(){
    button_VR *button=new button_VR();
    button->SetOrigin(10,10);
    button->SetDimensions(50,20);
    button->setText("press");
    button->setVisibility(true);
    button->SetToStateColor();
    button->setTextColor(Clr_White);
    buttons.push_back(button);
}

void ShowDir::MakeLine(){
    TextLine *line=new TextLine();
    line->SetTextColor(Clr_White);
    line->SetBackGroundColor(Clr_Gray);
    displayLines.push_back(line);
}

//class members/functions

void ShowDir::SetupDirWindow(int left, int top){
    myDrawPad->Initiate();
    FilePointer::FindCurrentPlaneDir();
    if (displayLines.size()==0){
        background.SetVisibility(true);
        background.SetColor(Clr_DarkGray);
        background.SetOrigin(0,0);
        for (int Cpt(0);Cpt<8;Cpt++)  MakeLine();
        for (int b(0);b<5;b++) MakeButton();
        charHeight=fontMan::GetFontSize(0);
        int orX(10),orY(10);
        general.SetVisibility(true);
        general.SetColor(Clr_Gray);
        general.SetOrigin(orX,orY);

        DrawLogic::SetBackGroundColor(Clr_Gray);
        dirN.Setup(hghDisp,320,10+orX,70+orY);
        fileN.Setup(hghDisp,280,350+orX,70+orY);
        dirN.SetBckColor(Clr_PaperWhite);
        fileN.SetBckColor(Clr_PaperWhite);
        dirN.SetupDirectoryReader();
        fileN.SetDirToRead(dirN.GetActualPathName());
        in_top=top;
        in_left=left;

        //windowTitle = 0,dirTitle=1, fileTitle=2,DirSelected=3,FileSelected=4,FilePicker=5,DirReader=6

        int TitleSize=displayLines[dirTitle]->GetStringWidth();
        int fileTitleSize=displayLines[fileTitle]->GetStringWidth();
        int dirSelectedSize=MeasureString("Directory Selected :");
        int fileSelectedSize=MeasureString("File Selected :");

        //button_ok=0,button_Cancel=1,button_SelDir=2,button_All=3,button_txt=4

        buttons[button_ok]->SetDimensions(30,20);
        buttons[button_ok]->SetOrigin(365+orX,orY+10);
        buttons[button_ok]->setText("OK");
        buttons[button_ok]->setVisibility(false);

        buttons[button_Cancel]->SetDimensions(40,20);
        buttons[button_Cancel]->SetOrigin(290+orX,buttons[button_ok]->GetBottom());
        buttons[button_Cancel]->setText("Cancel");
        buttons[button_Cancel]->setVisibility(true);

        buttons[button_SelDir]->SetDimensions(40,20);
        buttons[button_SelDir]->SetOrigin(120+orX,buttons[button_ok]->GetBottom());
        buttons[button_SelDir]->setText("Select");
        buttons[button_SelDir]->setVisibility(false);

        buttons[button_All]->SetDimensions(30,20);
        buttons[button_All]->SetOrigin(290+orX,fileN.GetTop()+47);
        buttons[button_All]->setText("*.*");
        buttons[button_All]->setVisibility(true);

        buttons[button_txt]->SetDimensions(30,20);
        buttons[button_txt]->SetOrigin(buttons[button_All]->GetLeft()+30,buttons[button_All]->GetBottom());
        buttons[button_txt]->setText(".txt");
        buttons[button_txt]->setVisibility(false);

        sliderBox.SetOrigin(buttons[button_All]->GetLeft()-1,buttons[button_All]->GetBottom()-1);
        sliderBox.SetDimensions(67,22);
        sliderBox.SetColor(Clr_White);
        sliderBox.SetVisibility(true);

        general.SetDimensions(fileN.GetRight()+80,sliderBox.GetTop()+40);
        background.SetDimensions(general.GetWidth()+2*orX,general.GetHeight()+2*orY);

        displayLines[Title]->setText("Choose a File to Open");
        displayLines[Title]->SetFontSize(1);
        int titleW=displayLines[Title]->GetStringWidth();
        displayLines[Title]->SetDimensions(titleW,15);
        displayLines[Title]->SetOrigin((background.GetWidth()/2-titleW/2),general.GetTop()-20);
        displayLines[Title]->SetBackGroundColor(general.GetColor());

        displayLines[extension]->setText("Text files (.txt) in : .."+dirN.GetActualDirName());
        displayLines[extension]->SetDimensions(320,charHeight+2);
        displayLines[extension]->SetOrigin(sliderBox.GetRight()+2,sliderBox.GetBottom()+2);
        displayLines[extension]->SetBackGroundColor(general.GetColor());

        displayLines[dirTitle]->setText("Choose a directory :");
        displayLines[dirTitle]->SetDimensions(TitleSize,charHeight+2);
        displayLines[dirTitle]->SetOrigin(dirN.GetLeft(),dirN.GetTop()+10);
        displayLines[dirTitle]->SetBackGroundColor(general.GetColor());

        displayLines[fileTitle]->setText("Click the file to display :");
        displayLines[fileTitle]->SetDimensions(fileTitleSize,charHeight+2);
        displayLines[fileTitle]->SetOrigin(fileN.GetLeft(),fileN.GetTop()+10);
        displayLines[fileTitle]->SetBackGroundColor(general.GetColor());

        displayLines[DirSelected]->setText("Directory Selected :");
        displayLines[DirSelected]->SetDimensions(dirSelectedSize,charHeight+2);
        displayLines[DirSelected]->SetOrigin(dirN.GetLeft(),dirN.GetBottom()-charHeight-4);
        displayLines[DirSelected]->SetBackGroundColor(general.GetColor());

        displayLines[DirReader]->setText("");
        displayLines[DirReader]->SetDimensions(dirN.GetWidth()+10,charHeight+2);
        displayLines[DirReader]->SetTextColor(Clr_BlackInk);
        displayLines[DirReader]->SetOrigin(dirN.GetLeft(),displayLines[DirSelected]->GetBottom()-15);
        displayLines[DirReader]->SetTextXY(1,2);
        displayLines[DirReader]->SetBackGroundColor(Clr_PaperWhite);

        displayLines[FileSelected]->setText("File Selected :");
        displayLines[FileSelected]->SetTextColor(Clr_White);
        displayLines[FileSelected]->SetDimensions(fileSelectedSize,charHeight+2);
        displayLines[FileSelected]->SetOrigin(fileN.GetLeft(),displayLines[DirSelected]->GetBottom());
        displayLines[FileSelected]->SetBackGroundColor(general.GetColor());

        displayLines[FilePicker]->setText("");
        displayLines[FilePicker]->SetDimensions(fileN.GetWidth()+20,charHeight+2);
        displayLines[FilePicker]->SetTextColor(Clr_BlackInk);
        displayLines[FilePicker]->SetOrigin(displayLines[FileSelected]->GetLeft()+3,displayLines[FileSelected]->GetBottom()-15);
        displayLines[FilePicker]->SetTextXY(2,2);
        displayLines[FilePicker]->SetBackGroundColor(Clr_PaperWhite);

        displayLines[DirReader]->PrintString();
        displayLines[FilePicker]->PrintString();
    }
    else{      
        wWidth=background.GetWidth();
        wHeight=background.GetHeight();
        buttons[button_ok]->setVisibility(false);
        buttons[button_SelDir]->setVisibility(false);

    }
}

void ShowDir::ActivateWindow(){
    myDrawPad->ToUpperLevel();
    wWidth=background.GetWidth();
    wHeight=background.GetHeight();
    myDrawPad->SetNewSize(wWidth,wHeight);
    myDrawPad->SetWindowHandle(myWindow);
    dirN.ShowAll();
    fileN.ShowAll();
    DrawLogic::UpdateTexture();
    if (XPLMWindowIsInVR(myWindow)){
        newT=nB+wHeight;
        nR=newL+wWidth;
        XPLMSetWindowGeometryVR(myWindow,wWidth,wHeight);
    }
    else{
        XPLMGetWindowGeometry(myWindow, &newL, &newT, &nR, &nB);
        newT=nB+wHeight;
        nR=newL+wWidth;
        XPLMSetWindowGeometry(myWindow,newL,newT,nR,nB);
    }
}

void ShowDir::Update(){
    if (myWindow){
        myDrawPad->ToUpperLevel();
        XPLMGetWindowGeometry(myWindow, &newL, &newT, &nR, &nB);
        int wW(0),wH(0);
        if (XPLMWindowIsInVR(myWindow)){
            XPLMGetWindowGeometryVR(myWindow,&wW,&wH);
        }
        else {
            wW=nR-newL;
            wH=newT-nB;
        }
        if ((wH!=wHeight)|(wW!=wWidth)){
            if (XPLMWindowIsInVR(myWindow)){
                XPLMSetWindowGeometryVR(myWindow,wWidth,wHeight);
            }
            else{
                newT=nB+wHeight;
                nR=newL+wWidth;
                XPLMSetWindowGeometry(myWindow,newL,newT,nR,nB);
                XPLMGetWindowGeometry(myWindow, &newL, &newT, &nR, &nB);
            }
        }
        if (dirN.NeedsToDisplay()) {dirN.DisplayPage(true);}
        if (fileN.NeedsToDisplay()) fileN.DisplayPage(true);
        myDrawPad->UpdateDrawPad(wWidth,wHeight,newL,nB,nR,newT);
    }
}

void ShowDir::DrawDirWindow(XPLMWindowID){
    myDrawPad->ToUpperLevel();
    myDrawPad->RenderContent();
}

int  ShowDir::processMouseDn(int x,int y){
    myDrawPad->ToUpperLevel();
    int cx(x-newL),cy(y-nB);
    buttonPressed=-1;
    SectionPressed=-1;
    if (dirN.ProceedClick(cx,cy))  {SectionPressed=DirReader; return DirReader;}
    if (fileN.ProceedClick(cx,cy)) {SectionPressed=FilePicker; return FilePicker;}
    for (ulong b(0);b<5;b++) {
        if (buttons[b]->IsHere(cx,cy)) {
            buttonPressed=static_cast<int>(b);
        }
    }
    return buttonPressed;
}

void ShowDir::processMouseDrag(int x,int y){
    myDrawPad->ToUpperLevel();
    int cx(x-newL),cy(y-nB);

    if (SectionPressed==DirReader){
        dirN.ProceedClickCont(cx,cy);
        dirN.ColorFirstLines();
        return;
    }
    if (SectionPressed==FilePicker){
        fileN.ProceedClickCont(cx,cy);
        return;
    }
}

int  ShowDir::processMouseUp(int,int){
myDrawPad->ToUpperLevel();
    int retVal=-1;
    if (buttonPressed>-1) {
        retVal=buttonPressed;
        switch (buttonPressed){
        case button_ok:{
            SelectFileLine();
            string dr=dirN.GetActualPathName(), fn=displayLines[FilePicker]->GetText();
            if(dr!=""&&fn!=""){
                FilePointer::AddFile(dirN.GetActualPathName(),displayLines[FilePicker]->GetText());
            }
            fileSelected="";
            filePathSelected="";
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
                buttons[button_All]->setVisibility(false);
                buttons[button_txt]->setVisibility(true);
                UpdateDirInfo();
            }
            break;
        }
        case button_txt:{
            if (!fileN.GetTxtOption()) {
                fileN.ToggleAllTxt();
                fileN.ShowAll();
                buttons[button_All]->setVisibility(true);
                buttons[button_txt]->setVisibility(false);
                UpdateDirInfo();
            }
            break;
        }
        }
    }
    if (SectionPressed>-1) {

        if (SectionPressed==DirReader) {
            dirN.ProceedEndClick();
            if (dirN.HasSelection()){
                displayLines[DirReader]->setText(dirN.StringSelected());
                displayLines[DirReader]->PrintString();
                buttons[button_SelDir]->setVisibility(true);}
        }

        if (SectionPressed==FilePicker) {
            fileN.ProceedEndClick();
            if (fileN.HasSelection()){
                displayLines[FilePicker]->setText(fileN.StringSelected());
                displayLines[FilePicker]->PrintString();
                buttons[button_ok]->setVisibility(true);}
        }

    }

    dirN.SetInkColor(Clr_BlackInk);
    dirN.ColorFirstLines();
    return retVal;
}

void ShowDir::SelectDir(){
    dirN.ReadSelectedDir();
    string current=dirN.GetActualPathName();
    fileN.SetDirToRead(current);
    buttons[button_ok]->setVisibility(false);
    buttons[button_SelDir]->setVisibility(false);
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
    displayLines[extension]->setText(info);
    displayLines[extension]->PrintString();
}

int  ShowDir::MeasureString(string in_String){
    return fontMan::MeasureString(in_String);
}

int  ShowDir::GetRight(){return general.GetRight();}
int  ShowDir::GetTop(){return general.GetTop();}
int  ShowDir::GetBottom(){return general.GetBottom();}
int  ShowDir::GetLeft(){return general.GetLeft();}
int  ShowDir::GetWidth(){return general.GetWidth();}
int  ShowDir::GetHeight(){return general.GetHeight();}

void ShowDir::CloseDirWindow(){
    myDrawPad->ToUpperLevel();
    displayLines[DirReader]->setText("");
    displayLines[FilePicker]->setText("");
    myWindow=nullptr;
    myDrawPad->CloseWindow();
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

void ShowDir::SetWindowId(XPLMWindowID inw){
    myWindow=inw;
}
