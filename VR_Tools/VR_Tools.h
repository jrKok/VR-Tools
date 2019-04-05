#ifndef SHOW_FPS_TEST_H
/* future developments :
 *
 *version 1.3
 *
 *
 *
 *  - key shortcuts for text : RWY,ILS, CRS, HDG, BRG, VOR,NDB, ALT, qnh, hPa, WPT, FRQ, Comm,
 *    switch to,holding,flight level ... (2 chars or 1 spec Key)
 *
 *
 * version 1.3.3 :
 *
 *
 * Make a weather report to show like FPS :
 *  - wind speed : sim/weather/wind_speed_kt[0]
 *  - wind direction :  sim/weather/wind_direction_degt[0]
 *  - turbulence : sim/weather/turbulence[0]
 *  - qnh : sim/weather/barometer_sealevel_inhg
 *  - visibility : sim/weather/visibility_reported_m
 *  - temperature on the ground : sim/weather/temperature_sealevel_c
 *
 * full report if sim/atc/atis_enabled
 * short report if sim/atc/com1_tuned_facility	int	n >0
 *
 * make a .ini option to be able to toggle or not the physical keyboard (off by default),
 *  .ini options : keyboard only when motors off/aircraft speed =0 or activation only by plugin menu
 * define international keyboards, make a line in .ini to define which keyboard is to be used.
 * correct bug for frequencies read when power is off
 * bring back tuning to radios from edit window also...
 * menus : new file when avaible for V2), )
 * look again into comm frequencies (sometimes 0.005 doesn't seem to be taken)
 * add an ini option to select if toggle means close text window or hide window
 *
 * version 1.3.4
 *
 * change logic for simultaneous keypresses
 * create a "new file" command (for edit)
 * make helper dialogs for control keys

 *
 * UTF extended (with either a custom table, or use X Planes measure function)
 * multifiles (not multiwindows) :
 *  make a list of active files, viewable in VR and in the menu, max 15 files, navigate to a file
 *   by click, by "next" "previous" "first" "last" button
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
#elif __GNUC__
    #include <OpenGL/gl.h>
#else
    #include <gl/GL.h>
#endif

#ifndef XPLM301
    #error This is made to be compiled against the XPLM301 SDK
#endif
#endif // SHOW_FPS_TEST_H
