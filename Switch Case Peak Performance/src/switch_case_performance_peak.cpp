#include <iostream>

#include <cassert>

#include <ctime>

// g++ switch_case_performance_peak.cpp -o out.out -O1 && ./out.out
// -fjump-tables

#define start_time clock_t s_t_a_r_t = clock();

#define end_time printf("[Cpu_time_used: %f]\n", static_cast<double>(clock() - s_t_a_r_t) / CLOCKS_PER_SEC);


void __pause() {
	char s[2];
	printf("PAUSED\n");
	if(fgets(s, sizeof s, stdin)) {}
}


enum class V {
    NUM,
    STRING,
    ARR,
    DICT,
    FUNC,
};


int _plain_func_return(V) __attribute__ ((noinline));

int _plain_func_return(V v) {
    volatile int i = 0;

    i = 1;

    return i;
}

// inline int _switch_case(V);
// inline int _single_if_condition(V);

int _switch_case_return(V v) {

    volatile int i = 0;
/*
_switch_case_return(V):
        mov     DWORD PTR [rsp-4], 0
        mov     edi, edi
        jmp     [QWORD PTR .L6[0+rdi*8]]
.L6:
        .quad   .L10
        .quad   .L9
        .quad   .L8
        .quad   .L7
        .quad   .L5
.L10:
        mov     DWORD PTR [rsp-4], 1
        mov     eax, DWORD PTR [rsp-4]
        ret
.L9:
        mov     DWORD PTR [rsp-4], 2
        mov     eax, DWORD PTR [rsp-4]
        ret
.L8:
        mov     DWORD PTR [rsp-4], 3
        mov     eax, DWORD PTR [rsp-4]
        ret
.L7:
        mov     DWORD PTR [rsp-4], 4
        mov     eax, DWORD PTR [rsp-4]
        ret
.L5:
        mov     DWORD PTR [rsp-4], 5
        mov     eax, DWORD PTR [rsp-4]
        ret
*/
    switch(v) {
        case V::NUM: {
            i = 1;
            return i;
        }
        case V::STRING: {
            i = 2;
            return i;
        }
        case V::ARR: {
            i = 3;
            return i;
        }
        case V::DICT: {
            i = 4;
            return i;

        }
        case V::FUNC: {
            i = 5;
            return i;
        }
        default: {
            // See: https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
            // Also, see: https://stackoverflow.com/questions/54764535/what-optimizations-does-builtin-unreachable-facilitate
            __builtin_unreachable();
        }
    }


    return i;

}


int _switch_case(V v) {

    volatile int i = 0;
/*
_switch_case(V):
        mov     DWORD PTR [rsp-4], 0
        mov     edi, edi
        jmp     [QWORD PTR .L15[0+rdi*8]]
.L15:
        .quad   .L19
        .quad   .L18
        .quad   .L17
        .quad   .L16
        .quad   .L14
.L19:
        mov     DWORD PTR [rsp-4], 1
.L20:
        mov     eax, DWORD PTR [rsp-4]
        ret
.L18:
        mov     DWORD PTR [rsp-4], 2
        jmp     .L20
.L17:
        mov     DWORD PTR [rsp-4], 3
        jmp     .L20
.L16:
        mov     DWORD PTR [rsp-4], 4
        jmp     .L20
.L14:
        mov     DWORD PTR [rsp-4], 5
        jmp     .L20
*/
    switch(v) {
        case V::NUM: {
            i = 1;
            break;
        }
        case V::STRING: {
            i = 2;
            break;
        }
        case V::ARR: {
            i = 3;
            break;
        }
        case V::DICT: {
            i = 4;
            break;

        }
        case V::FUNC: {
            i = 5;
            break;
        }
        default: {
            // See: https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
            // Also, see: https://stackoverflow.com/questions/54764535/what-optimizations-does-builtin-unreachable-facilitate
            __builtin_unreachable();
        }
    }


    return i;

}


int __switch_case(V v) {

    volatile int i = 0;

/*
__switch_case(V):
        mov     DWORD PTR [rsp-4], 0
        cmp     edi, 4
        ja      .L16
        mov     edi, edi
        jmp     [QWORD PTR .L18[0+rdi*8]]
.L18:
        .quad   .L22
        .quad   .L21
        .quad   .L20
        .quad   .L19
        .quad   .L17
*/

    switch(v) {
        case V::NUM: {
            i = 1;
            break;
        }
        case V::STRING: {
            i = 2;
            break;
        }
        case V::ARR: {
            i = 3;
            break;
        }
        case V::DICT: {
            i = 4;
            break;

        }
        case V::FUNC: {
            i = 5;
            break;
        }
        default: {
            break;
        }
    }


    return i;

}

int _single_if_condition(V v){ 
    volatile int i = 0;

    /*
        mov     DWORD PTR [rsp-4], 0
        mov     eax, 1
        cmp     edi, 1
        jne     .L12
        mov     eax, DWORD PTR [rsp-4]
    */

    // NOTE: ERROR AND THROW ESSENTIALLY
    if(v != V::STRING) {
        return 1;
    }

    return i;

}


/*
// inline int _switch_go(V) __attribute__((always_inline));

int _switch_go(V v) {


// __label__ l00, l01, l02, l03, l04, _default, defer;

  // As mentioned before, we keep the array preserved between function invocations.
  static void* cases[] = { // Jump Table
    &&l00,
    &&l01, 
    &&l02,
    &&l03,
    &&l04,
  };



  // CMP For the default case - also implemented by the switch case
  // if((unsigned)v > 10) {
  /   goto _default;
  // }

  // Jumps to the entry
  goto *cases[(unsigned)v];

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

  _default: {
       r = 11;
       goto defer; // break
  }

  // For us, defer serves as the alternative to where the "break" keyword jumps to as far as the switch case and jump table
  defer: {}
    return r;
}
*/


int main() {

  const size_t TEST_BOUNDARY = 1000000;

  const size_t RANGE = 100;

/*
START: SWITCH_CASE_CONDITION DEFAULT BREAK
0.00063072
END: SWITCH_CASE_CONDITION DEFAULT BREAK
START: SWITCH_CASE_CONDITION DEFAULT UNREACHABLE
0.00055233
END: SWITCH_CASE_CONDITION DEFAULT UNREACHABLE
START: SWITCH_CASE_CONDITION UNREACHABLE AND RETURN AS OPPOSED TO BREAK
0.00052462
END: SWITCH_CASE_CONDITION UNREACHABLE AND RETURN AS OPPOSED TO BREAK
START: PLAIN FUNC - NO JMP
0.00040733
END: PLAIN FUNC - NO JMP

START: SWITCH_CASE_CONDITION DEFAULT BREAK
0.00074139
END: SWITCH_CASE_CONDITION DEFAULT BREAK
START: SWITCH_CASE_CONDITION DEFAULT UNREACHABLE
0.00056842
END: SWITCH_CASE_CONDITION DEFAULT UNREACHABLE
START: SWITCH_CASE_CONDITION UNREACHABLE AND RETURN AS OPPOSED TO BREAK
0.00055968
END: SWITCH_CASE_CONDITION UNREACHABLE AND RETURN AS OPPOSED TO BREAK
START: PLAIN FUNC - NO JMP
0.00043133
END: PLAIN FUNC - NO JMP
*/



  std::cout << "START: SWITCH_CASE_CONDITION DEFAULT BREAK" << std::endl;
  {

    double _SUM = 0;
    volatile V _t = V::NUM;

    for(size_t i = 0; i < RANGE; i++) {
        start_time;
        for(size_t i = 0; i < TEST_BOUNDARY; i++) {
            assert(__switch_case(_t) != 0);
        }
        _SUM += static_cast<double>(clock() - s_t_a_r_t) / CLOCKS_PER_SEC;
    }

    std::cout << _SUM/RANGE << std::endl;

  }
  std::cout << "END: SWITCH_CASE_CONDITION DEFAULT BREAK" << std::endl;


  std::cout << "START: SWITCH_CASE_CONDITION DEFAULT UNREACHABLE" << std::endl;
  {

    double _SUM = 0;
    volatile V _t = V::NUM;

    for(size_t i = 0; i < RANGE; i++) {
        start_time;
        for(size_t i = 0; i < TEST_BOUNDARY; i++) {
            assert(_switch_case(_t) != 0);
        }
        _SUM += static_cast<double>(clock() - s_t_a_r_t) / CLOCKS_PER_SEC;
    }

    std::cout << _SUM/RANGE << std::endl;

  }
  std::cout << "END: SWITCH_CASE_CONDITION DEFAULT UNREACHABLE" << std::endl;

  /*
  std::cout << "START: SWITCH_GO_CONDITION" << std::endl;
  {

    for(size_t i = 0; i < 10; i++) {

        start_time;
        for(size_t i = 0; i < TEST_BOUNDARY; i++) {
            assert(_switch_go(V::NUM) != 0);
        }
        end_time;
    }

  }
  std::cout << "END: SWITCH_GO_CONDITION" << std::endl;
  */

  std::cout << "START: SWITCH_CASE_CONDITION UNREACHABLE AND RETURN AS OPPOSED TO BREAK" << std::endl;
  {

    double _SUM = 0;
    volatile V _t = V::NUM;

    for(size_t i = 0; i < RANGE; i++) {
        start_time;
        for(size_t i = 0; i < TEST_BOUNDARY; i++) {
            assert(_switch_case_return(_t) != 0);
        }
        _SUM += static_cast<double>(clock() - s_t_a_r_t) / CLOCKS_PER_SEC;
    }

    std::cout << _SUM/RANGE << std::endl;

  }
  std::cout << "END: SWITCH_CASE_CONDITION UNREACHABLE AND RETURN AS OPPOSED TO BREAK" << std::endl;

  std::cout << "START: PLAIN FUNC - NO JMP" << std::endl;
  {

    double _SUM = 0;
    volatile V _t = V::NUM;

    for(size_t i = 0; i < RANGE; i++) {
        start_time;
        for(size_t i = 0; i < TEST_BOUNDARY; i++) {
            assert(_plain_func_return(_t) != 0);
        }
        _SUM += static_cast<double>(clock() - s_t_a_r_t) / CLOCKS_PER_SEC;
    }

    std::cout << _SUM/RANGE << std::endl;

  }
  std::cout << "END: PLAIN FUNC - NO JMP" << std::endl;

  /*
  std::cout << "START: SINGLE_IF_CONDITION" << std::endl;
  {
    double _SUM = 0;

    for(size_t i = 0; i < RANGE; i++) {
        start_time;
        for(size_t i = 0; i < TEST_BOUNDARY; i++) {
            assert(_single_if_condition(V::NUM) != 0);
            // assert(_single_if_condition(V::STRING) != 0);
        }
        _SUM += static_cast<double>(clock() - s_t_a_r_t) / CLOCKS_PER_SEC;
    }
    std::cout << _SUM/RANGE << std::endl;

  }
  std::cout << "END: SINGLE_IF_CONDITION" << std::endl;
  */

  return 0;
}
