#pragma once

struct debug_t {
    vm_t *vm;
    wire_t *root;

    canvas_t *canvas;
    bool toggle_light_button_is_pressed;
    bool step_button_is_pressed;
    bool run_button_is_pressed;
    bool end_button_is_pressed;

    bool is_running;
    size_t running_frame_count;
    size_t running_speed; // times per second

    hash_t *node_hash;
    hash_t *node_model_hash;
    node_physics_t *node_physics;
};

debug_t *debug_new(vm_t *vm);
void debug_destroy(debug_t **self_pointer);

void debug_toggle_light(debug_t *self);
bool debug_is_any_button_pressed(debug_t *self);

hash_t *debug_new_node_hash(debug_t *self);
void debug_update(debug_t *self);

void debug_start(vm_t *vm);
