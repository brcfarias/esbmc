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
  size_t size; // elements in use
} List;

/* ---------- init ---------- */
static inline bool list_init(List *l, Object *backing)
{
  l->items = backing;
  l->size = 0;
  return true;
}

/* ---------- bounds check ---------- */
static inline bool list_in_bounds(const List *l, size_t index)
{
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

static inline void *list_get_as(const List *l, size_t i, size_t expect_type)
{
  const Object *o = list_cat(l, i);
  return (o && o->type_id == expect_type) ? (void *)o->value : NULL;
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
list_replace(List *l, size_t index, const void *new_value, size_t type_id)
{
  if (index >= l->size)
    return false;
  l->items[index].value = new_value;
  l->items[index].type_id = type_id;
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
#  define TYPE_HASH(T) list_hash_string(#  T)

// Macro to push a string
#  define list_push_str(array, str) list_push((array), (str), TYPE_HASH(char *))

/* ---------- helper to check type ---------- */
#  define list_is_type(array, index, T)                                          \
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
  __attribute__((annotate("__ESBMC_inf_size"))) static Object storage[1];

  List l;
  list_init(&l, storage);

  // push integer
  int iv = 42;
  assert(list_push(&l, &iv, list_hash_string("int")));

  // push string (includes '\0')
  assert(list_push(&l, "hello", list_hash_string("char *")));

  // push struct
  Point p = {3, 4};
  list_push(&l, &p, list_hash_string("Point"));

  /* check with typed accessor */
  int *ip = (int *)list_get_as(&l, 0, list_hash_string("int"));
  assert(ip && *ip == 42);

  char *sp = (char *)list_get_as(&l, 1, list_hash_string("char *"));
  assert(sp && strcmp(sp, "hello") == 0);

  Point *pp = (Point *)list_get_as(&l, 2, list_hash_string("Point"));
  assert(pp && pp->x == 3 && pp->y == 4);

  // replace
  int nx = 777;
  assert(list_replace(&l, 0, &nx, list_hash_string("int")));

  ip = (int *)list_get_as(&l, 0, list_hash_string("int"));
  assert(ip && *ip == 777);

#if 0
  //erase index 1 (string) TODO: Mark element as invalid
   va_erase(&l, 1);
#endif

  /* pop last (Point) */
  assert(list_pop(&l));

  return 0;
}
