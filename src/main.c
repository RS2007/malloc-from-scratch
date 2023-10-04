#include "mymem.h"
#include <stdio.h>

typedef struct test_struct_t {
  int a;
  int b;
} test_struct_t;

void print_test(test_struct_t *test) { printf("%d %d\n", test->a, test->b); }

int main(void) {
  test_struct_t *test = (test_struct_t *)my_malloc(sizeof(test_struct_t));
  void *large_mem = my_malloc(500000);
  test->a = 3;
  test->b = 4;
  print_test(test);
  my_free(test);
  my_free(large_mem);
  return 0;
}
