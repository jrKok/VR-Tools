#ifndef SHOW_FPS_TEST_GLOBAL_H
#define SHOW_FPS_TEST_GLOBAL_H

#if defined(SHOW_FPS_TEST_LIBRARY)
#  define SHOW_FPS_TESTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define SHOW_FPS_TESTSHARED_EXPORT Q_DECL_IMPORT
#endif
#if IBM
   #include <filesystem>
    using ulong=unsigned long long;
    using namespace std::experimental::filesystem;
#endif
#if LIN
   #include <experimental/filesystem>
    using namespace  std::experimental::filesystem;
#endif

const int MaxWWidth(1500);
const int MaxWHeight(900);


enum {LineUp=1,LineDown=2,PageUp=3,PageDown=4,LiftClick=5};
enum {BestSplitAtSpace=0,ForceSplitAtSize=1,TruncateKeepLeft=2,DontTruncate=3,TruncateKeepRight=4};
enum {
    Clr_LightBlue=0,
    Clr_SelectBlue=1,
    Clr_PushedBlue=2,
    Clr_Cyan=3,
    Clr_White=4,
    Clr_Black=5,
    Clr_BlackInk=6,
    Clr_PaperWhite=7,
    Clr_PaperDusk=8,
    Clr_Amber=9,
    Clr_Red=10,
    Clr_RedKey=11,
    Clr_Green=12,
    Clr_LighterGray=13,
    Clr_LightGray=14,
    Clr_Gray=15,
    Clr_DarkGray=16,
    Clr_Background=17,
    Clr_BackgroundS=18,
    Clr_BckgrdW=19,
    Clr_InkSelect=20,
    Clr_TextSelectBlue=21,
    Clr_SparkSilver=22
};
#endif // SHOW_FPS_TEST_GLOBAL_H
/*
 * frequencies  NAV108.00:117.95
 *              Com118.00↕136.95
 *              ADF200:1800
*/
