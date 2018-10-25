#include "stringops.h"

stringOps::stringOps()
{

}

void stringOps::WriteDebug(string message){
    string in_String="VR Tools : " +message+"\n";
    XPLMDebugString((char*)in_String.c_str());
}

bool stringOps::contains(const string inString, const string inContains){
    std::size_t fd=inString.find(inContains);
    if (fd!=string::npos) return true;
        else return false;
}

string stringOps::splitRight(const string inString, const string inSplit){
    std::size_t len=inString.length(),lenSpl=inSplit.length();
    if ((len>0)&&(lenSpl>0)){
        std::size_t fd=inString.find(inSplit);
        if(fd != string::npos){
            string retStr=inString.substr(fd+lenSpl,len-fd+lenSpl);
            return(retStr);
        }else return("");
    }
    return("");
}

string stringOps::cleanOut(const string inString,const string inToClean){
    size_t len=inToClean.size();
    string outString=inString;
    size_t pos=outString.find(inToClean);
    while(pos!=string::npos){
        outString=outString.erase(pos,len);
        pos=outString.find(inToClean);
    }
    return (outString);
}

string stringOps::bestLeft(string &inString,int MaxL){
    string left("");
    if (MaxL>0)
    {
      left=inString.substr(0,MaxL);//do a brutal split at MaxL
      inString=inString.substr(MaxL);
      size_t posSpc=left.find_last_of(" ");//try to split at last space found
      if (posSpc!=string::npos){
          inString=left.substr(posSpc+1)+inString; // if possible do the split not including the space itself.
          left=left.substr(0,posSpc);
      }
    }
    return left;
}

string stringOps::bestLeftSize(string &inString, int in_sz){// used for Best Split At Space
    string left("");
    if (in_sz>0){
      int MaxL=findLengthForSize(inString,in_sz);
      left=inString.substr(0,MaxL);//do a brutal split at MaxL
      inString=inString.substr(MaxL);
      size_t posSpc=left.find_last_of(" ");//try to split at last space found
      if (posSpc!=string::npos){
          inString=left.substr(posSpc+1)+inString; // if possible do the split not including the space itself.
          left=left.substr(0,posSpc+1);//but here include the space
      }
    }
    return left;
}

string stringOps::splitAtSize(string &inString, int in_sz){//used for truncate at left and forced split at length
    //the caller has to ensure that inString is superior to in_sz
    string left("");
    if (in_sz>0){
       int MaxL=findLengthForSize(inString,in_sz);
       left=inString.substr(0,MaxL);//do a brutal split at MaxL
       inString=inString.substr(MaxL);
    }
    return left;
}

string stringOps::splitRightAtSize(string &inString, int in_sz){
    string right("");
    if (in_sz>0){
        ulong lgth=inString.length();
        for (ulong it(lgth);it>0;it--){
            right=inString.substr(it,(lgth-it));
            if ((int)XPLMMeasureString(xplmFont_Proportional,(char*)right.c_str(),right.size())>=in_sz){
                right=inString.substr(it+1,(lgth-it));
            break;
            }
        }
    }
    return right;
}

int stringOps::findLengthForSize (string inString,int in_sz){
    ulong len(0);
    string testString;
    do{
       len++;
       testString=inString.substr(0,len);
    }
    while ((StringSize(testString)<in_sz)&&(len<=inString.size()));
    return (static_cast<int>(len-1));
}

int stringOps::StringSize(string in_str){
    int retVal;
    retVal=static_cast<int>(XPLMMeasureString(xplmFont_Proportional,(char*)in_str.c_str(),in_str.size()));
    return retVal;
}

string stringOps::ToUTF8(string ansiSTR){

    string utf8str("");

    for (char fromStr: ansiSTR){
        unsigned char ansiC=reinterpret_cast<unsigned char&>(fromStr);
        if (ansiC<128)   utf8str.append(1,fromStr);
        else {
            utf8str.append(1,char(0xC0 | (ansiC >> 6)));
            utf8str.append(1,char(0x80 | (ansiC & 0x3F)));}
    }

    return utf8str;
}

string stringOps::DecodeInstruction(string in_instr, string &out_right,string &comment){
//decodes a line of the .ini file
    string left(""),right("");
    if ((in_instr.find(";")==0)) {
        comment=in_instr;
        return ("");
    }

    if (!contains(in_instr,"=")) return left;
    auto posEq=in_instr.find("=");
    left=in_instr.substr(0,posEq);
    right=in_instr.substr(posEq+1);
    left=Trim(left," \t");
    if (contains(left,";")) return (""); //then line is commented
    //if there is text in quotes, extract it, and it will become the value to return
    int posSemi(-1);
    if (contains(right,";")) posSemi=right.find(";");
    if (contains(right,"\"")){
        auto firstQt=right.find("\"");
        auto secondQt=right.find_last_of("\"");
        if (posSemi>=0){
            if (firstQt<posSemi){
               if (secondQt==firstQt) right="";
               else {
                   out_right=Trim(right.substr(firstQt+1,(secondQt-firstQt-1))," \t");
                   right=right.substr(secondQt+1);
                   if (contains(right,";")) {
                       posSemi=right.find(";");
                       comment=right.substr(posSemi);}
                   return left;
               }
            }
        }else{
            if(secondQt==firstQt) return left;
            else{
                right=right.substr(firstQt+1,(secondQt-firstQt-1));
            }
        }

    }
    //no text in quotes, first split of any comments
    if (posSemi>-1){
        comment=right.substr(posSemi);//return the comment with the semi colon
        right=right.substr(0,posSemi);
    }
    //then return the trimmed remainder and the left part of the instruction

    out_right=Trim(right," \t");
    return left;
}

string stringOps::Trim(const string& str,
                            const string& whitespace)
{//removes starting and trailing "whitespace")
    if (str=="") return "";
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

string stringOps::RemoveLastUTFCharFromString(string in_string){
    ulong lg=in_string.length();
    if (lg>0)
    {
        unsigned char lastC=static_cast<unsigned char>(in_string.back());
        if ((lastC>=0x80)&&(lg>1)) in_string=in_string.substr(0,lg-2);
        else in_string=in_string.substr(0,lg-1);
    }
    return in_string;
}
