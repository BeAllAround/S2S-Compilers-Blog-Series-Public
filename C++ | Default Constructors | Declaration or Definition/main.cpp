#include <iostream>

#include <vector>

#include <unordered_map>

#include <cassert>


// Lazy Constructor
// UB sensitive with specific methods - make sure to use the safe ones
// With heap-based or mmap or any other custom memory,
// it forces reallocation - that's why is guareetted to work


class MyImplt {
    public:
        int* i {nullptr};

        MyImplt() {
            // i = new int(1);
        }

        // If the container is not rellocation based
        // do something like the following
        void setUp() {
            i = new int(1);
        }
        

};

class A {
    public:
    std::vector<int> v;
    std::unordered_map<int, int> m;

    int i {0};


    A() : i{1} // OK, It is a primitive after all - or you can just do "int i = 1"
    {

    }


};

// static std::vector<int> v;

int main() {
    // A a;

    // a.v = std::vector<int>(); // No allocations for the move assignment either!
    // A a1 = std::move(a); // Same goes for the move construtor!

    
  

    // a.m.reserve(10);

    // a.v.push_back(10);

    // int i0 = *(a.v.data()+0);
    // int i = a.v.front(); // UB!
    // int ii = a.v[1]; // UB!
                     // if(entry != m.end()) return entry->second;
    
    // assert(ii == 0);
    // int i = a.v.at(0);
    // a.v.push_back(1);

    /*
    A a1;

    // Expensive operation
    for(int i = 0; i < 100; i++) {
        A a;
        // v.push_back(std::move(a)); // E.g. moving it out of the scope
        // a.v.reserve(10); // Equivalent to setUp
    }
    */

    return 0;
}

