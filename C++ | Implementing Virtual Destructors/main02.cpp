#include <iostream>

#include <vector>


#include <cassert>
#include <cstring>

#include <cstdint>


#define start_time clock_t s_t_a_r_t = clock();

#define end_time printf("[Cpu_time_used: %f]\n", static_cast < double > (clock() - s_t_a_r_t) / CLOCKS_PER_SEC);


enum class RuntimeSwitch : uint {
    BaseType,
    DerivedType
};

class Base;

class Derived;

class Base {
    public:
    int* n {nullptr};

    RuntimeSwitch type {RuntimeSwitch::BaseType};

    Base(RuntimeSwitch type) : type {type} {
      n = new int(1);
    }

    void clean_up_heap() {
      delete n;
      n = nullptr;
    }

    inline ~Base(); 
};

class Derived {
    public:
        union UBase { // : public Base {}
            Base base;

            UBase() {}

            ~UBase() __attribute__((always_inline)) {

            }
        } ubase;

        int* n1 {nullptr};
        // std::vector<int> v;

        Derived() {
            new(&ubase.base) Base(RuntimeSwitch::DerivedType); // Same as Derived() : Base()

            n1 = new int(2);

            // v.push_back(0);
        }

        void sanity_check() {

            assert(n1 != nullptr);
            assert( (reinterpret_cast<Base*>(this)->n) != nullptr );

            /*
            std::cout << "Derived::n1 " << *n1 << std::endl;

            std::cout << "Derived::Base::n " << 
                *(reinterpret_cast<Base*>(this)->n) << std::endl;
            */
        }

        void destroy_base() {
          ubase.base.clean_up_heap();
        }

        ~Derived() {
            delete n1;
            n1 = nullptr;
        }

};

inline Base::~Base() {
    // The recurring weakness of this implementation is higher branch misses
    if(type == RuntimeSwitch::DerivedType) {
        reinterpret_cast<Derived*>(this)->~Derived();
    }
    /*
    switch(type) {
        case RuntimeSwitch::DerivedType: {
            reinterpret_cast<Derived*>(this)->~Derived();
            break;
        }
        default: {
            __builtin_unreachable(); // Elimitinating the default case from the assembly
            // break;
        }
    }
    */
  /*
    void* dispatch_table[] = { &&OP_BASE, &&OP_DERIVED };

    // Fetch first instruction
    goto *dispatch_table[(uint)type];

    OP_BASE: {
        goto _BREAK;
    }

    OP_DERIVED: {
        reinterpret_cast<Derived*>(this)->~Derived();
        goto _BREAK;
    }

    _BREAK: {

    }
  */
    /* START: ~Base() */
    delete n;
    n = nullptr;
    /* END: ~Base() */
}


void benchmark01() {

  #define __TEST_MAX 10 // NOTE: FOR BENCHMARKING // MAKE SURE TO MODIFY(INCREASE THE SIZE_T) FOR TESTING

  std::cout << "START: HEAP ALLOCATION" << std::endl;
  for(size_t j = 0; j < 1; j++) {

    start_time;
    for (volatile size_t i = 0; i < __TEST_MAX; i++)
    {

        // Derived* d = new Derived();
        Base *b = reinterpret_cast<Base*>(
            new(::operator new(sizeof(Derived))) Derived()
            // new Derived()
            // d
        );

        /*
        NOTE: Not allowed!
        error: cannot convert ‘Derived*’ to ‘Base*’ in initialization
        101 |     Base *b = new Derived();

        */

        ((Derived*)b)->sanity_check();

        // These two will work for C++11 and newer!
        b->~Base();
        ::operator delete (b);

        // delete b; // Mismatched new/delete size value: 16 // OK FOR C++11 - ALL NEWER THAN THAT WILL HAVE MISMATCHED new/delete
    }
    end_time;
    std::cout << "END: HEAP ALLOCATION" << std::endl;
  }

}


int main() {

    std::cout << "sizeof(Derived): " << sizeof(Derived) << std::endl;
    std::cout << "sizeof(Base): " << sizeof(Base) << std::endl;

    {
        Derived derived;

        assert(
            reinterpret_cast<uintptr_t>(&derived) == 
            reinterpret_cast<uintptr_t>(&derived.ubase.base)
        ); // NOTE: Inheritance check for safe upcasting and downcasting


        derived.destroy_base(); // This needs to call ~Base() due to the std::vector<int> example but this is GOOD ENOUGH HERE!

    }


    {
      benchmark01();
    }


    return 0;
}
