#include <memory>

#include <iostream>

#include <cassert>


#define start_time clock_t s_t_a_r_t = clock();

#if __GNUG__
#define end_time printf("[Cpu_time_used: %f]\n", static_cast<double>(clock() - s_t_a_r_t) / CLOCKS_PER_SEC);
#else
#define end_time printf("[Cpu_time_used: %f]\n", ((double) (clock() - s_t_a_r_t)) / CLOCKS_PER_SEC);
#endif

using namespace std;

struct TreeNode {

    int val { 0 };

    shared_ptr<TreeNode> left { nullptr };
    shared_ptr<TreeNode> right { nullptr };

    TreeNode() {}

    TreeNode(int val) : val{val} {}

};


void print_inorder(const shared_ptr<TreeNode>& node) {

    if(node.get() == nullptr) {
        return;
    }

    print_inorder(node.get()->left);

    std::cout << node.get()->val << " ";

    print_inorder(node.get()->right);

}

struct CallFrame {

    // args
    struct Args {
        shared_ptr<TreeNode> node { nullptr };
    } args;


    // Return Vals
    struct Ret {
        void* origin { nullptr };
    } ret;

    CallFrame() {}

    CallFrame(const shared_ptr<TreeNode>& node) {
        args.node = node;
    }


    CallFrame(const CallFrame& other) {
        args.node = other.args.node;
        ret.origin = other.ret.origin;
    }


    CallFrame(CallFrame&& other) {
        args.node = std::move(other.args.node);
        ret.origin = std::move(other.ret.origin); // NOT NEEDED FOR PRIMITIVE
    }

    CallFrame& operator=(const CallFrame& other) {
        args.node = other.args.node;
        ret.origin = other.ret.origin;
        return *this;
    }

    CallFrame& operator=(CallFrame&& other) {
        args.node = std::move(other.args.node);
        ret.origin = std::move(other.ret.origin); // NOT NEEDED FOR PRIMITIVE
        return *this;
    }

    ~CallFrame() {}

};


// Translation based on the function above
/*
print_inorder(std::shared_ptr<TreeNode> const&):
        push    rbx
        mov     rbx, rdi
        mov     rdi, QWORD PTR [rdi]
        test    rdi, rdi
        je      .L12
        add     rdi, 8
        call    print_inorder(std::shared_ptr<TreeNode> const&)
        mov     rax, QWORD PTR [rbx]
        mov     esi, DWORD PTR [rax]
        mov     edi, OFFSET FLAT:std::cout
        call    std::ostream::operator<<(int)
        mov     rdi, rax
        mov     edx, 1
        mov     esi, OFFSET FLAT:.LC0
        call    std::basic_ostream<char, std::char_traits<char>>& std::__ostream_insert<char, std::char_traits<char>>(std::basic_ostream<char, std::char_traits<char>>&, char const*, long)
        mov     rdi, QWORD PTR [rbx]
        add     rdi, 24
        call    print_inorder(std::shared_ptr<TreeNode> const&)
.L12:
        pop     rbx
        ret
*/

// Cannot be inlined!
// inline void _print_order(const shared_ptr<TreeNode>& _node) __attribute__((always_inline));

void _print_order(const shared_ptr<TreeNode>& _node) {

    // NOTE: CAN'T INLINE THIS FUNCTION DUE TO THE COMPUTED GOTO: YUCK!
    // __label__ call1_origin;
    // __label__ call2_origin;

    // void *jmps_origin[] = { &&call1_origin, &&call2_origin };

    // Can be dynamically allocated stack, which means we can call it as long as there is memory available
    static CallFrame stack[20];

    // static size_t stack_c = 0;
    size_t stack_c = 0;

    // PUSH
    stack[stack_c] = CallFrame(_node);
    stack_c++;


    // CALL
    _print_in_order: {}

    // function args
    const shared_ptr<TreeNode>& node = stack[stack_c-1].args.node; // last: stack_c-1

    { // FUNCTION BODY
        if(node.get() == nullptr) {
            goto _ret;
        }

        // _print_in_order(node.get()->left);
        {
            // NOTE: Limit the scope of a by declaring it within a block statement that ends before the label.
            // REFERENCE and KUDOS TO: https://stackoverflow.com/a/70944477/14102601
            CallFrame call1 = CallFrame();
            call1.args.node = node.get()->left;
            call1.ret.origin = &&call1_origin;
            // PUSH
            stack[stack_c++] = std::move(call1);

            // CALL
            goto _print_in_order;
        }
        // ORIGIN OF RET
        call1_origin: {}

        {
            // NOTE: NEEDS TO BE HERE SINCE AFTER COMPUTED GOTO TO CALL1_ORIGIN: THE NODE ABOVE IS LOST
            const shared_ptr<TreeNode>& node = stack[stack_c-1].args.node; // last: stack_c-1
            // std::cout << "CALL1 RET: " << stack_c << std::endl;
            std::cout << node.get()->val << " ";
        }

        // _print_in_order(node.get()->right);
        {
            const shared_ptr<TreeNode>& node = stack[stack_c-1].args.node;

            CallFrame call2 = CallFrame();

            call2.args.node = node.get()->right;
            call2.ret.origin = &&call2_origin;

            // PUSH
            stack[stack_c++] = std::move(call2);

            // CALL
            goto _print_in_order;
        }
        // ORIGIN OF RET
        call2_origin: {}

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
        stack_c--;

        // NOTE: INITIAL PUSH ON LINE 137
        if(stack_c == 0) {
            goto _done;
            // return;
        }

        // std::cout << "stack_c: " << stack_c << std::endl;


        // ret
        void* ret_origin = stack[stack_c].ret.origin;
        goto *ret_origin;

        // const shared_ptr<TreeNode>& node = stack[stack_c].args.node;


        // if(node.get() != nullptr) {
            // std::cout << node.get()->val << std::endl;

        // }


        /*

        if(ret_origin == &&call1_origin)  {
            goto *jmps_origin[0];
        } else {
            goto *jmps_origin[1];
        }
        */

        // std::cout << "AAA" << std::endl;

    }


    _done: {}


    assert(stack_c == 0);

}



#define _SIZE 10000

shared_ptr<TreeNode> _make_root() {
    shared_ptr<TreeNode> root = make_shared<TreeNode>(4);

    root.get()->left = make_shared<TreeNode>(2);
    root.get()->right = make_shared<TreeNode>(5);

    root.get()->left.get()->left = make_shared<TreeNode>(1);
    root.get()->left.get()->right = make_shared<TreeNode>(3);

    root.get()->right.get()->left = make_shared<TreeNode>(6);
    root.get()->right.get()->right = make_shared<TreeNode>(7);

    root.get()->right.get()->left->left = make_shared<TreeNode>(0);
    root.get()->right.get()->left->right = make_shared<TreeNode>(-1);

    root.get()->right.get()->right->left = make_shared<TreeNode>(8);
    root.get()->right.get()->right->right = make_shared<TreeNode>(9);

    root.get()->right.get()->right->right->left = make_shared<TreeNode>(77);
    root.get()->right.get()->right->right->right = make_shared<TreeNode>(88);


    root.get()->right.get()->left->left->left = make_shared<TreeNode>(99);
    root.get()->right.get()->left->left->right = make_shared<TreeNode>(111);


    return root;
}




void benchmark_01() {
    shared_ptr<TreeNode> root = _make_root();

    start_time;
    for(size_t i = 0; i < _SIZE; i++) {
        _print_order(root);
        std::cout << std::endl;
    }

    end_time;


}


void benchmark_02() {
    shared_ptr<TreeNode> root = _make_root();

    start_time;
    for(size_t i = 0; i < _SIZE; i++) {
        print_inorder(root);
        std::cout << std::endl;
    }

    end_time;

}


int main() {

    shared_ptr<TreeNode> root = make_shared<TreeNode>(4);


    root.get()->left = make_shared<TreeNode>(2);
    root.get()->right = make_shared<TreeNode>(5);

    root.get()->left.get()->left = make_shared<TreeNode>(1);
    root.get()->left.get()->right = make_shared<TreeNode>(3);

    root.get()->right.get()->left = make_shared<TreeNode>(6);
    root.get()->right.get()->right = make_shared<TreeNode>(7);


    print_inorder(root);
    std::cout << std::endl;


    _print_order(root);
    std::cout << std::endl;


    // benchmark_01();

    // benchmark_02();


    return 0;
}
