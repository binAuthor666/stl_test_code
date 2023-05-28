#include"myList-iter.h"
void main()
{
    List<int> mylist;
    for(int i=0;i<5;++i)
    {
        mylist.insert_front(i);
        mylist.insert_end(i+2);
    }
    mylist.display();

    ListIter<ListItem<int> >begin(mylist.front());
    ListIter<ListItem<int> >end;
    ListIter<ListItem<int> >iter;

    iter=find(begin,end,3);
    if(iter==end)
        cout<<"no found"<<endl;
    else
        cout<<"found."<<iter-<value()<<endl;
    
    iter=find(begin,end,7);
    if(iter==end)
        cout<<"no found"<<endl;
    else
        cout<<"found."<<iter-<value()<<endl;
}