#include <iostream>

#include <memory>

#include <stdlib.h>

#define start_time clock_t s_t_a_r_t = clock();

#if __GNUG__
#define end_time printf("[Cpu_time_used: %f]\n", static_cast<double>(clock() - s_t_a_r_t) / CLOCKS_PER_SEC);
#else
#define end_time printf("[Cpu_time_used: %f]\n", ((double) (clock() - s_t_a_r_t)) / CLOCKS_PER_SEC);
#endif


/*

// See: Section 34.4 Allocators of The C++ Programming Language 4th Edition


References:

https://en.cppreference.com/w/cpp/memory/allocator
https://en.cppreference.com/w/cpp/memory/allocator/allocate
https://www.amazon.com/C-Programming-Language-4th/dp/0321563840
https://gcc.gnu.org/onlinedocs/gcc-4.6.3/libstdc++/api/a00958_source.html

*/

// See: 34. Memory and Resources

// never compile with -O3 - it might optimize your code too much and remove the unused allocated memory

/*
#include <iostream>

class S{
    public:
    S() {
        std::cout << "S()" << std::endl;
        
    }
    
    void d() {
        
    }
    
    ~S() {
std::cout << "~S()" << std::endl;
        
        
    }
    
};

int main() {
    using SP = S*;
    
    S* s = new S;
    
    s.~SP();
    
    

    return 0;
}
*/

class S {

  public:
    S() : a{new int(0)} {
      std::cout << "S()" << std::endl;
    }

    S(int i) : a{new int(i)} {
      std::cout << "S(int)" << std::endl;
    };

    void foo() {
      std::cout << "foo: " << a << std::endl;
    }

    S(const S&s) {
      *this = s;
    }

    S(S&& s) {
      *this = s;
    }


    S&operator=(const S& s) {
      std::cout << "const ref& operator=" << std::endl;
      a = new int(*s.a);

      return *this;
    }

    S& operator=(S&& s) {
      std::cout << "operator=" << std::endl;
      a = s.a;

      s.a = nullptr;

      return *this;
    }

    ~S() {
      std::cout << "~S()" << std::endl;
      if(a != nullptr) {
        delete a;
        a = nullptr;
      }
    }

  private:
    int* a {nullptr};
};


struct S1 {

    int i1 { 0 };
    int i2 { 0 };

    S1(int) {
    }

    S1() {

    }

    ~S1() 
    {
        // delete &i1; // NOW THE DESTRUCTOR CALL/INVOCATION WON'T BE OPTIMIZED OUT AS DETAILED OUT BELOW
        
    }

};

int main() {

/*
.L2:
        mov     DWORD PTR [rdx], 0
        mov     DWORD PTR [rdx+4], 0
        add     rdx, 8
        cmp     rdx, rcx
        jne     .L2
        call    operator delete(void*)
        mov     eax, 0
        add     rsp, 8
        ret
*/
  {
    size_t size = 5;
    // allocate
    S1* s = static_cast<S1*>(::operator new(size * sizeof(S1)));


    // construct
    for(size_t i = 0; i < size; i++) {
      new (s+i) S1(i);
    }

    // s[0].foo();

    // destroy
    S1* start = s;
    S1* end = s+size;
    while(start != end) {
      // destroy
      (start++)->~S1(); // NOTE: -O1 OPTIMIZED OUT AS WELL! SINCE THE DESTRUCTOR DOES NOTHING AND ONCE IT IS INLINED - IT IS NOT CONSIDERED AS SOMETHING THAT NEEDS TO BE EXECUTED
    }

    // deallocate
    ::operator delete(s);
  }

  return 0;
}
