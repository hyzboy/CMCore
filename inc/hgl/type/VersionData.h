#pragma once

namespace hgl
{
    /**
    * 版本数据基类模板
    */
    template<typename T> class VersionData
    {
        uint32 version;
        uint32 cur_version;

        T cur_data;

    protected:
        
        const uint32 UpdateVersion()                                                                ///<更新版本号
        {
            //版本号只是为了记录变化，让程序知道和上次不一样，所以最大值是多少其实无所谓的
            version=(version >= 1000000000u)?0:++version;
            return version;
        }

        virtual void MakeNewestData(T &)=0;                                                         ///<生成最新的数据(需要派生类重载)

    public:

        VersionData(const T &init_data)
        {
            version=0;
            cur_version=0;
            cur_data=init_data;
        }

        VersionData(const VersionData *vd)
        {
            version=vd->version;
            cur_version=vd->cur_version;

            cur_data=vd->cur_data;
        }

        virtual ~VersionData()=default;

        const T &operator = (const T &data)                                                       ///<赋值操作符
        {
            if (data == cur_data)
                return cur_data;

            cur_data=data;
            cur_version=version;

            UpdateVersion();

            return cur_data;
        }

        void UpdateNewestData()
        {
            if(IsNewestVersion())
                return;

            MakeNewestData(cur_data);
            cur_version=version;
        }

        const uint32    GetNewestVersion()const { return version; }                         ///<取得最新的版本号(注意数据可能不是最新的)
        const uint32    GetCurrentVersion()const { return cur_version; }                    ///<取得当前数据的版本号

        const bool      IsNewestVersion()const { return cur_version == version; }           ///<当前数据是否最新版本

        const T &       GetCurrentVersionData()const { return cur_data; }                   ///<取得当前版本的数据(注意可能不是最新的)

        const T &       GetNewestVersionData(){UpdateNewestData();return cur_data;}         ///<取得最新版本的数据

        const uint32    GetNewestVersionData(T &result)                                     ///<取得最新版本的数据
        {
            UpdateNewestData();

            result=cur_data;
            return cur_version;
        }

        /**
         * 取得最新版本的数据(如果已有数据已经是最新版本，那么不会产生任何操作)
         * @param result 数据结果
         * @param had_version 已有数据的版本号
         * @return 返回最新的版本号
         */
        const uint32    GetNewestVersionData(T &result,const uint32 had_version)
        {
            if (had_version == cur_version)
                return cur_version;

            return GetNewestVersionData(result);
        }
    };//class VersionData
}//namespace hgl
