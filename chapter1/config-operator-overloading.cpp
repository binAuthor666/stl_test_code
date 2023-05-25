/*
 * @Author: 计佳斌bin
 * @Date: 2023-05-23 17:30:47
 * @LastEditTime: 2023-05-23 17:57:02
 * @FilePath: \mycpp_code\stl_test_code\config-operator-overloading.cpp
 */
#include<iostream>
using namespace std;

class INT
{
    friend ostream& operator<<(ostream& os,const INT& i);
    public:
    INT(int i):m_i(i){};
    INT& operator++()
    {
        ++(this->m_i);
        return *this;
    }
    const INT operator++(int)
    {
        INT temp=*this;
        ++(*this);
        return temp;
    }
    INT& operator--()
    {
        --(this->m_i);
        return *this;
    }
    const INT operator--(int)
    {
        INT temp=*this;
        --(*this);
        return temp;
    }

    int& operator*() const
    {
        return (int&)m_i;
    }
    private:
    int m_i;
};

ostream& operator<<( ostream& os,const INT& i)
{
    os<<'['<<i.m_i<<']';
    return os;
}

int main()
{
    INT I(5);
    cout<<I++;
    cout<<++I;
    cout<<I--;
    cout<<--I;
    cout<<*I;
}