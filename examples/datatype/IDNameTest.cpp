#include<hgl/type/IDName.h>
#include<iostream>

int main(int,char **)
{
    using namespace hgl;

    HGL_DEFINE_ANSI_IDNAME(AIDName)

    AIDName id3("idA");
    AIDName id4("idB");

    AIDName id5("idA");
    AIDName id6("idB");

    std::cout<<"id5 id:"<<id5.GetID()<<" name:\""<<id5.GetName()<<"\", ClassHC: "<<std::hex<<id5.GetClassHC()<<std::endl;
    std::cout<<"id6 id:"<<id6.GetID()<<" name:\""<<id6.GetName()<<"\", ClassHC: "<<std::hex<<id6.GetClassHC()<<std::endl;

    std::cout<<"id3 id:"<<id3.GetID()<<" name:\""<<id3.GetName()<<"\", ClassHC: "<<std::hex<<id3.GetClassHC()<<std::endl;
    std::cout<<"id4 id:"<<id4.GetID()<<" name:\""<<id4.GetName()<<"\", ClassHC: "<<std::hex<<id4.GetClassHC()<<std::endl;

    if(id3==id5)
        std::cout<<"id3==id5 is OK!"<<std::endl;
    else
        std::cout<<"id3!=id5 is BUG!"<<std::endl;
}
