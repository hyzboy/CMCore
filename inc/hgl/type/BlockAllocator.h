#pragma once

#include<hgl/type/SimpleValuePool.h>
#include<hgl/type/FlatOrderedSet.h>

namespace hgl
{
    /**
     * 数据链管理器(注：它只管理这个链，并不管理数据)<br>
     * 它的思想是将空间分配成一个个固定长度的单位，然后通过链表来管理这些单位的使用情况。
     * 当用户释放一个空间时，会自动合并相邻的空间节点。如果没有相邻的空间节点，会自动创建一个新的空间节点。
     * 当用户申请一个新的空间时，会自动查找最适合的空间节点。
     */
    class BlockAllocator
    {
        SeriesInt series;                   ///<序号池

        int max_count;                      ///<最大数量

    public:

        /**
        * 用户数据节点信息
        */
        struct UserNode
        {
        private:

            int start;                      ///<起始数据块
            int count;                      ///<占用数据块数量

            friend class BlockAllocator;

        public:

            const int GetStart()const{return start;}
            const int GetCount()const{return count;}
            const int GetEnd()const{return start+count;}
        };//struct UserNode

    protected:

        SimpleValuePool<UserNode> ud_pool;    ///<用户数据占用信息池

        FlatOrderedValueSet<UserNode *> ud_set;      ///<用户数据占用合集

    public:

        /**
        * 数据块链表节点
        */
        struct ChainNode
        {
            ChainNode *prev;
            ChainNode *next;

            int start;                      ///<起始数据块
            int count;                      ///<数据块数量

        public:

            const int GetEnd()const{return start+count;}
        };//struct ChainNode

    private:

        SimpleValuePool<ChainNode> node_pool; ///<链表节点池

        int free_count;                     ///<空闲数据块数量

        ChainNode *start,*end;

    public:

        const int GetMaxCount   ()const{return max_count;}
        const int GetFreeCount  ()const{return free_count;}

        ChainNode *GetStartNode ()const{return start;}
        ChainNode *GetEndNode   ()const{return end;}

    public:

        BlockAllocator();
        virtual ~BlockAllocator()=default;

        bool Init(const int mc);                            ///<初始化数据链

        UserNode *Acquire(const int acquire_count);         ///<请求一个数据区块
        bool Release(UserNode *ud);                         ///<释放一个数据区块
    };//class BlockAllocator
}//namespace hgl
