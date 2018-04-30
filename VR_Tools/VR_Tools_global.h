#ifndef SHOW_FPS_TEST_GLOBAL_H
#define SHOW_FPS_TEST_GLOBAL_H

#if defined(SHOW_FPS_TEST_LIBRARY)
#  define SHOW_FPS_TESTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define SHOW_FPS_TESTSHARED_EXPORT Q_DECL_IMPORT
#endif

enum{B_Load_File=0,
     B_Page_Up=1,
     B_Reload=1,
     B_Line_Up=2,
     B_Auto=2,
     B_Toggle_Line=3,
     B_Line_Down=4,
     B_Page_Down=5,
     B_Longer_Lines=6,
     B_Smaller_Lines=7,
     B_More_Lines=8,
     B_Less_Lines=9,
     B_NAV1 = 10,
     B_NAV2 = 11,
     B_COM1 = 12,
     B_COM2 = 13,
     B_ADF1 = 14,
     B_ADF2 = 15,
     B_Show_All=16};
enum {LineUp=1,LineDown=2,PageUp=3,PageDown=4,LiftClick=5};
enum {BestSplitAtSpace=0,ForceSplitAtSize=1,TruncateAtSize=2,DontTruncate=3};
#endif // SHOW_FPS_TEST_GLOBAL_H
/*
 * frequencies  NAV108.00:117.95
 *              Com118.00↕136.95
 *              ADF200:1800
*/
