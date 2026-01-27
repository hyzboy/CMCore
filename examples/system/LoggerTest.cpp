#include<hgl/log/log.h>

using namespace hgl;

struct TestObject
{
    OBJECT_LOGGER;

public:

    TestObject(int index)
    {
        Log.SetLoggerInstanceName(OSString::numberOf(index));

        LogVerbose("TestObject()");
    }

    ~TestObject()
    {
        LogVerbose("~TestObject()");
    }
};

int main()
{
    TestObject to(1);

    return 0;
}
