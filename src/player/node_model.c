#include "index.h"

node_model_t *
node_model_new(vec2_t position) {
    node_model_t *self = new(node_model_t);
    self->position = position;
    self->velocity = (vec2_t) { .x = 0, .y = 0 };
    self->force = (vec2_t) { .x = 0, .y = 0 };
    return self;
}

void
node_model_destroy(node_model_t **self_pointer) {
    assert(self_pointer);
    if (*self_pointer == NULL) return;

    node_model_t *self = *self_pointer;
    free(self);
    *self_pointer = NULL;
}
