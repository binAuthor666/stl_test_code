inline size_t __deque_buf_size(size_t n,size_t sz)
{
    return n!=0?n:(sz<512?size_t(512/sz):size_t(1));
}
template<class T,class Alloc=alloc,size_t BufSiz=0>
class deque{
public:
    typedef T value_type;
    typedef value_type* pointer;
protected:
    //元素的指针的指针
    typedef pointer* map_pointer;

protected:
map_pointer map;

size_type map_size;
};

template<class T,class Ref,class Ptr,size_t BufSiz>
struct __deque_iterator{
    typedef __deque_iterator<T,T&,T*,BufSiz> iterator;
    typedef __deque_iterator<T,const T&,const T*,BufSiz> const_iterator;
    static size_t buffer_size(){return __deque_buf_size(BufSiz,sizeof(T));}

    //未继承std::iterator，所以必须自行撰写五个必要的迭代器相应型别
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef Ptr pointer;
    typedef Ref reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T** map_pointer;

    typedef __deque_iterator self;

    //保持与容器的联结
    T* cur;
    T* first;
    T* last;
    map_pointer node;//指向管控中心

    void set_node(map_pointer new_node){
        node=new_node;
        first=*new_node;
        last=first+difference_type(buffer_size());
    }

    reference operator*() const {return *cur;}
    pointer operator->() const {return &(operator*());}
    difference_type operator-(const self& x) const{
        return difference_type(buffer_size())*(node-x.node-1)+
        (cur-first)+((x.last-x.cur));
    }

    self& operator++(){
        ++cur;
        if(cur==last){
            set_node(node+1);
            cur=first;
        }
        return *this;
    }
    self operator++(int){ //后缀++
        self tmp=*this;
        ++*this;
        return tmp;
    }
    self& operator--(){
        if(cur==first){
            set_node(node-1);
            cur=last;
        }
        --cur;
        return *this;
    }
    self operator--(int){
        self tmp=*this;
        --*this;
        return tmp;
    }

    //随机存取
    self& operator+=(difference_type n){
        difference_type offset=n+(cur-first);
        if(offset>=0&&offset<difference_type(buffer_size()))
            cur+=n;
        else{
            difference_type node_offset=
            offset>0?offset/difference_type(buffer_size())
                :-difference_type((-offset-1)/buffer_size())-1;
            set_node(node+node_offset);
            cur=first+(offset-node_offset*difference_type(buffer_size()));
        }
        return *this;
    }

    self operator+(difference_type n) const{
        self tmp=*this;
        return tmp+=n;
    }

    self& operator-=(difference_type n){
        return *this+=-n;
    }

    self operator-(difference_type n) const{
        self tmp=*this;
        return tmp-=n;
    }
    //随机存取
    reference operator[](difference_type n) const {return *(*this+n);}

    bool operator==(const self& x) const {return cur==x.cur;}
    bool operator!=(const self& x) const {return !(*this==x);}
    bool operator<(const self& x)const{
        return (node==x.node)?(cur<x.cur):(node<x.node);
    }
};

template<class T,class Alloc=alloc,size_t BufSiz=0>
class deque{
public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef size_t size_type;
public:
    typedef __deque_iterator<T,T&,T*,BufSiz> iterator;
protected:
    typedef pointer* map_pointer;

protected:
    iterator start;
    iterator finish;
    map_pointer map;
    size_type map_size;

public:
    iterator begin(){return start;}
    iterator end(){return finish;}

    reference operator[](size_type n){
        return start[difference_type(n)];
    }

    reference front(){return *start;}
    reference back(){
        iterator tmp=finish;
        --tmp;
        return *tmp;
    }
    //下行最后又两个';',虽奇怪但合乎语法
    size_type size() const {return finish-start;;}
    size_type max_size() const {return size_type(-1);}
    bool empty() const {return finish==start;}
protected:
    //空间配置器，每次配置一个元素大小
    typedef simple_alloc<value_type,Alloc> data_allocator;
    //空间配置器，每次配置一个指针大小
    typedef simple_alloc<pointer,Alloc> map_allocator;

    deque(int n,const value_type& value)
    :start(),finish(),map(0),map_size(0)
    {
        fill_initialize(n,value);
    }

    void fill_initialize(size_type n,const value_type& value);
    void create_map_and_notes(size_type num_elements);
    void push_back_aux(const value_type& t);
    void push_front_aux(const value_type& t);
    void reserve_map_at_back(size_type nodes_to_add=1){
        if(nodes_to_add+1>map_size-(finish.node-map)){
            //map尾端的备用空间不足
            reallocate_map(nodes_to_add,false);
        }
    }
    void reserve_map_at_front(size_type nodes_to_add=1){
        if(nodes_to_add>start.node-map){
            reallocate_map(nodes_to_add,true);
        }
    }
    void reallocate_map(size_type nodes_to_add,bool add_at_front);
    void pop_back_aux();
    void pop_front_aux();
    iterator insert_aux(iterator pos,const value_type& x);
    
public:
    
    void push_back(const value_type& t){
        if(finish.cur!=finish.last-1){
            construct(finish.cur,t);
            ++finish.cur;
        }
        else
            push_back_aux(t);

    }

    void push_front(const value_type& t){
        if(start.cur!=start.finish){
            construct(start.cur-1,t);
            --start.cur;
        }
        else
            push_front_aux(t);
    }

    void pop_back(){
        if(finish.cur!=finish.first){
            --finish.cur;
            destroy(finish.cur);
        }
        else
            //最后缓冲区没有任何元素
            pop_back_aux();
    }
    void pop_front(){
        if(start.cur!=start.last-1){
            //第一缓冲区有两个（或更多）元素
            destroy(start.cur);
            ++start.cur;
        }
        else
            //第一缓冲区仅有一个元素
            pop_front_aux();
    }
    iterator erase(iterator first,iterator last);

    iterator insert(iterator position,const value_type& x){
        if(position.cur==start.cur){
            push_front(x);
            return start;
        }
        else if(position.cur==finish.cur){
            push_back(x);
            iterator tmp=finish;
            --tmp;
            return tmp;
        }
        else
            return insert_aux(position,x);
    }
};

template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::fill_initialize(size_type n,
                                    const value_type& value)
{
    create_map_and_notes(n);
    map_pointer cur;
    __STL_TRY{
        for(cur=start.node;cur<finish.node;++cur)
            uninitialized_fill(finish.first,finish.cur,value);
    }
    catch(...){
        ...
    }
}

template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::create_map_and_notes(size_type num_elements)
{
    size_type num_node=num_elements/buffer_size()+1;

    map_size=max(initial_map_size(),num_nodes+2);
    map=map_allocator::allocate(map_size);

    map_pointer nstart=map+(map_size-num_nodes)/2;
    map_pointer nfinish=nstart+num_nodes-1;

    map_pointer cur;
    __STL_TRY{
        for(cur=nstart;cur<=nfinish;++cur)
            *cur=allocate_node();
    }
    catch(...){
        //"commit or rollback"
        ...
    }

    //为deque内的两个迭代器start和end设定正确内容
    start.set_node(nstart);
    finish.set_node(nfinish);
    start.cur=start.first;
    start.cur=finish.first+num_elements%buffer_size();
}

template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::push_back_aux(const value_type& t){
    value_type t_copy=t;
    reserve_map_at_back(); //若符合某种条件则必须重换一个map
    *(finish.node+1)=allocate_node();
    __STL_TRY{
        construct(finish.cur,t_copy);
        finish.set_node(finish.node+1);
        finish.cur=finish.first;
    }
    __STL_UNWIND(deallocate_node(*(finish.node+1)));
}

template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::push_front_aux(const value_type& t)
{
    value_type t_copy=t;
    reserve_map_at_front();
    *(start.node-1)=allocate_node();
    __STL_TRY{
        start.set_node(start.node-1);
        start.cur=start.last-1;
        construct(start.cur,t_copy);
    }
    catch(...){
        //commit or rollback
        start.set_node(start.node-1);
        start.cur=start.first;
        deallocate_node(*(start.node-1));
        throw;
    }
}

template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::reallocate_map(size_type nodes_to_add,bool add_at_front){
    size_type old_num_nodes = finish.node-start.node+1;
    size_type new_num_nodes = old_num_nodes+nodes_to_add;

    map_pointer new_nstart;
    if(map_size>2*new_num_nodes){
        new_nstart=map+(map_size-new_num_nodes)/2
                    +(add_at_front?nodes_to_add:0);
        if(new_nstart<start.node)
            copy(start.node,finish.node+1,new_nstart);
        else
            copy_backward(start.node,finish.node+1,new_nstart+old_num_nodes);
    }
    else{
        size_type new_map_size=map_size+max(map_size,nodes_to_add)+2;
        //配置一块空间
        map_pointer new_map=map_allocator::allocate(new_map_size);
        new_nstart=new_map+(new_map_size-new_num_nodes)/2
                    +(add_at_front?nodes_to_add:0);
        //把原map内容拷贝过来
        copy(start.node,finish.node+1,new_start);
        //释放原map
        map_allocator::deallocate(map,map_size);
        //设定新map的起始地址与大小
        map=new_map;
        map_size=new_map_size;
    }

    //重新设定迭代器start和finish
    start.set_node(new_nstart);
    finish.set_node(new_nstart+old_num_nodes-1);

}

template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::pop_back_aux(){
    deallocate_node(finish.first);//释放最后一个缓冲区
    finish.set_node(finish.node-1);
    finish.cur=finish.last-1;
    destroy(finish.cur);
}

template<class T,class Alloc,size_t BufSize>
void deque<T,Alloc,BufSize>::pop_front_aux(){
    deatroy(start.cur);//将第一个缓冲区的第一个（也是最后一个、唯一一个）元素析构
    deallocate_node(start.first);
    start.set_node(start.node+1);
    start.cur=start.first;
}
template<class T,class Alloc,size_t BufSize>
deque<T,Alloc,BufSize>::iterator deque<T,Alloc,BufSize>::erase(iterator first,iterator last){
    if(first==start&&last==finish){
        clear();
        return finish;
    }
    else{
        difference_type n=last-first; //消除区间的长度
        difference_type elems_before=first-start;   //消除区间前方的元素个数
        if(elems_before<(size()-n)/2){  //如果前方元素较少
            copy_backward(start,first,last);    //向后移动前方元素（覆盖清除区间）
            iterator new_start =start+n;
            destroy(start,new_start); //移动完毕，将冗余的元素析构
            //以下将冗余的缓冲区释放
            for(map_pointer cur=start.node;cur<new_start;++cur)
                data_allocator::deallocate(*cur,buffer_size());
            start=new_start;
        }
        else{//如果消除区间后方的元素比较少
            copy(last,finish,first); //向前移动后方元素（覆盖消除区间）
            iterator new_finish=finish-n;
            destroy(new_finish,finish);
            for(map_pointer cur=new_finish.node+1;cur<=finish.node;++cur)
                data_allocator::deallocate(*cur,buffer_size());
            finish=new_finish;
        }
        return start+elems_before;
    }
}

template<class T,class Alloc,size_t BufSize>
typename deque<T,Alloc,BufSize>::iterator
deque<T,Alloc,BufSize>::insert_aux(iterator pos,const value_type& x){
    difference_type index=pos-start;//插入点之前的元素个数
    value_type x_copy=x;
    if(index<size()/2){ //如果插入点之前的元素比较少
        push_front(front());
        iterator front1=start;
        ++front1;
        iterator front2=front1;
        ++front2;
        pos=start+index;
        iterator pos1=pos;
        ++pos1;
        copy(front2,pos1,front1);//元素移动
    }
    else{
        push_back(back());
        iterator back1=finish;
        --back1;
        iterator back2=back1;
        --back2;
        pos=start+index;
        copy_backward(pos,back2,back1);//元素移动
    }
    *pos=x_copy;
    return pos;
}