#include "index.h"

struct path_t {
    stack_t *segment_stack;
    char *string;
    bool is_absolute;
};

path_t *
path_new(const char *string) {
    path_t *self = new(path_t);
    self->segment_stack = string_stack_new();
    if (string_starts_with(string, "/"))
        self->is_absolute = true;
    path_join(self, string);
    return self;
}

void
path_destroy(path_t **self_pointer) {
    assert(self_pointer);
    if (*self_pointer) {
        path_t *self = *self_pointer;
        stack_destroy(&self->segment_stack);
        string_destroy(&self->string);
        free(self);
        *self_pointer = NULL;
    }
}

bool
path_is_relative(const path_t *self) {
    return !self->is_absolute;
}

bool
path_is_absolute(const path_t *self) {
    return self->is_absolute;
}

path_t *
path_copy(path_t *self) {
    return path_new(path_string(self));
}

bool
path_equal(path_t *x, path_t *y) {
    return string_equal(path_string(x), path_string(y));
}

typedef struct {
    const char *string;
    char *segment;
}  entry_t;

static entry_t *
next_segment(const char *string) {
    if (string_is_empty(string))
        return NULL;

    int index = string_find_index(string, '/');
    if (index == -1) {
        entry_t *entry = new(entry_t);
        entry->string = string + string_length(string);
        entry->segment = string_copy(string);
        return entry;
    }

    entry_t *entry = new(entry_t);
    entry->string = string + index;
    if (string_length(entry->string) > 0)
        entry->string++;

    entry->segment = string_slice(string, 0, index);
    return entry;
}

static void
path_update_string(path_t *self) {
    size_t length = stack_length(self->segment_stack);
    size_t size = 0;
    for (size_t i = 0; i < length; i++) {
        char *segment = stack_get(self->segment_stack, i);
        size += string_length(segment);
        size += 1;
    }

    string_destroy(&self->string);
    char *string = NULL;
    if (path_is_absolute(self)) {
        self->string = allocate(size + 1);
        self->string[0] = '/';
        string = self->string + 1;
    } else {
        self->string = allocate(size);
        string = self->string;
    }

    for (size_t i = 0; i < length; i++) {
        char *segment = stack_get(self->segment_stack, i);
        strcat(string, segment);
        string += string_length(segment);
        if (i == length - 1) {
            string[0] = '\0';
        } else {
            string[0] = '/';
            string++;
        }
    }
}

static void
path_execute(path_t *self, char *segment) {
    if (string_is_empty(segment))
        return;
    else if (string_equal(segment, "."))
        return;
    else if (string_equal(segment, "..")) {
        string_destroy(&segment);
        segment = stack_pop(self->segment_stack);
        string_destroy(&segment);
        return;
    }
    else
        stack_push(self->segment_stack, segment);
}

void
path_join(path_t *self, const char *string) {
    entry_t *entry = next_segment(string);
    while (entry) {
        path_execute(self, entry->segment);
        string = entry->string;
        free(entry);
        entry = next_segment(string);
    }

    path_update_string(self);
}

const char *
path_string(path_t *self) {
    if (self->string)
        return self->string;

    path_update_string(self);
    return self->string;
}
