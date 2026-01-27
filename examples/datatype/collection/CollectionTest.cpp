#include<hgl/type/Collection.h>
//#include<hgl/type/ValueArray.h>
#include<iostream>

using namespace hgl;
using namespace std;

using               TEST_TYPE   =char;
using               MyEnumerator=ElementEnumerator<TEST_TYPE>;
using               MyCheck     =CheckElementEqual<TEST_TYPE>;

constexpr   size_t  UNIT_BYTES  =sizeof(TEST_TYPE);

void out(const Collection &c)
{
    MyEnumerator me(&c);

#if 0
    const TEST_TYPE *str=me.begin();
    const size_t count=me.size();

    for(size_t i=0;i<count;i++)
    {
        std::cout<<*str<<' ';
        ++str;
    }
#else
    for(const TEST_TYPE &value:me)
        std::cout<<value<<" ";
#endif//

    std::cout<<std::endl;
}

void out3(const Collection &c)
{
    const char *str=(char *)c.begin();
    const size_t count=c.GetCount();

    for(size_t i=0;i<count;i++)
    {
        std::cout<<str[0]<<str[1]<<str[2]<<' ';
        str+=3;
    }

    std::cout<<std::endl;
}

int main(int,char **)
{
    std::cout<<"test one byte Collection."<<std::endl<<std::endl;
    {
        Collection cu(UNIT_BYTES);

        for(TEST_TYPE i=0;i<10;i++)
        {
            cu.AddValue<TEST_TYPE>('A'+i);
            out(cu);
        }

        std::cout<<"insert 'A' data at first."<<std::endl;
        cu.Insert(0,"A");
        out(cu);

        std::cout<<"indexOf(E) is "<<cu.indexOfValue('E')<<std::endl;

        std::cout<<"remove second data."<<std::endl;
        cu.RemoveAt(1);out(cu);

        std::cout<<"remove all 'A'"<<std::endl;
        cu.RemoveValue('A');out(cu);

        std::cout<<"remove 3 items from third"<<std::endl;
        cu.RemoveAt(2,3);out(cu);

        {
            Collection del_cu(UNIT_BYTES);
            CheckElementEqual<TEST_TYPE> cee;

            del_cu.AddValue<TEST_TYPE>('I');
            del_cu.AddValue<TEST_TYPE>('F');

            std::cout<<"remove all 'I' and 'F'"<<std::endl;
            cu.RemoveCollection(del_cu,&cee);out(cu);
        }

        std::cout<<"exchanged two elements, they at 0 and 2"<<std::endl;
        cu.Exchange(0,2);out(cu);
    }

    std::cout<<std::endl;
    std::cout<<"test three bytes Collection."<<std::endl<<std::endl;
    {
        Collection cu3(3);

        for(TEST_TYPE i=0;i<10;i++)
        {
            char data[3];

            data[0]='A'+i;
            data[1]='a'+i;
            data[2]='0'+i;

            cu3.Add(data);
            out3(cu3);
        }

        cu3.Insert(0,0);out3(cu3);

        std::cout<<"indexOf(Ee4) is "<<cu3.indexOf("Ee4")<<std::endl;

        std::cout<<"remove second data."<<std::endl;
        cu3.RemoveAt(1);out3(cu3);

        std::cout<<"remove all 'Aa0'"<<std::endl;
        cu3.Remove("Aa0");out3(cu3);

        std::cout<<"remove 3 items from third"<<std::endl;
        cu3.RemoveAt(2,3);out3(cu3);

        {
            Collection del_cu(3);
            CheckElementMemcmp cee(3);

            del_cu.Add("Ii8");
            del_cu.Add("Ff5");

            std::cout<<"remove all 'Ii8' and 'Ff5'"<<std::endl;
            cu3.RemoveCollection(del_cu,&cee);out3(cu3);
        }

        std::cout<<"exchanged two elements, they at 0 and 2"<<std::endl;
        cu3.Exchange(0,2);out3(cu3);
    }
}
