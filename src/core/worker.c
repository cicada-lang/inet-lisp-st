#include "index.h"

worker_t *
worker_new(mod_t *mod) {
    worker_t *self = new_page_aligned(worker_t);
    self->mod = mod;
    self->task_list = list_new_with((destroy_fn_t *) task_destroy);
    // TODO We should use value_destroy to create value_stack.
    self->value_stack = stack_new();
    self->return_stack = stack_new_with((destroy_fn_t *) frame_destroy);
    self->player_node_set = set_new();
    self->node_id_count = 0;
    return self;
}

void
worker_destroy(worker_t **self_pointer) {
    assert(self_pointer);
    if (*self_pointer == NULL) return;

    worker_t *self = *self_pointer;
    list_destroy(&self->task_list);
    stack_destroy(&self->value_stack);
    stack_destroy(&self->return_stack);
    set_destroy(&self->player_node_set);
    free(self);
    *self_pointer = NULL;
}

node_t *
worker_add_node(worker_t* self, const node_ctor_t *ctor) {
    node_t *node = node_new(ctor, ++self->node_id_count);

    if (player_flag)
        set_add(self->player_node_set, node);

    return node;
}

void
worker_delete_node(worker_t* self, node_t *node) {
    if (player_flag)
        set_delete(self->player_node_set, node);

    node_destroy(&node);
}

wire_t *
worker_add_wire(worker_t* self) {
    (void) self;
    wire_t *wire = wire_new();
    return wire;
}

void
worker_delete_wire(worker_t* self, wire_t *wire) {
    (void) self;
    wire_destroy(&wire);
}

char *
worker_fresh_name(worker_t* self) {
    size_t max_string_length = 256;
    char *buffer = allocate(max_string_length);
    sprintf(buffer, "%lu", self->fresh_name_count++);
    char *string = string_copy(buffer);
    free(buffer);
    return string;
}

void
worker_return_task(worker_t* self, task_t *task) {
    list_unshift(self->task_list, task);
}

void
worker_print(const worker_t *self, file_t *file) {
    fprintf(file, "<worker>\n");

    size_t task_count = list_length(self->task_list);
    fprintf(file, "<task-queue length=\"%lu\">\n", task_count);
    for (size_t i = 0; i < task_count; i++) {
        task_t *task = list_get(self->task_list, i);
        task_print(task, file);
    }
    fprintf(file, "</task-queue>\n");

    worker_print_return_stack(self, file);
    worker_print_value_stack(self, file);

    fprintf(file, "</worker>\n");
}

void
worker_print_return_stack(const worker_t *self, file_t *file) {
    size_t return_stack_length = stack_length(self->return_stack);
    fprintf(file, "<return-stack length=\"%lu\">\n", return_stack_length);
    for (size_t i = 0; i < return_stack_length; i++) {
        frame_t *frame = stack_get(self->return_stack, i);
        frame_print(frame, file);
    }

    fprintf(file, "</return-stack>\n");
}

void
worker_print_value_stack(const worker_t *self, file_t *file) {
    size_t value_stack_length = stack_length(self->value_stack);
    fprintf(file, "<value-stack length=\"%lu\">\n", value_stack_length);
    for (size_t i = 0; i < value_stack_length; i++) {
        value_t value = stack_get(self->value_stack, i);
        value_print(value, file);
        fprintf(file, "\n");
    }

    fprintf(file, "</value-stack>\n");
}
