#include<cstring>
#include<iostream>
#include<hgl/type/DataArray.h>

int main(int,char **)
{
    char left   []="1234567890";
    char right  []="2345678901";

    std::cout<<"memcmp: "<<std::memcmp(left,right,10)<<std::endl;

    return 0;
}