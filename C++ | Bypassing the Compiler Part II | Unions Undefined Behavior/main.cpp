#include <iostream>

#include <cassert>



class S {
    public:
    int* ptr; // {nullptr};

    S() {
        std::cout << "S()" << std::endl;
    }

    S(S&&) {
        std::cout << "S(S&&)" << std::endl;
    }

    S(const S&) {
        std::cout << "S(S&)" << std::endl;
    }

    S(S* n) {
        std::cout << "S(S*) " << n << std::endl;
        n->print();
    }


    void print() {
        std::cout << "S::print(): " << this << std::endl;
        // assert(this != nullptr);
    }


    ~S(){
        std::cout << "~S()" << std::endl;
    }

};


class S1 {
    public:

    // If we just flip these, it is a whole different story!
    int* ptr; // {nullptr};
    S s {S()};


    // S1() = default;
    S1() {}

    // S1(S&& s) : s{std::move(s)} {} // NOTE: The "S s{S()};" declaration ignored above in this "direct initializer" case

    ~S1(){

    }
};

class UnionClass {
    public:
    union {
        S s;
        S1 s1;
    };

    UnionClass() {
        new (&s) S();
        // s.ptr = new int(10);
    }

    UnionClass(int* _int) {
        // new (&s) S1(); // YOU CAN ALSO LITERALLY DO THIS
        new (&s1) S1();
        s1.ptr = _int;
        // s1.s.ptr = _int;
    }

    UnionClass(const UnionClass&) {
    }


    ~UnionClass() {

    }

};



int main() {
    // unsigned char instance[sizeof(S1)];
    // *((S1*)(instance+0)) = S1();
    // *((S*)(instance+0)) = S();

    {
        std::cout << sizeof(S) << std::endl;
        std::cout << sizeof(S1) << std::endl;
        std::cout << sizeof(UnionClass) << std::endl;

        UnionClass u1;

        int* p = new int(10);

        // UnionClass u3;

        u1.s.ptr = p;

        UnionClass u2 = p;

        std::cout << u1.s.ptr << std::endl;
        std::cout << u2.s.ptr << std::endl;

        std::cout << *(u2.s.ptr) << std::endl;
        std::cout << *(u2.s1.ptr) << std::endl;

        // std::cout << *(u2.s1.ptr) << std::endl;

        // std::cout << u1.s1.ptr << std::endl;


        // std::cout << u2.s.ptr << std::endl;
        // std::cout << u3.s.ptr << std::endl;
        
        delete p;
    }

/*
{

    S s = nullptr;

    S* s1 = nullptr;

    s1->print();

    S* s2;

    S s3;

    std::cout << &s << std::endl;
    std::cout << s1 << std::endl;
    std::cout << s2 << std::endl;
    std::cout << &s3 << std::endl;
}
*/

/*
    
    {
        S1 s1;
        S1 s1 = S();

    }
    */

    return 0;
}
