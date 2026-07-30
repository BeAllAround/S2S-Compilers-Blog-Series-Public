
#include <iostream>

#include <vector>

// #include "flat_hash_map.hpp"

class S {
  public:
  int* i_ptr { nullptr };

  S() {
    // std::cout << "S()" << std::endl;
  }

  S(int i) {
    // std::cout << "S(int)" << std::endl;
    i_ptr = new int(i);
  }

  S(const S& other) {
    // std::cout << "S(const S&)" << std::endl;
    i_ptr = new int(*other.i_ptr);
  }

  // NOTE: std::vector will invoke the copy constructor during reallocation, but only if the element's move constructor is not marked noexcept
  S(S&& other) noexcept 
  {
    // std::cout << "S(S&&)" << std::endl;
    i_ptr = other.i_ptr;
    other.i_ptr = nullptr;
  }


  S& operator=(const S& other) {
    // std::cout << "S&operator=(const S&)" << std::endl;
    if(i_ptr != nullptr) {
      delete i_ptr;
    }
    i_ptr = new int(*other.i_ptr);
    return *this;
  }


  S& operator=(S&& other) {
    // std::cout << "S&operator=(S&&)" << std::endl;
    if(i_ptr != nullptr) {
      delete i_ptr;
    }
    i_ptr = other.i_ptr;
    other.i_ptr = nullptr;
    return *this;
  }

  ~S() {
    // std::cout << "~S()" << std::endl;

    // if(i_ptr != nullptr) { // The delete below already does this check, effectively

    delete i_ptr;
    i_ptr = nullptr;

    // }

  }
};

#define start_time clock_t s_t_a_r_t = clock();

#define end_time printf("[Cpu_time_used: %f]\n", static_cast < double > (clock() - s_t_a_r_t) / CLOCKS_PER_SEC);


// g++ main.cpp -o out.out --std=c++20 -O0 && ./out.out
double benchmark01() {
  double _d = 0;

  size_t _SIZE = 20000;

  S* v1 = (S*)::operator new(sizeof(S) * _SIZE);

  S* v2 = (S*)::operator new(sizeof(S) * _SIZE);

  for(size_t i = 0; i < _SIZE; i++) {
    new(v1+i) S(); // S(i);
  }



  start_time;
  for(size_t i = 0; i < _SIZE; i++) {
    new(v2+i) S(std::move(v1[i]));
    (v1+i)->~S(); // WITH OR WITHOUT
  }


  _d += static_cast < double > (clock() - s_t_a_r_t) / CLOCKS_PER_SEC;


  for(size_t i = 0; i < _SIZE; i++) {
    (v2+i)->~S();

  }

  ::operator delete(v1);
  ::operator delete(v2);


  return _d;

}



// Very interesting guarding technique if anything throws! https://github.com/gcc-mirror/gcc/blob/master/libstdc%2B%2B-v3/include/bits/vector.tcc#L612
int main() {

  /*
  {
    std::vector<S> v;

    v.reserve(10);


    for(size_t i = 0; i < 12; i++) {
      v.push_back(S(i));
      // NOTE: Just like the emplace_back of the ska::flat_hash_map, the emplace_back invokes: the "S(S&&), ~S()" internally as the move operation opon each reallocation cycle
    }

  }
  */

  /*
  {
    ska::flat_hash_map<int, S> h;

    h.reserve(10);
    // The S(S&&), ~S() for each item during the reallocation. See the comments in the flat_hash_map.hpp, void rehash(size_t)

    for(size_t i = 0; i < 20; i++) {
      h[i] = S(i);
    }

  }
  */

  size_t N = 100;

  double _d = 0;

  for(size_t i = 0; i < N; i++) {
    _d += benchmark01();
  }

  std::cout << "TIME ELAPSED: " << _d << std::endl;



  return 0;
}

