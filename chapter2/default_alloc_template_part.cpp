/*
 * @Author: 计佳斌bin
 * @Date: 2023-05-25 15:18:27
 * @LastEditTime: 2023-05-25 16:12:29
 * @FilePath: \mycpp_code\stl_test_code\chapter2\default_alloc_template_part.cpp
 */

enum{__ALLIGN=8};//小型区块的上调边界
enum{__MAX_BYTES=128};//小型区块的上限
enum{__NFREELISTS=__MAX_BYTES/__ALLIGN};//free-lists个数

//以下是第二级配置器的部分实现内容：
//注意，无"template型别参数",且第二参数完全没派上用场
//第一参数用于多环境编程下
template<bool threads,int inst>
class __default_alloc_template{
private:
    //ROUND_UP()将bytes上调至8的倍数
    static size_t ROUND_UP(size_t bytes)
    {
        return (((bytes)+__ALLIGN-1)&~(__ALLIGN-1));
    }
private:
    union obj
    {
        union obj* free_list_link;
        char client_data[1];
    };
private:
    //16个free-list
    static obj* volatile free_list[__NFREELISTS];
    //以下函数根据区块大小，决定使用第n号free-list,n从0开始
    static size_t FREELIST_INDEX(size_t bytes){
        return (((bytes)+__ALLIGN-1)/__ALLIGN-1);
    }
    //返回一个大小为n的对象，并可能加入大小为n的其他区块到free list
    static void* refill(size_t n);
    //配置一大块空间，可容纳nobjs个大小为"size"的区块
    //如果配置nobjs个区块有所不便，nobjs可能会降低
    static char* chunk_alloc(size_t size,int &nobjs);

    //chunk allocation state
    static char* start_free;//内存池起始位置，只在chunk_alloc()中变化
    static char* end_free;//内存池结束位置，只在chunk_alloc()中变化
    static char* heap_size;

public:
    //n must be > 0
    static void* allocate(size_t n) {
        obj* volatile* my_free_list;
        obj* result;
        //大于128就调用第一级配置器
        if(n>(size_t)__MAX_BYTES){
            return (malloc_alloc::allocate(n));
        }
        //寻找16个free lists中适当的一个
        my_free_list=free_list+FREELIST_INDEX(n);
        result=*my_free_list;
        if(result==0){
            //没找到可用的free list,准备重新填充free list
            void* r=refill(ROUND_UP(n));
            return r;
        }
        //调整free list
        *my_free_list=result->free_list_link;
        return (result);
    }
    //p不可以是0
    static void* deallocate(void* p,size_t n){
        obj* q=(obj*)p;
        obj* volatile* my_free_list;

        //大于128就调用第一级配置器
        if(n>(size_t)__MAX_BYTES){
            malloc_alloc::deallocate(p,n);
            return;
        }
        //寻找对应的free list
        my_free_list = free_list+FREELIST_INDEX(n);
        //调整free list，回收区块
        q->free_list_link=*my_free_list;
        *my_free_list=q;
    }
    static void* reallocate(void* p,size_t old_sz,size_t new_sz);
};

//以下是static data member的定义与初值设定
template <bool threads,int inst>
char* __default_alloc_template<threads,inst>::start_free=0;
template <bool threads,int inst>
char* __default_alloc_template<threads,inst>::end_free=0;
template <bool threads,int inst>
char* __default_alloc_template<threads,inst>::heap_size=0;

template <bool threads,int inst>
__default_alloc_template<threads,inst>::obj* volatile __default_alloc_template<threads,inst>::free_list[__NFREELISTS]
=(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,);