#include<hgl/type/ConstStringSet.h>
#include<iostream>

using namespace hgl;
using namespace std;

int main()
{
    ConstAnsiStringSet css;

    cout << "Add 'duplicate'..." << endl;
    int id1 = css.Add("duplicate", 9);
    cout << "id1 = " << id1 << ", count = " << css.GetCount() << endl;
    cout << "String at ID 0: " << css.GetString(0) << endl;

    cout << "\nAdd 'duplicate' again..." << endl;
    int id2 = css.Add("duplicate", 9);
    cout << "id2 = " << id2 << ", count = " << css.GetCount() << endl;

    cout << "\nAdd 'different'..." << endl;
    int id3 = css.Add("different", 9);
    cout << "id3 = " << id3 << ", count = " << css.GetCount() << endl;
    cout << "String at ID 0: " << css.GetString(0) << endl;
    cout << "String at ID 1: " << css.GetString(1) << endl;

    cout << "\nAdd 'duplicate' third time..." << endl;
    int id4 = css.Add("duplicate", 9);
    cout << "id4 = " << id4 << ", count = " << css.GetCount() << endl;
    cout << "Expected id4 = 0, got " << id4 << endl;

    cout << "\nAll strings:" << endl;
    for(int i = 0; i < css.GetCount(); i++)
    {
        cout << "ID " << i << ": " << css.GetString(i) << endl;
    }

    return 0;
}
