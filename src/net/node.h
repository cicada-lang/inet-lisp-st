#pragma once

struct node_t {
    const node_ctor_t *ctor;
    size_t id;
    value_t *ports;
    atomic_bool atomic_is_matched;
};

node_t *node_new(const node_ctor_t *ctor, size_t id);
void node_destroy(node_t **self_pointer);

void node_set(node_t *self, size_t index, value_t value);
value_t node_get(const node_t *self, size_t index);

void node_print_name(const node_t *self, file_t *file);
void node_print(const node_t *self, file_t *file);

bool node_is_primitive(const node_t *self);
