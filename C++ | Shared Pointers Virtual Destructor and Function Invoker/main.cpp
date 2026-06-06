#include <iostream>

#include <memory>

class Base;
class Derived;

class Base {
  public:
    Base() {}
    Base(int _n) {
      _type = _n;
    }


    void get_attr(const char*) // virtual void get_attr(const char*) {}
    {
    	std::cout << "Base get_attr(const char*)" << std::endl;
    	return;
    }

    ~Base(); // virtual ~Base();

  private:
    int* n = new int;
    int _type { 0 };
};


class Derived : public Base {
  public:
    Derived() : Base(1) {
      n = new int(1);
    }

    void get_attr(const char*) {
        std::cout << "Derived get_attr(const char*)" << std::endl;
    	return;
    }
    

    ~Derived() {
      std::cout << "~Derived()" << std::endl;
      destroy();
    }

    void destroy() {
      delete n;
    }

  private:
    int* n;
    // std::shared_ptr<Base> ab = std::make_shared<Base>();
    
};

Base::~Base() {
  std::cout << "~Base()" << std::endl;

  delete n;

  /*
  if(_type == 1) {
    ((Derived*)this)->destroy();
  }
  */

}


template<class T>
struct Invoker {
	static void invoke() {
		std::cout << typeid(T).name() << std::endl;

	}
};

class In {
	private:
		void(*func)();
	public:

	template<class T>
		In(const T&in) {
      // NOTE: TAKES A SNAPSHOT OF YOUR FUNCTION AND PASSES IT AROUND AS IT DOESN'T ALTER THE FUNCTION TYPE SIGNATURE
			func = &Invoker<T>::invoke;
		}


	void invoke() {
		func();
	}
};



int main() {

  std::shared_ptr<Base> b = std::make_shared<Derived>();
  // std::unique_ptr<Base> b = std::make_unique<Derived>(); // LEAK
  // std::unique_ptr<Base> b (new Derived()); // LEAK


  // In _i = std::string("AAAA");

  // _i.invoke();

  /*
   * Works without virtual since the deleter is essentially the following for make_shared at compile time.
   * void _deleter<Derived>(void* b) {
   *  ((Derived*)b)->~Derived();
   *  ::operator delete((Derived*)b);
   * }
   */

  std::cout << "sizeof(Base) " << sizeof(Base) << std::endl;
  std::cout << "sizeof(Derived) " << sizeof(Derived) << std::endl;

  {
    /*
    Base* b = new Derived();
    
    ((Base*)b)->get_attr("AA"); // if Base get_attr is virtual, then it will be Derived get_attr even with Derived::get_attr(const char*) being non-virtual
    // NOTE: This is actually great since it is not possible to inline a virtual function

    std::cout << b << std::endl;
    */

   // ((Derived*)b)->~Derived();
   // ::operator delete((Derived*)b);
   // delete (Derived*)b;

  }


  return 0;
}
