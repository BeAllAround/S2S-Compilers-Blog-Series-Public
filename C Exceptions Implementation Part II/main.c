/* C implementation QuickSort */
// Kudos to https://gist.github.com/adwiteeya3/f1797534506be672b591f465c3366643

#include <stdio.h>

#include <stdlib.h>

#include <stdint.h>

/*
 * NOTE:
 * Performance When Thrown (The Exceptional Path)
 * Throwing an exception is orders of magnitude slower than returning an error code.
 * Cost: Throwing can be 100x to 1,000x slower than a standard function return.
*/


void printArray(int* arr, int size)
{
	int i;
	for (i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}



inline void swap(int*, int*) __attribute__((always_inline));

inline void swap(int *a, int *b)
{
	int t = *a;
	*a = *b;
	*b = t;
}

inline int partition(int*, int, int) __attribute__((always_inline));

inline int partition(int* arr, int low, int high)
{
	int pivot = arr[high]; 
	int i = (low - 1);	   

	for (int j = low; j <= high - 1; j++)
	{
		if (arr[j] < pivot)
		{
			i++; 
			swap(&arr[i], &arr[j]);
		}
	}
	swap(&arr[i + 1], &arr[high]);
	return (i + 1);
}

void quickSort(int* arr, int low, int high)
{
    // throw 1;
	if (low < high)
	{
		int pi = partition(arr, low, high);

		quickSort(arr, low, pi - 1);
		quickSort(arr, pi + 1, high);
	}
}

enum _ErrorFlag { _true, _false }; 
typedef uint8_t ErrorFlag;

union Variant { // sizeof(union) takes the size of the largest member so that is why using it in this scenario pays the dividends
    const char* excp_message;
    // struct Exception* exception;
    // void v; // RETURN VALUE IF A PRIMITIVE, [POINTER], STRUCT BUT NOT VOID
    int ret_value;
};

typedef struct VariantValue {
    union Variant value; 
    ErrorFlag err_flag; // While this is a great optimization for void-returning function, we will have to include the err_flag in order to determine the active member of the union - in other words, whether or not we have thrown from a function.

} VariantValue;


// static inline VariantValue makeVariantValue(const char*, ErrorFlag);

static inline VariantValue makeVariantValue(const char* excp_message, ErrorFlag err_flag) {
    VariantValue val = {};

    val.value.excp_message = excp_message; // NOTE: WE CAN ALSO USE NULL TO CHECK FOR CONST CHAR* AND OTHER TYPE ALLOCATED ON HEAP - IF AND ONLY IF WE HAVE A VOID-RETURNING FUNCTION. See the comment above.
    val.err_flag = err_flag;

    return val;

}

void free_string(char **p) {
    if (*p) {
        free(*p);
        // Dangling pointer
        *p = NULL;
    }
}

#define STR_INNER(x) #x
#define STR(x) STR_INNER(x)

static int _call_number = 0;

#define CATCH(v) if(v.err_flag == _true)
#define TRY else

#define __THROWS__

VariantValue quickSort_throws(int* arr, int low, int high) __THROWS__ { 

    printf("quickSort_throws called %d\n", ++_call_number);

    __attribute__((cleanup(free_string))) char *string = malloc(100);

    if(arr[0] == 1) {
        VariantValue excp_value = makeVariantValue("Error raised! at " STR(__LINE__), _true);
        // free(free_string);
        return excp_value;
    }

    if (low < high)
	{
		int pi = partition(arr, low, high);

		VariantValue call1 = quickSort_throws(arr, low, pi - 1);
        if(call1.err_flag == _true) {
            // free(free_string);
            // catch { // ESSENTIALLY, THIS IS OUR CATCH BLOCK
            // return makeVariantValue("Error raised! at " STR(__LINE__), _true);
            // }
            return call1;
        }

		VariantValue call2 = quickSort_throws(arr, pi + 1, high);
        if(call2.err_flag == _true) {
            // free(free_string);
            return call2;
        }

	}

    VariantValue excp_value = makeVariantValue(NULL, _false);
    // free(free_string);
    return excp_value;

}

/*
NOTE: For each function call, you will need a new VariantValue struct implementation.
*/

VariantValue f3() __THROWS__;
VariantValue f2(int) __THROWS__;
VariantValue f1(int) __THROWS__;

VariantValue f3() __THROWS__ {
    VariantValue f2_call = f2(10);

    // The catch BLOCK if NEEDED
    if(f2_call.err_flag == _true) {
        return f2_call;
    }
    return f2_call;
}


VariantValue f2(int i) __THROWS__ {
    VariantValue f1_call = f1(i);

    // The catch BLOCK if NEEDED
    if(f1_call.err_flag == _true) {
        VariantValue f2_return = makeVariantValue("f2() Exception " STR(__LINE__), _true); // throw object
        // f2_return.value.ret_value = 33;

        return f2_return;
    }
    return f1_call;

}

VariantValue f1(int i) __THROWS__ {

    VariantValue f1_return = makeVariantValue("f1() Exception " STR(__LINE__), _true); // throw object
    // f1_return.value.ret_value = i;

    return f1_return;
}


int main() {

    printf("sizeof(enum ErrorFlag) %ld\n", sizeof(enum _ErrorFlag));

    printf("sizeof(ErrorFlag) %ld\n", sizeof(ErrorFlag));
    printf("sizeof(union Varient) %ld\n", sizeof(union Variant));

    printf("sizeof(struct VariantValue) %ld\n", sizeof(VariantValue));

    printf("START: Block 1\n");
    {

	    int arr[] = {10, 7, 8, 9, 1, 5};
	    int n = sizeof(arr) / sizeof(arr[0]);

	    printArray(arr, n);

	    quickSort(arr, 0, n - 1);

	    printf("Sorted array: \n");
    	printArray(arr, n);
    }
    printf("END: Block 1\n");

    printf("START: Block 2\n");
    {

	    int arr[] = {10, 7, 8, 9, 1, 5};
	    int n = sizeof(arr) / sizeof(arr[0]);

	    printArray(arr, n);

	    VariantValue value = quickSort_throws(arr, 0, n - 1);

        if(value.err_flag == _true) {
            printf("Exception (\"%s\") thrown!\n", value.value.excp_message);
        } else {
	        printf("Sorted array: \n");
    	    printArray(arr, n);
        }

    }
    printf("END: Block 2\n");


    printf("START: Block 3\n");
    {

	    VariantValue value = f3();

        CATCH(value) {
            // DO SOMETHING IN THE CATCH BLOCK
            printf("Exception (\"%s\") thrown!\n", value.value.excp_message);

        } TRY {
	        printf("f3() returned  %d \n", value.value.ret_value);
        }


    }
    printf("END: Block 3\n");


    return 0;
}
