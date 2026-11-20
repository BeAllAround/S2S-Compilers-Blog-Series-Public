#include <iostream>

#include <variant>

#include <cassert>

#include <cstdint>


#define DO_NOT_OPTIMIZE_AWAY(var) asm volatile("" : : "g"(var) : "memory")


// Reference Collapsing
// See: https://www.ibm.com/docs/en/xl-c-and-cpp-aix/16.1.0?topic=operators-reference-collapsing-c11

template<class T>
__attribute__((noinline))  void print(T&& arg) {
    std::cout << arg << std::endl;
}

int main()
{

    union {
        int i;
        const char* s;

        void* v;
    };

    s = "AAA";


    // &i == &s && &s == &v
    assert(
        reinterpret_cast<uintptr_t>(&i) == 
        reinterpret_cast<uintptr_t>(&s) &&
        reinterpret_cast<uintptr_t>(&s) == 
        reinterpret_cast<uintptr_t>(&v) 
    );

    std::cout << (&i) << std::endl;
    std::cout << (&s) << std::endl;
    std::cout << (&v) << std::endl;


    
    print(
        *(const char**)(&i)
    );


    return 0;
}


