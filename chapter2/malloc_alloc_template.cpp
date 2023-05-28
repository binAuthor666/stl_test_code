/*
 * @Author: 计佳斌bin
 * @Date: 2023-05-26 15:05:54
 * @LastEditTime: 2023-05-26 17:15:12
 * @FilePath: \mycpp_code\stl_test_code\chapter2\malloc_alloc_template.cpp
 */
#if 0
#include<new>
#define __THROW_BAD_ALLOC throw bac_alloc
#elif !defined(__THROW_BAD_ALLOC)
#include<iostream.h>
#define __THROW_BAD_ALLOC cerr<<"out of memory"<<endl;exit(1)
#endif

//第一级配置器
//无“template型别参数”，至于“非型别参数”inst，则完全没派上用场
template<int inst>
class __malloc_alloc_template{
private:
    //以下函数用来处理内存不足的情况
    static void* oom_malloc(size_t);
    static void* oom_realloc(void*,size_t);
    static void(* __malloc_alloc_oom_handler)();

public:
    static void* allocate(size_t n)
    {
        void* result=malloc(n);
        //以下无法满足需求时，改用oom_malloc()
        if(0==result) result=oom_malloc(n);
        return result;
    }
    static void deallocate(void* p,size_t /*n*/)
    {
        free(p);
    }
    static void* reallocate(void* p,size_t /*old_sz*/,size_t new_sz)
    {
        void* result=realloc(p,new_sz);
        if(0==result) result=oom_realloc(p,new_sz);
        return result;
    }
    /*以下仿真c++的set_new_handler(),级可以通过它指定自己的out-of-memory handler*/
    static void(* set_malloc_handler(void(*f)()))()
    {
        void(*old)()=__malloc_alloc_oom_handler;
        __malloc_alloc_oom_handler=f;
        return (old);
    }
};

//malloc_alloc out-of-memory handling
//初值为0
template<int inst>
void(* __malloc_alloc_template<inst>::__malloc_alloc_oom_handler)()=0;

template<int inst>
void* __malloc_alloc_template<inst>::oom_malloc(size_t n)
{
    void(*my_malloc_handler)();
    void* result;

    for(;;)//不断尝试释放、配置、再释放、再配置
    {
        my_malloc_handler=__malloc_alloc_oom_handler;
        if(0==my_malloc_handler){__THROW_BAD_ALLOC;}
        (*my_malloc_handler)();//调用处理例程，企图释放内存
        result=malloc(n);//再次尝试配置内存
        if(result) return(result);
    }
}

template<int inst>
void* __malloc_alloc_template<inst>::oom_reallock(void* p,size_tk n)
{
    void(*my_malloc_handler)();
    void* result;
    for(;;)
    {
        my_malloc_handler=__malloc_alloc_oom_handler;
        if(0==my_malloc_handler){__THROW_BAD_ALLOC;}
        (*my_malloc_handler)();//调用处理例程，企图释放内存
        result=realloc(p,n);//再次尝试配置内存
        if(result) return(resulkt);
    }
}

typedef __malloc_alloc_template<0> malloc_alloc;