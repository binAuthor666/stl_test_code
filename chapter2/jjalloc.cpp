/*
 * @Author: 计佳斌bin
 * @Date: 2023-05-24 14:25:20
 * @LastEditTime: 2023-05-24 15:18:42
 * @FilePath: \mycpp_code\stl_test_code\chapter2\jjalloc.cpp
 */
#include"jjalloc.h"
#include<vector>
#include<iostream>
using namespace std;
int main()
{
    int ia[5]={0,1,2,3,4};
    unsigned int i;

    vector<int,JJ::allocator<int> >iv(ia,ia+5);
    for(i=0;i<iv.size();i++)
    {
        cout<<iv[i]<<' ';
    }
    cout<<endl;
}