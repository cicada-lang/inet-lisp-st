#include "index.h"

node_def_t *
node_def_new(const char *name, size_t arity) {
    node_def_t *self = new(node_def_t);
    self->name = string_copy(name);
    self->arity = arity;
    self->port_defs = allocate_pointers(self->arity);
    return self;
}

void
node_def_destroy(node_def_t **self_pointer) {
    assert(self_pointer);
    if (*self_pointer) {
        node_def_t *self = *self_pointer;
        for (size_t i = 0; i < self->arity; i++) {
            port_def_t *port_def = self->port_defs[i];
            if (port_def) {
                port_def_destroy(&port_def);
            }
        }

        free(self->port_defs);
        free(self->name);
        free(self);
        *self_pointer = NULL;
    }
}

size_t
node_def_find_port_index(
    const node_def_t *node_def,
    const char *port_name
) {
    for (size_t i = 0; i < node_def->arity; i++) {
        port_def_t *port_def = node_def->port_defs[i];
        if (string_equal(port_def->name, port_name))
            return i;
    }

    printf("[node_def_find_port_index] fail to find index of node_name: %s, port_name: %s\n",
           node_def->name,
           port_name);

    assert(false);
}
