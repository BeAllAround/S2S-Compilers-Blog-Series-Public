#include <iostream>

#include <vector>

#include <cassert>

template<class T>
inline void* cpp_realloc(size_t, T*, size_t);

#define STB_DS_IMPLEMENTATION
#define STBDS_REALLOC(c, p, s, T) cpp_realloc<T>(c, p, s)
#define STBDS_FREE(c,p) ::operator delete[](p) 
// delete[] reinterpret_cast<stbds_array_header*>(p) 
// OR, ::operator delete[](p)

#include <stb_ds.h>



#include <cstdint>



template<class T>
inline void* cpp_realloc(size_t c, stbds_array_header* old, size_t new_size) {

  /*
  std::cout << "SIZE_C " << c << std::endl;
  std::cout << "OLD " << old << std::endl; // old == NULL
  std::cout << "NEW_SIZE " << new_size << std::endl;
  std::cout << sizeof(T) << std::endl;
  std::cout << sizeof(stbds_array_header) << std::endl;
  */

  char* ptr = reinterpret_cast<char*>(new char[new_size]);

  /*
  if(old == NULL) {
    return ptr;
  }
  */

  T* _ptr = reinterpret_cast<T*>(ptr + sizeof(stbds_array_header));

  T* _old = reinterpret_cast<T*>(old + 1);

  for(size_t i = 0; i < c; i++) {
    // std::cout << *(_old[i].i_ptr) << std::endl;
    new(_ptr + i) T(std::move(_old[i]));
  }

  if(old != NULL) {
    new(ptr+0) stbds_array_header(*old); // old[0]
  }


  // NOTE: EXPLICIT DESTRUCTORS NOT NEEDED!

  // delete[] reinterpret_cast<char*>(old); // OR, BUT NOT delete[]
  ::operator delete[] (old);

  // ptr += sizeof(stbds_array_header);

  return ptr;

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

  T& pop() {
    stack_count--;
    return *reinterpret_cast<T*>(buffer + ( stack_count * sizeof(T)));
  }

  T&at(size_t i) {
    return *reinterpret_cast<T*>(buffer + ( i * sizeof(T)));
  }


  ~StaticStorage() {
    for(size_t i = 0; i < stack_count; i++) {
      reinterpret_cast<T*>(buffer + (i * sizeof(T)))->~T();
    }
  }


};


#define _storage_add(storage, item, T) \
    new(&storage.buffer[storage.stack_count * sizeof(T)]) item; \
    storage.stack_count++; \



class S;

class S {
  public:
  int* i_ptr { nullptr };
  std::string str; // NOTE: FOR TESTING STB DS REALLOCATION

  S() {
    std::cout << "S()" << std::endl;
  }

  S(int i) {
    std::cout << "S(int)" << std::endl;
    i_ptr = new int(i);
    str = std::to_string(reinterpret_cast<uintptr_t>(i_ptr));
  }

  S(const S& other) {
    std::cout << "S(const S&)" << std::endl;
    i_ptr = new int(*other.i_ptr);
    str = other.str;
  }

  S(S&& other) {
    std::cout << "S(S&&)" << std::endl;
    i_ptr = other.i_ptr;
    other.i_ptr = nullptr;
    str = std::move(other.str);
  }


  S& operator=(const S& other) {
    std::cout << "S&operator=(const S&)" << std::endl;
    if(i_ptr != nullptr) {
      delete i_ptr;
    }
    i_ptr = new int(*other.i_ptr);
    str = other.str;

    return *this;
  }


  S& operator=(S&& other) {
    std::cout << "S&operator=(S&&)" << std::endl;
    if(i_ptr != nullptr) {
      delete i_ptr;
    }
    i_ptr = other.i_ptr;
    other.i_ptr = nullptr;

    // new(&str) std::string(std::move(other.str)); // cleap on str needs to be invoked as well so this is out of the consideration.
    str = std::move(other.str);

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




inline S make_s(int n, int n1) { // __attribute__((always_inline))

    S s = S(n + n1);
    
    *s.i_ptr += n;

    // Copy Elision
    return s;

}
// OR Lambda in the current scope but make sure to be careful with lambdas in terms of optimization and inlining so always keep tabs on the assembly output

/*
struct main__Lamba0011 {

    int* a;

    main__Lambda001()

    int operator()() {
        int i = 0;

        return this->a;
    }


};
*/

#define decl_iso_var(decl_name, T) unsigned char decl_name[sizeof(T)]

#define init_iso_var(decl_name, INIT) new(decl_name) INIT

#define access_iso_var(decl_name) (*(reinterpret_cast<S*>(decl_name)))


namespace CppWrapper {

    template<class T>
    struct StbDs {

        T* arr = NULL;



        StbDs() = default;

        StbDs(const StbDs&) = delete;
        StbDs(StbDs&&) = delete;

        StbDs&operator=(const StbDs&) = delete;
        StbDs&operator=(StbDs&&) = delete;


        ~StbDs() {
            if(arr != NULL) {
                for(size_t i = 0; i < arrlenu(arr); i++) {
                    (arr+i)->~T();
                }

                arrfree(arr);

            }
        }


    };
}

using namespace CppWrapper;

int main() {


    std::cout << "START: Block5" << std::endl;
    {
        S* arr = NULL;

        // std::cout << stbds_header(arr)->length << std::endl;

        // arrpush
        // arrput(arr, S(0));
        arrpush_new_place(arr, S(10));



        for(size_t i = 0; i < arrlenu(arr); i++) {
            (arr+i)->~S();
        }

        arrfree(arr);
    }
    std::cout << "END: Block5" << std::endl;



    std::cout << "START: Block6" << std::endl;
    {
        StbDs<S> arr;

        // arrpush(arr.arr, S(10));
        arrpush_new_place(arr.arr, S(10));
        arrpush_new_place(arr.arr, S(10));

        assert(*((arr.arr+0)->i_ptr) == 10);

    }
    std::cout << "END: Block6" << std::endl;


    // TODO: FIXME: THE REALLOCATION: STBDS_REALLOC
    std::cout << "START: Block7" << std::endl;
    {
        StbDs<S> arr;

        const int _range = 5;

        // arrsetcap(arr.arr, 10); // WORKS

        std::cout << "stbds_arrcap(arr.arr) " << stbds_arrcap(arr.arr) << std::endl;

        for(int i = 0; i < _range; i++) {
          arrpush_new_place(arr.arr, S(i));
        }

        std::cout << "stbds_arrcap(arr.arr) " << stbds_arrcap(arr.arr) << std::endl;

        for(int i = 0; i < _range; i++) {
          std::cout << (arr.arr+i) << std::endl;
          std::cout << "i_ptr: "  << *((arr.arr+i)->i_ptr) << std::endl;
          assert(*((arr.arr+i)->i_ptr) == i);
          assert((arr.arr+i)->str == std::to_string(reinterpret_cast<uintptr_t>((arr.arr+i)->i_ptr)));
        }

    }
    std::cout << "END: Block7" << std::endl;


    return 0;
}
