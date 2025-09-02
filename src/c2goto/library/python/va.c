#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>


#define TYPE_TAG(T)      ((size_t)&TAG_##T)
#define DEF_TYPE_TAG(T)  static const char TAG_##T = 0

#define TYPE_EQ(tag, T)         ((tag) == TYPE_TAG(T))
#define OBJ_IS(obj_ptr, T)      ((obj_ptr)->type_tag == TYPE_TAG(T))

DEF_TYPE_TAG(int);
DEF_TYPE_TAG(char_ptr);
DEF_TYPE_TAG(Point);


#if 0
typedef struct {
    void   *value;     // payload alocado individualmente (dono: VarArray)
    size_t  size;      // tamanho em bytes do payload
    size_t  type_hash; // hash do "tipo" (ex.: TYPE_HASH(int), TYPE_HASH("MyType"))
} Object;

typedef struct {
    Object *objs;      // vetor de Objects
    size_t  size;      // quantidade em uso
    size_t  capacity;  // slots alocados
} VarArray;

static inline size_t va_hash_bytes(const void *data, size_t len) {
    const uint8_t *p = (const uint8_t *)data;
    uint64_t h = 1469598103934665603ULL;   // offset basis
    for (size_t i = 0; i < len; ++i) {
        h ^= p[i];
        h *= 1099511628211ULL;            // prime
    }
    return (size_t)h;
}

static inline size_t va_hash_cstr(const char *s) {
    return va_hash_bytes(s, s ? strlen(s) : 0);
}

#define TYPE_HASH(T) va_hash_cstr(#T)
#endif

typedef struct {
    const void *value;     // ponteiro para dados
    size_t      size;      // tamanho em bytes do payload
    size_t      type_hash; // hash do "tipo"
} Object;

typedef struct {
    size_t size;      // quantidade em uso
} VarArray;

// UM ÚNICO array infinito que armazena diretamente os Objects
__attribute__((annotate("__ESBMC_inf_size"))) 
static Object __ESBMC_objects[1];

/* ---------- lifecycle ---------- */
static inline bool va_init(VarArray *a) {
    a->size = 0;
    return true;
}

static inline void va_free(VarArray *a) {
    a->size = 0;
}

static inline bool va_push_copy(VarArray *a, const void *data, size_t len, size_t type_hash) {
    // Usa a->size diretamente como índice
    __ESBMC_objects[a->size].value = data;
    __ESBMC_objects[a->size].size = len;
    __ESBMC_objects[a->size].type_hash = type_hash;
    
    a->size++;
    return true;
}

static inline bool va_replace_copy(VarArray *a, size_t index, const void *data, size_t len, size_t type_hash) {
    if (index >= a->size) return false;

    __ESBMC_objects[index].value = data;
    __ESBMC_objects[index].size = len;
    __ESBMC_objects[index].type_hash = type_hash;
    return true;
}

/* ---------- getters ---------- */
static inline const Object* va_get_cptr(const VarArray *a, size_t index) {
    if (index >= a->size) return NULL;
    return &__ESBMC_objects[index];
}
static inline Object* va_get_ptr(VarArray *a, size_t index) {
    if (index >= a->size) return NULL;
    return &__ESBMC_objects[index];
}

/* ---------- pop / erase ---------- */
static inline bool va_pop(VarArray *a) {
    if (a->size == 0) return false;
    a->size--;
    return true;
}

// /* ---------- helpers de conveniência ---------- */
static inline bool va_push_cstr(VarArray *a, const char *cstr) {
    size_t n = cstr ? (strlen(cstr) + 1) : 1;
    const char empty = '\0';
    const void *src = cstr ? (const void*)cstr : (const void*)&empty;
    return va_push_copy(a, src, n, TYPE_TAG(char_ptr));
}

// #define TYPE_HASH_OF(x) TYPE_HASH(__typeof__(x))

typedef struct { int x, y; } Point;

int main(void) {
    VarArray a;
    if (!va_init(&a/*, 2*/)) return 1;

    // push de inteiro
    int iv = 42;
    va_push_copy(&a, &iv, sizeof iv, /*TYPE_HASH*/TYPE_TAG(int));

    // push de string (inclui '\0')
    va_push_cstr(&a, "hello");

    // push de struct
    Point p = {3, 4};
    va_push_copy(&a, &p, sizeof p, /*TYPE_HASH*/TYPE_TAG(Point));

    // leitura com checagem de hash (opcional, mas recomendável)
    const Object *o0 = va_get_cptr(&a, 0);
    if (o0 && o0->type_hash == /*TYPE_HASH*/TYPE_TAG(int)) {
        printf("int: %d\n", *(int*)o0->value);
    }
    assert(*(int*)o0->value == 42);

    const Object *o1 = va_get_cptr(&a, 1);
    if (o1 && o1->type_hash == TYPE_TAG(char_ptr)) {
        printf("str: %s\n", (char*)o1->value);
    }

    const Object *o2 = va_get_cptr(&a, 2);
    if (o2 && o2->type_hash == TYPE_TAG(Point)) {
        Point *pp = (Point*)o2->value;
        printf("Point{%d,%d}\n", pp->x, pp->y);
    }

    // replace
    int nx = 777;
    va_replace_copy(&a, 0, &nx, sizeof nx, TYPE_TAG(int));
    printf("int: %d\n", *(int*)o0->value);

    o0 = va_get_cptr(&a, 0);
    assert(*(int*)o0->value == 777);

    //erase index 1 (string)
    va_erase(&a, 1);

    // pop último (Point)
    va_pop(&a);

    va_free(&a);
    return 0;
}