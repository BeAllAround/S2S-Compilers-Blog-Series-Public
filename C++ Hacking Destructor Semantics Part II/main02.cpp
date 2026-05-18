#include <iostream>

#include <cassert>
#include <cstring>




const char* return_C_string() {
  union U {
    std::string s;

    ~U() __attribute__((always_inline)) {}
  };

  U s = {};

  new(&s.s) std::string("AD"); // NOTE: Small string optimization in this case results in undefined behaviour
  s.s.reserve(100); // NOTE: FIXES THE SMALL STRING OPTIMIZATION HURDLE // ALSO: COMMENT THIS OUT AND COMPARE THE OUTPUTS/RESULTS WITH -O0, -O1, -O2, -O3 level optimizations

  std::cout << s.s.c_str() << std::endl;

  return s.s.c_str();
}

int main() {

  const char* c_string = return_C_string();


  std::cout << c_string << std::endl;

  assert(strcmp(c_string, "AD") == 0);

  // TODO: HOW TO DESTROY c_string in this case
  
  // delete c_string;
  // free(c_string);


  return 0;
}
