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
};