
#include <iostream>

#include <cassert>


class Base {
    public:
    int i {2};
    Base() {}

};


class Derived {
    public:
    Base base {}; // NOTE: Base base() makes sizeof(Derived) 4 - what the shoot?
    int ii {10};
};




int main01();

int main() {

    std::cout << sizeof(Base) << std::endl;
    std::cout << sizeof(Derived) << std::endl;
    

    Base* b = (Base*)(new Derived());
    

    std::cout << (((Derived*)b)->ii) << std::endl; // 10

    std::cout << (b->i) << std::endl; // 2


    delete (Derived*) b;

    return 0;
    // return main01();
}


class Val {
    public:

    // int c {10};
    int i {22};
    int ii {33};

    Val() {
        // i = 22;
    }

    ~Val() {

    }
};



int main01() {

    std::cout << "sizeof(size_t) " << sizeof(size_t) << std::endl;
    std::cout << "sizeof(int) " << sizeof(int) << std::endl;
    std::cout << "sizeof(Val) " << sizeof(Val) << std::endl;
    std::cout << "sizeof(int) " << sizeof(int) << std::endl;


    void* v = new Val();



    std::cout << (Val*)v << std::endl;
    std::cout << v << std::endl;

    std::cout << "i address: " << (&(((Val*)v)->i)) << std::endl;

    // std::cout << "c address: " << (&(((Val*)v)->c)) << std::endl;

    std::cout << ( size_t(v) - size_t((Val*)v) ) << std::endl;



    assert(
        ((int*)(v+0)) == v+0 &&
        v+0 == (&(((Val*)v)->i))
    ); // same as &(v->i)


    std::cout << *((int*)(v)) << std::endl; // v->i
    std::cout << *((int*)(v+4)) << std::endl; // v->ii




    delete (Val*)v;

    return 0;
}


