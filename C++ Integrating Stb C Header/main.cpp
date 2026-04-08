#include <iostream>

#include <vector>

#include <cassert>


#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>


#include <cstdint>



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
    i_ptr = new int(*other.i_ptr);
    str = other.str;

    return *this;
  }


  S& operator=(S&& other) {
    std::cout << "S&operator=(S&&)" << std::endl;
    i_ptr = other.i_ptr;
    other.i_ptr = nullptr;

    // new(&str) std::string(std::move(other.str)); // cleap on str needs to be invoked as well so this is out of the consideration.
    // TODO FIXME: PROBLEMS FOR STB DS
    // str = std::move(other.str);
    // str = other.str;

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

    std::cout << "START: Block1" << std::endl;
    {
        decl_iso_var(s1, S);

        volatile int n = 5;
        volatile int n1 = 5;

        S s = S(n + n1);

        *s.i_ptr += n;

        init_iso_var(s1, S(std::move(s)));

        assert(*access_iso_var(s1).i_ptr == 15);
        /*
            S(int)
            S(S&&)
            ~S()
            ~S()
        */

        access_iso_var(s1).~S();

    }
    std::cout << "END: Block1" << std::endl;
    

    std::cout << "START: Block2" << std::endl;
    {

        decl_iso_var(s1, S);

        volatile int n = 5;
        volatile int n1 = 5;

        init_iso_var(s1, S(make_s(n, n1)));
        // new(&s1[0]) S(make_s(n, n1));
        // Semantically same s 
        // new(&s1[0]) S(S(n)); 

        assert(*access_iso_var(s1).i_ptr == 15);
        /*
            S(int)
            ~S()
        */

        access_iso_var(s1).~S();

    }
    std::cout << "END: Block2" << std::endl;


    std::cout << "START: Block3" << std::endl;
    {

        decl_iso_var(s1, S);

        init_iso_var(s1, S(0));
        // s1;

        std::vector<S> v;


        v.push_back(std::move(access_iso_var(s1)));
        // new(buffer+item_count++) S(std::move(s));
    }
    std::cout << "END: Block3" << std::endl;


    std::cout << "START: Block4" << std::endl;
    {
        StaticStorage<S, 3> _list;

        volatile int n = 1;
        volatile int n1 = 10;

        /*
        S s = S(4);

        *s.i_ptr += n + n1;
        */



        _storage_add(_list, S(make_s(n, n1)), S);

        assert(*_list.at(0).i_ptr == 12);


        // _list.add(S(0)); // rvalue


    }
    std::cout << "END: Block4" << std::endl;



    std::cout << "START: Block5" << std::endl;
    {
        S* arr = NULL;

        // arrpush
        arrput(arr, S(0));


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

        assert(*((arr.arr+0)->i_ptr) == 10);

    }
    std::cout << "END: Block6" << std::endl;


    // TODO: FIXME: THE REALLOCATION: STBDS_REALLOC
    std::cout << "START: Block7" << std::endl;
    {
        StbDs<S> arr;

        const int _range = 5;

        // arrsetcap(arr.arr, 1);
        std::cout << "stbds_arrcap(arr.arr) " << stbds_arrcap(arr.arr) << std::endl;

        for(int i = 0; i < _range; i++) {
          arrpush_new_place(arr.arr, S(i));
        }

        std::cout << "stbds_arrcap(arr.arr) " << stbds_arrcap(arr.arr) << std::endl;
        // NOTE: C REALLOC SOMEHOW JUST WORKS WITH I_PTR MEMBER ONLY - MOST LIKELY DUE TO THE IMPLEMENTATION OF REALLOC ON THE OS LEVEL
        for(int i = 0; i < _range; i++) {
          std::cout << (arr.arr+i) << std::endl;
          std::cout << *((arr.arr+i)->i_ptr) << std::endl;
          assert(*((arr.arr+i)->i_ptr) == i);
        }

    }
    std::cout << "END: Block7" << std::endl;


    return 0;
}
