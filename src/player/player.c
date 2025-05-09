#include "index.h"

player_t *
player_new(worker_t *worker) {
    player_t *self = new(player_t);
    self->worker = worker;

    size_t width = 90 * TILE;
    size_t height = 60 * TILE;
    size_t scale = 2;

    canvas_t *canvas = canvas_new(width, height, scale);
    canvas->state = self;
    canvas->title = "inet debug";
    self->canvas = canvas;

    self->node_hash = hash_new();
    self->node_model_hash = hash_new();
    hash_set_destroy_fn(self->node_model_hash, (destroy_fn_t *) node_model_destroy);

    self->is_running = false;
    self->running_frame_count = 0;
    self->running_speed = 3;

    size_t padding_x = 4 * TILE;
    size_t padding_y = 4 * TILE;
    self->node_physics = node_physics_new(
        padding_x,
        padding_y,
        width - padding_x * 2,
        height - padding_y * 2);

    return self;
}

void
player_destroy(player_t **self_pointer) {
    assert(self_pointer);
    if (*self_pointer == NULL) return;

    player_t *self = *self_pointer;
    canvas_destroy(&self->canvas);
    hash_destroy(&self->node_model_hash);
    free(self);
    *self_pointer = NULL;
}

bool
player_is_any_button_pressed(player_t *self) {
    return (self->toggle_light_button_is_pressed ||
            self->step_button_is_pressed ||
            self->run_button_is_pressed ||
            self->end_button_is_pressed);
}

static void
init_canvas_theme(canvas_t *canvas) {
    canvas->palette[BG_COLOR] = 0xff000000;
    canvas->palette[SL_COLOR] = 0xffcd2e3a;
    canvas->palette[FG_COLOR] = 0xff000000;
    canvas->palette[AP_COLOR] = 0xffffffff;
}

void
player_toggle_light(player_t *self) {
    color_t fg_color = self->canvas->palette[FG_COLOR];
    color_t ap_color = self->canvas->palette[AP_COLOR];

    self->canvas->palette[BG_COLOR] = fg_color;
    self->canvas->palette[FG_COLOR] = ap_color;
    self->canvas->palette[AP_COLOR] = fg_color;
}

static void
init_canvas_asset_store(canvas_t *canvas) {
    char *root = dirname(dirname(dirname(string_copy(__FILE__))));
    canvas_init_asset_store(canvas, string_append(root, "/assets"));
}

static void
init_canvas_font(canvas_t *canvas) {
    blob_t *blob = canvas_asset_store_get(canvas, "fonts/unifont_all-16.0.02.hex");
    canvas->font = font_from_hex_string(blob_string(blob));
}

static void
on_frame(player_t *self, canvas_t *canvas, uint64_t passed) {
    (void) passed;

    if (self->is_running)
        self->running_frame_count += passed;

    if (self->running_frame_count > canvas->frame_rate / self->running_speed) {
        task_t *task = list_pop(self->worker->task_list);
        if (!task) return;
        
        worker_handle_task(self->worker, task);
        player_update(self);
        self->running_frame_count = 0;
    }

    node_physics_fake_simulate(
        self->node_physics,
        self->node_hash,
        self->node_model_hash);

    canvas_clear_clickable_area(canvas);

    draw_background(self, canvas, false);
    draw_net(self, canvas);
    draw_toolbar(self, canvas);
    draw_cursor(self, canvas);
}

hash_t *
player_new_node_hash(player_t *self) {
    hash_t *node_hash = hash_new();
    node_t *node = set_first(self->worker->player_node_set);
    while (node) {
        hash_set(node_hash, (void *) node->id, node);
        node = set_next(self->worker->player_node_set);
    }

    return node_hash;
}

void
player_update(player_t *self) {
    hash_t *new_node_hash = player_new_node_hash(self);
    node_physics_update_nodes(
        self->node_physics,
        new_node_hash,
        self->node_hash,
        self->node_model_hash);
    hash_destroy(&self->node_hash);
    self->node_hash = new_node_hash;
}

static void
init_node_hash(player_t *self) {
    hash_destroy(&self->node_hash);
    self->node_hash = player_new_node_hash(self);
}

static void
init_node_physics(player_t *self) {
    node_physics_add_nodes(
        self->node_physics,
        self->node_hash,
        self->node_model_hash);
}

static void
on_click(player_t *self, canvas_t *canvas, uint8_t button, bool is_release) {
    (void) canvas;

    // to handle click inside a button,
    // but release outside the button.

    if (button == 1) {
        if (is_release) {
            self->toggle_light_button_is_pressed = false;
            self->step_button_is_pressed = false;
            self->run_button_is_pressed = false;
            self->end_button_is_pressed = false;
        }
    }
}

void
player_start(worker_t *worker) {
    srand(time(NULL));

    player_t *self = player_new(worker);

    init_node_hash(self);
    init_node_physics(self);

    init_canvas_theme(self->canvas);
    init_canvas_asset_store(self->canvas);
    init_canvas_font(self->canvas);
    self->canvas->on_frame = (on_frame_fn_t *) on_frame;
    self->canvas->on_click = (on_click_fn_t *) on_click;
    self->canvas->hide_system_cursor = true;

    canvas_open(self->canvas);
    player_destroy(&self);
}
