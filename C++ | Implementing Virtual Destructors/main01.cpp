#include <iostream>

#include <vector>


#include <cassert>
#include <cstring>



#define start_time clock_t s_t_a_r_t = clock();

#define end_time printf("[Cpu_time_used: %f]\n", static_cast < double > (clock() - s_t_a_r_t) / CLOCKS_PER_SEC);


class Base {
    public:
    int* n {nullptr};

    Base() {
      n = new int(1);
    }

    virtual ~Base() {
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

        void sanity_check() {

            assert(n1 != nullptr);
            assert(
                (reinterpret_cast<Base*>(this)->n) != nullptr
            );

            /*
            std::cout << "Derived::n1 " << *n1 << std::endl;

            std::cout << "Derived::Base::n " << 
                *(reinterpret_cast<Base*>(this)->n) << std::endl;
            */
        }

        void derived_call_only() {
            assert(n1 != nullptr);

            std::cout << "Derived CALL! Derived::n1 " << *n1 << std::endl;
        }

        ~Derived() {
            delete n1;
            n1 = nullptr;
        }

};


void benchmark01() {

  #define __TEST_MAX 10 // NOTE: FOR BENCHMARKING // MAKE SURE TO MODIFY(INCREASE THE SIZE_T) FOR TESTING

  for(size_t j = 0; j < 1; j++) {

    std::cout << "START: HEAP ALLOCATION" << std::endl;
    start_time;
    for (volatile size_t i = 0; i < __TEST_MAX; i++)
    {
      Base *b = new(::operator new(sizeof(Derived))) Derived();

      ((Derived*)b)->sanity_check();

      b->~Base();
      ::operator delete(b);

      // delete b;
    }
    end_time;
    std::cout << "END: HEAP ALLOCATION" << std::endl;
  }

}


int main() {

    std::cout << "sizeof(Derived): " << sizeof(Derived) << std::endl;
    std::cout << "sizeof(Base): " << sizeof(Base) << std::endl;

    {
      benchmark01();
    }


    return 0;
}
