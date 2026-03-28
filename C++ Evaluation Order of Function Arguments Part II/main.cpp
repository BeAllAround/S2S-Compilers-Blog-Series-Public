#include <iostream>

#define start_time clock_t s_t_a_r_t = clock();

#if __GNUG__
#define end_time printf("[Cpu_time_used: %f]\n", static_cast < double > (clock() - s_t_a_r_t) / CLOCKS_PER_SEC);
#else
#define end_time printf("[Cpu_time_used: %f]\n", ((double)(clock() - s_t_a_r_t)) / CLOCKS_PER_SEC);
#endif

enum class ValueType {
  NUM,
  CHAR,
  BOOL,
  _NULL,

  STR,
  DICT,
  ARRAY,

  V1,
  V2,
  V3,
  V4,
  V5,
  V6,
  V7
};

class Value_int {
  double value {
    0
  };
  public:
    Value_int(const double & v): value {
      v
    } {}

  double get_int() const {
    return value;
  }

  friend inline Value_int plus(const Value_int & left,
    const Value_int & right) {
    return left.get_int() + right.get_int();
  }
};

// #define _DEFAULT_BRANCH() break
#define _DEFAULT_BRANCH() __builtin_unreachable() 

class Value {
  private: ValueType type;

  union {
    int num;
    char c;
    std::string str;
    // std::vector<int> arr;
  };

  public: Value(const Value & value) {}

  explicit Value(std::string && str): str {
    std::move(str)
  },
  type {
    ValueType::STR
  } {
    // new(&this->str) std::string{std::move(str)};
  }

  explicit Value(int num): num {
    num
  },
  type {
    ValueType::NUM
  } {}

  ValueType get_type() const {
    return type;
  }

  void print() {
    switch (get_type()) {
    case ValueType::NUM: {
      std::cout << num << std::endl;
      break;
    }
    case ValueType::STR: {
      std::cout << str << std::endl;
      break;
    }
    default: {
      _DEFAULT_BRANCH();
    }
    }

  }
  int get_num() const {
    return num;
  }

  const std::string & get_string() const {
      return str;
    }

    ~Value() {
      switch (get_type()) {
      case ValueType::NUM:
      case ValueType::CHAR:
      case ValueType::BOOL:
      case ValueType::_NULL:
        break;
      case ValueType::STR: {
        str.~basic_string();
        break;
      }
      default: {
        _DEFAULT_BRANCH();
      }
      }
    }

  friend inline Value plus(const Value & left,
    const Value & right)  {
    switch (left.get_type()) {
    case ValueType::NUM: {
      switch (right.get_type()) {
      case ValueType::NUM: {
        return Value(left.get_num() + right.get_num());
      }
      case ValueType::CHAR: {
        break;
      }
      case ValueType::BOOL: {
        break;
      }
      case ValueType::_NULL: {
        break;
      }
      case ValueType::STR: {
        return Value(std::to_string(left.get_num()) + right.get_string());
      }
      case ValueType::DICT: {
        break;
      }

      case ValueType::V1: {
        std::cout << "V1" << std::endl;
        break;
      }
      case ValueType::V2: {
        std::cout << "V2" << std::endl;
        break;
      }
      case ValueType::V3: {
        std::cout << "V3" << std::endl;
        break;
      }
      case ValueType::V4: {
        std::cout << "V4" << std::endl;
        break;
      }
      case ValueType::V5: {
        std::cout << "V5" << std::endl;
        break;
      }
      default: {
        _DEFAULT_BRANCH();
      }

      }
    }
    case ValueType::CHAR: {
      break;
    }
    case ValueType::BOOL: {
      break;
    }
    case ValueType::_NULL: {
      break;
    }
    case ValueType::STR: {
      switch (right.get_type()) {
      case ValueType::STR: {
        return Value(std::to_string(left.get_num()) + right.get_string());
      }
      case ValueType::NUM: {
        return Value(left.get_string() + std::to_string(right.get_num()));
      }
      case ValueType::V1: {
        std::cout << "V1" << std::endl;
      }
      case ValueType::V2: {
        std::cout << "V2" << std::endl;
      }
      case ValueType::V3: {
        std::cout << "V3" << std::endl;
      }
      case ValueType::V4: {
        std::cout << "V4" << std::endl;
      }
      case ValueType::V5: {
        std::cout << "V5" << std::endl;
      }

      default: {
        _DEFAULT_BRANCH();

      }
      }
    }
    case ValueType::DICT: {
      break;
    }
    case ValueType::V1: {
      std::cout << "V1" << std::endl;
      break;
    }
    case ValueType::V2: {
      std::cout << "V2" << std::endl;
      break;
    }
    case ValueType::V3: {
      std::cout << "V3" << std::endl;
      break;
    }
    case ValueType::V4: {
      std::cout << "V4" << std::endl;
      break;
    }
    case ValueType::V5: {
      std::cout << "V5" << std::endl;
      break;
    }
    case ValueType::V6: {
      std::cout << "V6" << std::endl;
      break;
    }
    case ValueType::V7: {
      std::cout << "V7" << std::endl;
      break;
    }
    default: {
      _DEFAULT_BRANCH();
    }
    }
    return Value(0);
  }

};

double plus(double left, double right) {
  return left + right;
}


double _one() {
    std::cout << "one()" << std::endl;
    return 1;
}

double _two() {
    std::cout << "two()" << std::endl;
    return 2;
}

double _three() {
    std::cout << "three()" << std::endl;
    return 3;
}

double _four() {
    std::cout << "four()" << std::endl;
    return 4;
}

double _five() {
    std::cout << "five()" << std::endl;

    return 5;
}

double _six() {
    std::cout << "six()" << std::endl;
    return 6;
}

double _seven() {
    std::cout << "seven()" << std::endl;
    return 7;
}


#define _plus(whence, a, b) ({ \
    const auto& left = a; \
    const auto& right = b; \
    printf("whence: %s\n", whence); \
    plus(left, right); \
}) \

/*

// gcc -E output after the preprocessor
Value v1 = ({
  const auto& left = Value(_one() + rand() % 11);
  const auto& right = ({
    const auto& left = Value(_two());
    const auto& right = ({
      const auto& left = Value(_three());
      const auto& right = ({
        const auto& left = Value(_four() + 32222);
        const auto& right = ({
          const auto& left = Value(_five());
          const auto& right = ({
            const auto& left = Value(_six());
            const auto& right = ({
              const auto& left = Value(_seven());
              const auto& right = Value(12222);
              printf("whence: %s\n", "7");
              plus(left, right);
            });
            printf("whence: %s\n", "6");
            plus(left, right);
          });
          printf("whence: %s\n", "5");
          plus(left, right);
        });
        printf("whence: %s\n", "4");
        plus(left, right);
      });
      printf("whence: %s\n", "3");
      plus(left, right);
    });
    printf("whence: %s\n", "2");
    plus(left, right);
  });
  printf("whence: %s\n", "1");
  plus(left, right);
})
# 432 "/app/example.cpp";
*/

/*
        addsd   xmm0, QWORD PTR [rsp+8]
        cvttsd2si       eax, xmm0
        mov     DWORD PTR [rsp+24], eax
        call    _two()
        mov     DWORD PTR [rsp+64], 0
        cvttsd2si       eax, xmm0
        mov     DWORD PTR [rsp+72], eax
        call    _three()
        mov     DWORD PTR [rsp+112], 0
        cvttsd2si       eax, xmm0
        mov     DWORD PTR [rsp+120], eax
        call    _four()
        mov     DWORD PTR [rsp+160], 0
        addsd   xmm0, QWORD PTR .LC23[rip]
        cvttsd2si       eax, xmm0
        mov     DWORD PTR [rsp+168], eax
        call    _five()
        mov     DWORD PTR [rsp+208], 0
        cvttsd2si       eax, xmm0
        mov     DWORD PTR [rsp+216], eax
        call    _six()
        mov     DWORD PTR [rsp+256], 0
        cvttsd2si       eax, xmm0
        mov     DWORD PTR [rsp+264], eax
        call    _seven()
        mov     DWORD PTR [rsp+304], 0
        cvttsd2si       eax, xmm0
        mov     DWORD PTR [rsp+312], eax
        mov     DWORD PTR [rsp+400], 0
        mov     DWORD PTR [rsp+408], 12222
        lea     rdi, [rsp+352]
        lea     rdx, [rsp+400]
        lea     rsi, [rsp+304]
        call    plus(Value const&, Value const&) # assembly call 7
        lea     rbx, [rsp+400]
        mov     rdi, rbx
        call    Value::~Value() [complete object destructor] # RValue destruct at the end of the expression (semi colon) but in this case after the macro is replaced at the preproecssor level it is const auto& ref.
        lea     rdi, [rsp+304]
        call    Value::~Value() [complete object destructor]
        lea     rdx, [rsp+352]
        lea     rsi, [rsp+256]
        mov     rdi, rbx
        call    plus(Value const&, Value const&) # assembly call 6
        lea     rbx, [rsp+352]
        mov     rdi, rbx
        call    Value::~Value() [complete object destructor]
        lea     rdi, [rsp+256]
        call    Value::~Value() [complete object destructor]
        lea     rdx, [rsp+400]
        lea     rsi, [rsp+208]
        mov     rdi, rbx
        call    plus(Value const&, Value const&) # assembly call 5 ... etc down the tree of calls below
        lea     rbx, [rsp+400]
        mov     rdi, rbx
        call    Value::~Value() [complete object destructor]
        lea     rdi, [rsp+208]
        call    Value::~Value() [complete object destructor]
        lea     rdx, [rsp+352]
        lea     rsi, [rsp+160]
        mov     rdi, rbx
        call    plus(Value const&, Value const&)
        lea     rbx, [rsp+352]
        mov     rdi, rbx
        call    Value::~Value() [complete object destructor]
        lea     rdi, [rsp+160]
        call    Value::~Value() [complete object destructor]
        lea     rdx, [rsp+400]
        lea     rsi, [rsp+112]
        mov     rdi, rbx
        call    plus(Value const&, Value const&)
        lea     rbx, [rsp+400]
        mov     rdi, rbx
        call    Value::~Value() [complete object destructor]
        lea     rdi, [rsp+112]
        call    Value::~Value() [complete object destructor]
        lea     rdx, [rsp+352]
        lea     rsi, [rsp+64]
        mov     rdi, rbx
        call    plus(Value const&, Value const&)
        lea     rdi, [rsp+352]
        call    Value::~Value() [complete object destructor]
        lea     rdi, [rsp+64]
        call    Value::~Value() [complete object destructor]
        lea     rdi, [rsp+448]
        mov     rdx, rbx
        lea     rsi, [rsp+16]
        call    plus(Value const&, Value const&)
        mov     rdi, rbx
        call    Value::~Value() [complete object destructor]
        lea     rdi, [rsp+16]
        call    Value::~Value() [complete object destructor]
        pxor    xmm0, xmm
*/


int main() {
  // Benchmark
  const size_t i_end = 1;

  srand(time(0));

  std::cout << "sizeof(Value): " << sizeof(Value) << std::endl;

  double _SUM = 0;


/*
  Value v1 = plus(Value(_one() + rand() % 11),
    plus(Value(_two()),
        plus(Value(_three()),
            plus(Value(_four() + 2222),
                plus(Value(_five()),
                    plus(Value(_six()),
                        plus(Value(_seven()), Value(12222))))))));
*/

  Value v1 = _plus("1", Value(_one() + rand() % 11), // assembly call 7
    _plus("2", Value(_two()), // assembly call 6
        _plus("3", Value(_three()), // assembly call 5
            _plus("4", Value(_four() + 32222), // assembly call 4
                _plus("5", Value(_five()), // assembly call 3
                    _plus("6", Value(_six()), // assembly call 2
                        _plus("7", Value(_seven()), Value(12222)) //  assembly call 1
                    )
                )
            )
        )
      )
  );
 

  volatile double ii = v1.get_num();

  std::cout << ii << std::endl;




  return 0;
}
