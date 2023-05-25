#include<iostream>

union obj{
    char a;
    char b;
};

int main()
{
    obj o;
    o.a='a';
    std::cout<<o.a<<std::endl;
    std::cout<<o.b<<std::endl;
    o.b='b';
    std::cout<<o.a<<std::endl;
    std::cout<<o.b<<std::endl;
    std::cout<<sizeof(o)<<std::endl;
    std::cout<<sizeof(char)<<std::endl;
    std::cout<<o.a<<std::endl;
    std::cout<<o.b<<std::endl;
}