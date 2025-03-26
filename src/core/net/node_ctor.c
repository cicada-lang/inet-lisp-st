#include "index.h"

object_spec_t node_ctor_object_spec = {
    .name = "node-ctor",
    .print_fn = (print_fn_t *) node_ctor_print,
};

node_ctor_t *
node_ctor_new(const char *name, size_t arity) {
    node_ctor_t *self = new(node_ctor_t);
    self->spec = &node_ctor_object_spec;
    self->name = string_copy(name);
    self->arity = arity;
    self->port_infos = allocate_pointers(arity);
    size_t init_array_size = 32;
    self->rule_array = array_new_with(init_array_size, (destroy_fn_t *) rule_destroy);
    return self;
}

void
node_ctor_destroy(node_ctor_t **self_pointer) {
    assert(self_pointer);
    if (*self_pointer) {
        node_ctor_t *self = *self_pointer;
        for (size_t i = 0; i < self->arity; i++) {
            port_info_t *port_info = self->port_infos[i];
            if (port_info) {
                port_info_destroy(&port_info);
            }
        }

        // does not own `self->primitive`
        array_destroy(&self->rule_array);
        free(self->name);
        free(self);
        *self_pointer = NULL;
    }
}

bool
is_node_ctor(value_t value) {
    if (!is_xobject(value)) return false;
    return as_object(value)->spec == &node_ctor_object_spec;
}

node_ctor_t *
as_node_ctor(value_t value) {
    assert(is_node_ctor(value));
    return (node_ctor_t *) value;
}

size_t
node_ctor_find_port_index(
    const node_ctor_t *node_ctor,
    const char *port_name
) {
    for (size_t i = 0; i < node_ctor->arity; i++) {
        port_info_t *port_info = node_ctor->port_infos[i];
        if (string_equal(port_info->name, port_name))
            return i;
    }

    printf("[node_ctor_find_port_index] fail to find index of node_name: %s, port_name: %s\n",
           node_ctor->name,
           port_name);

    assert(false);
}

void
node_ctor_print(const node_ctor_t *self, file_t *file) {
    fprintf(file, "(%s)", self->name);
}
