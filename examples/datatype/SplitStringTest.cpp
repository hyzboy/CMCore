#include<hgl/type/SplitString.h>
#include<iostream>

using namespace hgl;
using namespace std;

void Output(U8StringList &sl,const char *str)
{
    cout<<str<<": ";

    for(int i=0;i<sl.GetCount();i++)
        cout<<(char *)"\""<<(char *)sl[i].c_str()<<"\" ";

    cout<<endl;
    sl.Clear();
}

int main(int,char **)
{
    U8StringList sl;

    U8String str=u8"hello game world!";

    {
        U8String left,right;

        SpliteByString(str, U8String(u8"game"), &left, &right);

        cout<<"SpliteByString"<<endl;
        cout << "left: \"" <<(char *) left.c_str() <<"\"" << endl;
        cout << "right: \""<<(char *)right.c_str() <<"\"" << endl;
    }

    {
        SplitToStringListBySpace(sl,str);

        Output(sl,"SplitToStringListBySpace");
    }

    {
        SplitToStringListByChar(sl,str,u8' ');

        Output(sl,"SplitToStringListByChar");
    }

    {
        str=u8"Hello\rgame,\nhello\r\nworld!";

        SplitToStringListByEnter(sl,str);

        Output(sl,"SplitToStringListByEnter");
    }

    {
        str=u8"Hello, Game. World!";

        SplitToStringListByCodes(sl,str);

        Output(sl,"SplitToStringListByCodes");
    }

    {
        str=u8"123,456 789-000";

        SplitToStringListByDigit(sl,str);

        Output(sl,"SplitToStringListByDigit");
    }

    {
        str=u8"0x123,0x456 0x789-0x000";

        SplitToStringListByXDigit(sl,str);

        Output(sl,"SplitToStringListByXDigit");
    }

    {
        str=u8"123.456 789 0.123 3.1415 .5";

        SplitToStringListByFloat(sl,str);

        Output(sl,"SplitToStringListByFloat");
    }

    {
        str=u8"#include<hello.h>\n"
            "#include\"world.h\"\n";

        SplitToStringListByChars(sl,str,U8String(u8"<>\"\n"));

        Output(sl,"SplitToStringListByChars");
    }
}
