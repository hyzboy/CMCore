#include<hgl/type/ConstStringSet.h>
#include<iostream>

using namespace hgl;
using namespace std;

char rand_str[5];

int make_rand_str()
{
    int len=rand()%3+1;

    for(int i=0;i<len;i++)
    {
        int v=rand()%(26+26+10);

        if(v<26)
            rand_str[i]=v%26+'a';
        else
        if(v<52)
            rand_str[i]=(v-26)%26+'A';
        else
            rand_str[i]=(v-52)%10+'0';
    }

    rand_str[len]=0;
    return len;
}

int main(int,char **)
{
    ConstAnsiStringSet cass;
    ConstAnsiStringView csv;
    int len;
    int id;

    for(int i=0;i<1000;i++)
    {
        len=make_rand_str();

        id=cass.AddString(csv,rand_str,len);

        std::cout<<i<<" : "<<id<<" : "<<rand_str<<std::endl;
    }

    std::cout<<"save to filename: csv.txt"<<std::endl;

    SaveToTextFile(OS_TEXT("csv.txt"),&cass,false,true);
    SaveToTextFile(OS_TEXT("csv_id.txt"),&cass,true,true);

    return 0;
}