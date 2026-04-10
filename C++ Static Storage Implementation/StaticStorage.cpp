#include <iostream>


#include <cassert>



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

const size_t _stack_size = 10;

static unsigned char buffer[sizeof(S) * _stack_size];

int main0() {
  size_t stack_c = 0;

  // Add element
  new(&buffer[stack_c * sizeof(S)]) S(0);
  stack_c++;




  defer: {
    // Invoke Destructors
    for(size_t i = 0; i < stack_c; i++) {
      reinterpret_cast<S*>(buffer + (i * sizeof(S)))->~S();
    }
  }

  return 0;
}

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
  // NOTE: POP FOR STATIC STORAGE IS ILL_ADVISED, RESULTING IN MEMORY LEAKS (EVEN WHEN THERE IS A CAST, WE ARE STILL POINTINT TO UCHAR BUFFER)
  // MAKE SURE TO TEST THIS AT the -O0 level optimizations CUZ THE COMPILER JUST IS TAKES STUFF OUT AT O3
  T& pop() { // SAME GOES FOR const T&
    stack_count--;
    T& popped = *reinterpret_cast<T*>(buffer + ( stack_count * sizeof(T)));

    // *reinterpret_cast<T*>(buffer + ( stack_count * sizeof(T))) = T();


    return popped;
  }

  const T& at(size_t i) {
    return *reinterpret_cast<T*>(buffer + ( i * sizeof(T)));
  }

  // TODO: WRAP UP IN A MACRO JUST LIKE _storage_add
  void insert(size_t i, T&& item) {
    T* _at = reinterpret_cast<T*>(buffer + ( i * sizeof(T)));
    *_at = std::move(item);
    // ALTERNATIVELY
    // _at->~T();
    // new(&buffer[i * sizeof(T)]) T(std::move(item));
  }


  ~StaticStorage() {
    for(size_t i = 0; i < stack_count; i++) {
      reinterpret_cast<T*>(buffer + (i * sizeof(T)))->~T();
    }
  }


};

#define _storage_add(storage, item, T) \
    new(&storage.buffer[storage.stack_count * sizeof(T)]) item; \
    storage.stack_count++;





int main1() {



  StaticStorage<S, 10> storage;

  // storage.add(S(1));
  // storage.add(S(2));


  _storage_add(storage, S(3), S);


  // S s = std::move(storage.pop()); // OK, ACTUALLY
  // assert(*s.i_ptr == 3);
  assert(*storage.at(0).i_ptr == 3);

  return 0;
}




int main() {
  return main1();
}


/* 
References:
- https://en.cppreference.com/w/cpp/types/aligned_storage.html
- The C++ Programming Language | 35.4.2 Type Generators
*/
