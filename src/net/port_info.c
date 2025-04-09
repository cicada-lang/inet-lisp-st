#include "index.h"

port_info_t *
port_info_new(char *name, bool is_principal) {
    port_info_t *self = new(port_info_t);
    self->name = name;
    self->is_principal = is_principal;
    return self;
}

void
port_info_destroy(port_info_t **self_pointer) {
    assert(self_pointer);
    if (*self_pointer == NULL) return;

    port_info_t *self = *self_pointer;
    free(self->name);
    free(self);
    *self_pointer = NULL;
}

array_t *
port_info_array_new(size_t size) {
    return array_new_with(size, (destroy_fn_t *) port_info_destroy);
}

port_info_t *
port_info_from_name(char *name) {
    if (string_ends_with(name, "!")) {
        char *new_name = string_slice(name, 0, strlen(name) - 1);
        string_destroy(&name);
        return port_info_new(new_name, true);
    } else {
        return port_info_new(name, false);
    }
}
