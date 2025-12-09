#include <assert.h>

extern int nondet_int(void);

int main(void)
{
  int x = nondet_int();
  // Constrain the domain so the solver has to decide feasibility.
  __ESBMC_assume(x >= 0 && x <= 4);

  // Property that is true for all allowed x except 1 and 3.
  assert(x != 1 && x != 3);
  return 0;
}
