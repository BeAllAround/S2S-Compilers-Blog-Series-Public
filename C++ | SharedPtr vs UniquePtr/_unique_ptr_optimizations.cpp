#include <iostream>

#include <memory>

#include <vector>

#include <cassert>




class S1 {
    public:
    int* i {nullptr};

    S1(int ii) {
        i = new int(ii);
        // std::cout << "S1()" << std::endl;
    }

    S1(const S1&) = delete;

    S1(S1&&) = delete;


    S1&&operator=(const S1&) = delete;
    S1&&operator=(S1&&) = delete;


    ~S1() {
        // std::cout << "~S1()" << std::endl;
        delete i;
        i = nullptr;
    }

};

/*
struct D // deleter
{
    D() {};
    D(const D&) { std::cout << "D copy ctor\n"; }
    D(D&) { std::cout << "D non-const copy ctor\n"; }
    D(D&&) { std::cout << "D move ctor \n"; }
    void operator()(S1* p) const
    {
        std::cout << "D is deleting an S1\n";
        // delete p;
    };
};
*/

#define DO_NOT_OPTIMIZE_AWAY(var) asm volatile("" : : "g"(var) : "memory")

int main() {
    volatile int i = 1;

    std::unique_ptr<S1> p1 (new S1(i));


    
    std::unique_ptr<S1> p2 = std::move(p1);


    DO_NOT_OPTIMIZE_AWAY(p1);
    DO_NOT_OPTIMIZE_AWAY(p2);



    printf("%p\n", p1.get()); // thsi variable is treated as "S* p1 = nullptr" at -O3 optimizations so there are the two ~unique_ptr(). Compare to -O0 levels


    
    printf("%p\n", p2.get());



    return 0;
}

