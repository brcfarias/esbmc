#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>

// #define TYPE_TAG(T)      ((size_t)&TAG_##T)
// #define DEF_TYPE_TAG(T)  static const char TAG_##T = 0

// #define TYPE_EQ(tag, T)         ((tag) == TYPE_TAG(T))
// #define OBJ_IS(obj_ptr, T)      ((obj_ptr)->type_tag == TYPE_TAG(T))

static inline size_t va_hash_string(const char *str) {
    size_t hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash;
}

// Macro para obter hash de tipo dinamicamente
#define VA_TYPE_HASH(T) va_hash_string(#T)

// Macro para push com tipo automático
#define va_push(array, var) \
    va_push_copy((array), &(var), sizeof(var), VA_TYPE_HASH(__typeof__(var)))

// Macro para push de string
#define va_push_str(array, str) \
    va_push_copy((array), (str), strlen(str) + 1, VA_TYPE_HASH(char*))


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

typedef struct { int x, y; } Point;

int main(void) {
    VarArray a;
    if (!va_init(&a)) return 1;

    // push de inteiro
    int iv = 42;
    va_push(&a, iv);

    // push de string (inclui '\0')
    va_push_str(&a, "hello");

    // push de struct
    Point p = {3, 4};
    va_push(&a, p);

    // leitura com checagem de hash
    const Object *o0 = va_get_cptr(&a, 0);
    if (o0 && o0->type_hash == VA_TYPE_HASH(int)) {
        printf("int: %d\n", *(int*)o0->value);
    }
    assert(*(int*)o0->value == 42);

    const Object *o1 = va_get_cptr(&a, 1);
    if (o1 && o1->type_hash == VA_TYPE_HASH(char_ptr)) {
        printf("str: %s\n", (char*)o1->value);
    }

    const Object *o2 = va_get_cptr(&a, 2);
    if (o2 && o2->type_hash == VA_TYPE_HASH(Point)) {
        Point *pp = (Point*)o2->value;
        printf("Point{%d,%d}\n", pp->x, pp->y);
    }

    // replace
    int nx = 777;
    va_replace_copy(&a, 0, &nx, sizeof nx, VA_TYPE_HASH(int));
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