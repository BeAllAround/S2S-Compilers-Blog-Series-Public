
#include <iostream>




class S {
    public:
    int i {3};
    int ii {98};
    int iii {100};

    S() {

    }

    void init() {
        i = 3;

        ii = 98;

        iii = 100;

    }


};


void init_S(S* _this) {
    _this->i = 3;
    _this->ii = 98;
    _this->iii = 100;
}

int main() {

    S* s = new S();


    std::cout << s << std::endl;

    void* rawMemory = ::operator new(sizeof(S));

    S* p = reinterpret_cast<S*>(rawMemory);
    p->init();

    // p = new (rawMemory) S(); // why does this work with S() then?


    std::cout << p << std::endl;

    std::cout << p->i << std::endl;
    std::cout << p->ii << std::endl;
    std::cout << p->iii << std::endl;

    delete s;

    delete p;

    return 0;
}



