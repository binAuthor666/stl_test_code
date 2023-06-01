/*
 * @Author: 计佳斌bin
 * @Date: 2023-05-29 15:55:08
 * @LastEditTime: 2023-05-29 15:59:39
 * @FilePath: \mycpp_code\stl_test_code\chapter3\tag-test.cpp
 */
#include<iostream>
using namespace std;
struct B{};
struct B1:public B{};
struct B2:public B1{};

template<class T>
inline void fun(T& p,B)
{
    cout<<"B VERSION"<<endl;
}

template<class T>
inline void fun(T& p,B2)
{
    cout<<"B2 VERION"<<endl;
}

int main()
{
    int* p;
    fun(p,B());
    fun(p,B1());
    fun(p,B2());
}