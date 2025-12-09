#include <assert.h>

int main(void)
{
  double x = 0.1;
  double y = 0.2;
  double z = x + y;

  assert(z > 0.299 && z < 0.301);
  return 0;
}
