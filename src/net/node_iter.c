#include "index.h"

struct node_iter_t {
    node_t *root;
    list_t *occurred_node_list;
    list_t *remaining_node_list;
};

node_iter_t *
node_iter_new(node_t *root) {
    assert(root);

    node_iter_t *self = new(node_iter_t);
    self->root = root;
    self->occurred_node_list = list_new();
    self->remaining_node_list = list_new();
    return self;
}

void
node_iter_destroy(node_iter_t **self_pointer) {
    assert(self_pointer);
    if (*self_pointer) {
        node_iter_t *self = *self_pointer;
        list_destroy(&self->occurred_node_list);
        list_destroy(&self->remaining_node_list);
        free(self);
        *self_pointer = NULL;
    }
}

node_t *
node_iter_first(node_iter_t *self) {
    node_t *node = self->root;
    list_push(self->occurred_node_list, node);

    for (size_t i = 0; i < node->ctor->arity; i++) {
        if (!is_wire(node->ports[i])) continue;

        wire_t *wire = as_wire(node->ports[i]);
        if (wire->opposite && is_wire(wire->opposite)) {
            wire_t *opposite_wire = as_wire(wire->opposite);
            if (opposite_wire->node == NULL)
                continue;

            if (list_has(self->occurred_node_list, opposite_wire->node) ||
                list_has(self->remaining_node_list, opposite_wire->node))
                continue;

            list_push(self->remaining_node_list, opposite_wire->node);
        }
    }

    return node;
}

node_t *
node_iter_next(node_iter_t *self) {
    node_t *node = list_pop(self->remaining_node_list);
    if (!node) return NULL;

    list_push(self->occurred_node_list, node);

    for (size_t i = 0; i < node->ctor->arity; i++) {
        if (!is_wire(node->ports[i])) continue;

        wire_t *wire = as_wire(node->ports[i]);
        if (wire->opposite && is_wire(wire->opposite)) {
            wire_t *opposite_wire = as_wire(wire->opposite);
            if (opposite_wire->node == NULL ||
                list_has(self->occurred_node_list, opposite_wire->node) ||
                list_has(self->remaining_node_list, opposite_wire->node))
                continue;

            list_push(self->remaining_node_list, opposite_wire->node);
        }
    }

    return node;
}
