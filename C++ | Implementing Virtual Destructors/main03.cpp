#include <iostream>

#include <vector>


#include <cassert>
#include <cstring>

#include <cstdint>

#include <alloca.h>


// g++ main.cpp -o out.out -O0 -Wall && valgrind -s --leak-check=full --show-leak-kinds=all ./out.out


#define start_time clock_t s_t_a_r_t = clock();

#define end_time printf("[Cpu_time_used: %f]\n", static_cast < double > (clock() - s_t_a_r_t) / CLOCKS_PER_SEC);

class Base;
class Derived;


void destroy_Base(Base* _this);
void destroy_Derived(Base* _this);



struct BaseVTable {
    // void(*print)();
    void(*destroy)(Base*);
};


BaseVTable base_vtable = {
    .destroy = destroy_Base
};

BaseVTable derived_vtable = {
    // .print = print_derived;
    .destroy = destroy_Derived
};


class Base {
    public:

    int* n {nullptr};
    BaseVTable* v_table {nullptr}; // This one is the 8-byte addition, effectively!


    Base(BaseVTable* v_table) : v_table {v_table} {
      n = new int(1);
    }

    inline ~Base(); 
};

class Derived {
    public:
        union UBase {
            Base base;

            UBase() {}

            ~UBase() __attribute__((always_inline)) {

            }
        } ubase; // NOTE: Avoiding recursive destructor!, unsigned char obj[sizeof(Base)];

        std::vector<int> v;
        int* n1 {nullptr};

        Derived() {
            new(&ubase.base) Base(&derived_vtable); // Same as Derived() : Base()

            n1 = new int(2);

            v.push_back(0);
        }

        void print_n() {

            assert(n1 != nullptr);
            assert( (reinterpret_cast<Base*>(this)->n) != nullptr );

            /*
            std::cout << "Derived::n1 " << *n1 << std::endl;

            std::cout << "Derived::Base::n " << 
                *(reinterpret_cast<Base*>(this)->n) << std::endl;
            */
        }

        ~Derived() {
            delete n1;
            n1 = nullptr;
            // v.~std::vector<int>();
        }

};

inline Base::~Base() {

    // Dispatch the virtual destructor
    v_table->destroy(this);

    /*
    // START: ~Base()
    delete n;
    n = nullptr;
    // END: ~Base()
    */
}

inline void destroy_Base(Base* _this) {
    delete _this->n;
    _this->n = nullptr;
}

void destroy_Derived(Base* _this) {
    Derived* __this = reinterpret_cast<Derived*>(_this);

    __this->~Derived();

    // Not gonna do it if we have std::vector<int> or similar containers
    /*
    delete __this->n1;
    __this->n1 = nullptr;
    */


    destroy_Base(_this); // Inline-able here!
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

        // ((Derived*)b)->print_n();

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

    /*
    {
        Derived derived;

        assert(
            reinterpret_cast<uintptr_t>(&derived) == 
            reinterpret_cast<uintptr_t>(&derived.ubase.base)
        ); // NOTE: Inheritance check for upcasting and downcasting
    }
    */

    {
      benchmark01();
    }


    return 0;
}
