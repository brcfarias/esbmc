#include <assert.h>
#include <pthread.h>

static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
static int shared = 0;

static void *worker(void *arg)
{
  (void)arg;
  pthread_mutex_lock(&m);
  shared++;
  pthread_mutex_unlock(&m);
  return 0;
}

int main(void)
{
  pthread_t t1, t2;

  pthread_create(&t1, 0, worker, 0);
  pthread_create(&t2, 0, worker, 0);

  pthread_join(t1, 0);
  pthread_join(t2, 0);

  assert(shared == 2);
  return 0;
}
