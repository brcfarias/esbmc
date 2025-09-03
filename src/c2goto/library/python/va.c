#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct
{
  const void *value; // data pointer
  size_t type_id;    // hashed type name
} Object;

typedef struct
{
  Object *items;
  size_t size;      // elements in use
} List;

/* ---------- init ---------- */
static inline bool list_init(List *l, Object *backing)
{
  l->items = backing;
  l->size = 0;
  return true;
}

/* ---------- bounds check ---------- */
static inline bool list_in_bounds(const List *l, size_t index) {
  return index < l->size;
}

/* ---------- getters ---------- */
static inline Object *list_at(List *l, size_t index)
{
  return list_in_bounds(l, index) ? &l->items[index] : NULL;
}

static inline const Object *list_cat(const List *l, size_t index)
{
  return list_in_bounds(l, index) ? &l->items[index] : NULL;
}

/* ---------- push element ---------- */
static inline bool list_push(List *l, const void *value, size_t type_id)
{
  l->items[l->size].value = value;
  l->items[l->size].type_id = type_id;
  l->size++;
  return true;
}

/* ---------- replace element ---------- */
static inline bool
list_replace(List *l, size_t index, const void *value, size_t type_hash)
{
  if (index >= l->size)
    return false;
  l->items[index].value = value;
  l->items[index].type_id = type_hash;
  return true;
}

/* ---------- pop / erase ---------- */
static inline bool list_pop(List *l)
{
  if (l->size == 0)
    return false;
  l->size--;
  return true;
}

#if 0
static inline void list_free(List *a)
{
  a->size = 0;
}
#endif

/* ---------- type hashing ---------- */
static inline size_t list_hash_string(const char *str)
{
  size_t hash = 5381;
  int c;
  while ((c = *str++))
  {
    hash = ((hash << 5) + hash) + c;
  }
  return hash;
}

#if 0
// Macro to get a type hash dynamically
#define TYPE_HASH(T) list_hash_string(#T)

// Macro to push a string
#define list_push_str(array, str) list_push((array), (str), TYPE_HASH(char *))

#define list_get_as(array, index, ptr, T)                                        \
  do                                                                           \
  {                                                                            \
    const Object *obj = list_cat((array), (index));                       \
    *(ptr) = (obj && obj->type_hash == TYPE_HASH(T)) ? (T *)obj->value : NULL; \
  } while (0)

/* ---------- helper to check type ---------- */
#  define va_is_type(array, index, T)                                          \
    ({                                                                         \
      const Object *obj = va_get_cptr((array), (index));                       \
      obj && obj->type_hash == TYPE_HASH(T);                                   \
    })
#endif

typedef struct
{
  int x, y;
} Point;

int main(void)
{
  __attribute__((
    annotate("__ESBMC_inf_size"))) static Object storage[1];

  List l;
  if (!list_init(&l, storage))
    return 1;

  // push integer
  int iv = 42;
  list_push(&l, &iv, list_hash_string("int"));

  // push string (includes '\0')
  list_push(&l, "hello", list_hash_string("char *"));

  // push struct
  Point p = {3, 4};
  list_push(&l, &p, list_hash_string("Point"));

  // read with hash check
  const Object *o0 = list_cat(&l, 0);
  if (o0 && o0->type_id != list_hash_string("int"))
  {
    assert(0);
  }
  assert(*(int *)o0->value == 42);

  // read with automatic type check
#if 0
  int *int_ptr = NULL;
  list_get_as(&l, 0, &int_ptr, int);
  if (int_ptr)
  {
    printf("int: %d\n", *int_ptr);
  }
  else
  {
    assert(0);
  }
  assert(*int_ptr == 42);
#endif

  // read with hash check
  const Object *o1 = list_cat(&l, 1);
  if (o1 && o1->type_id != list_hash_string("char *"))
  {
    assert(0);
  }
  assert(strcmp((char *)(o1->value), "hello") == 0);

  // read with hash check
  const Object *o2 = list_cat(&l, 2);
  if (o2 && o2->type_id != list_hash_string("Point"))
  {
    assert(0);
  }
  assert(((Point *)o2->value)->x == 3);
  assert(((Point *)o2->value)->y == 4);

#if 0
   if (!va_is_type(&l, 0, int)) {
       assert(0);
   }
#endif

  // replace
  int nx = 777;
  list_replace(&l, 0, &nx, list_hash_string("int"));

  o0 = list_cat(&l, 0);
  assert(*(int *)o0->value == 777);

#if 0
  //erase index 1 (string) TODO: Mark element as invalid
   va_erase(&l, 1);
#endif

  // pop last (Point)
  list_pop(&l);

#if 0
  list_free(&l);
#endif

  return 0;
}
