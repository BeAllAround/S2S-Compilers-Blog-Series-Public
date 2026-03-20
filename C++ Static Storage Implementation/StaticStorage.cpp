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
    return *reinterpret_cast<S*>(buffer + ( stack_count * sizeof(T)));
  }


  ~StaticStorage() {
    for(size_t i = 0; i < stack_count; i++) {
      reinterpret_cast<S*>(buffer + (i * sizeof(T)))->~S();
    }
  }


};


int main1() {


  StaticStorage<S, 10> storage;

  storage.add(S(1));
  storage.add(S(2));


  S s = std::move(storage.pop());


  assert(*s.i_ptr == 2);

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
