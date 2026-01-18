#include<hgl/time/DateTime.h>
#include<iostream>

using namespace hgl;
using namespace std;

int main(int,char **)
{
    CalendarDate date(2024,1,15);
    TimeOfDay time(14,30,45,123456);

    DateTimeFormat dtf(DateFormat("YYYY-MM-DD"), TimeFormat("HH:MM:SS.us"), " ");

    AnsiString dt_string = DateTimeToString(date, time, dtf);

    cout << "Formatted DateTime: " << dt_string.c_str() << endl;

    return 0;
}
