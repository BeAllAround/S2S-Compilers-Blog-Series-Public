
#include <iostream>


#include <cassert>

class S1 {
    public:
    S1() {
        std::cout << "S1()" << std::endl;
    }

    S1(const S1&) {
        std::cout << "S1(const S1&)" << std::endl;

    }
    S1(S1&&) {
        std::cout << "S1(S1&&)" << std::endl;
    }


    S1& operator=(S1&&) {
        std::cout << "operator=(S1&&)" << std::endl;
        return *this;
    }

    S1& operator=(const S1&) {
        std::cout << "operator=(const S1&)" << std::endl;
        return *this;
    }


    ~S1() {
        std::cout << "~S1()" << std::endl;
    }

};


class S2 {
    public:
    S2() {
        std::cout << "S2()" << std::endl;
    }

    S2(const S2&) {
        std::cout << "S2(const S2&)" << std::endl;

    }
    S2(S2&&) {
        std::cout << "S2(S2&&)" << std::endl;
    }


    S2& operator=(S2&&) {
        std::cout << "operator=(S2&&)" << std::endl;
        return *this;
    }

    S2& operator=(const S2&) {
        std::cout << "operator=(const S2&)" << std::endl;
        return *this;
    }


    ~S2() {
        std::cout << "~S2()" << std::endl;
    }

};


class MyS3 {
    public:

    size_t _size {0};
    int *ptr {nullptr};
    S1 s1;
    S2 s2;

    MyS3() {

    }

    MyS3(size_t n) : _size(n), ptr{new int[_size]}, s1{S1()}, s2{S2()} {
        assert(ptr != nullptr);
        *ptr = 3;
        assert(*ptr == 3);

        // s1 = S1();
        // s2 = S2();

    }


    ~MyS3() {
        // NOTE: Redundant Check
        // Such checks are redundant because the delete operator performs this check internally before proceeding.
        /*
            * Standard Guarantee:
            * This behavior has been guaranteed since the C++98 standard,
            * and remains consistent in all modern versions, including C++11, C++17, and C++20.
        */
        if(ptr != nullptr) {
            // HOWEVER, WILL BE NEEDED IF CASE WE ARE USING ptr->my_member or similar
            delete[] ptr;
            // ::operator delete (ptr);
            ptr = nullptr;
        }

    }

};


int main() {
    MyS3 s3 = 1;
    
    MyS3 s4;

    return 0;
}
