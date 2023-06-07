template<class T>
struct __list_node{
    typedef void* void_pointer;
    void_pointer prev;
    void_pointer next;
    T data;
};

/*list迭代器*/
template<class T,class Ref,class Ptr>
struct __list_iterator{
    typedef __list_iterator<T,T&,T*> iterator;
    typedef __list_iterator<T,Ref,Ptr> self;

    typedef bidirectional_iterator_tag iterator_category;
    typedef T value_type;
    typedef Ptr pointer;
    typedef Ref reference;
    typedef __list_node<T>* link_type;
    typedef size_t size_type;
    typedef ptrdiff_t defference_type;

    link_type node; //迭代器内部的普通指针，指向list的节点

    //constructor
    __list_iterator(link_type x):node(x){}
    __list_iterator(){}
    __list_iterator(const iterator& x):node(x.node){}

    bool operator ==(const self& x) const {return node==x.node;}
    boll operator !=(const self& x) const {return node!=x.node;}
    //迭代器取值，取的是节点的数据值
    reference operator*() const {return (*node).data;}

    //以下是迭代器的成员存取运算
    pointer operator->() const{return &(operator*());}

    self& operator++(){
        node=(link_type)((*node).next);
        return *this;
    }
    self operator++(int){
        self tmp=*this;
        ++*this;
        return tmp;
    }

    //对迭代器递减1
    self& operator--(){
        node=(link_type)((*node).prev);
        return *this;
    }
    self operator--(int){
        self tmp=*this;
        --*this;
        return tmp;
    }
};


template<class T,class Alloc=alloc>
class list{
protected:
    typedef __list_node<T> list_node;
    typedef simple_alloc<list_node,Alloc> list_node_allocator;
public:
    typedef list_node* link_type;

protected:
    link_type node;

    //配置一个节点并传回
    link_type get_node(){return list_node_allocator::allocate();}
    //释放一个节点
    void put_node(link_type p){list_node_allocator::deallocate(p);}
    //产生（配置并构造）一个节点，带有元素值
    link_type create_node(const T& x){
        link_type p=get_node();
        construct(&p->data,x);
        return p;
    }
    //销毁（析构并释放）一个节点
    void destroy_node(link_type p){
        destroy(&p->data);
        put_node(p);
    }

    void empty_initialize(){
        node=get_node();
        node->next=node;
        node->prev=node;
    }
    iterator insert(iterator position,const T& x){
        link_type tmp=create_node(x);
        tmp->next=position.node;
        tmp->prev=position.node->prev;
        (link_type(position.node->prev))->next=tmp;
        position.node->prev=tmp;
        return tmp;
    }

    void transfer(iterator position,iterator first,iterator last){
        if(position!=last){
            (*(link_type((*last.node).prev))).next=position.node;
            (*(link_type((*first.node).prev))).next=last.node;
            (*(link_type((*position.node).prev))).next=first.node;
            link_type tmp=link_type((*position.node).prev);
            (*position.node).prev=(*last.node).prev;
            (*last.noed).prev=(*first.node).prev;
            (*first.node).prev=tmp;
        }
        
    }

public:
    list(){empty_initialize();}//产生一个空链表

    iterator begin(){return (link_type)((*node).next);}
    iterator end(){return node;}
    bool empty() const {return node->next==node;}
    size_type size() const {
        size_type result=0;
        distance(begin(),end(),result);
        return result;
    }
    reference front(){return *begin();}
    reference back(){return *(--end());}

    void push_front(const T& x){insert(begin(),x);}
    void push_back(const T& x){insert(end(),x);}

    iterator erase(iterator position){
        link_type next_node=link_type(position.node->next);
        link_type prev_node=link_type(position.node->prev);
        prev_node->next=next_node;
        next_node->prev=prev_node;
        destroy_node(position.node);
        return iterator(next_node);
    }
    
    void pop_front(){erase(begin());}
    void pop_back(){
        iterator tmp=end();
        erase(--tmp);
    }

    void clear();
    void remove(const T& value);
    void unique();
    
    //将x接合于position之前，x必须不同于*this
    void splice(iterator position,list& x)
    {
        if(!x.empty())
            transfer(position,x.begin(),x.end());
    }

    //将i所指元素接合于position所指位置之前。position和i可指向同一个list
    void splice(iterator position,list&,iterator i){
        iterator j=i;
        ++j;
        if(position==i||position==j) return;
        transfer(position,i,j);
    }

    //将[first,last)内的所有元素接合于position所指位置之前
    //position和[first,last)可指向同一个list
    //但position不能位于[first,last)之内
    void splice(iterator position,list&,iterator first,iterator last){
        if(first!=last){
            transfer(position,first,last);
        }
    }

    void merge(list<T,Alloc>& x);
    void reverse();
    void sort();
};

//清除所有节点
template<class T,class Alloc>
void list<T,Alloc>::clear()
{
    link_type cur=(link_type)node->next;
    while(cur!=node)
    {
        link_type tmp=cur;
        cur=(link_type)cur->next;
        destroy_node(tmp);
    }
    //恢复node原始状态
    node->next=node;
    node->prev=node;
}

//将数值为value之所有元素移除
template<class T,class Alloc>
void list<T,Alloc>::remove(const T& value){
    iterator first=begin();
    iterator last=end();
    while(first!=last){
        iterator next=first;
        ++next;
        if(*first==value) erase(first);
        first=next;
    }
}

//移除数值相同的连续元素。注意：只有“连续而相同的元素”，才会被移除剩一个
template<class T,class Alloc>
void list<T,Alloc>::unique(){
    iterator first=begin();
    iterator last=end();
    if(first==last) return;
    iterator next=first;
    while(++next!=last){
        if(*first==*next)
            erase(next);
        else
            first=next;
        next=first;
    }
}

//将x合并到*this上，两个list的内容都必须先经过递增排序
template<class T,class Alloc>
void list<T,Alloc>::merge(list<T,Alloc>& x){
    iterator first1=begin();
    iterator last1=end();
    iterator first2=x.begin();
    iterator last2=end();

    while(first1!=last1&&first2!=last2)
        if(*first2<*first1){
            iterator next=first2;
            transfer(first1,first2,++next);
            first2=next;
        }else
            ++first1;
    if(first2!=last2) transfer(last1,first2,last2)
}

template<class T,class Alloc>
void list<T,Alloc>::reverse(){
    if(node->next==node||link_type(node->next)->next==node)
        return;
    iterator first=begin();
    ++first;
    while(first!=end()){
        iterator old=first;
        ++first;
        transfer(begin(),old,first);
    }
}

//list不能使用STL算法sort(),因为算法sort()只接受RamdonAccessIterator
template<class T,class Alloc>
void list<T,Alloc>::sort(){
    if(node->next==node||link_type(node->next)->next==node)
        return;
    
    //中介数据存放区
    list<T,Alloc> carry;
    list<T,Alloc> counter[64];
    int fill=0;
    while(!empty()){
        carry.splice(carry.begin(),*this,begin());
        int i=0;
        while(i<fill&&!counter[i].empty()){
            counter[i].merge(carry);
            carry.swap(counter[i++]);
        }
        carry.swap(counter[i]);
        if(i==fill) ++fill;
    }
    for(int i=1;i<fill;++i)
        counter[i].merge(counter[i-1]);
    swap(counter[fill-1]);
}