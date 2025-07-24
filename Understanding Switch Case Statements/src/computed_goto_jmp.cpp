#include <iostream>

class S {
  public:
    S() : data{new int} {}
  ~S() {
    // std::cout << "~S()" << std::endl;
    delete data;
  }

  private:
  int* data { nullptr };

};

// It is the same behaviour with __attribute__(cleanup) with GCC. Check out "computed_goto_jmp.c"

int main() {

  volatile int c = 0;

  static void* table[] = { &&foo, &&bar, &&zoo };

  goto *table[0];


  foo: {
       S s;

       if(c == 100) return 0;

       c++;
       goto bar; // OK
       // goto *table[1]; // LEAK
       // goto *table[0]; // LEAK
       // goto foo; // OK

     }
  
  bar: {
         S s;
       }

  
  {
    S s;
    {
      S s;

      static void* _table[] = { &&_exit };

      // s.~S(); // NOTE: Calling this for the outer scope is a problem
      // NOTE: ONLY LEAKS WHEN THE COMPUTED GOTO BLOCK ISN'T DEFINED IN THE CURRENT SCOPE WHERE IT IS SUPPOSED TO BE RELEASED.
      // 8 bytes lost
      
      // goto *table[2]; // LEAK
      // goto zoo; // OK - STILL, A NORMAL GOTO
      // goto _exit; // OK
      
      goto *_table[0]; // OK

      _exit: {
         return 0;
       }

    }
  }

zoo: {
    }

  return 0;
}
