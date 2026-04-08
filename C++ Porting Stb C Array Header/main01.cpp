#include <iostream>

#include <cassert>

#include <cstdint>


struct _header {
    size_t length;

    _header() {
      std::cout << "_header()" << std::endl;
    }
};



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
    str = std::move(other.str);
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


int main02() {
  return 0;
}

int main01() {
  std::cout << sizeof(unsigned char) << std::endl;
  std::cout << sizeof(char) << std::endl;

    char* ptr = reinterpret_cast<char*>(new char[sizeof(_header) + sizeof(S)]);

    S s1 = S(13);

    new(ptr + 0* sizeof(_header)) _header();

    reinterpret_cast<_header*>(ptr+ 0 *sizeof(_header))->length = 1;

    new(ptr + sizeof(_header) + 0 * sizeof(S)) S(std::move(s1));


    assert(reinterpret_cast<_header*>(ptr+0)->length == 1);


    // TODO: REALLOCATION OF _HEADER AND BUFFER+COPY/MOVE
    //
    //


    S* _ptr = reinterpret_cast<S*>(ptr + sizeof(_header));

    std::cout << *((_ptr+0)->i_ptr) << std::endl;


    reinterpret_cast<S*>(ptr + sizeof(_header))->~S();

    ::operator delete[](ptr);
    // OR, delete[] reinterpret_cast<char*>(ptr);
    // OR, delete[] (reinterpret_cast<_header*>(_ptr) - 1);

    return 0;
}

int main() {
  return main01();
}


