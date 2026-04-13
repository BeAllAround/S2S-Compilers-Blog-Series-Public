#include <iostream>


#include <cstdio>
#include <cstdarg>
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
    std::cout << "~S() " << this << std::endl;

    if(i_ptr != nullptr) {
      delete i_ptr;
      i_ptr = nullptr;
    }

  }
};

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




S r_s(S s) {
    return s;
}

/*
void print_numbers(int count, ...) {
    StaticStorage<S, 2> _arr;
    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++) {
        // std::cout << val.i_ptr << std::endl;
        // std::cout << va_arg(args, S).i_ptr << std::endl;
        _storage_add(_arr, 
        S(static_cast<S>(std::move(va_arg(args, S)))),
        S);



    }

    va_end(args);
    std::cout << std::endl;
}
*/

template<class T>
inline void _print(T&& first) {
    std::cout << &first << std::endl;
}

template<class T, class... Args>
inline void _print(T&& first, Args&&... args) {
    std::cout << &first << std::endl;
    // _storage_add(_buffer, T(std::move(first)), T)
    _print(args ...);
}


#define _storage_add(storage, item, T) \
    new(&storage.buffer[storage.stack_count * sizeof(T)]) item; \
    storage.stack_count++;


#define FE_1(m, _storage, T, x)      m(_storage, T, x)
#define FE_2(m, _storage, T, x, ...) m(_storage, T, x) FE_1(m, _storage, T, __VA_ARGS__)
#define FE_3(m,_storage, T, x, ...) m(_storage, T, x) FE_2(m, _storage, T, __VA_ARGS__)
#define FE_4(m,_storage, T, x, ...) m(_storage, T, x) FE_3(m, _storage, T, __VA_ARGS__)
// So on and so forth
// Sadly, this is limited at compile time


#define GET_MACRO(_storage, T, _1,_2,_3, _4, NAME, ...) NAME
#define FOR_EACH(action, _storage, T, ...) \
  GET_MACRO(_storage, T, __VA_ARGS__, FE_4, FE_3, FE_2, FE_1)(action, _storage, T, __VA_ARGS__)

// GET_MACRO(_storage, T, S(1), S(2), S(3), FE_3, FE_2, FE_1)


#define _MAKEARRAY(_storage, T, item) _storage_add(_storage, item, T)

#define MAKESTATICSTORAGE(_storage, T, ...)  FOR_EACH(_MAKEARRAY, _storage, T, __VA_ARGS__) 

int main() {

    // std::initializer_list<S> __arr = { S(1), S(2), S(3) };

/*
    StaticStorage<S, 4> _arr;

    _storage_add(_arr, S(1), S);
    _storage_add(_arr, S(2), S);
    _storage_add(_arr, S(3), S);
    _storage_add(_arr, S(4), S);
    */

    // _arr.add(S(1));
    // _arr.add(S(2));

    // S s = S(0);

    // _print(s);
    // _print(S(1), S(2), S(3), S(4));

    StaticStorage<S, 4> _arr;


    // FOR_EACH(_MAKEARRAY, _arr, S, S(1), S(2), S(3))
    MAKESTATICSTORAGE(
        _arr,
        S, 
        S(1), S(2), S(3), S(4)
    )

    /*
    // OK
      S(int)
      S(int)
      S(int)
      ~S() 0x7ffec578aad0
      ~S() 0x7ffec578aad8
      ~S() 0x7ffec578aae0
     */


    // r_s(S(1)); // FAIL: S(int), S(S&&), ~S(), ~S()

    return 0;
}
