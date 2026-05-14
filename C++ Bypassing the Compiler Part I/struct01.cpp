
#include <iostream>
#include <cassert>

struct S {
    int i;

/*
    S(int i) {
        std::cout << "S(int) " << this << std::endl;
        this->i = i;
    }
    */
};


void* __s;

void mutate_s(const S& s) {
    std::cout << &s << " s.i " << s.i << std::endl;
    ((S*)(reinterpret_cast<size_t>(&s)))->i = 3;

    __s = ((S*)(reinterpret_cast<size_t>(&s)));

}


int main() {

    // S s = { 1 };
    // S s1 = { 2 };


    // mutate_s(s);
    // mutate_s(s1);


    mutate_s(S(1));

    // std::cout << ((S*)(reinterpret_cast<size_t>(__s)))->i << std::endl;
    assert(((S*)(reinterpret_cast<size_t>(__s)))->i == 3);

    // PRIMITIVES CANNOT BE RELEASED AS THEY STAY IN THE SAME REGISTRY REGION (ON STACK), at the -O3 level optimizations
    /*
        sub     rsp, 24
        lea     rdi, [rsp+12]
        mov     DWORD PTR [rsp+12], 1
        call    "mutate_s(S const&)"
        lea     rdi, [rsp+12]
        mov     DWORD PTR [rsp+12], 1
        call    "mutate_s(S const&)"
        xor     eax, eax
    */
    mutate_s(S(1));

    // std::cout << s.i << std::endl;
    // std::cout << s1.i << std::endl;

    return 0;
}
