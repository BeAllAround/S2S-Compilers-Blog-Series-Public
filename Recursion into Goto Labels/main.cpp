#include <vector>
#include <stack>

#include <iostream>


#include <ctime>
#include <cassert>

#define start_time clock_t s_t_a_r_t = clock();

#if __GNUG__
#define end_time printf("[Cpu_time_used: %f]\n", static_cast<double>(clock() - s_t_a_r_t) / CLOCKS_PER_SEC);
#else
#define end_time printf("[Cpu_time_used: %f]\n", ((double) (clock() - s_t_a_r_t)) / CLOCKS_PER_SEC);
#endif

const size_t stack_0_size = 250000;
const size_t _end = stack_0_size;


void _recursive(size_t n) {
  if(n == stack_0_size) {
    // __pause();
    return;
  }

  // CODE

  return _recursive(n+1);


}

struct CallFrame {
  size_t n { 0 };
  // size_t return_value { 0 };

  CallFrame(size_t n) : n {n} {}
  CallFrame()
  // __attribute__((always_inline)) 
  {}

  CallFrame(const CallFrame& c) {
    *this = c;
  }

  CallFrame& operator=(const CallFrame& other) {
    n = other.n;
    return *this;
  }

  ~CallFrame() __attribute__((always_inline))  {}

};

class my_stack : public std::stack<CallFrame> {
  public:
    using std::stack<CallFrame>::c;
};
 
// static unsigned char buffer[sizeof(CallFrame) * _end];
// static CallFrame static_call_stack[_end];

void test_0() {
  __label__ _recursive_label;

  size_t stack_p = 0;


  static CallFrame call_stack[_end];


  // static CallFrame

  // my_stack stack;
  
  // stack.reserve(_end);


  // CALL, PUSH
  call_stack[stack_p++] = CallFrame(0);

  // call_stack[stack_p] = new(&buffer[stack_p * sizeof(CallFrame)]) CallFrame(0);
  // stack_p++;


_recursive_label: // void _recursive_label(size_t n);
  {
    size_t n = call_stack[stack_p - 1].n;
    if(n == _end-1) { //  if(n == stack_0_size) {
      goto _done;
    }

    /*
    {
      // BEFORE CODE

    }
    */

      // CALL recursive_label
       // stack.push_back(CallFrame(stack[stack.size()-1].n+1));

       // call_stack[stack_p] = new(&buffer[stack_p * sizeof(CallFrame)]) CallFrame(call_stack[stack_p-1]->n+1);
       // stack_p++;

       call_stack[stack_p++] = CallFrame(n+1); // _recursive(n+1);

       goto _recursive_label;
       // AFTER_CODE?? See below


  }

_done: {
         /*
         while(stack.size() != 0) {
           // CallFrame& callframe = 
          stack.pop_back();
          // AFTER CODE
         }
         */

         size_t _p = sizeof(call_stack)/sizeof(CallFrame);
         // size_t _p = stack.size();
         assert(_p == _end);

         while(_p != 0) {
           // pop, ret
           --_p;

           // AFTER CODE
           // std::cout << call_stack[_p].n << std::endl;
         }

       }

}



int main() {

  volatile size_t n = 0;
  {
    start_time;
    _recursive(n);
    end_time;
  }

  {
    start_time;
    test_0();
    end_time;
  }

  // __pause();

  return 0;

}
