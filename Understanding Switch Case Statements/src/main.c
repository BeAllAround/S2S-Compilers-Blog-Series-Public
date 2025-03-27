#include <stdio.h>
#include <time.h>

#define start_time clock_t s_t_a_r_t = clock();
#define end_time printf("[CPU Time Used: %f]\n", (double)(clock() -          s_t_a_r_t) / CLOCKS_PER_SEC);

/*
void __pause() {
  char s[2];
  printf("PAUSED");
  if(fgets(s, sizeof s, stdin)) {}
}
*/

enum N {
    N0,
    N1,
    N2,
    N3,
    N4,
    N5,
    N6,
    N7,
    N8,
    N9,
    N10,
};

void _switch_enum_run(enum N type) {

  volatile int r; // NOTE: preventing compiler from optimizing it out

  switch(type) {
    case N0: {
      r = 0;
      break;
    }
    case N1: {
      r = 1;
      break;
    }
    case N2: {
      r = 2;
      break;
    }
    case N3: {
      r = 3;
      break;
    }
    case N4: {
      r = 4;
      break;
    }
    case N5: {
      r = 5;
      break;
    }
    case N6: {
      r = 6;
      break;
    }
    case N7: {
      r = 7;
      break;
    }
    case N8: {
      r = 8;
      break;
    }
    case N9: {
      r = 9;
      break;
    }
    case N10: {
      r = 10;
      break;
    }
    default: {
      r = 11;
      break;
    }
  }

}


void _switch_run(unsigned int);

inline void _switch_run(unsigned int type) {

  volatile int r; // NOTE: preventing compiler from optimizing it out

  switch(type) {
    case 0: {
      r = 0;
      break;
    }
    case 1: {
      r = 1;
      break;
    }
    case 2: {
      r = 2;
      break;
    }
    case 3: {
      r = 3;
      break;
    }
    case 4: {
      r = 4;
      break;
    }
    case 5: {
      r = 5;
      break;
    }
    case 6: {
      r = 6;
      break;
    }
    case 7: {
      r = 7;
      break;
    }
    case 8: {
      r = 8;
      break;
    }
    case 9: {
      r = 9;
      break;
    }
    case 10: {
      r = 10;
      break;
    }
    default: {
      r = 11;
      break;
    }
  }

}



void _if_run(unsigned int type) {

  volatile int r; // NOTE: preventing compiler from optimizing it out

  if(type == 0) {
    r = 0;
  } else if(type == 1) {
    r = 1;
  } else if(type == 2) {
    r = 2;
  } else if(type == 3) {
    r = 3;
  } else if(type == 4) {
    r = 4;
  } else if(type == 5) {
    r = 5;
  } else if(type == 6) {
    r = 6;
  } else if(type == 7) {
    r = 7;
  } else if(type == 8) {
    r = 8;
  } else if(type == 9) {
    r = 9;
  } else if(type == 10) {
    r = 10;
  } else {
    r = 11;
  }

}

 /*
} else if(type == 1000) {
r = 11;
 */


static inline void _switch_go(unsigned int v) {
  // __label__ l00, l01, l02, l03, l04, l05, l06, l07, l08, l09, l10, _default, defer;

  // As mentioned before, we keep the array preserved between function invocations.
  static void* cases[] = {
    &&l00,
    &&l01, 
    &&l02,
    &&l03,
    &&l04,
    &&l05,
    &&l06,
    &&l07,
    &&l08,
    &&l09,
    &&l10,
  };


  // CMP For the default case - also implemented by the switch case
  if(v > 10) {
    goto _default;
  }

  goto *cases[v];

  volatile int r;

  l00: {
       r = 0;
       goto defer; // break
     }
  l01: {
       r = 1;
       goto defer; // break
     }
  l02: {
       r = 2;
       goto defer; // break
     }
  l03: {
         r = 3;
         goto defer; // break
       }
  l04: {
         r = 4;
         goto defer; // break
       }
  l05: {
         r = 5;
         goto defer;
       }
  l06: {
         r = 6;
         goto defer;
       }
  l07: {
         r = 7;
         goto defer;
       }
  l08: {
         r = 8;
         goto defer;
       }
  l09: {
         r = 9;
         goto defer;
       }
  l10: {
         r = 10;
         goto defer;
       }


  _default: {
       r = 11;
       goto defer; // break
  }

  // For us, defer serves as the alternative to where the "break" keyword jumps to as far as the switch case and jump table
  defer: {
         return;
       }
}

/*
enum VV {
  VV01,
  VV02,
};
*/

#define VV00 0
#define VV01 1
#define VV02 2
// ...
#define VV10 10


typedef void (*Handler)(volatile int*);

void func3 (volatile int* r) { *r = 3; }
void func2 (volatile int* r) { *r = 2; }
void func1 (volatile int* r) { *r = 1; }
void func0 (volatile int* r) { *r = 0; }


void func_pointer_jump_table(unsigned int type) {

  static Handler jump_table[4] = { func0, func1, func2, func3 };

  // type = type % 4;

  if(type > 3) {
    // ... Default case
    return;
  }

  volatile int r;

  jump_table[type](&r);

}


void _switch_unpacked(unsigned int type) {
  volatile int r; // NOTE: preventing compiler from optimizing it out

  switch(type) {
    case 0: {
      r = 0;
      break;
    }
    case 1: {
      r = 1;
      break;
    }
    case 2: {
      r = 2;
      break;
    }
    case 3: {
        r = 3;
        break;
    }
    case 4: {
        r = 4;
        break;
    }
    case 5: {
        r = 5;
        break;
    }
    case 100: {
        r = 100;
        break;
    }
    case 101: {
        r = 101;
        break;
    }
    case 102: {
        r = 102;
        break;
    }
    case 103: {
        r = 103;
        break;
    }
    case 104: {
        r = 104;
        break;
    }
    case 105: {
        r = 105;
        break;
    }
    default: {
        r = 11;
        break;
    }
  }
}

void _switch_test(unsigned int type) {
  volatile int r; // NOTE: preventing compiler from optimizing it out

  switch(type) {
    case 0: {
      r = 0;
      break;
    }
    case 1: {
      r = 1;
      break;
    }
    case 2: {
      r = 2;
      break;
    }
    case 3: {
        r = 3;
        break;
    }
    default: {
        r = 11;
        break;
    }
  }
}

#define __switch_go(type) { \
    __label__ l00, l01, l02, _default, defer; \
    static void* jtable[] = {&&l00, &&l01, &&l02}; \
    if((unsigned int)type > 2) goto _default; \
    goto* jtable[type]; \
    volatile int r; \
    l00: { \
        r = 0; \
        goto defer; \
    } \
    l01: { \
        r = 1; \
        goto defer; \
    } \
    l02: { \
        r = 2; \
        goto defer; \
    } \
    _default: { \
        r = 11; \
        goto defer; \
    } \
    defer: { \
    } \
    }

/*
#define switch_go(type) ({ \
    __label__ l01, l02, l03, _default, defer; \
    static void* jtable[] = { &&l01, &&l02, &&l03 }; \
    volatile int r; \
    if((unsigned int)type > 2) goto _default; \
    goto *jtable[type]; \
    l01: {\
       r = 1; \
       goto defer; \
    } \
    l02: {\
      r = 2; \
      goto defer; \
    } \
    l03: {\
     r = 3; \
     goto defer; \
    } \
    _default: { \
      r = 11; \
      goto defer; \
    } \
    defer: { \
    } \
    r; \
})


int main() {
    volatile unsigned int value = 10;

    printf("%d\n", switch_go(20));

    printf("%d\n", switch_go(2));

    printf("%d\n", switch_go(0));

    printf("%d\n", switch_go(1));

    printf("%d\n", switch_go(1));

    printf("%d\n", switch_go(value));

    return 0;
}
*/

int main() {

  volatile unsigned int value = 10;
  volatile unsigned int value1 = 11;
  volatile unsigned int value2 = 0;
  volatile unsigned int value3 = 2;

  {
    printf("_switch_enum\n");
    for(int i = 0; i < 10; i++) {
      start_time;

      for(int i = 0; i < 1000000; i++) {
        _switch_enum_run(N10);
      }

      end_time;
    }
    printf("_switch_enum\n");
  }


  {
    printf("_switch_run\n");
    for(int i = 0; i < 10; i++) {
      start_time;
    
      for(int i = 0; i < 1000000; i++) {
        _switch_run(value);
      }

      end_time;
    }
    // _switch_run(value);
    // _switch_run(value);
    // _switch_run(value);
    printf("_switch_run\n");
  }

  {
    printf("_if_run\n");
    for(int i = 0; i < 10; i++) {
      start_time;

      for(int i = 0; i < 1000000; i++) {
        _if_run(value);
      }

      end_time;
    }
    printf("_if_run\n");
  }

  {
    printf("_switch_go\n");
    for(int i = 0; i < 10; i++) {
      start_time;

      for(int i = 0; i < 1000000; i++) {
        _switch_go(value);
      }

      end_time;
    }
    printf("_switch_go\n");
  }

  {
    printf("__switch_go\n");
    for(int i = 0; i < 10; i++) {
      start_time;

      for(int i = 0; i < 1000000; i++) {
        __switch_go(value3);
      }

      end_time;
    }
    // __switch_go(value3);
    // __switch_go(value3);

    printf("__switch_go\n");
  }

  {
    printf("func_pointer_jump_table\n");
    for(int i = 0; i < 10; i++) {
      start_time;

      for(int i = 0; i < 1000000; i++) {
        func_pointer_jump_table(0);
      }

      end_time;
    }
    printf("func_pointer_jump_table\n");
  }


  return 0;
}
