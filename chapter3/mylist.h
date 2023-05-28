/*
 * @Author: 计佳斌bin
 * @Date: 2023-05-28 15:07:35
 * @LastEditTime: 2023-05-28 15:13:44
 * @FilePath: \mycpp_code\stl_test_code\chapter3\mylist.h
 */
template<typename T>
class List
{
    void insert_front(T value);
    void insert_end(T value);
    void display(std::ostream &os=std::cout) const;

private:
    ListItem<T> *_end;
    ListItem<T> *_front;
    long _size;
};

template<typename T>
class ListItem
{
public:
    T value() const{return _value;}
    ListItem* next() const{return _next;}

private:
    T _value;
    ListItem* _next;//单向链表
};