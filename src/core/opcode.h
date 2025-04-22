#pragma once

typedef enum {
    OP_APPLY,
    OPCODE_LITERAL,
    OPCODE_GET_VARIABLE,
    OPCODE_SET_VARIABLE,
} op_kind_t;

struct opcode_t {
    op_kind_t kind;
    union {
        struct { size_t arity; } apply;
        struct { value_t value; } literal;
        struct { size_t index; } get_variable;
        struct { size_t index; } set_variable;
    };
};

opcode_t *op_apply(size_t arity);
opcode_t *opcode_literal(value_t value);
opcode_t *opcode_get_variable(size_t index);
opcode_t *opcode_set_variable(size_t index);

void op_destroy(opcode_t **self_pointer);

void op_print(const opcode_t *self, file_t *file);
