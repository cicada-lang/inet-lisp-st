#include "index.h"

struct wire_iter_t {
    wire_t *root;
    list_t *occurred_wire_list;
    list_t *remaining_node_list;
    node_t *node;
    size_t index;
};

wire_iter_t *
wire_iter_new(wire_t *root) {
    wire_iter_t *self = new(wire_iter_t);
    self->root = root;
    self->occurred_wire_list = list_new();
    self->remaining_node_list = list_new();
    return self;
}

void
wire_iter_destroy(wire_iter_t **self_pointer) {
    assert(self_pointer);
    if (*self_pointer) {
        wire_iter_t *self = *self_pointer;
        list_destroy(&self->occurred_wire_list);
        list_destroy(&self->remaining_node_list);
        free(self);
        *self_pointer = NULL;
    }
}

wire_t *
wire_iter_first(wire_iter_t *self) {
    list_push(self->occurred_wire_list, self->root);

    if (self->root->opposite && is_wire(self->root->opposite)) {
        list_push(self->occurred_wire_list, self->root->opposite);

        wire_t *opposite_wire = as_wire(self->root->opposite);
        if (opposite_wire->node) {
            self->node = opposite_wire->node;
            self->index = 0;
        }        
    }

    return self->root;
}

wire_t *
wire_iter_next(wire_iter_t *self) {
    if (!self->node) return NULL;

    while (self->index < self->node->ctor->arity) {
        size_t i = self->index++;

        if (!is_wire(self->node->ports[i])) continue;
        wire_t *wire = as_wire(self->node->ports[i]);
        if (list_has(self->occurred_wire_list, wire)) continue;
        list_push(self->occurred_wire_list, wire);

        if (wire->opposite) {
            if (list_has(self->occurred_wire_list, wire->opposite)) continue;
            list_push(self->occurred_wire_list, wire->opposite);

            if (wire->opposite && is_wire(wire->opposite)) {
                wire_t *opposite_wire = as_wire(wire->opposite);
                if (opposite_wire->node && !list_has(self->remaining_node_list, opposite_wire->node))
                    list_push(self->remaining_node_list, opposite_wire->node);
            }
        }

        return wire;
    }

    self->node = list_pop(self->remaining_node_list);
    self->index = 0;
    return wire_iter_next(self);
}
