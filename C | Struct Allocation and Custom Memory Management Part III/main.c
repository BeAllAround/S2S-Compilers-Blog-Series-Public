#include <stdio.h>
#include <stdlib.h>
#include <string.h>



struct A {
    int i;
    const char* name;
    /*
    const char* name01;
    const char* name02;
    const char* name03;
    const char* name04;
    const char* name05;
    const char* name06;
    */
};


void A(struct A* this, int i, const char* name) {
    this->i = i;
    this->name = name;
}

int main() {


    // struct A a;

    // A(&a, 10, "Alex");
    // a.i = 10;
    // a.name = "Alex";

    // register struct A* a_m = (struct A*)malloc(sizeof(struct A)); // NOTE: Stored into the register; push rbp

    // struct A* a_m = (struct A*)malloc(sizeof(struct A));
    // struct A* a_m_1 = (struct A*)malloc(sizeof(struct A));
    // struct A* a_m_2 = (struct A*)malloc(sizeof(struct A));

    // A(a_m, 10, "Alex");

    // *a_m = a;

    // printf("struct A* a_m %p\n", a_m);
    // printf("struct A* a_m %p\n", a_m_1);
    // printf("struct A* a_m %p\n", a_m_2);



    return 0;
}
