/*
 * @Author: 计佳斌bin
 * @Date: 2023-05-28 15:14:31
 * @LastEditTime: 2023-05-28 15:23:55
 * @FilePath: \mycpp_code\stl_test_code\chapter3\myList-iter.h
 */
#include "mylist.h"
template<class Item>
struct ListIter
{
    Item* ptr;

    ListIter(Item* p=0):ptr(p){}

    //不必实现copy ctor，因为编译器提供的缺省行为已足够
    //不必实现operator=, 因为编译器提供的缺省行为已足够
    Item& operator*() const{return *ptr;}
    Item* operator->() const{return *ptr;}
    //pre-increment
    ListIter& operator++()
    {
        ptr=ptr->next();
        return *this;
    }
    //post-increment
    ListIter operator++(int)
    {
        ListIter tmp=*this;
        ++*this;
        return tmp;
    }

    bool operator==(const ListIter& i) const
    {
        return ptr==i.ptr;
    }
    bool operator!=(const ListIter& i) const
    {
        return ptr!=i.ptr;
    }
};