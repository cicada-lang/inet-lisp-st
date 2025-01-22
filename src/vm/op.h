#pragma once

typedef enum {
    OP_LOOKUP,
    OP_APPLY,
    OP_LITERAL,
    OP_LOCAL_GET,
    OP_LOCAL_SET,
} op_kind_t;

struct op_t {
    op_kind_t kind;
    union {
        struct { const def_t *def; } lookup;
        struct { size_t arity; } apply;
        struct { value_t value; } literal;
        struct { size_t index; } local_get;
        struct { size_t index; } local_set;
    };
};

op_t *op_lookup(const def_t *def);
op_t *op_apply(size_t arity);
op_t *op_literal(value_t value);
op_t *op_local_get(size_t index);
op_t *op_local_set(size_t index);

void op_destroy(op_t **self_pointer);

void op_print(const op_t *self, file_t *file);
