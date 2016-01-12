#ifndef OPTAB_H
#define OPTAB_H
#include <iostream>
#include<cstring>

constexpr int OPNUM = 45;
std::string reg = "AXLBSTF";

std::string OPTAB[OPNUM][3] =
{
    {"18","ADD","3"},
    {"90","ADDR","2"},
    {"40","AND","3"},
    {"B4","CLEAR","2"},
    {"28","COMP","3"},
    {"A0","COMPR","2"},
    {"24","DIV","3"},
    {"9C","DIVR","2"},
    {"3C","J","3"},
    {"30","JEQ","3"},
    {"34","JGT","3"},
    {"38","JLT","3"},
    {"48","JSUB","3"},
    {"00","LDA","3"},
    {"68","LDB","3"},
    {"50","LDCH","3"},
    {"08","LDL","3"},
    {"6C","LDS","3"},
    {"74","LDT","3"},
    {"04","LDX","3"},
    {"D0","LPS","3"},
    {"20","MUL","3"},
    {"98","MULR","2"},
    {"44","OR","3"},
    {"D8","RD","3"},
    {"AC","RMO","2"},
    {"4C","RSUB","3"},
    {"A4","SHIFTL","2"},
    {"A8","SHIFTR","2"},
    {"0C","STA","3"},
    {"78","STB","3"},
    {"54","STCH","3"},
    {"D4","STI","3"},
    {"14","STL","3"},
    {"7C","STS","3"},
    {"E8","STSW","3"},
    {"84","STT","3"},
    {"10","STX","3"},
    {"1C","SUB","3"},
    {"94","SUBR","2"},
    {"B0","SVC","2"},
    {"E0","TD","3"},
    {"2C","TIX","3"},
    {"B8","TIXR","2"},
    {"DC","WD","3"}
};


#endif
