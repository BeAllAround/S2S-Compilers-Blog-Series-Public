#include <iostream>

#include <vector>


#include <cassert>


// g++ main.cpp -o out.out -O0 -Wall && valgrind -s --leak-check=full --show-leak-kinds=all ./out.out


#define start_time clock_t s_t_a_r_t = clock();

#define end_time printf("[Cpu_time_used: %f]\n", static_cast < double > (clock() - s_t_a_r_t) / CLOCKS_PER_SEC);

/*

The Rule of Thumb: If your calculated allocation size approaches or exceeds a few hundred kilobytes, abort stack allocation entirely and fall back to heap memory (malloc or new).

For this reason, use macros or alloca - C++ specific as there can be no runtime function evaluation for C at the top level.


*/



template<size_t Size>
class CustomMemoryStorage {
  public:

  unsigned char buffer[Size];
  // unsigned char* buffer {nullptr};

  size_t current_block_size {0};


  CustomMemoryStorage() {
    /*
    Also see: https://man7.org/linux/man-pages/man3/alloca.3.html
    buffer = reinterpret_cast<unsigned char*>(
      ::operator new(Size)
    );
    */
  }


  // NOTE: THIS IS DELETED AS IT DEMANDS MORE FUNCTIONALITY AND RUNTIME OVERHEAD WITH THAT
  CustomMemoryStorage(const CustomMemoryStorage&) = delete;
  CustomMemoryStorage(CustomMemoryStorage&&) = delete;

  CustomMemoryStorage& operator=(const CustomMemoryStorage&) = delete;
  CustomMemoryStorage& operator=(CustomMemoryStorage&&) = delete;


  inline void* next_block_available(size_t n) {
    void* rawMemory = reinterpret_cast<void*>(buffer + current_block_size); // Block with the marked

    current_block_size += n;

    return rawMemory;
  }

  template<class T>
  inline void destroy(T* block) {
    block->~T();
  }

  ~CustomMemoryStorage() {
    // ::operator delete (buffer);
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

class OtherClass {
  public:
  std::vector<int> v;

  OtherClass() {
    v.push_back(0);
    v.push_back(1);
  }
};


// https://en.cppreference.com/cpp/memory
// https://en.cppreference.com/cpp/memory/new/operator_new
static CustomMemoryStorage<1000> allocator;


// template<class T, class... Args> inline T* __custom_stack_new(Args&&...) __attribute__((always_inline));

template<class T, class... Args>
inline T* __custom_stack_new(Args&&... args) {
    void* rawMemory = allocator.next_block_available(sizeof(T));

    return new (rawMemory) T(std::forward<Args>(args)...);
}


void benchmark01() {

#define __TEST_MAX 4000 // NOTE: FOR BENCHMARKING // MAKE SURE TO MODIFY(INCREASE THE SIZE_T) FOR TESTING
#define __CUSTOM_STACK_NEW_TEST

#ifdef __CUSTOM_STACK_NEW_TEST
  start_time;
  std::cout << "START: __CUSTOM_STACK_NEW_TEST" << std::endl;
  for (size_t i = 0; i < __TEST_MAX; i++)
  {
    Base *b = __custom_stack_new<Derived>();
    b->print_n();

    allocator.destroy(b);
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

    /*
    // Created for the padding problems
    struct DerivedBlock {
        Derived d;
        char c;
    };

    std::cout << "sizeof(DerivedBlock): " << sizeof(DerivedBlock) << std::endl;
    */
    std::cout << "sizeof(Derived): " << sizeof(Derived) << std::endl;
    std::cout << "sizeof(Base): " << sizeof(Base) << std::endl;

    std::cout << "sizeof(OtherClass): " << sizeof(OtherClass) << std::endl;

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

        void* rawMemory = allocator.next_block_available(sizeof(Derived));

        Base* b = new (rawMemory) Derived();

        b->print_n();

        allocator.destroy(b);

        std::cout << "END: SCOPE 1" << std::endl;
    }

    {
        std::cout << "START: SCOPE 2" << std::endl;

        Base* b = __custom_stack_new<Derived>();
        b->print_n();

        allocator.destroy(b);

        std::cout << "END: SCOPE 2" << std::endl;
    }

    {
        std::cout << "START: SCOPE 3" << std::endl;

        OtherClass* b = __custom_stack_new<OtherClass>();

        allocator.destroy(b);

        std::cout << "END: SCOPE 3" << std::endl;
    }

    {
        std::cout << "START: SCOPE 4" << std::endl;

        Base* derived_base = nullptr;

        for(size_t i = 0; i < 10; i++) {
            Base* b = __custom_stack_new<Derived>();
            b->print_n();

            allocator.destroy(b);
            // derived_base = b;
        }

        // reinterpret_cast<Derived*>(derived_base)->derived_call_only();

        // stack.delete_block(reinterpret_cast<Derived*>(derived_base)); // OK, TOO!
        // stack.delete_block(derived_base);

        std::cout << "END: SCOPE 4" << std::endl;
    }


    return 0;
}

/*
#include <iostream>

#include <memory>


#include <cassert>

#define _MAX 1000000

int main() {
  char c;
  constexpr size_t capacity = sizeof(int) * _MAX;

  int* a = reinterpret_cast<int*>(
      ::operator new(capacity)
  );

  // The only problem with this is filled memory stays occupied if we look at the system monitor/task manager. Otherwise, it is just unused - however - it is allocated if we look at the valgrind report.
  // for(int i = 0; i < _MAX; i++) {
  //  a[i] = i;
  // }

  // std::cout << (a+0) << std::endl;
  // std::cout << (a+1) << std::endl;
  // std::cout << (a+2) << std::endl;

  assert( ((char*)(a+1) - (char*)(a+0)) == 4);

  std::cout << ( (char*)(a+1) - (char*)(a+0) ) << std::endl;

  std::cout << "PAUSED_0" << std::endl;

  std::cin >> c;


  // std::cout << ((char*)a)[1] << std::endl;

  // Essentially, memset(a, 0, sizeof(((char*)a)) );
  // for(size_t i = 0; i < capacity; i++) {
  //   std::cout << ((char*)a)[i] << std::endl;
       // ((char*)a)[i] = 0;
  // }

  std::cout << "PAUSED_1" << std::endl;
  c = 0;
  std::cin >> c;




  ::operator delete(a);

  std::cout << "PAUSED_2" << std::endl;
  c = 0;
  std::cin >> c;


  return 0;
}

*/

