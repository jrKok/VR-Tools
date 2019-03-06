#ifndef SHOW_FPS_TEST_H
/* future developments :
 *
 *version 1.3
 *
 *
 * emit 1.3.0 log :
 *
 * improved performance
 * implemented copy to/Past from windows clipboard (text only)
 * implemented shortcut to current airplane's directory
 * after 11.30, fps read from new smoothed dataref
 * implemented an editor for hotspots
 *
 * ver 1.3.0
 *
 * for hotspots : make a relog button/command
 *
 * improve geometry of window in case of non constrained view of a large file (to avoid having difficulties grabbing the window)
 *
 *  - key shortcuts for text : RWY,ILS, CRS, HDG, BRG, VOR,NDB, ALT, qnh, hPa, WPT, FRQ, Comm,
 *    switch to,holding,flight level ... (2 chars or 1 spec Key)
 *
 *
 * version 1.3.1 :
 *
 * make a .ini option to be able to toggle or not the physical keyboard (off by default),
 *  .ini options : keyboard only when motors off/aircraft speed =0 or activation only by plugin menu
 * change key colors if physical keyboard activated
 * define international keyboards, make a line in .ini to define which keyboard is to be used.
 * correct bug for frequencies read when power is off
 * bring back tuning to radios from edit window also...
 * menus : new file when avaible for V2), )
 * look again into comm frequencies (sometimes 0.005 doesn't seem to be taken)
 * add an ini option to select if toggle means close text window or hide window
 *
 * version 1.3.2
 *
 * change logic for simultaneous keypresses
 *
 * version 1.4 :

 * create a "new file" command (for edit)
 * ask for saving modifications on quit (and/or plugin stop)
 * make helper dialogs for control keys

 *   verify with sim/graphics/VR/teleport_on_ground	int	n not >0, verify VR is on
 *
 * UTF extended (with either a custom table, or use X Planes measure function)
 * multifiles (not multiwindows) :
 *  make a list of active files, viewable in VR and in the menu, max 15 files, navigate to a file
 *   by click, by "next" "previous" "first" "last" button
 *
 * version 1.4.1
 *
 * make a numpad
 * modern openGL or vulkan
 * texture keyboard (could be done earlier)
 * fuel manager ?
 *
 * */


#if IBM
    #include <Windows.h>
#endif
#if LIN
    #include <GL/gl.h>
#elif __GNUC__
    #include <OpenGL/gl.h>
#else
    #include <gl/GL.h>
#endif

#ifndef XPLM301
    #error This is made to be compiled against the XPLM301 SDK
#endif
#endif // SHOW_FPS_TEST_H
