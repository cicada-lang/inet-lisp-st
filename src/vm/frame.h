#pragma once

frame_t *frame_new(const function_t *function);
void frame_destroy(frame_t **self_pointer);

bool frame_is_finished(const frame_t *self);
op_t *frame_fetch_op(frame_t *self);
void frame_print(const frame_t *self, file_t *file);

value_t frame_get_variable(const frame_t *self, size_t index);
void frame_set_variable(frame_t *self, size_t index, value_t value);

value_t frame_get_linear_variable(const frame_t *self, size_t index);
void frame_set_linear_variable(frame_t *self, size_t index, value_t value);
