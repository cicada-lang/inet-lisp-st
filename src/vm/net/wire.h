#pragma once

extern object_spec_t wire_object_spec;

struct wire_t {
    object_spec_t *spec;
    node_t *node;
    size_t index;
    value_t opposite;
};

wire_t *wire_new(void);
void wire_destroy(wire_t **self_pointer);

bool is_wire(value_t value);
wire_t *as_wire(value_t value);

const char *wire_name(const wire_t *self);
const char *wire_node_name(const wire_t *self);

void wire_free_from_node(wire_t *self);

bool wire_is_free(const wire_t *self);
bool wire_is_principal(const wire_t *self);

wire_t *wire_connect_wire(vm_t* vm, wire_t *first_wire, wire_t *second_wire);
wire_t *wire_connect_value(vm_t* vm, wire_t *wire, value_t value);

void wire_print_left(const wire_t *self, file_t *file);
void wire_print_right(const wire_t *self, file_t *file);
void wire_print(const wire_t *self, file_t *file);

void wire_print_net(wire_t *self, file_t *file);
