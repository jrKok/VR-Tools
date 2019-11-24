#ifndef SHOW_FPS_TEST_H
/*
 * Change log : put the "filter" command under VR Tools hierarchy
 *
 * Next :
 * make a stack for text files, to navigate easily from one file to another
 *
 *
 * future developments :
 *
 *version 1.3
 *
 *
 *
 *  - key shortcuts for text : RWY,ILS, CRS, HDG, BRG, VOR,NDB, ALT, qnh, hPa, WPT, FRQ, Comm,
 *    switch to,holding,flight level ... (2 chars or 1 spec Key)
 *
 *1.3.3
 * Linux
 *
 * version 1.3.4 :
 *
 *
 * make a .ini option to be able to toggle or not the physical keyboard (off by default),
 *  .ini options : keyboard only when motors off/aircraft speed =0 or activation only by plugin menu
 * define international keyboards, make a line in .ini to define which keyboard is to be used.
 * correct bug for frequencies read when power is off
 * bring back tuning to radios from edit window also...
 * add an ini option to select if toggle means close text window or hide window

 * change logic for simultaneous keypresses
 * create a "new file" command (for edit)
 * make helper dialogs for control keys
 *
 * multifiles (not multiwindows) :
 *  make a list of active files, viewable in VR and in the menu, max 15 files, navigate to a file
 *   by click, by "next" "previous" "first" "last" button
 * make a stack for text files
 *
 * version 1.3.5
 *
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
    #include <cstring>
    #include <cstdint>
#else
    #include <gl/GL.h>
#endif

#ifndef XPLM301
    #error This is made to be compiled against the XPLM301 SDK
#endif
#endif // SHOW_FPS_TEST_H
