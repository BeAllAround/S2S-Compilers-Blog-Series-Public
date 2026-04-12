#include <stdio.h>

#include <stdlib.h>

#include <stdint.h>



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
};

typedef struct VariantValue {
    union Variant value; 
    // ErrorFlag err_err; // While this is a great optimization for void-returning function, we will have to include the err_flag in order to determine the active member of the union - in other words, whether or not we have thrown from a function.

} VariantValue;


// static inline VariantValue makeVariantValue(const char*, ErrorFlag);

static inline VariantValue makeVariantValue(const char* excp_message) {
    VariantValue val = {};

    val.value.excp_message = excp_message; // NOTE: WE CAN ALSO USE NULL TO CHECK FOR CONST CHAR* AND OTHER TYPE ALLOCATED ON HEAP - IF AND ONLY IF WE HAVE A VOID-RETURNING FUNCTION. See the comment above.

    return val;

}

void free_string(char **p) {
    if (*p) {
        free(*p);
        // Dangling pointer
        *p = NULL;
    }
}

static int _call_number = 0;

VariantValue quickSort_throws(int* arr, int low, int high) { // throws

    printf("quickSort_throws called %d\n", ++_call_number);

    __attribute__((cleanup(free_string))) char *string = malloc(100);

    if(arr[0] == 1) {
        VariantValue excp_value = makeVariantValue("Error raised! at 104 LINE"); // TODO: MUST __LINE__
        // free(free_string);
        return excp_value;
    }

    if (low < high)
	{
		int pi = partition(arr, low, high);

		VariantValue call1 = quickSort_throws(arr, low, pi - 1);
        if(call1.value.excp_message != NULL) {
            // free(free_string);
            return call1;
        }

		VariantValue call2 = quickSort_throws(arr, pi + 1, high);
        if(call2.value.excp_message != NULL) {
            // free(free_string);
            return call2;
        }

	}

    VariantValue excp_value = makeVariantValue(NULL);
    // free(free_string);
    return excp_value;

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

        if(value.value.excp_message != NULL) {
            printf("Exception (\"%s\") thrown!\n", value.value.excp_message);
        } else {
	        printf("Sorted array: \n");
    	    printArray(arr, n);
        }

    }
    printf("END: Block 2\n");


    return 0;
}
