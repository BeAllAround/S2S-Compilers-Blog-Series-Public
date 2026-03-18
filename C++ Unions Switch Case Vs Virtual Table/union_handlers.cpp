#include <iostream>
#include <vector>

#define start_time clock_t s_t_a_r_t = clock();

#if __GNUG__
#define end_time printf("[Cpu_time_used: %f]\n", static_cast<double>(clock() - s_t_a_r_t) / CLOCKS_PER_SEC);
#else
#define end_time printf("[Cpu_time_used: %f]\n", ((double) (clock() - s_t_a_r_t)) / CLOCKS_PER_SEC);
#endif



// The goal of the following source code is to implement a new way we are "keeping track" of what member the union has active. What it is referred to as "A union-like class" is a class with a union that uses a "switch" case to determine which member is active. I am of the belief that once this is implemented in Decl that way, we constantly need to use switch case for one value and another in order to apply that certain operation. For example,
// 1+1 // two switch cases
//  switch(left.type()) {
//      case NUM: {
//        Num num_left = left.get_num(); 
//        switch(right.type()) {
//          case NUM: {
//            Num num_right = right.get_num();
//            // Apply the operation
//            return plus(num_left, num_right);
//          }
//        }
//      }
//  }

class Value;

/* 
// Proposed Design for Right, but does not make sense for now
enum class ValueNum;
class Right {
static Value plus(ValueNum, int, const Value);
}
*/

namespace UnionHandle {
  // TODO: Nested Namespaces for operatins, functions, etc.

  class Left;

  // Alias since the left value is actually Main value since we are evaluating from left to right operators
  using Main = Left;

  // Left Value - NOTE: Also the MAIN value
  // Left being the union invoke type
  class Left {
    public:
      // default
      static Value plus_default(const Value&, const Value&);
      // Number
      static Value plus_num(const Value&, const Value&);
      // String
      static Value plus_string(const Value&, const Value&);


      // functions including utilities, stdouts, etc.

      static  void print_default(const Value&);
      static  void print_num(const Value&);
      static  void print_string(const Value&);

      static void destroy(const Value&);
      static void destroy_num(const Value&);
      static void destroy_string(const Value&);


  };


  // Right Value
  // Right being the union invoke type
  class Right {
    public:
      // default
      static Value plus_default(int, const Value&);

      // Right being union Num
      static Value plus_num(int, const Value&);

      // CASE: num + string. Note that, at this point, we pretty much need to cover all of the combinations of types that can happened. For example, "num + num", "string + string", "num + string", "string + num", etc.
      // num + string
      static Value plus_num_string(int, const Value&);
      // string + num
      static Value plus_string_num(const std::string&, const Value&);

      static Value plus_string(const std::string&, const Value&);

      // Throw if it can't work with int as left. For example, "1+{}"
      static Value plus_num_throw(int, const Value&);

  };

}

void print_default(const Value&);

void print_num(const Value&);

using op_left_handler = Value(*)(const Value&, const Value&);
using op_right_num = Value(*)(int, const Value&);
using op_right_string = Value(*)(const std::string&, const Value&);
using op_print = void(*)(const Value&);
using value_destroy = void(*)(const Value&);

struct UnionHandler {
  // left/main
  op_left_handler plus;

  // NOTE: All right handlers are REQUIRED
  op_right_num plus_num;
  op_right_string plus_string;


  // left/main
  op_print print_val;
  value_destroy destroy;
};

UnionHandler handler_num = {
  UnionHandle::Left::plus_num, 

  UnionHandle::Right::plus_num,
  // string + num
  UnionHandle::Right::plus_string_num,

  // Main
  UnionHandle::Main::print_num,
  UnionHandle::Main::destroy_num
};

UnionHandler handler_string = {
  UnionHandle::Left::plus_string, 

  // num + string
  UnionHandle::Right::plus_num_string,
  UnionHandle::Right::plus_string,

  // Main
  UnionHandle::Main::print_string,
  UnionHandle::Main::destroy_string
};


enum class ValueType {
  NUM,
  CHAR,
  STR,
};

class Value {
  private:

    UnionHandler* handlers;

    union {
      int num;
      char c;
      std::string str;
      // std::vector<int> arr;
    };



  public:
    Value(const Value& value) {}

    explicit Value(std::string&& str) : str{std::move(str)}, handlers {&handler_string} {}

    explicit Value(int num) : num{num}, handlers{&handler_num} {}


    // NOTE: Aliases - basically for the runtime handlers determined by the constructor
    Value plus_num(int num) const {
      return handlers->plus_num(num, *this);
    }

    Value plus_string(const std::string& str) const {
      return handlers->plus_string(str, *this);
    }


    Value plus(const Value& right) const {
      return handlers->plus(*this, right);
    }

    void print() {
      handlers->print_val(*this);
    }
    // NOTE: END OF Aliases


    int get_num() const {
      return num;
    }

    const std::string& get_string() const {
      return str;
    }

    ~Value() {
      handlers->destroy(*this);
    }

    friend void UnionHandle::Left::destroy(const Value&);
    friend void UnionHandle::Left::destroy_num(const Value&);
    friend void UnionHandle::Left::destroy_string(const Value&);

    // friend Value UnionHandle::Left::plus_num(const Value&, const Value&);

    friend inline Value plus(const Value& left, const Value& right) {
      return left.handlers->plus(left, right);
    }

};

Value UnionHandle::Left::plus_default(const Value& left, const Value& right) {
  return right.plus_num(left.get_num());
}

// TODO
Value UnionHandle::Right::plus_default(int num, const Value& right) {
  return Value(num + right.get_num());
}

void UnionHandle::Main::print_default(const Value&) {
  std::cout << "NONE" << std::endl;
}



Value UnionHandle::Left::plus_num(const Value& left, const Value& right) {
  return right.plus_num(left.get_num());
  // return right.handlers->plus_num(left.get_num(), right);
}

Value UnionHandle::Right::plus_num(int num, const Value& right) {
  return Value(num + right.get_num());
}

void UnionHandle::Main::print_num(const Value& value) {
  std::cout << value.get_num() << std::endl;
}

void UnionHandle::Main::destroy_num(const Value& value) {
  // None
}



Value UnionHandle::Left::plus_string(const Value& left, const Value& right) {
  return right.plus_string(left.get_string());
}

// NOTE: str + str
Value UnionHandle::Right::plus_string(const std::string& str, const Value& right) {
  return Value(str + right.get_string());
}

// NOTE: num + string
Value UnionHandle::Right::plus_num_string(int num, const Value& right) {
  return Value(std::to_string(num) + right.get_string());
}


// NOTE: string + num
Value UnionHandle::Right::plus_string_num(const std::string& str, const Value& right) {
  return Value(str + std::to_string(right.get_num()));
}


void UnionHandle::Main::print_string(const Value& value) {
  std::cout << value.get_string() << std::endl;
}

void UnionHandle::Main::destroy_string(const Value& value) {
  value.str.~basic_string();
}


Value UnionHandle::Right::plus_num_throw(int, const Value&) {
  throw "STR ERROR";
}

int main() {
  // Benchmark
  const size_t i_end = 1000000;


  srand(time(0));


  std::cout << "sizeof(Value): " << sizeof(Value) << std::endl;

  double _SUM = 0;

  const size_t RANGE = 100;

  for(size_t i = 0; i < RANGE; i++) {


    volatile size_t ii;
    
    // TODO: Deeper tests
    start_time;
    for(size_t i = 0; i < i_end; i++) {

      Value v = plus(Value(i + rand()%11),
                  plus(Value(i),
                    plus(Value(i),
                      plus(Value(i + 2222),
                        plus(Value(11),
                          plus(Value(11),
                            plus(Value(11222), Value(12222))))))));
      ii = v.get_num();
    }

    _SUM += static_cast<double>(clock() - s_t_a_r_t) / CLOCKS_PER_SEC;

  }

  std::cout << _SUM/RANGE << std::endl;


  return 0;
}

    /*

    {
      start_time;
      for(size_t i = 0; i < i_end; i++) {
        Value v = plus(Value("AAAAAAAAAAAAAAAAAAAAAA"), Value(25688));
      }
      end_time;

    }

    {
      start_time;
      for(size_t i = 0; i < i_end; i++) {
        Value v1 = plus(Value(2222), plus(Value(11), plus(Value(11), plus(Value(11222), Value(12222)))));
        Value v = plus(Value(""), v1);
      }
      end_time;

    }
    */
  /*
  Value value1 = Value(10);

  Value value2 = Value(12);

  Value value3 = value1.plus(value2);

  Value value4 = value3.plus(Value(2));

  value3.print();
  value4.print();
  plus(Value(1), Value(2)).print();




  Value value5 = Value("AAA");

  plus(Value(1), Value("aaa")).print();
  plus(Value("aaa"), Value(1)).print();

  plus(Value("aa"), Value("bb")).print();
  */