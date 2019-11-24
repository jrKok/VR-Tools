#include "vrcommandfilter.h"

bool VRCommandFilter::commandBlock=false;
bool VRCommandFilter::filterEnabled=false;
bool VRCommandFilter::g_in_vr=false;
std::vector<int> VRCommandFilter::state=std::vector<int>();
std::vector<XPLMCommandRef> VRCommandFilter::Commands=std::vector<XPLMCommandRef>();
std::vector<void*> VRCommandFilter::refCons=std::vector<void*>();
XPLMCommandRef VRCommandFilter::CommandFilter=nullptr;
XPLMDataRef VRCommandFilter::g_vr_dref(nullptr);

VRCommandFilter::VRCommandFilter()
{

}

void VRCommandFilter::init(){
    commandBlock=false;
    filterEnabled=false;
    CommandFilter = XPLMCreateCommand("VR_Tools/Custom/Filter_Stick_Commands","Filter Stick");
    //Register Filter command Handler, will create other commands
    XPLMRegisterCommandHandler(CommandFilter,MyFilterCommandHandler,1,(void *)0); //to apply to the button handling the filtering like a "ctrl" key
    g_vr_dref = XPLMFindDataRef("sim/graphics/VR/enabled");
}

void VRCommandFilter::UndoFiltering(){
        //will unassign MyBlockCommandHandler for each command in the vector
        std::size_t lg=Commands.size();
        if (lg>0){
        for (std::size_t I(0);I<lg;I++){
            XPLMUnregisterCommandHandler(Commands[I],MyBlockFilterCommandHandler,1,refCons[I]);
            delete refCons[I];//I hope that is enough to avoid memory leaks
        }
        Commands.clear();
        state.clear();
        refCons.clear();
        filterEnabled=false;
        commandBlock=false;}
}

bool VRCommandFilter::SetupFiltering(){
    /* Reads output/settings/joystick preferences
     * Finds commands related to axis/commands
     * Builds a vector of these commands
     * assigns MyBlockFilterCommand to each of these commands
     * Builds a vector of states for each command (which will block or pass the command)
     */
    int nbcmd=0;
    std::ifstream jSettings("Output/preferences/X-Plane Joystick Settings.prf", std::ifstream::in);
    std::string lineRead;
    stringOps sto;
    while (getline(jSettings,lineRead)){
        if (sto.contains(lineRead,"AXIS_cmnd")){
            std::string eStr=sto.splitRight(lineRead," ");
            eStr=sto.cleanOut(eStr,"\r");
            eStr=sto.cleanOut(eStr,"\n");

            if (eStr!=""){//give this command, if it exists, a custom handler
               XPLMCommandRef cRef=XPLMFindCommand((char*)eStr.c_str());
               if (cRef!=nullptr){
                void * nb=new(int*);
                (*(int*)nb)=nbcmd;
                XPLMRegisterCommandHandler(cRef,MyBlockFilterCommandHandler,1,nb);
                Commands.push_back(cRef);
                state.push_back(0);
                refCons.push_back(nb);
                //Will delete the pointers later
               nbcmd++;}
            }
       }
    }
    jSettings.close();
    if (nbcmd>0) return true; else return false;
}



int VRCommandFilter::MyFilterCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon){//activated when a button for custom command for filtering is pressed
    switch (inPhase)
    {
    case xplm_CommandBegin:
       {const int vr_is_enabled = XPLMGetDatai(g_vr_dref);
        g_in_vr = vr_is_enabled;
        if (filterEnabled & g_in_vr)//I only can block commands if in VR and if CustomCommandHandlers are set
            commandBlock=false; //commands are processed
        else {if (g_in_vr) // if button pressed for the first time...
                   filterEnabled=SetupFiltering(); // try to setup filter of selected commands
              else filterEnabled=false;}
              }//If the filter button has never been pressed, commands are processed normally. I only begin filtering when button is pressed first
        break;
    case xplm_CommandContinue: //do nothing
        break;
    case xplm_CommandEnd:
        if (filterEnabled)
             commandBlock=true; //selected commands are blocked since filter button is released
        else commandBlock=false;
        break;
    }
    return 0;//in every case its my command and i've handled it
}

int VRCommandFilter::MyBlockFilterCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon){
    //Handels built-in commands selected by the filtering function
    int refconToInt=*(static_cast<int*>(inRefcon));
    unsigned long long refVal=static_cast<unsigned long long>(refconToInt);
    switch (inPhase)
    {
    case xplm_CommandBegin: //at the start of the command I will evaluate if I have to block the concerned command
       { if (commandBlock){
            state[refVal]=0;
            return 0;}//block command here
        else{
            state[refVal]=1;
            return 1;//its up to X Plane or others to process command
        }}
    case xplm_CommandContinue:
     {
      return (state[refVal]);}//we've thought out before if we block or pass on the command
    case xplm_CommandEnd:
     {
      return (state[refVal]);} //same as before ...
    }
    return 1;//if nothing else go and do what you need X Plane !
}
