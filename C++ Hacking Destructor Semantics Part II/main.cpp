#include <iostream>

#include <array>
#include <vector>


#include <initializer_list>

#include <cassert>



template<class T, size_t Size>
class StaticStorage {
  public:
  using ItemType = T;
  unsigned char buffer[sizeof(T) * Size];
  size_t stack_count = 0;

  StaticStorage() {}


  // NOTE: Deleted for now. Will think about this
  StaticStorage(const StaticStorage&) = delete;
  StaticStorage(StaticStorage&&) = delete;

  StaticStorage& operator=(const StaticStorage&) = delete;
  StaticStorage& operator=(StaticStorage&&) = delete;

  void add(const T& item) {
    new(&buffer[stack_count * sizeof(T)]) T(item);
    stack_count++;
  }

  void add(T&& item) {
    new(&buffer[stack_count * sizeof(T)]) T(std::move(item));
    stack_count++;
  }


  // T* pop();

  T& pop() {
    stack_count--;
    return *reinterpret_cast<T*>(buffer + ( stack_count * sizeof(T)));
  }


  ~StaticStorage() {
    for(size_t i = 0; i < stack_count; i++) {
      reinterpret_cast<T*>(buffer + (i * sizeof(T)))->~T();
    }
  }


};


#define _storage_add(storage, item, T) \
    new(&storage.buffer[storage.stack_count * sizeof(T)]) item; \
    storage.stack_count++; \




class S;

class S {
  public:
  int* i_ptr { nullptr };
  S() {
    std::cout << "S()" << std::endl;
  }

  S(int i) {
    std::cout << "S(int)" << std::endl;
    i_ptr = new int(i);
  }

  S(const S& other) {
    std::cout << "S(const S&)" << std::endl;
    i_ptr = new int(*other.i_ptr);
  }

  S(S&& other) {
    std::cout << "S(S&&)" << std::endl;
    i_ptr = other.i_ptr;
    other.i_ptr = nullptr;
  }


  S& operator=(const S& other) {
    std::cout << "S&operator=(const S&)" << std::endl;
    i_ptr = new int(*other.i_ptr);
    return *this;
  }


  S& operator=(S&& other) {
    std::cout << "S&operator=(S&&)" << std::endl;
    i_ptr = other.i_ptr;
    other.i_ptr = nullptr;
    return *this;
  }

  ~S() {
    std::cout << "~S()" << std::endl;

    if(i_ptr != nullptr) {
      delete i_ptr;
      i_ptr = nullptr;
    }

  }
};


// NOTE: classes/structs like this DO NOT NEED THE DESTRUCTOR
// Also see: https://github.com/BeAllAround/S2S-Compilers-Blog-Series-Public/blob/main/C%2B%2B%20Advanced%20Memory%20Allocation%20and%20Destructor%20Invocation/main.cpp
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


#define _CUSTOM_INIT(decl_name, T) unsigned char decl_name[sizeof(T)]

int main() {


    std::cout << "START: Block1" << std::endl;

    {
        unsigned char buffer[sizeof(S)];

        new(&buffer[0]) S(1);
/*
        lea     rbx, [rbp-48]
        mov     rsi, rbx
        mov     edi, 8
        call    operator new(unsigned long, void*)
        mov     r12, rax
        mov     r14d, 1
        mov     esi, 1
        mov     rdi, r12
        call    S::S(int) [complete object constructor]
        mov     eax, 0
        test    al, al
        je      .L51
        mov     rsi, rbx
        mov     rdi, r12
        call    operator delete(void*, void*)
.L51:
        nop

*/
        // SANITY CHECK
        assert(*(reinterpret_cast<S*>(buffer + (0))->i_ptr) == 1);


        std::vector<S> v;

        v.push_back(std::move(*(reinterpret_cast<S*>(buffer + (0)))));


        assert(*(v[0].i_ptr) == 1);



    }
    std::cout << "END: Block1" << std::endl;



    std::cout << "START: Block2" << std::endl;
    {

        // NOTE: CLOSURE PROBLEMS AS WELL
        auto process_s = []() {
            S s (1);

            // mutate/process s1 based on my code logic

            // Copy Elision
            return s;
        };

        union Ss {
            S s;

            ~Ss() __attribute__((always_inline)) {

            }

        };

        // Ss s = {};
        // new(&s.s) S(1);
/*
        lea     rbx, [rbp-96]
        mov     rsi, rbx
        mov     edi, 8
        call    operator new(unsigned long, void*)
        mov     r12, rax
        mov     r14d, 1
        mov     esi, 1
        mov     rdi, r12
        call    S::S(int) [complete object constructor]
        mov     eax, 0
        test    al, al
        je      .L52
        mov     rsi, rbx
        mov     rdi, r12
        call    operator delete(void*, void*)
.L52:
        nop

*/

        std::vector<S> v;

        // S s1 (1);
        /// mutate/process s1 based on my code logic

        // v.push_back(std::move(s1));

        // v.push_back(std::move(s.s));

        // v.push_back(S(1));


        // push_back new placement move
        // https://gcc.gnu.org/onlinedocs/gcc-4.6.3/libstdc++/api/a00958_source.html

        v.push_back(process_s()); // rvalue


        assert(*(v[0].i_ptr) == 1);

    }
    std::cout << "END: Block2" << std::endl;




    std::cout << "START: Block3" << std::endl;
    {
        // NOTE: CLOSURE PROBLEMS AS WELL
        auto process_s = []() {
            S s (1);


            // Copy Elision
            return s;
        };

        // std::array<S, 2> arr = { process_s(), process_s()};

        std::initializer_list<S> _list = {process_s(), process_s()};

    }
    std::cout << "END: Block3" << std::endl;





    return 0;
}
