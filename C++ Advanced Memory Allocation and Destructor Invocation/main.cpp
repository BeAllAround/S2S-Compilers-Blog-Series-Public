#include <iostream>


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
  /*
  T& pop() { // SAME GOES FOR const T&
    stack_count--;
    T& popped = *reinterpret_cast<T*>(buffer + ( stack_count * sizeof(T)));

    // *reinterpret_cast<T*>(buffer + ( stack_count * sizeof(T))) = T();


    return popped;
  }
  */

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

#define _storage_add(storage, item, T) \
    new(&storage.buffer[storage.stack_count * sizeof(T)]) item; \
    storage.stack_count++;


class S {

  public:
    S() : a{new int(0)} {
      std::cout << "S()" << std::endl;
    }

    S(int i) : a{new int(i)} {
      std::cout << "S(int)" << std::endl;
    };

    void foo() {
      std::cout << "foo: " << a << std::endl;
    }

    S(const S&s) {
      *this = s;
    }

    S(S&& s) {
      *this = s;
    }


    S&operator=(const S& s) {
      std::cout << "const ref& operator=" << std::endl;

      if(a != nullptr) {
        delete a;
      }

      a = new int(*s.a);

      return *this;
    }

    S& operator=(S&& s) {
      std::cout << "operator=" << std::endl;

      if(a != nullptr) {
        delete a;
      }

      a = s.a;

      s.a = nullptr;

      return *this;
    }

    ~S() {
      std::cout << "~S()" << std::endl;
      if(a != nullptr) {
        delete a;
        a = nullptr;
      }
    }

  private:
    int* a {nullptr};
};


struct S1 {

    int i1 { 0 };
    int i2 { 0 };

    S1(int i) {
        i1 = i;
        i2 = i;
    }

    S1() {

    }

    ~S1() 
    {
        // delete &i1; // NOW THE DESTRUCTOR CALL/INVOCATION WON'T BE OPTIMIZED OUT AS DETAILED OUT BELOW
        // OR
        // std::cout << i1 << std::endl;
    }

};

void _print() {
    std::cout << 1 << std::endl;
}

int main() {

/*
-O1
.L2:
        mov     DWORD PTR [rdx], 0
        mov     DWORD PTR [rdx+4], 0
        add     rdx, 8
        cmp     rdx, rcx
        jne     .L2
        call    operator delete(void*)
        mov     eax, 0
        add     rsp, 8
        ret
*/
  {
    size_t size = 5;
    // allocate
    S1* s = static_cast<S1*>(::operator new(size * sizeof(S1)));


    // construct
    for(size_t i = 0; i < size; i++) {
      new (s+i) S1(i);
    }

    // s[0].foo();

    // destroy
    S1* start = s;
    S1* end = s+size;
    while(start != end) {
      // destroy
      (start++)->~S1(); // NOTE: -O1 OPTIMIZED OUT AS WELL! SINCE THE DESTRUCTOR DOES NOTHING AND ONCE IT IS INLINED - IT IS NOT CONSIDERED AS SOMETHING THAT NEEDS TO BE EXECUTED
    }

    // deallocate
    ::operator delete(s);
  }


/*
-O3
.L13:
        movd    xmm1, eax
        mov     ecx, DWORD PTR [rsp+12]
        add     eax, 1
        add     rdx, 8
        pshufd  xmm0, xmm1, 0xe0
        movq    QWORD PTR [rdx-8], xmm0
        cmp     ecx, eax
        jg      .L13
.L12:
        call    _print()
        xor     eax, eax
        add     rsp, 72
        ret
*/
  {
    {

        StaticStorage<S1, 5> _list;

        volatile int _s = 5;
        for(int i = 0; i < _s; i++) {
            _storage_add(_list, S1(i), S1);
        }
        // ~StaticStorage() optimized out as the while(start != end) above
    }

    _print(); // ADDED TO DETERMINE THE ~StaticStorage() and if it is called

  }

  return 0;
}
