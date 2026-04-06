#include <iostream>


#include <vector>


#include <cassert>


struct CallFrame {

    // args
    struct Args {
        int n;
    } args;

    // Body Variables such as i that need to be in the current call frame as well. See void recursion01(int) below
    struct Body {
      int i;
    } vars;


    // Return Vals
    struct Ret {
        void* origin { nullptr };
        int value { 0 }; // RETURN VALUE
        // int value { -1000 }; // NOTE: SHOULD WORK TOO
    } ret;

    CallFrame() {}

    CallFrame(int n) {
        args.n = n;
    }


    CallFrame(const CallFrame& other) {
        args.n = other.args.n;
        vars.i = other.vars.i;
        ret.origin = other.ret.origin;
        ret.value = other.ret.value;
    }


    CallFrame(CallFrame&& other) {
        args.n = std::move(other.args.n);
        vars.i = other.vars.i;
        ret.origin = std::move(other.ret.origin); // NOT NEEDED FOR PRIMITIVE
        ret.value = other.ret.value;
    }

    CallFrame& operator=(const CallFrame& other) {
        args.n = other.args.n;
        vars.i = other.vars.i;
        ret.origin = other.ret.origin;
        ret.value = other.ret.value;

        return *this;
    }

    CallFrame& operator=(CallFrame&& other) {
        args.n = std::move(other.args.n);
        vars.i = other.vars.i;
        ret.origin = std::move(other.ret.origin); // NOT NEEDED FOR PRIMITIVE
        ret.value = other.ret.value;

        return *this;
    }

    ~CallFrame() {}

};


/*
// ASSEMBLY -O1 level (Cleanest assembly to read in my opinion)
recursion01(int):
        push    r14
        push    rbx # Register Body Vars?
        sub     rsp, 24
        cmp     edi, 1
        je      .L17
        cmp     edi, 2
        je      .L18
        mov     DWORD PTR [rsp+12], 3
        mov     eax, DWORD PTR [rsp+12]
        cmp     eax, 1
        jle     .L4
        mov     ebx, 1
.L14:
        mov     edi, ebx
        call    recursion01(int)
        add     ebx, 1
        mov     eax, DWORD PTR [rsp+12]
        cmp     eax, ebx
        jg      .L14
.L4:
        add     rsp, 24
        pop     rbx
        pop     r14
        ret
*/

void recursion01(int n) {
    if(n == 1) {
        std::cout << 1 << std::endl;
        return;
    } else if(n == 2) {
        std::cout << 2 << std::endl;
        return;
    }

    volatile int _end = 3; // prevent the compiler from optimizing the loop out

    for(int i = 1; i < _end; i++) {
        recursion01(i);
    }

}




int _recursion01(int _n) {

    // NOTE: CAN'T INLINE THIS FUNCTION DUE TO THE COMPUTED GOTO: YUCK!
    // __label__ call1_origin;
    // __label__ call2_origin;

    // void *jmps_origin[] = { &&call1_origin, &&call2_origin };

    // Can be dynamically allocated stack, which means we can call it as long as there is memory available
    static CallFrame stack[20];

    // static size_t stack_c = 0;
    size_t stack_c = 0; // mov     QWORD PTR [rbp-24], 0


    CallFrame initial_call = CallFrame();
    initial_call.args.n = _n;
    initial_call.vars.i = 0;
    /* OK
     * Preventing
     *  warning: ‘call1.CallFrame::vars.CallFrame::Body::i’ may be used uninitialized [-Wmaybe-uninitialized]
         vars.i = other.vars.i;
    */
    initial_call.ret.origin = nullptr; // Origin not required here


    // PUSH
    stack[stack_c] = std::move(initial_call);
    stack_c++;

    #define _last stack_c-1

    // int i = 1;

    // CALL
    __recursion_01: {}

    // function args
    int n = stack[_last].args.n; // last: stack_c-1


    { // FUNCTION BODY
        if(n == 1) {
            stack[_last].ret.value = 0; // return 0;
            std::cout << 1 << std::endl;
            goto _ret;
        } else if(n == 2) {
            stack[_last].ret.value = 0; // return 0;
            std::cout << 2 << std::endl;
            goto _ret;
        }
        #define _i stack[_last].vars.i
        _i = 1; // BODY VARS INIT WHERE IT SHOULD BE INITIALIZED AND EVALUATED BASED ON THE RECURSIVE FUNCTION ABOVE

        while(_i < 3) {
            {
                // NOTE: Limit the scope of a by declaring it within a block statement that ends before the label.
                CallFrame call1 = CallFrame();
                call1.args.n = _i; 
                call1.vars.i = 0;
                /* OK
                 * Preventing
                 *  warning: ‘call1.CallFrame::vars.CallFrame::Body::i’ may be used uninitialized [-Wmaybe-uninitialized]
                  vars.i = other.vars.i;
                */
                call1.ret.origin = &&call1_origin;
                // PUSH
                stack[stack_c++] = std::move(call1);

                // CALL
                goto __recursion_01;
            }
            // ORIGIN OF RET
            call1_origin: {}


           _i++; 
        }

    }

    _ret: {
        // Stack is empty
        /*
        if(stack_c == 0) {
            goto _done;
            // return;
        }
        */

        // pop rbx
        stack_c--;

        // NOTE: INITIAL PUSH ON LINE 137
        if(stack_c == 0) {
            goto _done;
            // return;
        }

        // std::cout << "stack_c: " << stack_c << std::endl;

        // ret
        void* ret_origin = stack[stack_c].ret.origin;
        goto *ret_origin;

    }


    _done: {}


    assert(stack_c == 0);


    return stack[stack_c].ret.value;

}


int main() {

    recursion01(0);

    _recursion01(0);




    return 0;
}
