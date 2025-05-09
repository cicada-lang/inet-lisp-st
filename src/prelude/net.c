#include "index.h"

void
x_connect(worker_t *worker) {
    value_t second = stack_pop(worker->value_stack);
    value_t first = stack_pop(worker->value_stack);

    if (is_wire(first)) {
        worker_connect(worker, as_wire(first), second);
    } else if (is_wire(second)) {
        worker_connect(worker, as_wire(second), first);
    } else {
        fprintf(stderr, "[x_connect] can not connect value to value\n");
        fprintf(stderr, "[x_connect] first: ");
        value_print(first, stderr);
        fprintf(stderr, "\n");
        fprintf(stderr, "[x_connect] second: ");
        value_print(second, stderr);
        fprintf(stderr, "\n");
        exit(1);
    }
}

void
x_link(worker_t *worker) {
    wire_t *first_wire = wire_new();
    wire_t *second_wire = wire_new();

    first_wire->opposite = second_wire;
    second_wire->opposite = first_wire;

    stack_push(worker->value_stack, first_wire);
    stack_push(worker->value_stack, second_wire);
}
