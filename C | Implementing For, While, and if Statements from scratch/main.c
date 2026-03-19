#include <stdio.h>

#define _FOR(INIT, CONTROL_EXPRESSION, ADJUSTMENT, BODY) { \
    __label__ _check, _body, _adjustment, _done; \
    INIT; \
    _check: { \
        if(CONTROL_EXPRESSION) goto _body; \
        goto _done; \
    } \
    _body: BODY \
    _adjustment: { ADJUSTMENT; } \
    goto _check; \
    _done: \
    } \

#define _WHILE(CONTROL_EXPRESSION, BODY) { \
    __label__ _adjustment, _body, _done; \
    _adjustment: { \
        if(CONTROL_EXPRESSION) goto _body; \
        goto _done; \
    } \
    _body: BODY \
    goto _adjustment; \
    _done: \
    } \

#define _BREAK goto _done

#define _CONTINUE goto _adjustment

#define _IF(CONDITION, BODY) { \
    __label__ _if_true, _if_false; \
    static void* branch[2] = { &&_if_false, &&_if_true }; \
    goto *branch[(int)(CONDITION)]; \
    _if_true: BODY \
    _if_false: \
} \

/*
int __init() {
    printf("_init()\n");
    return 0;
}

int __control_expression() {
    printf("_control_expression()\n");
    return 1;
}

int __adjustment() {
    printf("_adjustment()\n");
    return 0;
}
*/



int main() {
    /*
    for(int i = __init(); __control_expression() && i < 1; i++, __adjustment()) {
        printf("%d\n", i);
        continue;
    }

    _FOR(int i = __init(), (__control_expression() && i < 1), (i++, __adjustment()), {
        printf("%d\n", i);
        _CONTINUE;
    })
    */
/*
    _FOR(int i = 0, (i < 10), (i++), {
        printf("%d\n", i);
        _FOR(int j = 0, (j < 10), (j++), {
            printf("outer: %d\n", i);
            printf("inner: %d\n", j);
            _CONTINUE;
            _BREAK;
        })
    })
    */

/*
for(int i = 0; i < 1; i++) {

}
*/
    for(int i = 0; i < 5; i++) {
        printf("%d\n", i);
    }

    _FOR(int i = 0, (i < 5), (i++), {
        printf("%d\n", i);
    })
 

    int ii = 0;
    _WHILE(ii < 10, {
        printf("ii %d\n", ii);

        _BREAK;
        _CONTINUE;

        ii++;
    })

    _IF(0 < 2, {
        printf("_IF true\n");

        /*
        _IF(0 < 2, {
            printf("_IF true\n");
        })

        int ii = 0;
        _WHILE(ii < 10, {
            printf("ii %d\n", ii);

            _BREAK;
            _CONTINUE;

            ii++;
        })
        */
    })

    volatile int g = 0; 
    if(g < 2) {
        printf("if true\n");
    }



    return 0;
}



/*

C++ RAII TEST

#include <iostream>



struct S {


    S() {
        std::cout << "S()" << std::endl;
    }


    ~S() {
        std::cout << "~S()" << std::endl;
    }

};

#define _FOR(INIT, CONTROL_EXPRESSION, ADJUSTMENT, BODY) { \
    __label__ _check, _body, _adjustment, _done; \
    INIT; \
    _check: { \
        if(CONTROL_EXPRESSION) goto _body; \
        goto _done; \
    } \
    _body: BODY \
    _adjustment: { ADJUSTMENT; } \
    goto _check; \
    _done: \
    } \


int main() {


    for(int i = 0; i < 2; i++, printf("END\n")) {
        S s;
    }


    std::cout << "::RAII::" << std::endl;

    _FOR(int i = 0, (i < 2), (i++, printf("END\n")), {
        S s;
    })

    return 0;
}
*/
