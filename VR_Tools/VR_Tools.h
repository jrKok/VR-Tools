#ifndef SHOW_FPS_TEST_H

/****************************************************************************************
 * DISCLAIMERS                                                                          *
 * **************************************************************************************
 * PROGRAM VR_TOOLS by Jeroen Kok (jrKok)
 * a plugin for X Plane
 * Implements functionality aimed at enhancing the use of virtual reality in X Plane
 * code published on Github
 * DEVELOPED in the Qt IDE (Qt company https://www.qt.io/company)
 * under GPL licence : https://www.gnu.org/licenses/gpl-3.0.txt
      This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    Libraries used (credits to all developers) :

    *** Glew, The OpenGL Extension Wrangler Library github : https://github.com/nigels-com/glew
    *** Freetype, for font management https://www.freetype.org/
 **********************************************************************************************

* version 1.3.5
 *  Change log :
 *   performance optimization (the plugin renders much faster now and is compliant with LR guidelines)
 *   rendering for text made "kneeboard" style
 *   improved text windows layout
 *   improved the layout of the window for file opening
 *   relabeled buttons
 *   remade the "streaming" function for text files
 *   made a kneeboard background (in real size)
 *   corrected bug in the hotspot editor when hotspot file wasn't generated when the config.txt was corrupt
 *     will now overwrite a correctly formated config.txt
 *  new menu
 *  linux version
 *  make user manual
 *
 * to come in version 1.3.6
 *
 * FPS with bigger font if required
 * reimplement UTF button
 * linux system clipboard
 * implement tab with fixed positions and a backtab
 * reimplement delete lines in textreader (with a stack for deleted lined)
 * reimplement inisettings with a map for parameters
 * ATC reader (makes a commit to log, then reads ATC instructions), will do it on command
 * make international keyboards (french, german, dutch, swedish, norsk, dansk, italian, espagnol, then greek, russian for 1.3.6)
 * try to make a background image for layout and layout with edit, instead of kneeboard if interested
 * try to implement graphic representation of advanced options (low priority)
   bring DRef window under drawlogic
 *  change logic for simultaneous keypresses
 *  create a "new file" command (for edit)
 * see if read file can be made asynchronous
 *
 * */


#if IBM
    #include <Windows.h>
#endif
#if LIN
    #include <cstring>
    #include <cstdint>
#endif

#ifndef XPLM303
    #error This is made to be compiled against the XPLM303 SDK
#endif
#include <memory>
#endif // SHOW_FPS_TEST_H
