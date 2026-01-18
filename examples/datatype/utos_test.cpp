#include<iostream>
#include<stdlib.h>
#include<hgl/type/StrChar.h>

using namespace std;
using namespace hgl;

int main(int,char **)
{
    srand(time(nullptr));

    long int rr=rand();

    char str[64];

    for(uint i=2;i<=26+10;i++)
    {
        utos(   str,        //输出字符串
                64,         //输出字符串缓冲区长度
                rr,         //要转换的数值
                i);         //进制

        cout<<i<<":"<<str<<endl;
    }

    return 0;
}
