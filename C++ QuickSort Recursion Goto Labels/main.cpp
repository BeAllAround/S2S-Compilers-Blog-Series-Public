/* C implementation QuickSort */
// https://gist.github.com/adwiteeya3/f1797534506be672b591f465c3366643


#include <iostream>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <cassert>



void _quickSort(int*, int, int);
void printArray(int*, int);

// void quickSort(int*, int, int);



int cmp(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

void runTest(int input[], int n) {
    int *expected = (int*)malloc(n * sizeof(int));
    int *actual   = (int*)malloc(n * sizeof(int));

    memcpy(expected, input, n * sizeof(int));
    memcpy(actual, input, n * sizeof(int));

    // Ground truth
    qsort(expected, n, sizeof(int), cmp);

    // Your implementation
    _quickSort(actual, 0, n - 1);

    printArray(expected, n);
    printArray(actual, n);

    // Compare arrays
    for (int i = 0; i < n; i++) {
        assert(expected[i] == actual[i]);
    }

    free(expected);
    free(actual);
}

template<class T, size_t Size>
class StaticStorage {
  public:
  using ItemType = T;
  unsigned char buffer[sizeof(T) * Size];
  size_t stack_count = 0;

  StaticStorage() {}


  // NOTE: Deleted for now. Will think about this
  StaticStorage(const StaticStorage&) = delete;
  StaticStorage(StaticStorage&&) = delete;

  StaticStorage& operator=(const StaticStorage&) = delete;
  StaticStorage& operator=(StaticStorage&&) = delete;

  void add(const T& item) {
    new(&buffer[stack_count * sizeof(T)]) T(item);
    stack_count++;
  }

  void add(T&& item) {
    new(&buffer[stack_count * sizeof(T)]) T(std::move(item));
    stack_count++;
  }


  // T* pop();
  // NOTE: POP FOR STATIC STORAGE IS ILL_ADVISED, RESULTING IN MEMORY LEAKS (EVEN WHEN THERE IS A CAST, WE ARE STILL POINTINT TO UCHAR BUFFER)
  // MAKE SURE TO TEST THIS AT the -O0 level optimizations CUZ THE COMPILER JUST IS TAKES STUFF OUT AT O3
  T& pop() { // SAME GOES FOR const T&
    stack_count--;
    T& popped = *reinterpret_cast<T*>(buffer + ( stack_count * sizeof(T)));

    // *reinterpret_cast<T*>(buffer + ( stack_count * sizeof(T))) = T();


    return popped;
  }

  T& at(size_t i) {
    return *reinterpret_cast<T*>(buffer + ( i * sizeof(T)));
  }

  T& last() {
    return *reinterpret_cast<T*>(buffer + ( (stack_count-1) * sizeof(T)));
  }

  // TODO: WRAP UP IN A MACRO JUST LIKE _storage_add
  void insert(size_t i, T&& item) {
    T* _at = reinterpret_cast<T*>(buffer + ( i * sizeof(T)));
    *_at = std::move(item);
    // ALTERNATIVELY
    // _at->~T();
    // new(&buffer[i * sizeof(T)]) T(std::move(item));
  }


  ~StaticStorage() {
    for(size_t i = 0; i < stack_count; i++) {
      reinterpret_cast<T*>(buffer + (i * sizeof(T)))->~T();
    }
  }


};

#define _storage_add(storage, item, T) \
    new(&storage.buffer[storage.stack_count * sizeof(T)]) item; \
    storage.stack_count++;

// synonym
#define _storage_push _storage_add



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

/*
quickSort(int*, int, int):
        cmp     esi, edx
        jl      .L13
        ret
.L13:
        push    r15
        push    r14 # push vars: pi
        push    rbp
        push    rbx
        sub     rsp, 8
        mov     ebp, esi
        mov     ebx, edx
        mov     r14, rdi
        call    partition(int*, int, int)
        mov     r15d, eax
        lea     edx, [rax-1]
        mov     esi, ebp
        mov     rdi, r14
        call    quickSort(int*, int, int)
        lea     esi, [r15+1]
        mov     edx, ebx
        mov     rdi, r14
        call    quickSort(int*, int, int)
        add     rsp, 8
        pop     rbx
        pop     rbp
        pop     r14
        pop     r15
        ret
*/

void quickSort(int* arr, int low, int high)
{
	if (low < high)
	{
		int pi = partition(arr, low, high);

		quickSort(arr, low, pi - 1);
		quickSort(arr, pi + 1, high);
	}
}

struct CallFrame {

    // args
    struct Args {
        int* arr { nullptr };
        int low;
        int high;
    } args;

	  // Body Variables such as pi that need to be in the current call frame as well. See void quickSort(int*, int, int) above
	  struct Body {
		  int pi { 0 };
	  } vars;


    // Return Vals
    struct Ret {
        void* origin { nullptr };
		    // NOTE: quickSort return VOID so no return values NEEDED
    } ret;

    CallFrame() {}


    CallFrame(const CallFrame& other) {
        args.arr = other.args.arr;
        args.low = other.args.low;
        args.high = other.args.high;

        vars.pi = other.vars.pi;

        ret.origin = other.ret.origin;
    }


    CallFrame(CallFrame&& other) {
        args.arr = other.args.arr;
        args.low = other.args.low;
        args.high = other.args.high;

        // NOTE: NOT NEEDED since the stack.last() will always be correctly mutated beforehand. See below
        vars.pi = other.vars.pi;

        ret.origin = other.ret.origin;
    }

    CallFrame& operator=(const CallFrame& other) {

        args.arr = other.args.arr;
        args.low = other.args.low;
        args.high = other.args.high;

        vars.pi = other.vars.pi;


        ret.origin = other.ret.origin;

        return *this;
    }

    CallFrame& operator=(CallFrame&& other) {
        args.arr = other.args.arr;
        args.low = other.args.low;
        args.high = other.args.high;

        vars.pi = other.vars.pi;

        ret.origin = other.ret.origin;

        return *this;
    }

    ~CallFrame() {}

};

// TRANSLATION OF THE FUNCTION ABOVE
void _quickSort(int* _arr, int _low, int _high) {

    // NOTE: CAN'T INLINE THIS FUNCTION DUE TO THE COMPUTED GOTO: YUCK!
    // __label__ call1_origin;
    // __label__ call2_origin;

    // void *jmps_origin[] = { &&call1_origin, &&call2_origin };

    // Can be dynamically allocated stack, which means we can call it as long as there is memory available
    StaticStorage<CallFrame, 100> stack;


    // static StaticStorage<CallFrame, 20> stack;


    CallFrame initial_call = CallFrame();
    initial_call.args.arr = _arr;
    initial_call.args.low = _low;
    initial_call.args.high = _high;

    // PUSH
    _storage_push(stack, CallFrame(std::move(initial_call)), CallFrame);


    // CALL
    __quickSort: {}

    // function args
	  int* arr = stack.last().args.arr;

	  int low = stack.last().args.low;
	  int high = stack.last().args.high;


    { // FUNCTION BODY

      if (low < high)
      {
        int pi = partition(arr, low, high);

        // push vars
        stack.last().vars.pi = pi;

        {
          CallFrame call1 = CallFrame();
          call1.args.arr = arr;
          call1.args.low = low;
          call1.args.high = pi - 1;
          call1.ret.origin = &&call1_origin;
          // PUSH CALL
          _storage_push(stack, CallFrame(std::move(call1)), CallFrame);
          // CALL
          goto __quickSort;
        }
        // ORIGIN OF RET
        call1_origin:{}

        {
          int pi = stack.last().vars.pi;
          int *arr = stack.last().args.arr;

          int high = stack.last().args.high;
          CallFrame call2 = CallFrame();
          call2.args.arr = arr;
          call2.args.low = pi + 1;
          call2.args.high = high;
          call2.ret.origin = &&call2_origin;
          // PUSH
          _storage_push(stack, CallFrame(std::move(call2)), CallFrame);
          // CALL
          goto __quickSort;
        }
        // ORIGIN OF RET
        call2_origin:{}

      }

      goto _ret; // CAN BE JUST REMOVED

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
        stack.pop();

        // NOTE: INITIAL PUSH ON LINE 309
        if(stack.stack_count == 0) {
            goto _done;
            // return;
        }

        // std::cout << "stack_c: " << stack_c << std::endl;

        // ret
        void* ret_origin = stack.at(stack.stack_count).ret.origin;
        goto *ret_origin;

    }


    _done: {}


    // ONLY FOR UNIT TESTS
    // assert(stack.stack_count == 0);


    return;

}


void printArray(int* arr, int size)
{
	int i;
	for (i = 0; i < size; i++)
    std::cout << arr[i] << ' ';
  std::cout << std::endl;
}


int main01() {
	int arr[] = {10, 7, 8, 9, 1, 5};
	int n = sizeof(arr) / sizeof(arr[0]);

	printArray(arr, n);

	quickSort(arr, 0, n - 1);

	printf("Sorted array: \n");
	printArray(arr, n);

	return 0;
}

int main02() {
	int arr[] = {10, 7, 8, 9, 1, 5};
	int n = sizeof(arr) / sizeof(arr[0]);

	printArray(arr, n);

	_quickSort(arr, 0, n - 1);

	printf("Sorted array: \n");
	printArray(arr, n);

	return 0;
}

void assertSorted(int arr[], int n) {
    for (int i = 1; i < n; i++) {
        assert(arr[i] >= arr[i - 1]);
    }
}

// Common Test Scenarios
int main03() {

    // 1. Basic test (your original)
    int t1[] = {10, 7, 8, 9, 1, 5};

    // 2. Already sorted
    int t2[] = {1, 2, 3, 4, 5, 6};

    // 3. Reverse sorted (worst case for some implementations)
    int t3[] = {6, 5, 4, 3, 2, 1};

    // 4. All elements same
    int t4[] = {5, 5, 5, 5, 5};

    // 5. Single element
    int t5[] = {42};

    // 6. Two elements (sorted)
    int t6[] = {1, 2};

    // 7. Two elements (unsorted)
    int t7[] = {2, 1};

    // 8. Includes negative numbers
    int t8[] = {-3, -1, -7, -4, -5, -2};

    // 9. Mixed positive + negative
    int t9[] = {-10, 0, 5, -3, 8, -2, 1};

    // 10. Large duplicates
    int t10[] = {3, 1, 2, 3, 3, 3, 0, 3};

    // 11. Zeros only
    int t11[] = {0, 0, 0, 0};

    // 12. Large values
    int t12[] = {2147483647, -2147483648, 0, 999999, -999999};

    // 13. Random order
    int t13[] = {12, 4, 55, 23, 19, 1, 0, 87, 33};

    // 14. Nearly sorted (common real-world case)
    int t14[] = {1, 2, 3, 5, 4, 6, 7};

    // 15. Alternating high/low
    int t15[] = {1, 100, 2, 99, 3, 98, 4, 97};

    // Put all tests into arrays of pointers
    int* tests[] = {
        t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15
    };

    int sizes[] = {
        sizeof(t1)/sizeof(int),
        sizeof(t2)/sizeof(int),
        sizeof(t3)/sizeof(int),
        sizeof(t4)/sizeof(int),
        sizeof(t5)/sizeof(int),
        sizeof(t6)/sizeof(int),
        sizeof(t7)/sizeof(int),
        sizeof(t8)/sizeof(int),
        sizeof(t9)/sizeof(int),
        sizeof(t10)/sizeof(int),
        sizeof(t11)/sizeof(int),
        sizeof(t12)/sizeof(int),
        sizeof(t13)/sizeof(int),
        sizeof(t14)/sizeof(int),
        sizeof(t15)/sizeof(int)
    };

    int numTests = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < numTests; i++) {
        printf("\nTest %d:\nBefore: ", i + 1);
        printArray(tests[i], sizes[i]);

        _quickSort(tests[i], 0, sizes[i] - 1);

        printf("After:  ");
        printArray(tests[i], sizes[i]);
        
        assertSorted(tests[i], sizes[i]);
    }

    return 0;
}

int main04() {

    int t1[] = {10, 7, 8, 9, 1, 5};
    int t2[] = {1, 2, 3, 4, 5};
    int t3[] = {5, 4, 3, 2, 1};
    int t4[] = {5, 5, 5, 5};
    int t5[] = {42};
    int t6[] = {2, 1};
    int t7[] = {-3, -1, -7, -4};
    int t8[] = {-10, 0, 5, -3, 8};
    int t9[] = {3, 1, 2, 3, 3, 0};
    // int t10[] = {2147483647, -2147483648, 0}; // The expected output for this one is 2147483647 -2147483648 0 for some reason. INT_MAX for 32bit Overflow anyways ...

    runTest(t1, sizeof(t1)/sizeof(int));
    runTest(t2, sizeof(t2)/sizeof(int));
    runTest(t3, sizeof(t3)/sizeof(int));
    runTest(t4, sizeof(t4)/sizeof(int));
    runTest(t5, sizeof(t5)/sizeof(int));
    runTest(t6, sizeof(t6)/sizeof(int));
    runTest(t7, sizeof(t7)/sizeof(int));
    runTest(t8, sizeof(t8)/sizeof(int));
    runTest(t9, sizeof(t9)/sizeof(int));
    // runTest(t10, sizeof(t10)/sizeof(int));

    std::cout << "All tests passed" << std::endl;

    return 0;
}

void fuzzTest() {
    srand(time(NULL));

    for (int t = 0; t < 1000; t++) {
        int n = rand() % 100 + 1;
        int arr[100];

        for (int i = 0; i < n; i++) {
            arr[i] = rand() % 1000 - 500;
        }

        runTest(arr, n);
    }
}

int main()
{

	int r = (main03(), main04());

  fuzzTest();

  return r;

}
