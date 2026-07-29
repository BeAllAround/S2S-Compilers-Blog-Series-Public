

# C++ | Optimizing Move Until We Cannot



| ![Decl](https://raw.githubusercontent.com/BeAllAround/S2S-Compilers-Blog-Series-Public/refs/heads/main/Logo-DeCL_black-icon01.png) | Motivated and inspired by Decl. In development and soon to be open-sourced. |
| ------------------------------------------------------------ | ------------------------------------------------------------ |





## C++ | Optimizing Move Until We Cannot

For the video overview of this week, let's go with [C++ Optimizing Move and Destructor Semantics](https://youtu.be/mBO-YWfFJWQ) and optimize the C++ move semantics until it is not possible to optimize any more.



Consider the following code:

```cpp
std::vector<S> v;

S s = S(1);

v.push_back(std::move(s));
```

This code produces the following C++ RAII output:

```cpp
S(int) // OK! This is a just constructor
S(S&&) // OVERHEAD! Move involved in order to "move" the entity into the destination memory region of the vector item
~S()   // OVERHEAD! Destructor invoked for the rvalue at the end of the scope
~S()   // OK! Destructor invoked by the "~std::vector<S>()"
```



The question here is _"Why would the moved value even be eligible for a destructor if it is completely dumped or unused?"_ - the obvious answer is that C++ is required to take care of all the _hanging_ objects once they get out of scope.



The output above is also the case _even for_ a `prvalue` such as:

```cpp
std::vector<S> v;

v.push_back(S(1));
```

> It is also important to note that these overheads are ***not*** optimized at deeper optimization levels such as `-O1`, `-O2`, `-O3`. This is purely a C++ thing.



This is because the `std::vector::push_back` accepts an `T&& value` as the argument, which needs to be `std::move`d eventually and, inevitably.

For example,

```cpp
void push_back(T&& item) {
  new(&buffer[stack_count]) T(std::move(item));
  stack_count++;
}
```

For a standard library that is a `std::vector`, this is done via the allocator constructor so it does essentially come down to the move constructor.

```cpp
template<typename... _Args>
void construct(pointer __p, _Args&&... __args)
{
  ::new((void *)__p) _Tp(std::forward<_Args>(__args)...);
}
```

To fully understand the scope of this issue and in an attempt to narrow it down, we must implement something similar to the `std::vector` container given that we don't have the full control over a container like the `std::vector`.



We are gonna use the simple static storage implementation from the [C++ | Static Storage Implementation](https://youtu.be/CRrevxPuIq4) video.

The usage of it is as simple as the following:

```cpp
StaticStorage<S, 3> list;

list.add(S(1));
```

Just like that, we have manage to replicate the same C++ RAII flow:

```cpp
S(int) // OK! This is a just constructor
S(S&&) // OVERHEAD! Move involved in order to "move" the entity into the destination memory region of the vector item
~S()   // OVERHEAD! Destructor invoked for the prvalue at the end of the expression - the semi colon
~S()   // OK! Destructor invoked by the "~StaticStorage()"
```

With the following code, we are able to evaluate the lifespan of the "hanging" `prvalue` and prove that its lifetime ends at the end of the expression: the semi colon.

```cpp
{
  std::cout << "START" << std::endl;
  S(1);
  // Implicit ~S() invoked here
  std::cout << "END" << std::endl;
    
    
  std::cout << "START" << std::endl;
  S(3);
  // Implicit ~S() invoked here
  std::cout << "END" << std::endl;
  
  std::cout << "START" << std::endl;
  make_s(10, 3); // S make_s(int, int);
  // Implicit ~S() invoked here
  std::cout << "END" << std::endl;

}
// Outputs
/*
START
S(int)
~S()
END
START
S(int)
~S()
END
START
S(int)
~S()
END
*/
```



Nevertheless, the first thing to do here is not to have a function at all _since_ the function in C++ needs additional work like `std::move` to cast it back to the original type given that:

```cpp
void add(T&& item) {
  // Any time we use item, it is an lvalue so std::move(item) is required!
}
```



The closing thing to a function that avoids the standard function arguments is, of course, a macro - particularly the statement expression one.

So if we turn the `add` function into a macro, we get something like the following:

```cpp
#define _storage_add(storage, item, T) ({ \
    new(&storage.buffer[storage.stack_count * sizeof(T)]) item; \
    storage.stack_count++; \
})
```



Now, if we do something like this:

```cpp
StaticStorage<S, 3> list;

_storage_add(list, S(12), S)
```

This produces the following C++ RAII:

```cpp
S(int) // OK! This is a just constructor
~S()   // OK! Destructor invoked by the "~StaticStorage()"
```





The code, when replaced, at the preprocessor level doesn't apply any `moves` whatsoever:

It is a direct new placement so any overheads like `move` are optimized out since they are not necessary for the construction of an object.

```cpp
new(&storage.buffer[storage.stack_count * sizeof(S)]) S(12);
```



We have successfully optimized out the unnecessary move - but for a `prvalue` after all.

_What about_ the `rvalue` that we need to move out of the scope and, still, eliminate the move and the destructor invocation for the moved value that is, of course, considered "dumped", "unused", or "redundant" at that point so why have the bottleneck of C++ RAII?

Let's say there is some work that we need to do on the `s` before it becomes a part of the container buffer - either pushed or added in any way.

```cpp
StaticStorage<S, 3> list;

S s = S(1);

*s.i_ptr += n + n1; // Work with the s

// We are ready to add it onto our buffer, but it still needs to be moved!
_storage_add(list, S(std::move(s)), S);
```

We might have gotten a couple of steps forward, but we are back to square one with the following output:

```cpp
S(int) // OK! This is a just constructor
S(S&&) // OVERHEAD! Move involved in order to "move" the entity into the destination memory region of the vector item
~S()   // OVERHEAD! Destructor invoked for the rvalue at the end of the scope
~S()   // OK! Destructor invoked by the "~StaticStorage()"
```



There is one more trick we can pull out of our sleeve: **that is** the function _copy elision_.

In C++, returning a [temporary] local object by value often avoids any copy or move operations thanks to copy elision (also known as [(Named) Return Value Optimization](https://en.cppreference.com/cpp/language/copy_elision)). The compiler constructs the object directly in the caller's storage, effectively keeping the original object instead of creating a temporary copy.

So if we wrap up the work that needs to be done on your instance,

```cpp
inline S make_s(int n, int n1) __attribute__((always_inline));

inline S make_s(int n, int n1) {
  S s = S(n + n1);
  
  *s.i_ptr += n;
  
  // Copy Elision
  return s;
}
```

Please, note that we made the function `inline-able` and we are even forcing the inlining to each of its references in the code. This is done due to the fact that we must avoid the `CALL` overhead that function calls produce in our assembly. The call overhead involves `push + pop + ret` so it is definitely not something we can afford.

We are not using a lambda here either as it not a guarantee that all of the lambda calls will be inlined by the compiler.

Without further ado, let's add this onto our buffer storage!

```cpp
StaticStorage<S, 3> list;

_storage_add(list, S(make_s(n, n1)), S);
```

This, syntactically, looks like the new placement will apply just another copy or move - _however_, **THAT IS NOT *THE CASE*!**

The output of C++ RAII becomes:

```cpp
S(int) // // OK! This is a just constructor
~S()  // OK! Destructor invoked by the "~StaticStorage()"
```

This is because if the function return type in C++ is **non-reference type (by value or pointer)**, the return value of the expression becomes the `prvalue` so the compiler effectively optimizes this as:

```cpp
new(&storage.buffer[storage.stack_count]) S(/* Determined at runtime by make_s */);
```

This behavior is identical to the following:

```cpp
struct S {
    S(int x) { /* ... */ }
    
    // Testing it with copy and move deleted:
    S(const S&) = delete;
    S(S&&) = delete;
};

S s = make_s( ... ); // C++17+ Guaranteed Copy Elision. No copy or move. 
// Similar to
S s = S(1); // Direct Construction. Guaranteed no copy or move since C++17.
```



With that being our final optimization, we have reached the breaking point and the hard stop, thusly, we are not able to optimize it _any further_!



## Conclusion and YouTube Video

Just like with every other video and overview of mine, you should apply these "black-magic" optimizations only under specific circumstances and for particular use cases that truly require such aggressive optimization techniques — not in your general code. That said, studying them can still sharpen your understanding of C++.

Keep in mind that this is just a brief overview of the YouTube video! For the full experience with all the details we missed here, check out my [YouTube channel](https://www.youtube.com/@AlexMilenkovic-ks7xd) and [C++ Optimizing Move and Destructor Semantics](https://youtu.be/mBO-YWfFJWQ).

For the source code of this overview and the video, check out my GitHub repository at https://github.com/BeAllAround/S2S-Compilers-Blog-Series-Public.







