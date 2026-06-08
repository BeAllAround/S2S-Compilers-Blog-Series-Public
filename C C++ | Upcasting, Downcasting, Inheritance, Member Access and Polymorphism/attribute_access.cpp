#include <iostream>


#include <cassert>





class Val {
    public:

    // int c {10};
    int i {22};

    Val() {
        // i = 22;
    }

    ~Val() {

    }
};

int main() {

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


    std::cout <<
      "Val::i " <<
      ( // SCREW FREAKING CAST *((int*)v+4) ALREADY!
        (*(char*)(v+0)) +
        (*(char*)(v+1)) +
        (*(char*)(v+2)) +
        (*(char*)(v+3))
      )
    << std::endl;

    std::cout << ((int*)(v+0)) << std::endl;
    std::cout << *((int*)(v+0)) << std::endl;




    delete (Val*)v;

    return 0;
}
