#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <assert.h>


// Carve out the memory on stack
// Note that this is local to the function!
static unsigned char obj_stack[100][
                                sizeof(const char*) 
                                + sizeof(int)
                                // + padding + alignment
                                ];

static size_t obj_c = 0;

struct Obj {
    int i;
    /*
    int ii;
    int iii;
    int iii1;
    int iii2;
    */
    const char* str;
};

void create_obj() {
    volatile int ii = 1;

    struct Obj obj = {};

    obj.i = ii; // int i = 1;
    obj.str = "AAA"; // const char* str = "AAA";

    // It is important to note that the compiler will optimize the code without the struct obj referenced so there will be no sub, etc.
    printf("create_obj() %p\n", &obj);
    //return obj;
}

struct Obj return_obj() {
    // NOTE IF WE ARE RETURNING THE OBJECT FROM THE FUNCTION SCOPE. THE SUB AND STACK ISN'T ALLOCATED HERE
    // IT IS ALLOCATED IN THE FUNCTION WHERE IT IS CALLED FROM. See struct "struct Obj o1 = return_obj()" below and THE SUB FOR THE MAIN FUNCTION
    volatile int ii = 1;

    struct Obj obj = {};

    obj.i = ii; // int i = 1;
    obj.str = "AAA"; // const char* str = "AAA";

    // printf("create_obj() %p\n", &obj);

    return obj;
    //return obj;
}

unsigned char* allocate_next_block() { // Get the next memory block
/*
    if(obj_c == _ALLOC_STACK_MAX) {
        assert(0);
    }
    */
    
    return obj_stack[obj_c++];

}

void restore_block() {
    if(obj_c != 0) {
        obj_c--;
    }
}

unsigned char* makeObj(int ii, const char* str) {
/*
// NOTE: SHOULD BE LOCAL TO THE FUNCTION FOR THE ACCURATE IMPLEMENTATION AND SUB LOCAL RESTORE
// WHERE THE NUMBER_OF_STRUCT_INSTANCES REPRESENTS HOW MANY STRUCT INSTANCES WE HAVE IN THE LOCAL FUNCTION
// FOR EXAMPLE:
// struct Obj obj;
// struct Obj obj1;
// HOWEVER, THE PROBLEM HERE IS, AS SHOWN IN THE VIDEO, THAT WE CAN'T RETURN THIS AS IT IS A LOCAL VARIABLE ADDRESS.
unsigned char obj_stack[NUMBER_OF_STRUCT_INSTANCES][
                                sizeof(const char*) 
                                + sizeof(int)
                                // + padding + alignment
                                ];

 */
    unsigned char* object = allocate_next_block();

    /*
    struct Obj {
        int i;
        const char* str;

    };
    */
    // obj.i = ii
    *((int*)(object+0)) = ii;

    // printf("object.i %d\n", *((int*)(object+0)) );

    // Note: Having all sorts of asserts for correctness as this work is very fragile
    assert(*((int*)object+0) == ii);

    // obj.str = "AAB";
    *((const char**)(object+sizeof(int))) = str;

    assert(strcmp(*((const char**)(object+sizeof(int))), str) == 0); // warning: comparison with string literal results in unspecified behavior [-Waddress]

    return object;
}

/*
unsigned char* _makeObj() {
    unsigned char object[sizeof(int) + sizeof(const char*) + 4]; // sizeof members in order plus the padding, which I have no clue how to calculate at the moment.

    int ii = 132;

    // NOTE: Cast is based on: ((type)*)
    // obj.i = ii;
    // https://en.wikipedia.org/wiki/Type_punning
    *((int*)(object+0)) = ii;

    printf("object.i %d\n", *((int*)(object+0)) );

    assert(*((int*)object+0) == ii);

    // obj.str = "AAB";
    *((const char**)(object+sizeof(int))) = "AAB";


     return object; // BUSTED IN UB!

}
*/




int main() {
  /*
  Low level assembly written in C - assembly in C line-for-line interpretation
  */

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
            /*
            int ii; // enable this to make a point about the main function and struct allocation
            int ii1;
            int ii2;
            int ii3;
            */
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

       // assert(obj.i == ii);
       printf("obj %p\n", &obj);

  }

  create_obj();
  create_obj();


  // Function SCOPE is now:
  // "main":
  //      push    rbp     #
  //      mov     rbp, rsp  #,
  //      sub     rsp, 144  #,
  struct Obj o1 = return_obj();
  struct Obj o2 = return_obj();
  // struct Obj o3 = return_obj();
  // return_obj(); // WITH NO VARIABLE SIGNATURE, THE COMPILER WON'T INCREASE THE SUB AS THIS IS NOT LOOKED AT STORING THE MEMORY BLOCK



  {
    unsigned char* object = makeObj(20, "AAA"); // By its type signature, it looks like it is memory on heap but it is NOT!

    printf("object.i %d\n", *((int*)(object+0)) );

    printf("object.str %s\n", *((const char**)(object+sizeof(int))) );

    printf("object ptr %p\n", object);

    // We can now customize it and release at the end of the scope
    // as oppposed to the function exit
    restore_block();
  }

  {
    unsigned char* object = makeObj(30, "BBB"); // By its type signature, it looks like it is memory on heap but it is NOT!

    printf("object.i %d\n", *((int*)(object+0)) );

    printf("object.str %s\n", *((const char**)(object+sizeof(int))) );

    printf("object ptr %p\n", object);
  }


  for(int i = 0; i < 10; i++) {
    unsigned char* object = makeObj(20, "AAA"); // By its type signature, it looks like it is memory on heap but it is NOT!

    printf("object ptr %p\n", object);
    restore_block();
  }


  return 0;
}
