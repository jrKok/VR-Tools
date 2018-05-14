#include "stringops.h"

stringOps::stringOps()
{

}

void stringOps::WriteDebug(std::string message){
    std::string in_String="VR Tools : " +message+"\n";
    XPLMDebugString((char*)in_String.c_str());
}

bool stringOps::contains(const std::string inString, const std::string inContains){
    std::size_t fd=inString.find(inContains);
    if (fd!=std::string::npos) return true;
        else return false;
}

std::string stringOps::splitRight(const std::string inString, const std::string inSplit){
    std::size_t len=inString.length(),lenSpl=inSplit.length();
    if ((len>0)&&(lenSpl>0)){
        std::size_t fd=inString.find(inSplit);
        if(fd != std::string::npos){
            std::string retStr=inString.substr(fd+lenSpl,len-fd+lenSpl);
            return(retStr);
        }else return("");
    }
    return("");
}

std::string stringOps::cleanOut(const std::string inString,const std::string inToClean){
    size_t len=inToClean.size();
    std::string outString=inString;
    size_t pos=outString.find(inToClean);
    while(pos!=std::string::npos){
        outString=outString.erase(pos,len);
        pos=outString.find(inToClean);
    }
    return (outString);
}

std::string stringOps::bestLeft(std::string &inString,int MaxL){
    std::string left("");
    if (MaxL>0)
    {
      left=inString.substr(0,MaxL);//do a brutal split at MaxL
      inString=inString.substr(MaxL);
      size_t posSpc=left.find_last_of(" ");//try to split at last space found
      if (posSpc!=std::string::npos){
          inString=left.substr(posSpc+1)+inString; // if possible do the split not including the space itself.
          left=left.substr(0,posSpc);
      }
    }
    return left;
}

std::string stringOps::bestLeftSize(std::string &inString, int in_sz){// used for Best Split At Space
    std::string left("");
    if (in_sz>0){
      int MaxL=findLengthForSize(inString,in_sz);
      left=inString.substr(0,MaxL);//do a brutal split at MaxL
      inString=inString.substr(MaxL);
      size_t posSpc=left.find_last_of(" ");//try to split at last space found
      if (posSpc!=std::string::npos){
          inString=left.substr(posSpc+1)+inString; // if possible do the split not including the space itself.
          left=left.substr(0,posSpc);
      }
    }
    return left;
}

std::string stringOps::splitAtSize(std::string &inString, int in_sz){//used for truncate at left and forced split at length
    //the caller has to ensure that inString is superior to in_sz
    std::string left("");
    if (in_sz>0){
       int MaxL=findLengthForSize(inString,in_sz);
       left=inString.substr(0,MaxL);//do a brutal split at MaxL
       inString=inString.substr(MaxL);
    }
    return left;
}

std::string stringOps::splitRightAtSize(std::string &inString, int in_sz){
    std::string right("");
    if (in_sz>0){
        auto lgth=inString.length();
        for (int it(lgth-1);it>=0;it--){
            right=inString.substr(it,(lgth-it));
            if ((int)XPLMMeasureString(xplmFont_Proportional,(char*)right.c_str(),right.size())>=in_sz){
                right=inString.substr(it+1,(lgth-it));
            break;
            }
        }
    }
    return right;
}

int stringOps::findLengthForSize (std::string inString,int in_sz){
    int len(0);
    std::string testSring;
    do{
       len++;
       testSring=inString.substr(0,len);
    }
    while ((StringSize(testSring)<in_sz)&&(len<=inString.size()));
    return (len);
}

int stringOps::StringSize(std::string in_str){
    int retVal;
    retVal=(int)XPLMMeasureString(xplmFont_Proportional,(char*)in_str.c_str(),in_str.size());
    return retVal;
}

std::string stringOps::ToUTF8(std::string ansiSTR){
    int lgth=ansiSTR.length();
    char * conv=new (char [lgth+1]);
    strcpy(conv,ansiSTR.c_str());
    std::string utf8str("");

    for (int itr(0);itr<lgth;itr++){
        unsigned char ansiC=conv[itr];
        if (ansiC<128) {  utf8str.append(1,ansiC);}
        else {
            utf8str.append(1,unsigned char(0xC0 | (ansiC >> 6)));
            utf8str.append(1,unsigned char(0x80 | (ansiC & 0x3F)));}
    }
    delete[] conv;
    return utf8str;
}

std::string stringOps::DecodeInstruction(std::string in_instr, std::string &out_right,std::string &comment){

    std::string left(""),right("");
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

std::string stringOps::Trim(const std::string& str,
                            const std::string& whitespace)
{
    if (str=="") return "";
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}
