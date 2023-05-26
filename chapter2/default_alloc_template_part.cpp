/*
 * @Author: 计佳斌bin
 * @Date: 2023-05-25 15:18:27
 * @LastEditTime: 2023-05-26 15:04:45
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

//返回一个大小为n的对象，并可能加入大小为n的其他区块到free list
//假设n已经适当上调至8的倍数
template<bool threads,int inst>
void* __default_alloc_template<threads,inst>::refill(size_t n)
{
    int nobjs=20;
    //调用chunck_alloc(),尝试取得nobjs个区块作为free list的新节点
    //nobjs是引用传递
    char* chunk=chunk_alloc(n,nobjs);
    obj* volatile * my_free_list;
    obj* result;
    obj* current_obj,* next_obj;
    int i;
    //如果只有一个区块，这个区块就分配给调用者用，free list无新节点
    if(1==nobjs) return (chunk);
    //否则调整free list，纳入新节点
    my_free_list=free_list+FREELIST_INDEX(n);

    //以下在chunk空间内建立free list
    result=(obj*)chunk;//这块准备返回给客户端
    //以下引导free list指向新配置的空间（取自内存池）
    *my_free_list=next_obj=(obj*)(chunk+n);
    //以下将free list个节点串接起来
    for(i=1;;i++)
    {
        current_obj=next_obj;
        next_obj=(obj*)((char*)next_obj+n);
        if(nobjs-1==i)
        {
            current_obj->free_list_link=0;
            break;
        }
        else
        {
            current_obj->free_list_link=next_obj;
        }
    }
    return(result);
}

//从内存池中取空间给free list使用
//假设size已经上调至8的倍数
template<bool threads,int inst>
char* __default_alloc_template<threads,inst>::chunk_alloc(size_t size,int& nobjs)
{
    char* result;
    size_t total_bytes=size*nobjs;
    size_t bytes_left=end_free-start_free;//内存池剩余空间

    if(bytes_left>=total_bytes)
    {
        result=start_free;
        start_free+=total_bytes;
        return(result);
    }
    else if(bytes_left>=size)
    {
        nobjs=bytes_left/size;
        total_bytes=size*nobjs;
        result=start_free;
        start_free+=total_bytes;
        return(result);
    }
    else
    {
        size_t bytes_to_get=2*total_bytes+ROUND_UP(heap_size>>4);
        //以下试着让内存池中 残余零头还有利用价值
        if(bytes_left>0)
        {
            //配给适当的free_list
            obj* volatile* my_free_list=free_list+  FREELIST_INDEX(bytes_left);
            //将空间插入对应的free_list
            ((obj*)start_free)->free_list_link=*my_free_list;
            *my_free_list=(obj*)start_free;
        }

        //配置heap空间，用来补充内存池
        start_free=(char*)malloc(bytes_to_get);
        if(0==start_free)
        {
            //heap空间不足，malloc()失败
            int i;
            obj* volatile *my_free_list,*p;
            //搜寻“尚有未用区块，且区块够大”的free_list
            for(i=size;i<=__MAX_BYTES;i+=__ALLIGN)
            {
                my_free_list=free_list+FREELIST_INDEX(i);
                p=*my_free_list;
                if(0!=p)
                {
                    //调整free list以释放出未用区块
                    *my_free_list=p->free_list_link;
                    start_free=(char*)p;
                    end_free=start_free+i;
                    //递归调用字节，为了修正nobjs
                    return(chunk_alloc(size,nobjs));
                }
            }
            end_free=0;//找不到任何可用的内存
            //调用第一级配置器，寄希望于out-of-memory机制
            start_free=(char*)malloc_alloc::allocate(bytes_to_get);
            //这会导致抛出异常，或内存不足的清空获得改善
        }
        heap_size+=bytes_to_get;
        end_free=start_free+bytes_to_get;
        //递归调用自己，为了修正nobjs
        return (chunk_alloc(size,nobjs));
    }
}