#include "VR_Tools.h"
#include "opcenter.h"


/* Variables */

static OpCenter launcher;

/* Start Implementations */

void WriteDebug(string in_String){
    in_String="VR Tools : " +in_String+"\r\n";
    XPLMDebugString((char*)in_String.c_str());}

PLUGIN_API int XPluginStart(
                        char *		outName,
                        char *		outSig,
                        char *		outDesc)
{

    strcpy(outName, "VR Tools version 1.3.0 rc1");
    strcpy(outSig, "a plug-in by jrKok");
    strcpy(outDesc, "A plug-in to display some datarefs (FPS, speeds, g-Forces), to filter commands, to edit a text file");

    return (1);
}



PLUGIN_API void	XPluginStop(void)
{
launcher.HaltOperations();
}

PLUGIN_API void XPluginDisable(void) {launcher.SuspendOperations(); }

PLUGIN_API int  XPluginEnable(void)  { return (launcher.ResumeOperations()); }

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int msg, void * inParam){

    if (msg == XPLM_MSG_SCENERY_LOADED){
        if (!launcher.IsLaunched){
            launcher.SetupCenter();
            launcher.LaunchOperations();
        }
        else{
            VRCReader::ResetVRCReader();
        }
    }

    if (msg==XPLM_MSG_PLANE_LOADED){
        VRCReader::GetVRConfigFileName();
        if (VRCReader::HasVRConfig()){
            VRCReader::AnalyzeFile();
            WriteDebug("VRconfig : Number of Hotspots generated = "+std::to_string(VRCReader::GetHotspotCount()));
        }

    }
}
