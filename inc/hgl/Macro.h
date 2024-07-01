#pragma once

namespace hgl
{
    #define RETURN_OBJECT_OF_ARRAY(array,index,max_count)   return (index<0||index>=max_count)?nullptr:array[index];

    #define NEW_NULL_ARRAY(name,type,count) {   \
                                                name=new type[count];   \
                                                \
                                                memset(name,0,sizeof(type)*count);  \
                                            }

    #define SAFE_CLEAR(name)    {   \
                                    if(name)    \
                                    {   \
                                        delete name;    \
                                        name=nullptr;   \
                                    }   \
                                }

    #define SAFE_CLEAR_ARRAY(name)  {   \
                                        if(name)    \
                                        {   \
                                            delete[] name;  \
                                            name=nullptr;   \
                                        }   \
                                    }

    #define SAFE_CLEAR_OBJECT_ARRAY_OBJECT(name,num)   {   \
                                                    if(name&&num>=0)    \
                                                    {   \
                                                        int safe_clear_object_array_number=num; \
                                                        \
                                                        while(safe_clear_object_array_number--) \
                                                            if(name[safe_clear_object_array_number])    \
                                                                delete name[safe_clear_object_array_number];    \
                                                        \
                                                        delete[] name;  \
                                                        name=nullptr;   \
                                                    }   \
                                                }

    #define SAFE_CLEAR_OBJECT_ARRAY(name)   {   \
                                                for(auto *obj:name)\
                                                {   \
                                                    SAFE_CLEAR(obj);    \
                                                }   \
                                            }

    #define FREE_OBJECT_ARRAY(name,num) {   \
                                            if(name&&num>=0)    \
                                            {   \
                                                int free_object_array_number=num;   \
                                                \
                                                while(free_object_array_number--)   \
                                                    if(name[free_object_array_number])  \
                                                        delete name[free_object_array_number];  \
                                                \
                                                hgl_free(name); \
                                                name=nullptr;   \
                                            }   \
                                        }

    #define SAFE_FREE_OBJECT_ARRAY(name,num)    {   \
                                                    if(name)    \
                                                    {    \
                                                        FREE_OBJECT_ARRAY(name,num);    \
                                                \
                                                        name=null;  \
                                                    }\
                                                }

    #define SAFE_FREE(name)             {   \
                                            if(name)    \
                                            {   \
                                                hgl_free(name); \
                                                name=nullptr;   \
                                            }   \
                                        }
}//namespace hgl