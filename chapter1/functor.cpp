/*
 * @Author: 计佳斌bin
 * @Date: 2023-05-23 18:10:31
 * @LastEditTime: 2023-05-23 18:18:35
 * @FilePath: \mycpp_code\stl_test_code\functor.cpp
 */
#include<iostream>
//using namespace std;

//由于将operator()重载了，所以plus成立一个仿函数
template<typename T>
struct plus{
    T operator()(const T& a,const T& b){return a+b;}
};

template<typename T>
struct minus{
    T operator()(const T& a,const T& b){return a-b;}
};

int main()
{
    plus<int> plusobj;
    minus<int> minusobj;

    std::cout<<plusobj(3,5)<<std::endl;
    std::cout<<minusobj(3,5)<<std::endl;

    std::cout<<plus<int>()(5,1)<<std::endl;
    std::cout<<minus<int>()(5,1)<<std::endl;
}