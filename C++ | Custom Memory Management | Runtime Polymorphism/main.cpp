#include <iostream>


#include <cassert>


// g++ main.cpp -o out.out -O0 -Wall && valgrind -s --leak-check=full --show-leak-kinds=all ./out.out


#define start_time clock_t s_t_a_r_t = clock();

#define end_time printf("[Cpu_time_used: %f]\n", static_cast < double > (clock() - s_t_a_r_t) / CLOCKS_PER_SEC);



template<class T, size_t Size, size_t Align = alignof(T)>
class StaticStorage {
  public:

  struct TBlock {
    T block;
    char marked;
  };

  constexpr static size_t block_size = sizeof(TBlock);

  alignas(Align) unsigned char buffer[block_size * Size];

  // unsigned char buffer[sizeof(T) * Size];
  size_t stack_count = 0;

  StaticStorage() {}


  // NOTE: Deleted for now. Will think about this
  StaticStorage(const StaticStorage&) = delete;
  StaticStorage(StaticStorage&&) = delete;

  StaticStorage& operator=(const StaticStorage&) = delete;
  StaticStorage& operator=(StaticStorage&&) = delete;


  inline T* next_block_available() {
    return reinterpret_cast<T*>(buffer + (stack_count * block_size) ); // Block with the marked
    // Essentially, given that the first member is "T block" : return &((reinterpret_cast<TBlock*>(buffer + (stack_count * block_size) ))->block); // Block with the marked
  }

  template<class RP> // RP stands for Raw Pointer
  inline void mark_block(RP* block) {
    unsigned char* raw_block = reinterpret_cast<unsigned char*>(block);

    // NOTE: This cast is possible because the initial memory block came from the sizeof(TBlock) stack
    (reinterpret_cast<TBlock*> (raw_block))->marked = 1; // Note: Block marked!

  }

  inline void push_block() {
    stack_count++;
  }


  /*
  void pop_block() {
    stack_count--;
  }

  size_t block_pos(T* block) {
    return block - (buffer+0);
  }
  */


  template<class BT>
  inline void delete_block(BT* block) {
    // if(!marked) { assert (!marked); or throw; }

    block->~BT();

    unsigned char* raw_block = reinterpret_cast<unsigned char*>(block);

    // NOTE: This cast is possible because the initial memory block came from the sizeof(TBlock) stack
    (reinterpret_cast<TBlock*> (raw_block))->marked = 0; // Note: Block un-marked - now freed!

    // pop_block();
  }


  ~StaticStorage() {

    std::cout << "~StaticStorage()::stack_count: " << stack_count << std::endl;
    for(size_t i = 0; i < stack_count; i++) {
      void* raw_block = buffer + (i * block_size);


      char is_marked = (reinterpret_cast<TBlock*> (raw_block))->marked;

      if(is_marked != 0) {
        delete_block(
            reinterpret_cast<T*>(raw_block)
        );
      }


    }
  }


};

class Base {
    public:
    int* n {nullptr};

    Base() : n{new int(1)} {
      // std::cout << "Base()" << std::endl;
    }

    virtual void print_n() {
        assert(n != nullptr);

        std::cout << "Base::n " << *n << std::endl;
    }

    virtual ~Base() { // Try ~Base() to demonstrate
        std::cout << "~Base()" << std::endl;
        delete n;
        n = nullptr;
    }
};

class Derived : public Base {
    public:
        int* n1 {nullptr};

        Derived() : Base() {
            n1 = new int(2);
        }

        void print_n() override {

            assert(n1 != nullptr);
            assert(
                (reinterpret_cast<Base*>(this)->n) != nullptr
            );

            std::cout << "Derived::n1 " << *n1 << std::endl;

            std::cout << "Derived::Base::n " << 
                *(reinterpret_cast<Base*>(this)->n) << std::endl;
        }

        void derived_call_only() {
            assert(n1 != nullptr);

            std::cout << "Derived CALL! Derived::n1 " << *n1 << std::endl;
        }

        ~Derived() {
            std::cout << "~Derived()" << std::endl;
            delete n1;
            n1 = nullptr;
        }

};


static StaticStorage<Derived, 100> stack; // NOTE: Stack of Derived specifically


template<class T, class... Args>
inline T* __custom_stack_new(Args&&...) __attribute__((always_inline));

template<class T, class... Args>
inline T* __custom_stack_new(Args&&... args) {
    void* rawMemory = stack.next_block_available();

    stack.mark_block(rawMemory);

    stack.push_block();

    return new (rawMemory) T(std::forward<Args>(args)...);
}


void benchmark01() {

#define __TEST_MAX 4000 // 4000 // NOTE: FOR BENCHMARKING // MAKE SURE TO MODIFY(INCREASE THE STACK SIZE) static StaticStorage<Derived, N> stack FOR TESTING
#define __CUSTOM_STACK_NEW_TEST

#ifdef __CUSTOM_STACK_NEW_TEST
  start_time;
  std::cout << "START: __CUSTOM_STACK_NEW_TEST" << std::endl;
  for (size_t i = 0; i < __TEST_MAX; i++)
  {
    Base *b = __custom_stack_new<Derived>();
    b->print_n();

    stack.delete_block(b);
    // derived = b;
  }
  std::cout << "END: __CUSTOM_STACK_NEW_TEST" << std::endl;
  end_time;

#else
  // NOTE: Via valgrind, compare to the following

  start_time;
  std::cout << "START: HEAP ALLOCATION" << std::endl;
  for (size_t i = 0; i < __TEST_MAX; i++)
  {
    Base *b = new Derived();
    b->print_n();

    delete b;
  }
  std::cout << "END: HEAP ALLOCATION" << std::endl;
  end_time;
#endif

}


int main() {

    // Created for the padding problems
    struct DerivedBlock {
        Derived d;
        char c;
    };

    std::cout << "sizeof(DerivedBlock): " << sizeof(DerivedBlock) << std::endl;
    std::cout << "sizeof(Derived): " << sizeof(Derived) << std::endl;
    std::cout << "sizeof(Base): " << sizeof(Base) << std::endl;

    {
        std::cout << "START: SCOPE 0" << std::endl;

        // Can we do it without a pointer?
        // Heap is there and there only so that we can escape C++ static_cast meaning once var as upcast Base moves or is copies out of the scole lifetime - the downcasting is not possible as we are not pointing to the same memory region at that point

        // Base* b = new Derived();
        // delete b;

        void* rawMemory = ::operator new(sizeof(Derived));

        Base* p; //  = reinterpret_cast<Base*>(rawMemory);

        p = new (rawMemory) Derived();

        // Destroy
        p->~Base();

        // Deallocate
        ::operator delete(p);

        std::cout << "END: SCOPE 0" << std::endl;
    }

    {
        std::cout << "START: SCOPE 1" << std::endl;

        void* rawMemory = stack.next_block_available();

        stack.mark_block(rawMemory);

        stack.push_block();

        Base* b = new (rawMemory) Derived();

        b->print_n();

        stack.delete_block(b);

        std::cout << "END: SCOPE 1" << std::endl;
    }

    {
        std::cout << "START: SCOPE 2" << std::endl;

        Base* b = __custom_stack_new<Derived>();
        b->print_n();
        stack.delete_block(b);

        std::cout << "END: SCOPE 2" << std::endl;
    }


    {
        std::cout << "START: SCOPE 3" << std::endl;

        Base* derived_base = nullptr;

        for(size_t i = 0; i < 10; i++) {
            Base* b = __custom_stack_new<Derived>();
            b->print_n();

            stack.delete_block(b);
            // derived_base = b;
        }

        // reinterpret_cast<Derived*>(derived_base)->derived_call_only();

        // stack.delete_block(reinterpret_cast<Derived*>(derived_base)); // OK, TOO!
        // stack.delete_block(derived_base);

        std::cout << "END: SCOPE 3" << std::endl;
    }


    return 0;
}


/*

// Here is how vtable works:
// When (b = new Base()), Base::Base() uses the "vtable for Base":
// When (b = new Derived()), Derived::Derived() uses the "vtable for Derived":
// https://youtu.be/c1e9yGLw1aI


#include <iostream>


#include <cassert>


// g++ main.cpp -o out.out -O0 -Wall

class Base {
    public:
    int* n {nullptr};

    Base() : n{new int(1)} {
      // std::cout << "Base()" << std::endl;
    }

    virtual void print_n() {
        assert(n != nullptr);

        std::cout << "Base::n " << *n << std::endl;
    }

    virtual ~Base() { // Try ~Base() to demonstrate
        std::cout << "~Base()" << std::endl;
        delete n;
        n = nullptr;
    }
};

class Derived : public Base {
    public:
        int* n1 {nullptr};

        Derived() : Base() {
            n1 = new int(2);
        }

        void print_n() override {

            assert(n1 != nullptr);
            assert(
                (reinterpret_cast<Base*>(this)->n) != nullptr
            );

            std::cout << "Derived::n1 " << *n1 << std::endl;

            std::cout << "Derived::Base::n " << 
                *(reinterpret_cast<Base*>(this)->n) << std::endl;
        }

        void derived_call_only() {
            assert(n1 != nullptr);

            std::cout << "Derived CALL! Derived::n1 " << *n1 << std::endl;
        }

        ~Derived() {
            std::cout << "~Derived()" << std::endl;
            delete n1;
            n1 = nullptr;
        }

};


int main() {

    // Created for the padding problems
    struct DerivedBlock {
        Derived d;
        char c;
    };

    std::cout << "sizeof(DerivedBlock): " << sizeof(DerivedBlock) << std::endl;
    std::cout << "sizeof(Derived): " << sizeof(Derived) << std::endl;
    std::cout << "sizeof(Base): " << sizeof(Base) << std::endl;

    {
        std::cout << "START: SCOPE 0" << std::endl;

        // Can we do it without a pointer?
        // Heap is there and there only so that we can escape C++ static_cast meaning once var as upcast Base moves or is copies out of the scole lifetime - the downcasting is not possible as we are not pointing to the same memory region at that point

        // Base* b = new Derived();
        // delete b;

        void* rawMemory = ::operator new(sizeof(Derived));

        Base* p; //  = reinterpret_cast<Base*>(rawMemory);

        p = new (rawMemory) Derived();

        // Destroy
        p->~Base();

        // Deallocate
        ::operator delete(p);

        std::cout << "END: SCOPE 0" << std::endl;
    }


    return 0;
}

# GNU C++11 (Compiler-Explorer-Build-gcc--binutils-2.44) version 16.1.0 (x86_64-linux-gnu)
#       compiled by GNU C version 11.4.0, GMP version 6.3.0, MPFR version 4.2.2, MPC version 1.3.1, isl version isl-0.24-GMP

# GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
# options passed: -masm=intel -mtune=generic -march=x86-64 -g -O0 -std=c++11
#APP
#NO_APP
"Base::Base()":
        push    rbp     #
        mov     rbp, rsp  #,
        sub     rsp, 16   #,
        mov     QWORD PTR [rbp-8], rdi    # this, this
# /app/example.cpp:14:     Base() : n{new int(1)} {
        mov     edx, OFFSET FLAT:"vtable for Base"+16   # _1,
        mov     rax, QWORD PTR [rbp-8]    # tmp102, this
        mov     QWORD PTR [rax], rdx      # this_4(D)->_vptr.Base, _1
# /app/example.cpp:14:     Base() : n{new int(1)} {
        mov     edi, 4    #,
        call    "operator new(unsigned long)" #
# /app/example.cpp:14:     Base() : n{new int(1)} {
        mov     DWORD PTR [rax], 1        # MEM[(int *)_8],
# /app/example.cpp:14:     Base() : n{new int(1)} {
        mov     ecx, 0    # _11,
        mov     rdx, QWORD PTR [rbp-8]    # tmp104, this
        mov     QWORD PTR [rdx+8], rax    # this_4(D)->n, _7
        test    cl, cl  # _11
        je      .L5 #,
# /app/example.cpp:14:     Base() : n{new int(1)} {
        mov     rdi, rax  #, _7
        call    "operator delete(void*)"        #
# /app/example.cpp:14:     Base() : n{new int(1)} {
        nop     
.L5:
# /app/example.cpp:16:     }
        nop     
        leave   
        ret     
        .set    "Base::Base()","Base::Base()"
.LC0:
        .string "virtual void Base::print_n()"
.LC1:
        .string "/app/example.cpp"
.LC2:
        .string "n != nullptr"
.LC3:
        .string "Base::n "
"Base::print_n()":
        push    rbp     #
        mov     rbp, rsp  #,
        sub     rsp, 16   #,
        mov     QWORD PTR [rbp-8], rdi    # this, this
# /app/example.cpp:19:         assert(n != nullptr);
        mov     rax, QWORD PTR [rbp-8]    # tmp103, this
        mov     rax, QWORD PTR [rax+8]    # _1, this_7(D)->n
        test    rax, rax        # _1
        jne     .L7       #,
# /app/example.cpp:19:         assert(n != nullptr);
        mov     ecx, OFFSET FLAT:.LC0     #,
        mov     edx, 19   #,
        mov     esi, OFFSET FLAT:.LC1     #,
        mov     edi, OFFSET FLAT:.LC2     #,
        call    "__assert_fail" #
.L7:
# /app/example.cpp:21:         std::cout << "Base::n " << *n << std::endl;
        mov     esi, OFFSET FLAT:.LC3     #,
        mov     edi, OFFSET FLAT:"std::cout"      #,
        call    "std::basic_ostream<char, std::char_traits<char>>& std::operator<<<std::char_traits<char>>(std::basic_ostream<char, std::char_traits<char>>&, char const*)"       #
        mov     rdx, rax  # _2,
# /app/example.cpp:21:         std::cout << "Base::n " << *n << std::endl;
        mov     rax, QWORD PTR [rbp-8]    # tmp104, this
        mov     rax, QWORD PTR [rax+8]    # _3, this_7(D)->n
        mov     eax, DWORD PTR [rax]      # _4, *_3
        mov     esi, eax  #, _4
        mov     rdi, rdx  #, _2
        call    "std::ostream::operator<<(int)"     #
# /app/example.cpp:21:         std::cout << "Base::n " << *n << std::endl;
        mov     esi, OFFSET FLAT:"std::basic_ostream<char, std::char_traits<char>>& std::endl<char, std::char_traits<char>>(std::basic_ostream<char, std::char_traits<char>>&)"     #,
        mov     rdi, rax  #, _5
        call    "std::ostream::operator<<(std::ostream& (*)(std::ostream&))"      #
# /app/example.cpp:22:     }
        nop     
        leave   
        ret     
.LC4:
        .string "~Base()"
"Base::~Base()":
        push    rbp     #
        mov     rbp, rsp  #,
        sub     rsp, 16   #,
        mov     QWORD PTR [rbp-8], rdi    # this, this
# /app/example.cpp:24:     virtual ~Base() { // Try ~Base() to demonstrate
        mov     edx, OFFSET FLAT:"vtable for Base"+16   # _1,
        mov     rax, QWORD PTR [rbp-8]    # tmp101, this
        mov     QWORD PTR [rax], rdx      # this_5(D)->_vptr.Base, _1
# /app/example.cpp:25:         std::cout << "~Base()" << std::endl;
        mov     esi, OFFSET FLAT:.LC4     #,
        mov     edi, OFFSET FLAT:"std::cout"      #,
        call    "std::basic_ostream<char, std::char_traits<char>>& std::operator<<<std::char_traits<char>>(std::basic_ostream<char, std::char_traits<char>>&, char const*)"       #
# /app/example.cpp:25:         std::cout << "~Base()" << std::endl;
        mov     esi, OFFSET FLAT:"std::basic_ostream<char, std::char_traits<char>>& std::endl<char, std::char_traits<char>>(std::basic_ostream<char, std::char_traits<char>>&)"     #,
        mov     rdi, rax  #, _2
        call    "std::ostream::operator<<(std::ostream& (*)(std::ostream&))"      #
# /app/example.cpp:26:         delete n;
        mov     rax, QWORD PTR [rbp-8]    # tmp102, this
        mov     rax, QWORD PTR [rax+8]    # _10, this_5(D)->n
        test    rax, rax        # _10
        je      .L9 #,
# /app/example.cpp:26:         delete n;
        mov     rdi, rax  #, _10
        call    "operator delete(void*)"        #
.L9:
# /app/example.cpp:27:         n = nullptr;
        mov     rax, QWORD PTR [rbp-8]    # tmp103, this
        mov     QWORD PTR [rax+8], 0      # this_5(D)->n,
# /app/example.cpp:28:     }
        nop     
        leave   
        ret     
        .set    "Base::~Base()","Base::~Base()"
"Base::~Base()":
        push    rbp     #
        mov     rbp, rsp  #,
        sub     rsp, 16   #,
        mov     QWORD PTR [rbp-8], rdi    # this, this
# /app/example.cpp:28:     }
        mov     rax, QWORD PTR [rbp-8]    # tmp98, this
        mov     rdi, rax  #, tmp98
        call    "Base::~Base()"  #
# /app/example.cpp:28:     }
        mov     rax, QWORD PTR [rbp-8]    # tmp99, this
        mov     rdi, rax  #, tmp99
        call    "operator delete(void*)"        #
# /app/example.cpp:28:     }
        leave   
        ret     
"Derived::Derived()":
        push    rbp     #
        mov     rbp, rsp  #,
        push    rbx     #
        sub     rsp, 24   #,
        mov     QWORD PTR [rbp-24], rdi   # this, this
# /app/example.cpp:35:         Derived() : Base() {
        mov     rax, QWORD PTR [rbp-24]   # _1, this
        mov     rdi, rax  #, _1
        call    "Base::Base()"  #
# /app/example.cpp:35:         Derived() : Base() {
        mov     edx, OFFSET FLAT:"vtable for Derived"+16        # _2,
        mov     rax, QWORD PTR [rbp-24]   # tmp105, this
        mov     QWORD PTR [rax], rdx      # this_5(D)->D.36112._vptr.Base, _2
        mov     rax, QWORD PTR [rbp-24]   # tmp106, this
        mov     QWORD PTR [rax+16], 0     # this_5(D)->n1,
# /app/example.cpp:36:             n1 = new int(2);
        mov     edi, 4    #,
        call    "operator new(unsigned long)" #
# /app/example.cpp:36:             n1 = new int(2);
        mov     DWORD PTR [rax], 2        # MEM[(int *)_12],
# /app/example.cpp:36:             n1 = new int(2);
        mov     ecx, 0    # _15,
        mov     rdx, QWORD PTR [rbp-24]   # tmp108, this
        mov     QWORD PTR [rdx+16], rax   # this_5(D)->n1, _11
        test    cl, cl  # _15
        je      .L12        #,
# /app/example.cpp:36:             n1 = new int(2);
        mov     rdi, rax  #, _11
        call    "operator delete(void*)"        #
# /app/example.cpp:36:             n1 = new int(2);
        nop     
.L12:
# /app/example.cpp:37:         }
        jmp     .L15      #
# /app/example.cpp:37:         }
        mov     rbx, rax  # tmp109,
        mov     rax, QWORD PTR [rbp-24]   # _3, this
        mov     rdi, rax  #, _3
        call    "Base::~Base()"  #
        mov     rax, rbx  # D.39182, tmp109
        mov     rdi, rax  #, D.39182
        call    "_Unwind_Resume"        #
.L15:
# /app/example.cpp:37:         }
        mov     rbx, QWORD PTR [rbp-8]    #,
        leave   
        ret     
        .set    "Derived::Derived()","Derived::Derived()"
.LC5:
        .string "virtual void Derived::print_n()"
.LC6:
        .string "n1 != nullptr"
.LC7:
        .string "(reinterpret_cast<Base*>(this)->n) != nullptr"
.LC8:
        .string "Derived::n1 "
.LC9:
        .string "Derived::Base::n "
"Derived::print_n()":
        push    rbp     #
        mov     rbp, rsp  #,
        sub     rsp, 16   #,
        mov     QWORD PTR [rbp-8], rdi    # this, this
# /app/example.cpp:41:             assert(n1 != nullptr);
        mov     rax, QWORD PTR [rbp-8]    # tmp108, this
        mov     rax, QWORD PTR [rax+16]   # _1, this_12(D)->n1
        test    rax, rax        # _1
        jne     .L17      #,
# /app/example.cpp:41:             assert(n1 != nullptr);
        mov     ecx, OFFSET FLAT:.LC5     #,
        mov     edx, 41   #,
        mov     esi, OFFSET FLAT:.LC1     #,
        mov     edi, OFFSET FLAT:.LC6     #,
        call    "__assert_fail" #
.L17:
# /app/example.cpp:42:             assert(
        mov     rax, QWORD PTR [rbp-8]    # tmp109, this
        mov     rax, QWORD PTR [rax+8]    # _2, MEM[(struct Base *)this_12(D)].n
        test    rax, rax        # _2
        jne     .L18      #,
# /app/example.cpp:42:             assert(
        mov     ecx, OFFSET FLAT:.LC5     #,
        mov     edx, 42   #,
        mov     esi, OFFSET FLAT:.LC1     #,
        mov     edi, OFFSET FLAT:.LC7     #,
        call    "__assert_fail" #
.L18:
# /app/example.cpp:46:             std::cout << "Derived::n1 " << *n1 << std::endl;
        mov     esi, OFFSET FLAT:.LC8     #,
        mov     edi, OFFSET FLAT:"std::cout"      #,
        call    "std::basic_ostream<char, std::char_traits<char>>& std::operator<<<std::char_traits<char>>(std::basic_ostream<char, std::char_traits<char>>&, char const*)"       #
        mov     rdx, rax  # _3,
# /app/example.cpp:46:             std::cout << "Derived::n1 " << *n1 << std::endl;
        mov     rax, QWORD PTR [rbp-8]    # tmp110, this
        mov     rax, QWORD PTR [rax+16]   # _4, this_12(D)->n1
        mov     eax, DWORD PTR [rax]      # _5, *_4
        mov     esi, eax  #, _5
        mov     rdi, rdx  #, _3
        call    "std::ostream::operator<<(int)"     #
# /app/example.cpp:46:             std::cout << "Derived::n1 " << *n1 << std::endl;
        mov     esi, OFFSET FLAT:"std::basic_ostream<char, std::char_traits<char>>& std::endl<char, std::char_traits<char>>(std::basic_ostream<char, std::char_traits<char>>&)"     #,
        mov     rdi, rax  #, _6
        call    "std::ostream::operator<<(std::ostream& (*)(std::ostream&))"      #
# /app/example.cpp:48:             std::cout << "Derived::Base::n " << 
        mov     esi, OFFSET FLAT:.LC9     #,
        mov     edi, OFFSET FLAT:"std::cout"      #,
        call    "std::basic_ostream<char, std::char_traits<char>>& std::operator<<<std::char_traits<char>>(std::basic_ostream<char, std::char_traits<char>>&, char const*)"       #
        mov     rdx, rax  # _7,
# /app/example.cpp:49:                 *(reinterpret_cast<Base*>(this)->n) << std::endl;
        mov     rax, QWORD PTR [rbp-8]    # tmp111, this
        mov     rax, QWORD PTR [rax+8]    # _8, MEM[(struct Base *)this_12(D)].n
# /app/example.cpp:49:                 *(reinterpret_cast<Base*>(this)->n) << std::endl;
        mov     eax, DWORD PTR [rax]      # _9, *_8
        mov     esi, eax  #, _9
        mov     rdi, rdx  #, _7
        call    "std::ostream::operator<<(int)"     #
# /app/example.cpp:49:                 *(reinterpret_cast<Base*>(this)->n) << std::endl;
        mov     esi, OFFSET FLAT:"std::basic_ostream<char, std::char_traits<char>>& std::endl<char, std::char_traits<char>>(std::basic_ostream<char, std::char_traits<char>>&)"     #,
        mov     rdi, rax  #, _10
        call    "std::ostream::operator<<(std::ostream& (*)(std::ostream&))"      #
# /app/example.cpp:50:         }
        nop     
        leave   
        ret     
.LC10:
        .string "~Derived()"
"Derived::~Derived()":
        push    rbp     #
        mov     rbp, rsp  #,
        sub     rsp, 16   #,
        mov     QWORD PTR [rbp-8], rdi    # this, this
# /app/example.cpp:58:         ~Derived() {
        mov     edx, OFFSET FLAT:"vtable for Derived"+16        # _1,
        mov     rax, QWORD PTR [rbp-8]    # tmp102, this
        mov     QWORD PTR [rax], rdx      # this_6(D)->D.36112._vptr.Base, _1
# /app/example.cpp:59:             std::cout << "~Derived()" << std::endl;
        mov     esi, OFFSET FLAT:.LC10    #,
        mov     edi, OFFSET FLAT:"std::cout"      #,
        call    "std::basic_ostream<char, std::char_traits<char>>& std::operator<<<std::char_traits<char>>(std::basic_ostream<char, std::char_traits<char>>&, char const*)"       #
# /app/example.cpp:59:             std::cout << "~Derived()" << std::endl;
        mov     esi, OFFSET FLAT:"std::basic_ostream<char, std::char_traits<char>>& std::endl<char, std::char_traits<char>>(std::basic_ostream<char, std::char_traits<char>>&)"     #,
        mov     rdi, rax  #, _2
        call    "std::ostream::operator<<(std::ostream& (*)(std::ostream&))"      #
# /app/example.cpp:60:             delete n1;
        mov     rax, QWORD PTR [rbp-8]    # tmp103, this
        mov     rax, QWORD PTR [rax+16]   # _11, this_6(D)->n1
        test    rax, rax        # _11
        je      .L20        #,
# /app/example.cpp:60:             delete n1;
        mov     rdi, rax  #, _11
        call    "operator delete(void*)"        #
.L20:
# /app/example.cpp:61:             n1 = nullptr;
        mov     rax, QWORD PTR [rbp-8]    # tmp104, this
        mov     QWORD PTR [rax+16], 0     # this_6(D)->n1,
# /app/example.cpp:62:         }
        mov     rax, QWORD PTR [rbp-8]    # _3, this
        mov     rdi, rax  #, _3
        call    "Base::~Base()"  #
# /app/example.cpp:62:         }
        nop     
# /app/example.cpp:62:         }
        leave   
        ret     
        .set    "Derived::~Derived()","Derived::~Derived()"
"Derived::~Derived()":
        push    rbp     #
        mov     rbp, rsp  #,
        sub     rsp, 16   #,
        mov     QWORD PTR [rbp-8], rdi    # this, this
# /app/example.cpp:62:         }
        mov     rax, QWORD PTR [rbp-8]    # tmp98, this
        mov     rdi, rax  #, tmp98
        call    "Derived::~Derived()"       #
# /app/example.cpp:62:         }
        mov     rax, QWORD PTR [rbp-8]    # tmp99, this
        mov     rdi, rax  #, tmp99
        call    "operator delete(void*)"        #
# /app/example.cpp:62:         }
        leave   
        ret     
.LC11:
        .string "sizeof(DerivedBlock): "
.LC12:
        .string "sizeof(Derived): "
.LC13:
        .string "sizeof(Base): "
.LC14:
        .string "START: SCOPE 0"
.LC15:
        .string "END: SCOPE 0"
"main":
        push    rbp     #
        mov     rbp, rsp  #,
        push    r14     #
        push    r13     #
        push    r12     #
        push    rbx     #
        sub     rsp, 16   #,
# /app/example.cpp:75:     std::cout << "sizeof(DerivedBlock): " << sizeof(DerivedBlock) << std::endl;
        mov     esi, OFFSET FLAT:.LC11    #,
        mov     edi, OFFSET FLAT:"std::cout"      #,
        call    "std::basic_ostream<char, std::char_traits<char>>& std::operator<<<std::char_traits<char>>(std::basic_ostream<char, std::char_traits<char>>&, char const*)"       #
# /app/example.cpp:75:     std::cout << "sizeof(DerivedBlock): " << sizeof(DerivedBlock) << std::endl;
        mov     esi, 32   #,
        mov     rdi, rax  #, _1
        call    "std::ostream::operator<<(unsigned long)"     #
# /app/example.cpp:75:     std::cout << "sizeof(DerivedBlock): " << sizeof(DerivedBlock) << std::endl;
        mov     esi, OFFSET FLAT:"std::basic_ostream<char, std::char_traits<char>>& std::endl<char, std::char_traits<char>>(std::basic_ostream<char, std::char_traits<char>>&)"     #,
        mov     rdi, rax  #, _2
        call    "std::ostream::operator<<(std::ostream& (*)(std::ostream&))"      #
# /app/example.cpp:76:     std::cout << "sizeof(Derived): " << sizeof(Derived) << std::endl;
        mov     esi, OFFSET FLAT:.LC12    #,
        mov     edi, OFFSET FLAT:"std::cout"      #,
        call    "std::basic_ostream<char, std::char_traits<char>>& std::operator<<<std::char_traits<char>>(std::basic_ostream<char, std::char_traits<char>>&, char const*)"       #
# /app/example.cpp:76:     std::cout << "sizeof(Derived): " << sizeof(Derived) << std::endl;
        mov     esi, 24   #,
        mov     rdi, rax  #, _3
        call    "std::ostream::operator<<(unsigned long)"     #
# /app/example.cpp:76:     std::cout << "sizeof(Derived): " << sizeof(Derived) << std::endl;
        mov     esi, OFFSET FLAT:"std::basic_ostream<char, std::char_traits<char>>& std::endl<char, std::char_traits<char>>(std::basic_ostream<char, std::char_traits<char>>&)"     #,
        mov     rdi, rax  #, _4
        call    "std::ostream::operator<<(std::ostream& (*)(std::ostream&))"      #
# /app/example.cpp:77:     std::cout << "sizeof(Base): " << sizeof(Base) << std::endl;
        mov     esi, OFFSET FLAT:.LC13    #,
        mov     edi, OFFSET FLAT:"std::cout"      #,
        call    "std::basic_ostream<char, std::char_traits<char>>& std::operator<<<std::char_traits<char>>(std::basic_ostream<char, std::char_traits<char>>&, char const*)"       #
# /app/example.cpp:77:     std::cout << "sizeof(Base): " << sizeof(Base) << std::endl;
        mov     esi, 16   #,
        mov     rdi, rax  #, _5
        call    "std::ostream::operator<<(unsigned long)"     #
# /app/example.cpp:77:     std::cout << "sizeof(Base): " << sizeof(Base) << std::endl;
        mov     esi, OFFSET FLAT:"std::basic_ostream<char, std::char_traits<char>>& std::endl<char, std::char_traits<char>>(std::basic_ostream<char, std::char_traits<char>>&)"     #,
        mov     rdi, rax  #, _6
        call    "std::ostream::operator<<(std::ostream& (*)(std::ostream&))"      #
# /app/example.cpp:80:         std::cout << "START: SCOPE 0" << std::endl;
        mov     esi, OFFSET FLAT:.LC14    #,
        mov     edi, OFFSET FLAT:"std::cout"      #,
        call    "std::basic_ostream<char, std::char_traits<char>>& std::operator<<<std::char_traits<char>>(std::basic_ostream<char, std::char_traits<char>>&, char const*)"       #
# /app/example.cpp:80:         std::cout << "START: SCOPE 0" << std::endl;
        mov     esi, OFFSET FLAT:"std::basic_ostream<char, std::char_traits<char>>& std::endl<char, std::char_traits<char>>(std::basic_ostream<char, std::char_traits<char>>&)"     #,
        mov     rdi, rax  #, _7
        call    "std::ostream::operator<<(std::ostream& (*)(std::ostream&))"      #
# /app/example.cpp:88:         void* rawMemory = ::operator new(sizeof(Derived));
        mov     edi, 24   #,
        call    "operator new(unsigned long)" #
# /app/example.cpp:88:         void* rawMemory = ::operator new(sizeof(Derived));
        mov     QWORD PTR [rbp-40], rax   # rawMemory, _33
# /app/example.cpp:92:         p = new (rawMemory) Derived();
        mov     r12, QWORD PTR [rbp-40]   # _35, rawMemory
# /app/example.cpp:92:         p = new (rawMemory) Derived();
        mov     rsi, r12  #, _35
        mov     edi, 24   #,
        call    "operator new(unsigned long, void*)"       #
        mov     rbx, rax  # _37,
# /app/example.cpp:92:         p = new (rawMemory) Derived();
        mov     r14d, 1   # _39,
# /app/example.cpp:92:         p = new (rawMemory) Derived();
        mov     rdi, rbx  #, _37
        call    "Derived::Derived()"       #
# /app/example.cpp:92:         p = new (rawMemory) Derived();
        mov     eax, 0    # _41,
        mov     QWORD PTR [rbp-48], rbx   # p, _37
        test    al, al  # _41
        je      .L23        #,
# /app/example.cpp:92:         p = new (rawMemory) Derived();
        mov     rsi, r12  #, _35
        mov     rdi, rbx  #, _37
        call    "operator delete(void*, void*)"      #
# /app/example.cpp:92:         p = new (rawMemory) Derived();
        nop     
.L23:
# /app/example.cpp:95:         p->~Base();
        mov     rax, QWORD PTR [rbp-48]   # tmp118, p
        mov     rax, QWORD PTR [rax]      # _8, p_42->_vptr.Base
        add     rax, 8    # _9,
        mov     rdx, QWORD PTR [rax]      # _10, *_9
        mov     rax, QWORD PTR [rbp-48]   # tmp119, p
        mov     rdi, rax  #, tmp119
        call    rdx     # _10
# /app/example.cpp:98:         ::operator delete(p);
        mov     rax, QWORD PTR [rbp-48]   # tmp120, p
        mov     rdi, rax  #, tmp120
        call    "operator delete(void*)"        #
# /app/example.cpp:100:         std::cout << "END: SCOPE 0" << std::endl;
        mov     esi, OFFSET FLAT:.LC15    #,
        mov     edi, OFFSET FLAT:"std::cout"      #,
        call    "std::basic_ostream<char, std::char_traits<char>>& std::operator<<<std::char_traits<char>>(std::basic_ostream<char, std::char_traits<char>>&, char const*)"       #
# /app/example.cpp:100:         std::cout << "END: SCOPE 0" << std::endl;
        mov     esi, OFFSET FLAT:"std::basic_ostream<char, std::char_traits<char>>& std::endl<char, std::char_traits<char>>(std::basic_ostream<char, std::char_traits<char>>&)"     #,
        mov     rdi, rax  #, _11
        call    "std::ostream::operator<<(std::ostream& (*)(std::ostream&))"      #
# /app/example.cpp:104:     return 0;
        mov     eax, 0    # _49,
        jmp     .L28      #
# /app/example.cpp:92:         p = new (rawMemory) Derived();
        mov     r13, rax  # tmp122,
        test    r14b, r14b      # _39
        je      .L26        #,
# /app/example.cpp:92:         p = new (rawMemory) Derived();
        mov     rsi, r12  #, _35
        mov     rdi, rbx  #, _37
        call    "operator delete(void*, void*)"      #
# /app/example.cpp:92:         p = new (rawMemory) Derived();
        nop     
.L26:
        mov     rax, r13  # D.39189, tmp122
        mov     rdi, rax  #, D.39189
        call    "_Unwind_Resume"        #
.L28:
# /app/example.cpp:105: }
        add     rsp, 16   #,
        pop     rbx       #
        pop     r12       #
        pop     r13       #
        pop     r14       #
        pop     rbp       #
        ret     
"vtable for Derived":
        .quad   0
        .quad   "typeinfo for Derived"
        .quad   "Derived::print_n()"
        .quad   "Derived::~Derived()"
        .quad   "Derived::~Derived()"
"vtable for Base":
        .quad   0
        .quad   "typeinfo for Base"
        .quad   "Base::print_n()"
        .quad   "Base::~Base()"
        .quad   "Base::~Base()"
"typeinfo for Derived":
# <anonymous>:
# <anonymous>:
        .quad   "vtable for __cxxabiv1::__si_class_type_info"+16
# <anonymous>:
        .quad   "typeinfo name for Derived"
# <anonymous>:
        .quad   "typeinfo for Base"
"typeinfo name for Derived":
        .string "7Derived"
"typeinfo for Base":
# <anonymous>:
# <anonymous>:
        .quad   "vtable for __cxxabiv1::__class_type_info"+16
# <anonymous>:
        .quad   "typeinfo name for Base"
"typeinfo name for Base":
        .string "4Base"

*/