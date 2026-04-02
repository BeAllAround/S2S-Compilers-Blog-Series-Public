#include <iostream>

#include <initializer_list>
#include <vector>

#include <cassert>

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
  std::vector<S> _list;


  S(std::initializer_list<S> arr) {
    std::cout << "S(std::initializer_list<S>)" << std::endl;
    // _list = arr;

    for(auto start = arr.begin(); start != arr.end(); start++) {
        _list.push_back(*start);
    }

  }

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


#define _ARGS_GET(arr, inx) (*(((arr).begin()) + inx) )


S make_s() {
    S s ({ S(1), S(2) }); // rvalue

    std::cout << "ASSERT START" << std::endl;

    assert((*(_ARGS_GET(s._list, 0).i_ptr)) == 1);

    std::cout << "ASSERT END" << std::endl;

    return s;
}


void f(std::initializer_list<S> args) {

    /*
    */
    


    return f({S(0), S(1)});

}


void f1(S);
void f1(S, S);
void f1(S, S, S);




int main() {
    std::cout << sizeof(std::initializer_list<int>) << std::endl;
    std::cout << sizeof(std::initializer_list<S>) << std::endl;

    S s = make_s();


    // f({S(1), S(3)});




/*
    std::initializer_list<S> arr = { S(1), S(2), S(3)};

    std::initializer_list<S> arr1 = arr;


    assert(arr.begin() == arr1.begin());

*/

    // *(arr.begin()+0) = S(4);


/*
    StaticStorage<S, 3> arr;


    _storage_add(arr, S(1), S);
    _storage_add(arr, S(2), S);
    _storage_add(arr, S(3), S);

*/



    // s+0, s+2;





    return 0;
}

