#include <iostream>

#include <variant>


#define DO_NOT_OPTIMIZE_AWAY(var) asm volatile("" : : "g"(var) : "memory")


// Reference Collapsing
// See: https://www.ibm.com/docs/en/xl-c-and-cpp-aix/16.1.0?topic=operators-reference-collapsing-c11

template<class T>
__attribute__((noinline))  void print(T&& arg) {
    std::cout << arg << std::endl;
}

/*
void print(int arg) {
    std::cout << arg << std::endl;
}

void print(double arg) {
    std::cout << arg << std::endl;
}

void print(const std::string& arg) {
    std::cout << arg << std::endl;
}
*/

/*
void print(auto arg) {
    std::cout << arg << std::endl;

}
*/

// NOTE and COVER: -O1 vs -O3

// See: https://en.cppreference.com/cpp/utility/variant/get
// See: https://en.cppreference.com/cpp/utility/variant/visit2
// These two are powerful since they are constexpr-first functions with the runtime fallback. Since it an constexpr function cannot be executed at compile time, the runtime alternative is applied.

int main()
{

    volatile int i = 0;

    DO_NOT_OPTIMIZE_AWAY(i);

    std::variant<char, int, double, std::string> s = 'a';

    // std::get<1>(s);
    
    std::cout << sizeof(std::string) << std::endl;

    std::cout << "sizeof(std::variant): " << sizeof(s) << std::endl;

    std::visit([](auto x){ print(x); }, s);

    s = 123;

    if(i == 1) {
        s = 'b';
    } 
    else if(i == 2) {
        s = "bbbb";
    }
    else if(i == 3) {
        s = (double)2.3;
    }

    if(i == 100) {
     std::visit([](auto x){ print(x); }, s);
    }

    // std::visit([](auto x){ print(x);}, s);


    return 0;
}


