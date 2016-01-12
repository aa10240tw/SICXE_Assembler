#include"optab.h"
#include<cstdio>
#include<iostream>
#include<cstring>
#include<fstream>
#include<cstdlib>
#include <map>
#include <algorithm>
#include <iomanip>

using namespace std;
constexpr int LABEL =0;
constexpr int MNE = 1;
constexpr int OPERAND = 2;

constexpr int LOC =0;
constexpr int FORMAT = 1;
constexpr int ADDRESS = 2;

string sic[500][3];
string opcode[500][3];
map <string , string> SYMTAB;

int Search(string &mne)
{
    for(int i=0; i<OPNUM; i++)
    {
        if(mne == OPTAB[i][MNE])
            return i;
    }
    return -1;
};

string DecToHex(string &x)
{
    long num = strtol(x.c_str(),nullptr,10);

    long arr[6]= {1,16,256,4096,65536,1048576};
    char str[17] = "0123456789ABCDEF";

    string result;
    if(num>16777215)//format 4
    {
        result+=str[num/268435456];
        num=num%268435456;

        result+=str[num/16777216];
        num = num%16777216;
    }
    for(int i=3; i>=0; i--)
    {
        result+=str[num/arr[i]];
        num=num%arr[i];
    }
    return result;

}

int HexToDec(string &x)
{
    int num =0;
    for(int i=0; i<x.length(); i++)
        x[i] = (isalpha(x[i]))?(x[i]-'A'+10):(x[i]-'0');
    for(int i=0; i<x.length(); i++)
        num += x[i]*pow(16,x.length()-1-i);
    return num;
}

bool IsPc(int target, int Base ,int pc , bool BaseAsReg)
{
    int disp = target - pc ;
    if(disp >= (-2048) && disp <= 2047)
        return true;
    else if(!BaseAsReg && (disp = target - Base)>=0 && disp<=4095)
        return false;
    else
    {
        cout<< target << " " << pc << " " << Base <<endl;
        cout<< disp <<endl;
        cout<<"ERROR :\n OPERAND TOO LARGE AND EXTENDED FORMAT NOT SPECIFIED !! "<<endl;
        exit(1);
    }
}

int main()
{
    bool BaseAsReg = true;
    /*open file*/
    string filename("\0");
    cin >> filename;
    ifstream infile(filename);
    if(!infile.is_open())
    {
        cout << "No such file!";
        exit(1);
    }

    /*read file*/
    int rows=0;
    string line("\0");
    while(getline(infile,line))
    {
        int flag=0;
        int end=0;
        if(line[0]=='.')
            continue;
        while(static_cast<size_t>(end) < line.length() && end>=0)
        {
            int pos=line.find('\t',end);
            sic[rows][flag] = line.substr(end,pos-end);
            flag++;
            end = pos+1;
            if(!end)
                break;
        }
        rows++;
    }
    infile.close();

    /*pass1*/
    long LOCCTR = (sic[0][MNE]=="START")?strtol(sic[0][OPERAND].c_str(),nullptr,16): 0;
    long startAddress = LOCCTR;
    char loc[10]= {0};
    sprintf(loc,"%d",LOCCTR);
    opcode[0][LOC] = loc;
    opcode[0][LOC] = DecToHex(opcode[0][LOC]);
    opcode[1][LOC] = opcode[0][LOC];
    for(int i=1; i<rows; i++)
    {
        //symbol
        if(!sic[i][LABEL].empty())
        {
            if(SYMTAB[sic[i][LABEL]].empty())
            {
                char num[10]= {0};
                sprintf(num,"%d",LOCCTR);
                string tmp(num);
                SYMTAB[sic[i][LABEL]]=DecToHex(tmp);

            }
            else
            {
                cout << "ERROR FLAG:\nDUPLICATE SYMBOL :" << sic[i][LABEL] << "\n" << endl;
                exit(1);
                break;
            }
        }
        if(sic[i][MNE][0]=='+')//format 4
        {
            sic[i][MNE] = sic[i][MNE].substr(1,sic[i][MNE].length());
            opcode[i][FORMAT]="4";
        }
        //search OPTAB
        int index = Search(sic[i][MNE]);
        if(index != -1)
        {
            if(opcode[i][FORMAT].empty())
                opcode[i][FORMAT] = OPTAB[index][2];
            LOCCTR+=strtol(opcode[i][FORMAT].c_str(),nullptr,10);
        }
        else if(sic[i][MNE] == "WORD")
        {
            LOCCTR += 3;
        }
        else if(sic[i][MNE] == "RESW")
        {
            LOCCTR += 3 * strtol(sic[i][OPERAND].c_str(),nullptr,10);
        }
        else if(sic[i][MNE] == "RESB")
        {
            LOCCTR += strtol(sic[i][OPERAND].c_str(),nullptr,10);
        }
        else if(sic[i][MNE] == "BYTE")
        {
            LOCCTR += (sic[i][OPERAND][0] == 'C') ? (sic[i][OPERAND].length() - 3) : (sic[i][OPERAND].length() - 3)/2;
        }
        else if(sic[i][MNE] == "END")
        {
            opcode[i][LOC] = LOCCTR;
            break;
        }
        else if(sic[i][MNE] == "BASE" || sic[i][MNE] == "NOBASE") {}
        else
        {
            cout<<"ERROR :\nOPCODE : ( "<<sic[i][MNE]<<" ) NOT FOUND !\n";
            exit(1);
            break;
        }
        opcode[i][ADDRESS] = (index != -1)?OPTAB[index][0]:"\0";
        char num[10]= {0};
        sprintf(num,"%d",LOCCTR);
        opcode[i+1][LOC]=num;
        opcode[i+1][LOC] = DecToHex(opcode[i+1][LOC]);
    }
    char len [10]= {0};
    sprintf(len,"%X",(LOCCTR - startAddress));
    string progLength = len;

    /*pass2*/
    ofstream fileout;
    fileout.open("objectCode.o",ios_base::out);

    int Base = 0;
    //Header record
    string hRecord("H^");
    hRecord += sic[0][LABEL];
    for(int i=hRecord.length(); i<8; i++)
        hRecord += " ";
    hRecord += "^";
    for(int i=opcode[0][LOC].length(); i<6; i++)
        hRecord += "0";
    hRecord += opcode[0][LOC];
    hRecord += "^";
    for(int i=progLength.length(); i<6; i++)
        hRecord += "0";
    hRecord += progLength;
    fileout << hRecord << endl;

    //text record
    int recordLen = 0;
    int r = 0;
    string tRecord[100];
    tRecord[r] = "T^";
    for(int i=opcode[1][LOC].length(); i<6; i++)
        tRecord[r] += "0";
    tRecord[r] += opcode[1][LOC];
    for(int j=1; j<rows; j++)
    {
        bool newRecord = false, newLine = false;
        if(opcode[j][ADDRESS]!="\0")
        {
            int n=1,i=1,x=0,b=0,p=0,e=0;
            if(opcode[j][FORMAT]=="1")
            {
                if((recordLen+2)<=60)
                {
                    tRecord[r] += "^";
                    tRecord[r] += opcode[j][ADDRESS];
                    recordLen+=2;
                }
                else
                    newRecord=true;
            }
            else if(opcode[j][FORMAT]=="2")
            {
                char reg1='\0',reg2='\0';
                reg1 = sic[j][OPERAND][0];
                reg2 = (sic[j][OPERAND][1]==','?sic[j][OPERAND][2]:'\0');
                int find1 = reg.find(reg1);
                int find2 = (reg2!='\0') ? reg.find(reg2) :-1;
                if(reg[find1]=='B' && !BaseAsReg)
                {
                    fileout<<"ERROR : Base Register Cannot Be Used Right Now !!"<<endl;
                    exit(1);
                }
                opcode[j][ADDRESS] += (find1!=string::npos)?(48+find1):(48+reg1);
                if(find2==-1)
                {
                    opcode[j][ADDRESS] += '0';
                }
                else
                {
                    if(reg[find2]=='B' && !BaseAsReg)
                    {
                        fileout<<"ERROR : Base Register Cannot Be Used Right Now !!"<<endl;
                        exit(1);
                    }
                    opcode[j][ADDRESS] += (find2!=string::npos)?(48+find2):(48+reg2-1);
                }
                if((recordLen+4)<=60)
                {
                    tRecord[r] += "^";
                    tRecord[r] += opcode[j][ADDRESS];
                    recordLen+=4;
                }
                else
                    newRecord=true;
            }
            else if(opcode[j][FORMAT]=="3")
            {
                long target = 0;
                long pc = strtol(opcode[j+1][LOC].c_str(),nullptr,16);
                long disp=0;
                if(sic[j][OPERAND][0]=='#')//immediate addressing
                {
                    n= 0;
                    sic[j][OPERAND].erase(sic[j][OPERAND].begin());
                    if(isalpha(sic[j][OPERAND][0]))
                    {
                        target = strtol(SYMTAB[sic[j][OPERAND]].c_str(),nullptr,16);
                        disp = (IsPc(target, Base , pc ,BaseAsReg)==true)?(target - pc):(target - Base);
                        b= (IsPc(target, Base , pc ,BaseAsReg)==true)?0:1;
                        p=(b==0)?1:0;
                    }
                    else
                    {
                        disp = strtol(sic[j][OPERAND].c_str(),nullptr,10);
                    }
                }
                else if(sic[j][OPERAND][0]=='@')
                {
                    i=0;
                    sic[j][OPERAND].erase(sic[j][OPERAND].begin());
                    target = strtol(SYMTAB[sic[j][OPERAND]].c_str(),nullptr,16);
                    disp = (IsPc(target, Base , pc ,BaseAsReg)==true)?(target - pc):(target - Base);
                    b= (IsPc(target, Base , pc ,BaseAsReg)==true)?0:1;
                    p=(b==0)?1:0;
                }
                else if(isalpha(sic[j][OPERAND][0]))
                {
                    if(sic[j][OPERAND].find(",X") != string::npos)            // Indexed Addressing
                    {
                        x = 1;
                        sic[j][OPERAND].erase(sic[j][OPERAND].end()-2,sic[j][OPERAND].end());
                    }
                    target = strtol(SYMTAB[sic[j][OPERAND]].c_str(),nullptr,16);
                    disp = (IsPc(target, Base , pc ,BaseAsReg)==true)?(target - pc):(target - Base);
                    b= (IsPc(target, Base , pc ,BaseAsReg)==true)?0:1;
                    p=(b==0)?1:0;
                }
                //op+n+i
                char code1[10]= {0};
                sprintf(code1,"%X",(HexToDec(opcode[j][ADDRESS])+2*n+i));
                opcode[j][ADDRESS] = code1;
                if(opcode[j][ADDRESS].length()==1)
                    opcode[j][ADDRESS].insert(0,"0");
                //xbpe
                int xbpe = 8*x+4*b+2*p+e;
                char code2[10]= {0};
                sprintf(code2,"%X",(xbpe));
                opcode[j][ADDRESS] += code2;
                //disp
                disp = (disp<0)?(4096+disp):disp;
                char code3[10]= {0};
                sprintf(code3,"%X",(disp));
                if(strlen(code3)==1)
                    opcode[j][ADDRESS]+="00";
                else if(strlen(code3)==2)
                    opcode[j][ADDRESS]+="0";
                opcode[j][ADDRESS] += code3;
                if((recordLen+6)<=60)
                {
                    tRecord[r] += "^";
                    tRecord[r] += opcode[j][ADDRESS];
                    recordLen+=6;
                }
                else
                    newRecord=true;
            }
            else
            {
                string target("\0");
                e = 1;
                if(sic[j][OPERAND][0]=='#')
                {
                    n = 0;
                    sic[j][OPERAND].erase(sic[j][OPERAND].begin());
                    target = (isalpha(sic[j][OPERAND][0])) ? (SYMTAB[sic[j][OPERAND]]) : (DecToHex(sic[j][OPERAND]));
                }
                else if(sic[j][OPERAND][0]=='@')
                {
                    i=0;
                    sic[j][OPERAND].erase(sic[j][OPERAND].begin());
                    target = SYMTAB[sic[j][OPERAND]] ;
                }
                else
                {
                    if(sic[j][OPERAND].find(",X") != string::npos)            // Indexed Addressing
                    {
                        x = 1;
                        sic[j][OPERAND].erase(sic[j][OPERAND].end()-2,sic[j][OPERAND].end());
                    }
                    target = SYMTAB[sic[j][OPERAND]];
                }
                //op+n+i
                char code1[10]= {0};
                sprintf(code1,"%X",(HexToDec(opcode[j][ADDRESS])+2*n+i));
                opcode[j][ADDRESS] = code1;
                if(opcode[j][ADDRESS].length()==1)
                    opcode[j][ADDRESS].insert(0,"0");
                //xbpe
                int xbpe = 8*x+4*b+2*p+e;
                char code2[10]= {0};
                sprintf(code2,"%X",(xbpe));
                opcode[j][ADDRESS] += code2;
                //target
                if(target.length()==4)
                    opcode[j][ADDRESS] += "0";
                if((recordLen+8)<=60)
                {
                    tRecord[r] += "^";
                    tRecord[r] += opcode[j][ADDRESS];
                    recordLen+=8;
                }
                else
                    newRecord=true;
            }
        }
        else if(sic[j][MNE]=="BASE")
        {
            BaseAsReg = false;
            Base = strtol(SYMTAB[sic[j][OPERAND]].c_str(),nullptr,16);
        }
        else if(sic[j][MNE]=="NOBASE")
        {
            BaseAsReg = true;
        }
        else if(sic[j][MNE]=="BYTE")
        {
            sic[j][OPERAND].erase(sic[j][OPERAND].begin()+1);
            sic[j][OPERAND].erase(sic[j][OPERAND].end()-1);
            if(sic[j][OPERAND][0]=='C')
            {
                sic[j][OPERAND].erase(sic[j][OPERAND].begin());
                for(int temp = 0; temp < sic[j][OPERAND].length(); temp++)
                {
                    char code[10]= {0};
                    sprintf(code,"%X",sic[j][OPERAND][temp]);
                    opcode[j][ADDRESS] += code;
                }
            }
            else
            {
                sic[j][OPERAND].erase(sic[j][OPERAND].begin());
                opcode[j][ADDRESS] = sic[j][OPERAND];
            }
            if((recordLen+opcode[j][ADDRESS].length())<=60)
            {
                tRecord[r]+="^";
                tRecord[r]+=opcode[j][ADDRESS];
                recordLen+=opcode[j][ADDRESS].length();
            }
            else
                newRecord=true;
        }
        else if(sic[j][MNE]=="WORD")
        {
            opcode[j][ADDRESS] = DecToHex(sic[j][OPERAND]);
            for(int len=opcode[j][ADDRESS].length(); len<6; len++)
                opcode[j][ADDRESS].insert(0,"0");
            if((recordLen+6)<=60)
            {
                tRecord[r]+="^";
                tRecord[r]+=opcode[j][ADDRESS];
                recordLen+=6;
            }
            else
                newRecord=true;
        }
        else if(sic[j][MNE]=="RESW" || sic[j][MNE]=="RESB")
        {
            if(atoi(opcode[j+1][LOC].c_str())-atoi(opcode[j][LOC].c_str())>=1000)
                newLine=true;
        }
        else if(sic[j][MNE] == "END")
        {
            tRecord[r].insert(8,"^");
            char len[10]= {0};
            sprintf(len,"%X",recordLen/2);
            if(strlen(len)==1)
            {
                tRecord[r].insert(9,"0");
                tRecord[r].insert(10,len);
            }
            else
            {
                tRecord[r].insert(9,len);
            }
            fileout << tRecord[r] << endl;
            string eRecord("E^");
            eRecord+= SYMTAB[sic[j][OPERAND]];
            fileout << eRecord << endl;
            break;
        }
        if(recordLen==60 || newLine || newRecord)
        {

            tRecord[r].insert(8,"^");
            char len[10]= {0};
            sprintf(len,"%X",recordLen/2);
            if(strlen(len)==1)
            {
                tRecord[r].insert(9,"0");
                tRecord[r].insert(10,len);
            }
            else
            {
                tRecord[r].insert(9,len);
            }
            fileout << tRecord[r] << endl;
            recordLen = 0;
            r++;
            tRecord[r]="T^";
            for(int k=opcode[j+1][LOC].length(); k<6; k++)
                tRecord[r]+="0";
            tRecord[r] += opcode[j+1][LOC];
        }
        if(newRecord)
        {
            string loc("\0");
            for(int k= opcode[j][LOC].length(); k<6; k++)
                loc += "0";
            loc += opcode[j][LOC];
            tRecord[r].replace(2,7,loc);
            tRecord[r] += "^";
            tRecord[r] += opcode[j][ADDRESS];
            recordLen += opcode[j][ADDRESS].length();
        }
    }
    fileout.close();
    return 0;
}
