#include <assert.h>

int main(void)
{
  int acc = 0;
  for (int i = 0; i < 5; ++i)
    acc += i;

  assert(acc == 10);
  return 0;
}
