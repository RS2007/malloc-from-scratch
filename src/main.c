#include "mymem.h"
#include <stdio.h>

typedef struct test_struct_t {
  int a;
  int b;
} test_struct_t;

void print_test(test_struct_t *test) { printf("%d %d\n", test->a, test->b); }

int main(void) {
  test_struct_t *test = malloc(sizeof(test_struct_t));
  test->a = 3;
  test->b = 4;
  print_test(test);
  return 0;
}
