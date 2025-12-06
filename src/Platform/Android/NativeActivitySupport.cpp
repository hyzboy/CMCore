#include<hgl/platform/Platform.h>
#include<android/native_activity.h>

namespace hgl
{
    namespace logger
    {
        void SetLocalAppdataPath(const char *fn);
    }//namespace logger

    void InitAndroidSupport(struct ANativeActivity *native_activity)
    {
        //AAssetManager *assetManager = native_activity->activity->assetManager;

        logger::SetLocalAppdataPath(native_activity->internalDataPath);
    }
}//namespace hgl
