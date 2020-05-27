#include "VR_Tools.h"
#include "opcenter.h"
#include "hotspots.h"


/* Variables */

static std::unique_ptr<OpCenter> launcher(nullptr);

/* Start Implementations */

void WriteDebug(string in_String)
{
    in_String="VR Tools : " +in_String+"\r\n";
    XPLMDebugString((char*)in_String.c_str());
}

PLUGIN_API int XPluginStart(
                        char *		outName,
                        char *		outSig,
                        char *		outDesc)
{

    strcpy(outName, "VR Tools version 1.3.5");
    strcpy(outSig, "a plug-in by jrKok");
    strcpy(outDesc, "A plug-in to display some datarefs (FPS, speeds, g-Forces), to filter commands, to edit a text file");
        launcher=std::make_unique<OpCenter>();
        launcher->SetupCenter();
        launcher->LaunchOperations();

    return (1);
}



PLUGIN_API void	XPluginStop(void)
{
    launcher->HaltOperations();
    launcher.reset();
    launcher=nullptr;
    XPLMDebugString("VR Tools : Plugin Halted, Deleted OpCenter\n");
}

PLUGIN_API void XPluginDisable(void)
{
    launcher->SuspendOperations();
    XPLMDebugString("VR Tools : Plugin Disabled, preserved OpCenter\n");
}

PLUGIN_API int  XPluginEnable(void)
{
    if (launcher==nullptr) launcher=std::make_unique<OpCenter>();
        else return (launcher->ResumeOperations());
    return 1;
 }

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID, int msg, void * inParam)
{
    if (msg==XPLM_MSG_PLANE_LOADED){
        long idx=reinterpret_cast<long>(inParam);
        if (idx==0){
            VRCReader::GetVRConfigFileName();
            if (VRCReader::HasVRConfig()){
                VRCReader::AnalyzeFile();
                WriteDebug("VRconfig : Number of Hotspots generated = "+std::to_string(VRCReader::GetHotspotCount()));
                Hotspots::MakeMoveComplete();
                launcher->SetEnableHSMoves(VRCReader::HasHotspots());
            }
        }
    }
}
