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
    const Value & right) {
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

int main() {
  // Benchmark
  const size_t i_end = 1000000;

  srand(time(0));

  std::cout << "sizeof(Value): " << sizeof(Value) << std::endl;

  double _SUM = 0;

  const size_t RANGE = 100;

  for (size_t i = 0; i < RANGE; i++) {

    volatile size_t ii;

    // TODO: Deeper tests
    start_time;
    for (size_t i = 0; i < i_end; i++) {

      Value v = plus(Value(i + rand() % 11),
        plus(Value(i),
          plus(Value(i),
            plus(Value(i + 2222),
              plus(Value(11),
                plus(Value(11),
                  plus(Value(11222), Value(12222))))))));
      ii = v.get_num();
    }

    _SUM += static_cast < double > (clock() - s_t_a_r_t) / CLOCKS_PER_SEC;

  }

  std::cout << _SUM / RANGE << std::endl;

  return 0;
}