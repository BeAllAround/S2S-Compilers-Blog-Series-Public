#include <iostream>

#include <vector>


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


#define ISOLATION_DECLARATION(decl_type, decl_name, init, union_type)  \
union union_type { \
    decl_type decl_name; \
    ~union_type() __attribute__((always_inline)) {} \
} decl_name = {}; \
new(&decl_name.decl_name) init; \


#define ACCESS_ISO_DECL(decl_name) decl_name.decl_name

template<class T>
inline void iso_destroy(const T& decl_name) __attribute__((always_inline));


/*
  // GOTCHA!
  00117       void
  00118       destroy(pointer __p) { __p->~_Tp(); }

  https://gcc.gnu.org/onlinedocs/gcc-4.6.3/libstdc++/api/a00958_source.html

*/
template<class T>
inline void iso_destroy(const T& decl_name) {
  decl_name.~T();
}

#define DESTROY_ISO(decl_name) iso_destroy(decl_name.decl_name)



// Safe-checked by valgrind -s ./out.out

int main() {
    {
        ISOLATION_DECLARATION(S, s, S(1), Ss);

        std::cout << ACCESS_ISO_DECL(s).i_ptr << std::endl;;
        
        assert(*ACCESS_ISO_DECL(s).i_ptr == 1);

        // WHEN ACQUIRED BY STANDARD C++, IT BEHAVES AS BUILT-IN RAII BY DEFAULT
        S s1 = std::move(ACCESS_ISO_DECL(s));

        // MOVED
        assert(*s1.i_ptr == 1);


        // s1 implicitly destroyed
    }

    {

        ISOLATION_DECLARATION(S, s, S(1), Ss);

        std::cout << ACCESS_ISO_DECL(s).i_ptr << std::endl;;
        
        assert(*ACCESS_ISO_DECL(s).i_ptr == 1);


        DESTROY_ISO(s);

    }


    {
        ISOLATION_DECLARATION(std::vector<int>, v, std::vector<int>(), Vv);

        ACCESS_ISO_DECL(v).push_back(1);
        ACCESS_ISO_DECL(v).push_back(2);

        
        assert(ACCESS_ISO_DECL(v).size() == 2);
        assert(ACCESS_ISO_DECL(v).at(0) == 1);
        assert(ACCESS_ISO_DECL(v).at(1) == 2);

        // WHEN ACQUIRED BY STANDARD C++, IT BEHAVES AS BUILT-IN RAII BY DEFAULT
        std::vector<int> v1 = std::move(ACCESS_ISO_DECL(v));

        // MOVED
        assert(v1.size() == 2);

    }


    {
        ISOLATION_DECLARATION(std::vector<int>, v, std::vector<int>(), Vv);

        ACCESS_ISO_DECL(v).push_back(1);
        ACCESS_ISO_DECL(v).push_back(2);

        
        assert(ACCESS_ISO_DECL(v).size() == 2);
        assert(ACCESS_ISO_DECL(v).at(0) == 1);
        assert(ACCESS_ISO_DECL(v).at(1) == 2);

        DESTROY_ISO(v);



    }

    return 0;
}
