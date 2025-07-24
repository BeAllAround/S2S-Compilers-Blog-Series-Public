#include <stdio.h>
#include <stdlib.h>


typedef struct {
  int* data;
} S;

void clean_up_S(S *s)
{
  printf("clean_up %p\n", s);
  if(s->data != NULL) {
    free(s->data);
    s->data = NULL;
  }
}

static inline S create_s(int n) {
  S s;

  s.data = malloc(sizeof(int)*1);

  *s.data = n;

  return s;
}

// NOTE: Essentially, MOVE
static inline S* assign(S* this, S other) {
  clean_up_S(this);
  this->data = other.data;
  other.data = NULL;
  return this;
}

static inline S* assign_copy(S* this, S other) {
  clean_up_S(this);
  this->data = malloc(sizeof(int)*1);
  *(this->data) = *other.data;
  return this;
}

void test_0() {
  S s __attribute__ ((__cleanup__(clean_up_S))) = create_s(1);

  return;
}

int check(S* s) {
  return s->data == NULL;
}

int test_1() {
  S s __attribute__ ((__cleanup__(clean_up_S))) = create_s(1);

  return check(&s);
}

void test_goto() {
  volatile int c = 0;

  static void* table[] = { &&foo, &&bar };

  goto *table[0];


  foo: {
      S s __attribute__ ((__cleanup__(clean_up_S))) = create_s(1);

      static void* _table[] = { &&_exit };

       if(c == 100) return;

       c++;
       // goto bar; // OK
       // goto *table[1]; // LEAK
       // goto *table[0]; // LEAK
       // goto foo; // OK

       goto *_table[0]; // OK

      _exit: {
        S s __attribute__ ((__cleanup__(clean_up_S))) = create_s(1);
         return;
       }



     }

  bar: {
        S s __attribute__ ((__cleanup__(clean_up_S))) = create_s(1);
       }

  return;
}

int main() {
  S s __attribute__ ((__cleanup__(clean_up_S))) = create_s(1);
  S s1 __attribute__ ((__cleanup__(clean_up_S))) = create_s(3);

  // s = create_s(2);
  // assign(&s, create_s(2));

  // s = s1; // double free
  // assign(&s, s1);
  assign_copy(&s, s1);

  test_0();

  test_goto();

  printf("%d\n", test_1());

  return 0;
}
