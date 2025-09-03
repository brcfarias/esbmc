#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct
{
  const void *value; // pointer to data
  size_t type_hash;  // type name hash
} Object;

typedef struct
{
  Object *objects;
  size_t size; // number of elements in use
} VarArray;

/* ---------- lifecycle ---------- */
static inline bool va_init(VarArray *a, Object *objects)
{
  a->objects = objects;
  a->size = 0;
  return true;
}

static inline bool va_push_copy(VarArray *a, const void *data, size_t type_hash)
{
  a->objects[a->size].value = data;
  a->objects[a->size].type_hash = type_hash;
  a->size++;
  return true;
}

static inline bool
va_replace_copy(VarArray *a, size_t index, const void *data, size_t type_hash)
{
  if (index >= a->size)
    return false;
  a->objects[index].value = data;
  a->objects[index].type_hash = type_hash;
  return true;
}

/* ---------- getters ---------- */
static inline const Object *va_get_cptr(const VarArray *a, size_t index)
{
  if (index >= a->size)
    return NULL;
  return &a->objects[index];
}

static inline Object *va_get_ptr(VarArray *a, size_t index)
{
  if (index >= a->size)
    return NULL;
  return &a->objects[index];
}

/* ---------- pop / erase ---------- */
static inline bool va_pop(VarArray *a)
{
  if (a->size == 0)
    return false;
  a->size--;
  return true;
}

static inline size_t va_hash_string(const char *str)
{
  size_t hash = 5381;
  int c;
  while ((c = *str++))
  {
    hash = ((hash << 5) + hash) + c;
  }
  return hash;
}

static inline void va_free(VarArray *a)
{
  a->size = 0;
}

// Macro to get a type hash dynamically
#define VA_TYPE_HASH(T) va_hash_string(#T)

// Macro to push a string
#define va_push_str(array, str)                                                \
  va_push_copy((array), (str), VA_TYPE_HASH(char *))

#define va_get_as(array, index, ptr, T)                                        \
  do                                                                           \
  {                                                                            \
    const Object *obj = va_get_cptr((array), (index));                         \
    *(ptr) =                                                                   \
      (obj && obj->type_hash == VA_TYPE_HASH(T)) ? (T *)obj->value : NULL;     \
  } while (0)

/* ---------- helper para verificar tipo ---------- */
// #define va_is_type(array, index, T) \
//     ({ \
//         const Object *obj = va_get_cptr((array), (index)); \
//         obj && obj->type_hash == VA_TYPE_HASH(T); \
//     })

typedef struct
{
  int x, y;
} Point;

int main(void)
{
  __attribute__((
    annotate("__ESBMC_inf_size"))) static Object __ESBMC_objects[1];

  VarArray a;
  if (!va_init(&a, __ESBMC_objects))
    return 1;

  // push integer
  int iv = 42;
  va_push_copy(&a, &iv, va_hash_string("int"));

  // push string (includes '\0')
  va_push_str(&a, "hello");

  // push struct
  Point p = {3, 4};
  va_push_copy(&a, &p, va_hash_string("Point"));

  // read with hash check
  const Object *o0 = va_get_cptr(&a, 0);
  if (o0 && o0->type_hash != VA_TYPE_HASH(int))
  {
    assert(0);
  }
  assert(*(int *)o0->value == 42);

  // read with automatic type check
  int *int_ptr = NULL;
  va_get_as(&a, 0, &int_ptr, int);
  if (int_ptr)
  {
    printf("int: %d\n", *int_ptr);
  }
  else
  {
    assert(0);
  }
  assert(*int_ptr == 42);

  // read with hash check
  const Object *o1 = va_get_cptr(&a, 1);
  if (o1 && o1->type_hash != VA_TYPE_HASH(char *))
  {
    assert(0);
  }
  assert(strcmp((char *)(o1->value), "hello") == 0);

  // leitura com checagem de hash
  const Object *o2 = va_get_cptr(&a, 2);
  if (o2 && o2->type_hash != VA_TYPE_HASH(Point))
  {
    assert(0);
  }
  assert(((Point *)o2->value)->x == 3);
  assert(((Point *)o2->value)->y == 4);

  // if (!va_is_type(&a, 0, int)) {
  //     assert(0);
  // }

  // replace
  int nx = 777;
  va_replace_copy(&a, 0, &nx, VA_TYPE_HASH(int));

  o0 = va_get_cptr(&a, 0);
  assert(*(int *)o0->value == 777);

  //erase index 1 (string) TODO: Mark element as invalid
  // va_erase(&a, 1);

  // pop último (Point)
  va_pop(&a);

  va_free(&a);

  return 0;
}
