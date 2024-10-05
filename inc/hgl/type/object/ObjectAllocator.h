#pragma once
//#include<iostream>

namespace hgl
{
    struct ObjectBaseInfo;
    class Object;
    class ObjectManager;

    /**
     * 对象分配器.
     */
    struct ObjectAllocator
    {
        virtual void    Join    (ObjectManager *) {};                           ///<桥接一个对象管理器
        virtual void    Unjoin  (ObjectManager *) {};                           ///<断开一个对象管理器

        virtual Object *Create  (const ObjectBaseInfo &obi)=0;                  ///<创建一个新对象
        virtual void    Release (Object *)=0;                                   ///<释放一个对象

        /**
         * 准备断开对象管理器
         * @param not_release_objects_count 未释放的对象数量
         * @return 是否需要逐个释放(如返回true则会调用CleanNotReleaseObject逐个释放)
         */
        virtual bool    PreUnjoin(size_t not_release_objects_count)
        {
            //std::cerr<<"DefaultObjectManager::~DefaultObjectManager("<<GetTypename()<<") "<<object_set.size()<<" objects havn't been release."<<std::endl;
            return(true);
        }

        virtual void    CleanNotReleaseObject(Object *obj)                      ///<清理一个未释放的对象(用于没有手动释放时的自动清理)
        {
            //const ObjectBaseInfo &obi=obj->GetObjectBaseInfo();

            //std::cout<<"Object(hash_code="<<obi.hash_code
            //         <<", serial_number="<<obi.serial_number
            //         <<", source_file="<<obi.source_code_location.file
            //         <<", source_function="<<obi.source_code_location.func
            //         <<", source_line="<<obi.source_code_location.line
            //         <<")"<<std::endl;

            Release(obj);
        }
    };

    template<typename T> struct DefaultObjectAllocator:public ObjectAllocator
    {
        virtual Object *Create(const ObjectBaseInfo &obi)override
        {
            return new T(obi);
        }

        virtual void Release(Object *obj)override
        {
            delete obj;
        }
    };
}//namespace hgl

