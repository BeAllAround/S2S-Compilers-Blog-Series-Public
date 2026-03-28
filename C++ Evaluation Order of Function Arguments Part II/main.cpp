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
    const Value & right)  __attribute__((always_inline)) {
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


#define _plus(a, b) ({ \
    const auto& left = a; \
    const auto& right = b; \
    plus(left, right); \
}) \



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

  Value v1 = _plus(Value(_one() + rand() % 11),
    _plus(Value(_two()),
        _plus(Value(_three()),
            _plus(Value(_four() + 2222),
                _plus(Value(_five()),
                    _plus(Value(_six()),
                        _plus(Value(_seven()), Value(12222))))))));
 

  volatile double ii = v1.get_num();

  std::cout << ii << std::endl;




  return 0;
}
