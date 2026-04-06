#include <iostream>


#include <cassert>

void recursion01(int n) {
    if(n == 1) {
        std::cout << 1 << std::endl;
        return;
    } else if(n == 2) {
        std::cout << 2 << std::endl;
        return;
    }

    volatile int _end = 3; // prevent the compiler from optimizing the loop out

    for(int i = 1; i < _end; i++) {
        recursion01(i);
    }

}

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




inline S make_s(int n, int n1) { // __attribute__((always_inline))

    S s = S(n + n1);

    *s.i_ptr += n;

    // Copy Elision
    return s;

}
// OR Lambda in the current scope but make sure to be careful with lambdas in terms of optimization and inlining so always keep tabs on the assembly output

#define access_var(buffer) (*(reinterpret_cast<S*>(buffer + (0))))

int main() {

    std::cout << "START: Block1" << std::endl;
    {
        unsigned char buffer[sizeof(S)];

        volatile int n = 5;
        volatile int n1 = 5;

        S s = S(n + n1);

        *s.i_ptr += n;

        new(&buffer[0]) S(std::move(s));

        assert(*access_var(buffer).i_ptr == 15);
        /*
            S(int)
            S(S&&)
            ~S()
            ~S()
        */

        access_var(buffer).~S();

    }
    std::cout << "END: Block1" << std::endl;
    

    std::cout << "START: Block2" << std::endl;
    {
        unsigned char buffer[sizeof(S)];

        volatile int n = 5;
        volatile int n1 = 5;

        new(&buffer[0]) S(make_s(n, n1));
        // Semantically same s 
        // new(&buffer[0]) S(S(n)); 

        assert(*access_var(buffer).i_ptr == 15);
        /*
            S(int)
            ~S()
        */

        access_var(buffer).~S();

    }
    std::cout << "END: Block2" << std::endl;


    return 0;
}
