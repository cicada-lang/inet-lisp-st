#include "index.h"

node_t *
node_new(const node_ctor_t *ctor, size_t id) {
    node_t *self = new(node_t);
    self->ctor = ctor;
    self->id = id;
    self->ports = allocate_pointers(ctor->arity);
    self->atomic_is_matched = false;
    return self;
}

void
node_destroy(node_t **self_pointer) {
    assert(self_pointer);
    if (*self_pointer) {
        node_t *self = *self_pointer;
        free(self->ports);
        // Does NOT own wires in `wires`.
        free(self);
        *self_pointer = NULL;
    }
}

void
node_set(node_t *self, size_t index, value_t value) {
    assert(index < self->ctor->arity);
    self->ports[index] = value;

    if (is_wire(value)) {
        wire_t *wire = as_wire(value);
        assert(wire->node == NULL);
        wire->node = self;
        wire->index = index;
    }
}

value_t node_get(const node_t *self, size_t index) {
    assert(index < self->ctor->arity);
    return self->ports[index];
}

void
node_print_name(const node_t *self, file_t *file) {
    char *id_string = uint_to_subscript(self->id);
    fprintf(file, "%s%s", self->ctor->name, id_string);
    free(id_string);
}

void
node_print(const node_t *self, file_t *file) {
    fprintf(file, "(");
    node_print_name(self, file);
    fprintf(file, ")");
}

bool
node_is_primitive(const node_t *self) {
    return self->ctor->primitive != NULL;
}
