#include <iostream>


#include <cassert>



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


  T* next_block_available() {
    return reinterpret_cast<T*>(buffer + (stack_count * block_size) ); // Block with the marked
    // Essentially, given that the first member is "T block" : return &((reinterpret_cast<TBlock*>(buffer + (stack_count * block_size) ))->block); // Block with the marked
  }

  template<class RP> // RP stands for Raw Pointer
  void mark_block(RP* block) {
    unsigned char* raw_block = reinterpret_cast<unsigned char*>(block);

    // NOTE: This cast is possible because the initial memory block came from the sizeof(TBlock) stack
    (reinterpret_cast<TBlock*> (raw_block))->marked = 1; // Note: Block marked!

  }

  void push_block() {
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
  void delete_block(BT* block) {
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

        void print_n() {

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


static StaticStorage<Derived, 100> stack; // stack of derived specifically



template<class T, class... Args>
T* __custom_stack_new(Args&&... args) {
    void* rawMemory = stack.next_block_available();

    stack.mark_block(rawMemory);

    stack.push_block();

    return new (rawMemory) T(std::forward<Args>(args)...);
}


int main() {

    // Created for the padding problems
    struct DerivedBlock {
        Derived d;
        char c;
    };

    std::cout << sizeof(DerivedBlock) << std::endl;

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

        Base* derived = nullptr;

        for(size_t i = 0; i < 30; i++) {
            Base* b = __custom_stack_new<Derived>();
            b->print_n();

            stack.delete_block(b);
            // derived = b;
        }

        // reinterpret_cast<Derived*>(derived)->derived_call_only();

        // stack.delete_block(reinterpret_cast<Derived*>(derived)); // OK, TOO!
        // stack.delete_block(derived);

        std::cout << "END: SCOPE 3" << std::endl;
    }



    return 0;
}
