#include <iostream>

#include <vector>

#include <cassert>

#include <stdio.h>
#include <stdint.h> // uintptr_t

// -O3 -Wall

#define nullptr NULL

namespace std {
    // [[noreturn]]
      void __throw_bad_cast() { 
        printf("__throw_bad_cast THROWN!\n");
        throw "___bad_cast()";
    }
}

/*
void operator delete(void* ptr, size_t) noexcept {
    std::cout << "Custom DELETE OVERLOAD " << ptr << std::endl;
    // ::operator delete(ptr); // Stack overflow
    // free(ptr);
}
*/

class S;

class S {
  public:
    // int* i_ptr = nullptr; // warning: non-static data member initializers only available with '-std=c++11' or '-std=gnu++11' [-Wc++11-extensions]
    int* i_ptr;
  
  S() {
    i_ptr = nullptr;
    std::cout << "S()" << std::endl;
  }

  S(int i) {
    std::cout << "S(int)" << this << std::endl;
    i_ptr = new int(i);
  }

/*
  S(const S& other) {
    std::cout << "S(const S&)" << std::endl;
    i_ptr = new int(*other.i_ptr);
  }


  S(S& other) {
    std::cout << "S(S&&)" << std::endl;
    i_ptr = other.i_ptr;
    other.i_ptr = nullptr;
  }
*/

/*
  S& operator=(const S& other) {
    std::cout << "S&operator=(const S&)" << std::endl;
    if(i_ptr != nullptr) {
      delete i_ptr;
    }
    i_ptr = new int(*other.i_ptr);
    return *this;
  }
 */

/*
  uintptr_t get_p() const {
    return reinterpret_cast<uintptr_t>(this);
  }
  */


/*
  S& operator=(S& other) {
    std::cout << "S&operator=(S&&)" << std::endl;
    if(i_ptr != nullptr) {
      delete i_ptr;
    }
    i_ptr = other.i_ptr;
    other.i_ptr = nullptr;
    return *this;
  }
  */

  S& move(const S& other) {
    S* s = reinterpret_cast<S*>(
            reinterpret_cast<uintptr_t>(&other)
    );

    std::cout << "S& move(const S&)" << s << std::endl;
    if(i_ptr != nullptr) {
      delete i_ptr;
    }
    i_ptr = s->i_ptr;
    s->i_ptr = nullptr;
    return *this; 
  }
  

  S& move(S& other) {
    std::cout << "S& move(S&)" << std::endl;
    if(i_ptr != nullptr) {
      delete i_ptr;
    }
    i_ptr = other.i_ptr;
    other.i_ptr = nullptr;
    return *this; 
  }

  S& move(S* other) {
    std::cout << "S& move(S*)" << other << std::endl;
    if(i_ptr != nullptr) { // FIXME: ONLY NEEDED FOR MOVE ASSIGNMENT. NOT THE MOVE CONSTRUCTOR. TODO: REFACTOR
      delete i_ptr;
    }
    i_ptr = other->i_ptr;
    other->i_ptr = nullptr;
    return *this; 
  }

  S& copy(S& other) {
    std::cout << "S&copy(S&)" << std::endl;
    if(i_ptr != nullptr) { // FIXME: ONLY NEEDED FOR COPY ASSIGNMENT. NOT THE MOVE CONSTRUCTOR. TODO: REFACTOR
      delete i_ptr;
    }
    i_ptr = new int(*other.i_ptr);
    return *this;
  }

  void clean_up();


  ~S();

};

// NOTE: Preventing the compiler from the inlining for purposes of dissecting the assembly output more efficiently
void S::clean_up() {
    // std::cout << "clean_up()" << std::endl; // OK. CAN SEE "clean_up()" in the stdout
    printf("clean_up()\n"); // IT SEEMS LIKE IT IS NEVER REACHED FOR -O3 due to one of the conditional check/jump below. THIS IS ONLY THE CASE FOR printf. THIS IS LIKELY THE CASE BECAUSE IT BECOMES empty string due to OFFSET FLAT:std::ctype<char>::do_widen(char) const.
    if(i_ptr != nullptr) {
      delete i_ptr;
      i_ptr = nullptr;
    }

}

/*
-O3
...
        mov     rbx, rax
        mov     rax, QWORD PTR [rax]
        mov     rax, QWORD PTR [rax-24]
        mov     rdi, QWORD PTR [rbx+240+rax]
        test    rdi, rdi
        je      .L20
        cmp     BYTE PTR [rdi+56], 0
        je      .L15
        movsx   esi, BYTE PTR [rdi+67]
.L16:
        mov     rdi, rbx
        call    std::ostream::put(char)
        mov     rdi, rax
        call    std::ostream::flush()
        mov     edi, OFFSET FLAT:.LC5
        call    puts
        mov     rdi, QWORD PTR [rbp+0]
        test    rdi, rdi
        je      .L13
        add     rsp, 24
        mov     esi, 4
        pop     rbx
        pop     rbp
        jmp     operator delete(void*, unsigned long)
.L15:
        mov     QWORD PTR [rsp+8], rdi
        call    std::ctype<char>::_M_widen_init() const
        mov     rdi, QWORD PTR [rsp+8]
        mov     esi, 10
        mov     rax, QWORD PTR [rdi]
        mov     rax, QWORD PTR [rax+48]
        cmp     rax, OFFSET FLAT:std::ctype<char>::do_widen(char) const
        je      .L16
        call    rax
        movsx   esi, al
        jmp     .L16
.L13:
        add     rsp, 24
        pop     rbx
        pop     rbp
        ret
S::~S() (.cold):
.L20:
        call    std::__throw_bad_cast() # NOTE: It is NOT THIS CALL THAT THROWS DOUBLE FREE. IT IS OPERATOR DELETE. THAT'S WHY I OVERLOADED THAT GLOBAL OPERATOR ABOVE. IN ORDER TO SEE THIS, THE LIBRARY FUNCTIONS MUST NOT BE FILTERED OUT FROM THE ASSEMBLY OUTPUT. Also note that this cannot be caught.
        .set    S::~S() [complete object destructor],S::~S() [base object destructor]


*/
S::~S() {
    std::cout << "~S()" << this << std::endl;
    /*
    printf("~S()");
    printf("%p\n", this);
    */
    clean_up();
}


template<class T>
T* address_of_aggressive(const T& v) {
    std::cout << &v << std::endl;
    return reinterpret_cast<T*>(
            reinterpret_cast<uintptr_t>(&v)
    );
}


void _assert_S__ptr(S* s, const int i) {
  assert(*s->i_ptr == i);
}


int main00() {

  {

    S s = 1;

    std::cout << s.i_ptr << std::endl;

    S s1;

    s1.move(s);

    std::cout << s1.i_ptr << std::endl;

    std::cout << s.i_ptr << std::endl;



    S s2;

    S s_move = S(10);


    /*
    const S& s3 = s_move;
    S* dd = reinterpret_cast<S*>(
            reinterpret_cast<uintptr_t>(
                &s3
            )
    );
    */

    // In order to accomplish this, we need to pass it as a pointer
    
    // s2.move(&s_move);
   s2.move(S(10));
   // s2 = S(10); // move
   // s2 = s_move; // copy
   s2.copy(s_move);

/*
    S s2;
    S s_xvaluetemporary = S(10);
    s2.move(s_xvaluetemmpory); cleanup(s_xvaluetempory);
*/
   // s2.move(s_move);

    // s2.move(s_move);

    // assert(s_move.i_ptr == nullptr);
  }

  return 0;

}

int main01() {
    // DIFFERENT VALGRIND REPORTS FOR DIFFERENT LEVELS OF OPTIMIZATIONS: -O0, -O1, -O2, -O3
    {
      int n = 0;
      address_of_aggressive<S>(S(15));
      address_of_aggressive<S>(S(16)); // ~S() at the end of the expression: semi colon
      /*
      S(int)0x7fff1728d930
      ~S()0x7fff1728d930
      S(int)0x7fff1728d930
      ~S()0x7fff1728d930
      */

      address_of_aggressive<int>(10);
      address_of_aggressive< std::vector<int> >(std::vector<int>());

      address_of_aggressive<int>(n); // OK

      S* s = address_of_aggressive<S>(S(15));
      S* s1;
/*
main.cpp:208:43: note: unnamed temporary defined here
  208 |       S* s = address_of_aggressive<S>(S(15));
      |                                           ^
In file included from /usr/include/c++/13/cassert:44,
                 from main.cpp:5:
main.cpp:215:17: warning: using a dangling pointer to an unnamed temporary [-Wdangling-pointer=]
  215 |       assert(s->i_ptr == nullptr);
      |              ~~~^~~~~
main.cpp:208:43: note: unnamed temporary defined here
  208 |       S* s = address_of_aggressive<S>(S(15));

*/

      std::cout << "address_of: " << s << std::endl;
      std::cout << "address_of: " << s1 << std::endl; // Compare the outputs for -O0, -O1, -O2, -O3

      // NOTE: ONLY PASSES FOR -O0 level optimizations. It fails for the rest. But that (-O0) is not clean for "valgrind -s ./out.out" either
      // assert(s->i_ptr == nullptr);
      // assert(s == address_of_aggressive(S(10))); // VALGRIND WARNINGS AND ERRORS BUT IT DOES PASS FOR the -O0 level optimizations as the temporary rvalue appears to be at the same memory address/region.
      // std::cout << s->i_ptr << std::endl; // Compare the outputs for -O0, -O1, -O2, -O3
      // std::cout << *s->i_ptr << std::endl; // Compare the outputs for -O0, -O1, -O2, -O3
    }

    {
      _assert_S__ptr(address_of_aggressive<S>(S(100)), 100);
      _assert_S__ptr(address_of_aggressive<S>(S(101)), 101);

    }

  return 0;
}

int main02() {
  // NOTE: OK for the -O0 (and -O1 depending on the compiler version) level optimizations BUT "Invalid free() / delete / delete[] / realloc()" for the -O2, -O3
  S s (10);

  address_of_aggressive(s)->~S();
  // address_of_aggressive(s)->clean_up(); // OK at all times

  std::cout << "s.i_ptr " << s.i_ptr << std::endl;

  return 0;

}

int main() {

  main02();

  return 0;
}

