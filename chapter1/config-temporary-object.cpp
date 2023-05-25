/*
 * @Author: 计佳斌bin
 * @Date: 2023-05-23 17:14:52
 * @LastEditTime: 2023-05-23 17:20:16
 * @FilePath: \mycpp_code\stl_test_code\lconfig-temporary-object.cpp
 */
#include<vector>
#include<algorithm>
#include<iostream>
using namespace std;

template<typename T>
class print
{
    public:
    void operator()(const T& elem){
        cout<<elem<<endl;
    }
};

int main()
{
    int ia[6]={1,2,3,4,5,6};
    vector<int> iv(ia,ia+6);
    //print<int>()是一个临时对象，不是一个函数调用操作
    for_each(iv.begin(),iv.end(),print<int>());
}