#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

DEFINE_ESBMC_OVERFLOW_TYPE(char)

int main() {
  char a = __VERIFIER_nondet_char();
  char b = __VERIFIER_nondet_char();

  __ESBMC_overflow_result foo;

  foo = __ESBMC_overflow_result_plus(a, b);
  
  if (a == 100 && b == 100)
    assert(!foo.overflow);

  return 0;
}


