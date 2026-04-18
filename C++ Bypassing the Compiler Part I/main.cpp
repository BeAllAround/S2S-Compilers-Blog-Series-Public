#include <iostream>

#include <cassert>

#include <stdint.h> // uintptr_t

// -O0 -Wall --std=c++03

#define nullptr NULL


class S;

class S {
  public:
    // int* i_ptr = nullptr; // warning: non-static data member initializers only available with '-std=c++11' or '-std=gnu++11' [-Wc++11-extensions]
    int* i_ptr;
  
  S() {
    i_ptr = nullptr;
    std::cout << "S()" << std::endl;
  }

  S(int i) {
    std::cout << "S(int)" << this << std::endl;
    i_ptr = new int(i);
  }

/*
  S(const S& other) {
    std::cout << "S(const S&)" << std::endl;
    i_ptr = new int(*other.i_ptr);
  }


  S(S& other) {
    std::cout << "S(S&&)" << std::endl;
    i_ptr = other.i_ptr;
    other.i_ptr = nullptr;
  }
*/

/*
  S& operator=(const S& other) {
    std::cout << "S&operator=(const S&)" << std::endl;
    if(i_ptr != nullptr) {
      delete i_ptr;
    }
    i_ptr = new int(*other.i_ptr);
    return *this;
  }
 */

/*
  uintptr_t get_p() const {
    return reinterpret_cast<uintptr_t>(this);
  }
  */


/*
  S& operator=(S& other) {
    std::cout << "S&operator=(S&&)" << std::endl;
    if(i_ptr != nullptr) {
      delete i_ptr;
    }
    i_ptr = other.i_ptr;
    other.i_ptr = nullptr;
    return *this;
  }
  */

  S& move(const S& other) {
    S* s = reinterpret_cast<S*>(
            reinterpret_cast<uintptr_t>(&other)
    );

    std::cout << "S& move(const S&)" << s << std::endl;
    if(i_ptr != nullptr) {
      delete i_ptr;
    }
    i_ptr = s->i_ptr;
    s->i_ptr = nullptr;
    return *this; 
  }
  

  S& move(S& other) {
    std::cout << "S& move(S&)" << std::endl;
    if(i_ptr != nullptr) {
      delete i_ptr;
    }
    i_ptr = other.i_ptr;
    other.i_ptr = nullptr;
    return *this; 
  }

  S& move(S* other) {
    std::cout << "S& move(S*)" << other << std::endl;
    if(i_ptr != nullptr) { // FIXME: ONLY NEEDED FOR MOVE ASSIGNMENT. NOT THE MOVE CONSTRUCTOR. TODO: REFACTOR
      delete i_ptr;
    }
    i_ptr = other->i_ptr;
    other->i_ptr = nullptr;
    return *this; 
  }

  S& copy(S& other) {
    std::cout << "S&copy(S&)" << std::endl;
    if(i_ptr != nullptr) { // FIXME: ONLY NEEDED FOR COPY ASSIGNMENT. NOT THE MOVE CONSTRUCTOR. TODO: REFACTOR
      delete i_ptr;
    }
    i_ptr = new int(*other.i_ptr);
    return *this;
  }

  ~S() {
    std::cout << "~S()" << this << std::endl;

    if(i_ptr != nullptr) {
      delete i_ptr;
      i_ptr = nullptr;
    }

  }
};



int main() {
    S s = 1;

    std::cout << s.i_ptr << std::endl;

    S s1;

    s1.move(s);

    std::cout << s1.i_ptr << std::endl;

    std::cout << s.i_ptr << std::endl;



    S s2;

    S s_move = S(10);


    /*
    const S& s3 = s_move;
    S* dd = reinterpret_cast<S*>(
            reinterpret_cast<uintptr_t>(
                &s3
            )
    );
    */

    // In order to accomplish this, we need to pass it as a pointer
    
    // s2.move(&s_move);
   s2.move(S(10));
   // s2 = S(10); // move
   // s2 = s_move; // copy
   s2.copy(s_move);

/*
    S s2;
    S s_xvaluetemporary = S(10);
    s2.move(s_xvaluetemmpory); cleanup(s_xvaluetempory);
*/
   // s2.move(s_move);

    // s2.move(s_move);

    // assert(s_move.i_ptr == nullptr);


    return 0;
}
