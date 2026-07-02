#include <iostream>


#include <memory>


#define start_time clock_t s_t_a_r_t = clock();

#define end_time printf("[Cpu_time_used: %f]\n", static_cast<double>(clock() - s_t_a_r_t) / CLOCKS_PER_SEC);

#define _SIZE 1000000

// sudo perf stat ./out.out

/*
 * g++ main.cpp -o out.out -O0 && ./out.out
 * [Cpu_time_used: 0.083864]
 * [Cpu_time_used: 0.125482]

 // NOTE: unique_ptr IS MUCH MORE OPTIMIZIED BY THE COMPILER
 * g++ main.cpp -o out.out -O1 && ./out.out
 * [Cpu_time_used: 0.000565]
 * [Cpu_time_used: 0.031977]
 */



class Base {
  public:
    int* i;
    Base() {
      i = new int(1);
    }


    // virtual ~Base(); for unique_ptr
    ~Base() {
      delete i;
    }
};


class Derived : public Base {
  public:
    int* ii;

    Derived() : Base() {
      ii = new int(1);
    }



    ~Derived() {
      delete ii;
    }

};

int main() {


  {
    // NOT OK
    // std::unique_ptr<Base> p = std::make_unique<Derived>(); // deleter is not passed around.
    //
    //
    // std::shared_ptr<Base> p = std::shared_ptr<Derived>(); // since the deleter is passed around.

  }


  // TALK ABOUT MOVE, COPY CONSTRUCTORS/ASSIGNMENTS

  /*

  {
    start_time;

    std::cout << sizeof(int*) << std::endl;
    std::cout << sizeof(std::unique_ptr<int>) << std::endl;
    std::cout << sizeof(std::unique_ptr<char>) << std::endl;

    for(volatile size_t i = 0; i < _SIZE; i++) {
      std::unique_ptr<int> p = std::make_unique<int>(i);

      // volatile int ii = *p;
      // std::cout << p.get() << std::endl;

    }

    // total heap usage: 1,000,002 allocs, 1,000,002 frees, 4,074,752 bytes allocated
    

    end_time;
  }


  {
    start_time;

    std::cout << sizeof(std::shared_ptr<int>) << std::endl;
    std::cout << sizeof(std::shared_ptr<char>) << std::endl;

    for(volatile size_t i = 0; i < _SIZE; i++) {
      std::shared_ptr<int> p = std::make_shared<int>(i);

    }
    // total heap usage: 1,000,002 allocs, 1,000,002 frees, 24,074,752 bytes allocated


    end_time;
  }
  */



  return 0;
}
