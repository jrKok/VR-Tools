#include "stringops.h"
#include "drawlogic.h"
#include "fontman.h"

string stringOps::digits("0123456789");
string stringOps::numSigns("+.-");
stringOps::stringOps()
{

}

bool stringOps::contains(const string inString, const string inContains){
    if (inString=="") return false;
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
    ulong uMax=static_cast<ulong>(MaxL);
    if (MaxL>0)
    {
      left=inString.substr(0,uMax);//do a brutal split at MaxL
      inString=inString.substr(uMax);
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
      ulong MaxL=static_cast<ulong>(findLengthForSize(inString,in_sz));
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
       ulong MaxL=static_cast<ulong>(findLengthForSize(inString,in_sz));
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
            if (fontMan::MeasureString(right)>=in_sz){
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
    return fontMan::MeasureString(in_str);
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
    ulong posSemi(0); bool found(false);
    if (contains(right,";")) {posSemi=right.find(";");found=true;}
    if (contains(right,"\"")){
        auto firstQt=right.find("\"");
        auto secondQt=right.find_last_of("\"");
        if (found){
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
    if (found){
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

bool stringOps::IsANumber(string to_test){
    if (to_test=="") return false;
    string firstDigit=to_test.substr(0,1);
    if (digits.find(firstDigit)!=std::string::npos) return (true);
    if (numSigns.find(firstDigit)!=std::string::npos){
        if (to_test.size()==1) return false;
        firstDigit=to_test.substr(1,1);
        if (digits.find(firstDigit)!=std::string::npos) return (true);
        else
        {
            if ((firstDigit==".")){
                if (to_test.size()==2) return false;
                else
                    if (digits.find(to_test.substr(2,1))!=std::string::npos)
                        return true;
                    else {
                        return false;
                    }
            }
            else {
                return false;
            }
        }
    }
    else return (false);
}

string stringOps::ConvertFloatToString(float in_float,ulong prec) {
    if (prec==0) in_float=round(in_float);
    string fString("");
    fString=std::to_string(in_float);
    ulong posDot=fString.find(".");
    if (posDot!=string::npos){
        if(prec==0) fString=fString.erase(posDot,string::npos);
        else
        {if (fString.length()-posDot>prec+1) fString=fString.erase(posDot+prec+1,string::npos);}
    }
    return(fString);
}

float  stringOps::ConvertStringToFloat(string in_string) {
    if (in_string!=""){
        if (stringOps::IsANumber(in_string)){
            float out_float(0);
            out_float=stof(in_string);
            return out_float;
        }
        else{
            DrawLogic::WriteDebug("vrconfig a string couldn't be converted to a float (wasn't a number)");
            return 0;
        }
    }
    else {
    DrawLogic::WriteDebug("vrconfig a string couldn't be converted to a float (was empty)");
    return 0;
    }
}
