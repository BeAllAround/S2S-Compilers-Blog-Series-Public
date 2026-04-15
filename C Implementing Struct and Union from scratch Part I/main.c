#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <assert.h>


int main() {

  {
       /*
       union Obj {
       int i;
       const char* str;
       };
       */
    unsigned char object[sizeof(const char*)]; // Takes the sizeof of the larget member

    /*
       typeof(typeof(int)*) a; // int*
       int ii = 132;
       a = &ii;
    */

    int ii = 132;

    // NOTE: Cast is based on: ((type)*)
    *((int*)(object+0)) = ii;

    printf("object.i %d\n", *((int*)(object+0)) );
    assert(*((int*)object+0) == ii);

    *((const char**)(object+0)) = "AAB"; // str member now active

    printf("object.str %s\n", *((const char**)(object+0)) );
    // assert(*((const char**)object+0) == "AAB"); // warning: comparison with string literal results in unspecified behavior [-Waddress]
    assert(strcmp(*((const char**)object+0), "AAB") == 0); // warning: comparison with string literal results in unspecified behavior [-Waddress]

    // printf("a %d\n", *a );
  }



  {
       struct Obj {
       const char* str;
       int i;
       };

      assert(sizeof(int) + sizeof(const char*) + 4 == sizeof(struct Obj));

      printf("%ld\n", sizeof(int) + sizeof(const char*) + 4);

      // You cannot reliably calculate padding manually like this. 
      /*
Padding is implementation-defined, meaning it depends on:

architecture (32-bit vs 64-bit)
compiler
ABI alignment rules

For example:

On a 64-bit system:
int = 4 bytes
pointer = 8 bytes
struct will likely be 16 bytes, not 4 + 8 + 4 = 16 by coincidence, but because of alignment
On another system, this could be different
      */
    unsigned char object[sizeof(int) + sizeof(const char*) + 4]; // sizeof members in order plus the padding, which I have no clue how to calculate at the moment.

    /*
       typeof(typeof(int)*) a; // int*
       int ii = 132;
       a = &ii;
    */

    int ii = 132;

    // NOTE: Cast is based on: ((type)*)
    // obj.i = ii;
    // https://en.wikipedia.org/wiki/Type_punning
    *((int*)(object+0)) = ii;

    printf("object.i %d\n", *((int*)(object+0)) );

    assert(*((int*)object+0) == ii);

    // obj.str = "AAB";
    *((const char**)(object+sizeof(int))) = "AAB";
    /*
        mov     DWORD PTR [rbp-8], 132
        lea     rax, [rbp-32]
        mov     edx, DWORD PTR [rbp-8] # obj.i = ii
        mov     DWORD PTR [rax], edx
        lea     rax, [rbp-32]
        add     rax, 4
        mov     QWORD PTR [rax], OFFSET FLAT:.LC3 # obj.str = "AAB"
     */

    printf("object.str %s\n", *((const char**)(object+sizeof(int))) );

    // assert(*((const char**)object+0) == "AAB"); // warning: comparison with string literal results in unspecified behavior [-Waddress]
    assert(strcmp(*((const char**)(object+sizeof(int))), "AAB") == 0); // warning: comparison with string literal results in unspecified behavior [-Waddress]

    printf("object.i %d\n", *((int*)(object+0)) );
    assert(*((int*)object+0) == ii);

    // printf("a %d\n", *a );
  }

  {
       struct Obj {
            int i;
            const char* str;
       };
       volatile int ii = 1;

       struct Obj obj = {};
       
       obj.i = ii;
       obj.str = "AAA";

/*
        mov     DWORD PTR [rbp-36], 1
        mov     QWORD PTR [rbp-64], 0
        mov     QWORD PTR [rbp-56], 0
        mov     eax, DWORD PTR [rbp-36]
        mov     DWORD PTR [rbp-64], eax # obj.i = ii
        mov     QWORD PTR [rbp-56], OFFSET FLAT:.LC8 # obj.str = "AAB"
        mov     edx, DWORD PTR [rbp-64]
        mov     eax, DWORD PTR [rbp-36]
 */

       assert(obj.i == ii);

  }


  return 0;
}
