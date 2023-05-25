/*
 * @Author: 计佳斌bin
 * @Date: 2023-05-25 14:40:25
 * @LastEditTime: 2023-05-25 14:51:25
 * @FilePath: \mycpp_code\stl_test_code\chapter2\mytest.cpp
 */
#include<stdio.h>
#define INT_MAX 2147483647
#define INT_MIN (-INT_MAX - 1)
int data[10];
void datain()
{
    for(int i=0;i<10;i++)
        scanf("%d",&data[i]);

}

void solve()
{
    int min=INT_MAX,minIndex;
    int mast=INT_MIN,mastIndex;

    for(int i=0;i<10;i++)
    {
        if(data[i]<min)
        {
            min=data[i];
            minIndex=i;
        }
        if(data[i]>mast)
        {
            mast=data[i];
            mastIndex=i;
        }
    }

   int temp=data[0];
    data[0]= data[minIndex];
    data[minIndex]=temp;
    temp=data[9];
    data[9]=data[mastIndex];
    data[mastIndex]=temp;

}

void dataout()
{
    for(int i=0;i<10;i++)
        printf("%d ",data[i]);
}


int main()
{
    datain();
    solve();
    dataout();
}