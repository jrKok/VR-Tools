#ifndef SHOW_FPS_TEST_GLOBAL_H
#define SHOW_FPS_TEST_GLOBAL_H

#if defined(SHOW_FPS_TEST_LIBRARY)
#  define SHOW_FPS_TESTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define SHOW_FPS_TESTSHARED_EXPORT Q_DECL_IMPORT
#endif

enum{B_Load_File=0,
     B_Reload=1,
     B_Auto=2,
     B_Toggle_Line=3,
     B_UTF8=4,
     B_Undo=5,
     B_Edit_Line=6,
     B_Save=7,
     B_More_Lines=8,
     B_Less_Lines=9,
     B_NAV1 = 10,
     B_NAV2 = 11,
     B_COM1 = 12,
     B_COM2 = 13,
     B_ADF1 = 14,
     B_ADF2 = 15,
     B_Show_All=16,
     B_NextLine=17,
     B_PrevLine=18,
     B_FirstLine=19
     };
enum {LineUp=1,LineDown=2,PageUp=3,PageDown=4,LiftClick=5};
enum {BestSplitAtSpace=0,ForceSplitAtSize=1,TruncateKeepLeft=2,DontTruncate=3,TruncateKeepRight=4};
#endif // SHOW_FPS_TEST_GLOBAL_H
/*
 * frequencies  NAV108.00:117.95
 *              Com118.00↕136.95
 *              ADF200:1800
*/
