
#include <iostream>

#include <vector>


#include <cassert>
#include <cstring>




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

  static constexpr size_t CAPACITY = Size;

  // unsigned char buffer[CAPACITY];
  unsigned char* buffer {nullptr};

  size_t current_block_size {0};


  CustomMemoryStorage()
  {
    buffer = reinterpret_cast<unsigned char*>(
      ::operator new(Size)
    );
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

  void attach_buffer(void* buffer) {

    assert(this->buffer == nullptr); // ONLY ATTACH THE BUFFER IS PREVIOUS IS NULL

    this->buffer = reinterpret_cast<unsigned char*>(buffer);
  }

  // template<class T, class... Args> inline T* custom_new(Args&&...) __attribute__((always_inline));

  template<class T, class... Args>
  inline T* custom_new(Args&&... args) {
    void* rawMemory = next_block_available(sizeof(T));

    return new (rawMemory) T(std::forward<Args>(args)...);
  }

  inline ~CustomMemoryStorage() {
    ::operator delete (buffer);
  }


};

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
    if(i_ptr != nullptr) {
      delete i_ptr;
    }
    i_ptr = new int(*other.i_ptr);
    return *this;
  }


  S& operator=(S&& other) {
    std::cout << "S&operator=(S&&)" << std::endl;
    if(i_ptr != nullptr) {
      delete i_ptr;
    }
    i_ptr = other.i_ptr;
    other.i_ptr = nullptr;
    return *this;
  }

  ~S() {
    std::cout << "~S()" << std::endl;

    if(i_ptr != nullptr) { // The delete below already does this check, effectively
      delete i_ptr;
      i_ptr = nullptr;
    }

  }
};


static CustomMemoryStorage<1000> allocator;


std::vector<S*> fuzz_test_vector() {
  std::vector<S*> v;

  // NOTE: Confirming that using a primitive pointer fully escapes the variable (primitive or struct) restoration and the C++ RAII. 
  // This is due to the fact that, just like the memory on heap - this memory has to point to somewhere else - where it is left untouched by the current scope - global/top level, for example.
  S* s3 = allocator.custom_new<S>(30);

  v.push_back(
    allocator.custom_new<S>(10)
  );

  v.push_back(
    allocator.custom_new<S>(20)
  );


  v.push_back(s3);


  return v;
}


class S1 {
    public:
    int i; // {0};

    S1() {}
};


void f1(int i = 0) {
    S1 s;

    // Not always gonna be of the same address. Try -O3 optimizations and "-fsanitize=address" will fill it with 0 as a safe guard against illegal memory access - similar to what memset does effectively
    std::cout << &s << std::endl;

    // f1(i + 1);

    std::cout << s.i << std::endl;
    s.i = 1;

}

int main() {

    {
        f1();
        f1();
    }
    // Regardless of the scope
    // f1();

    std::cout << "START: SCOPE 5" << std::endl;
    {
        for(int i = 0; i < 10; i++) {
            static S s = S(i);
            std::cout << &s << std::endl;
        }
    }
    std::cout << "END: SCOPE 5" << std::endl;



   
    return 0;
}

