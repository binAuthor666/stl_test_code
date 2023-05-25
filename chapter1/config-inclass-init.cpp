/*
 * @Author: 计佳斌bin
 * @Date: 2023-05-23 17:19:32
 * @LastEditTime: 2023-05-23 17:22:22
 * @FilePath: \mycpp_code\stl_test_code\lconfig-inclass-init.cpp
 */
#include<iostream>
using namespace std;

template<typename T>
class testclass
{
public:
    static const int _datai=5;
    static const long _datal=3L;
    static const char _datac='c';
};

int main()
{
    cout<<testclass<int>::_datai<<endl;
    cout<<testclass<int>::_datal<<endl;
    cout<<testclass<int>::_datac<<endl;

}