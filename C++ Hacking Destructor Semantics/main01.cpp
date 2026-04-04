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




class Ss {
    public:
    S s;
    Ss() {
        std::cout << "Ss()" << std::endl;
    }

    // NOTE: ANYTHING IN THE CONSTRUCTOR AVOIDS INVOKING S() of the member s;
    // SAME GOES FOR Ss(S&& other) below
    Ss(int) : s{S()} {
        std::cout << "Ss(int)" << std::endl;
    }

    Ss(const char*) {
        std::cout << "Ss(const char*)" << std::endl;
        new(&s) S(); // ILL-ADVISED. SEE BELOW
    }


    Ss(bool) {
        std::cout << "Ss(bool)" << std::endl;
        s = S(); // MOVE ASSIGNMENT
    }


    Ss(S&& other) : s{std::move(other)} {
        std::cout << "Ss(S&&)" << std::endl;
    }


    ~Ss() {
        std::cout << "~Ss()" << std::endl;
    }

};



int main() {


    std::cout << "START: s1" << std::endl;
    {
        Ss s1;
    }
    std::cout << "END: s1" << std::endl;



    std::cout << "START: s2" << std::endl;
    {
        Ss s2 (1);
    }
    std::cout << "END: s2" << std::endl;


    std::cout << "START: s3" << std::endl;
    {
        Ss s3 ("AAAA");
    }
    std::cout << "END: s3" << std::endl;


    std::cout << "START: s4" << std::endl;
    {
        Ss s4 (true);
    }
    std::cout << "END: s4" << std::endl;


    std::cout << "START: s5" << std::endl;
    {
        Ss s5 = S();
    }
    std::cout << "END: s5" << std::endl;



    {
        std::vector<int> v;
        v.push_back(1);
        v.push_back(2);

        // NOTE: LEAK AS COPY/MOVE ASSIGNMENT CLEANS UP THE PREVIOUS INSTANCE STATE. e.g. v = std::vector<int>();
        // new(&v) std::vector<int>();
    }




    return 0;
}

