#include "index.h"

static void
draw_node(player_t *self, canvas_t *canvas, size_t node_id, node_model_t *node_model) {
    node_t *node = hash_get(self->node_hash, (void *) node_id);
    if (!node) return;

    char *subscript = uint_to_subscript(node->id);
    char *name = string_append(node->ctor->name, subscript);
    text_t *text = text_from_string(name);
    free(subscript);
    free(name);

    size_t text_width = font_text_width(canvas->font, text);
    size_t text_height = 16;
    size_t padding_x = TILE / 2;
    size_t padding_y = 2;

    size_t x = self->node_physics->x + node_model->position.x - (text_width / 2);
    size_t y = self->node_physics->y + node_model->position.y - (text_height / 2);

    size_t width = text_width + padding_x * 2;
    size_t height = 2 * TILE;

    canvas_fiil_rect_round(
        canvas,
        x - padding_x,
        y - padding_y,
        width,
        height + padding_y * 2,
        canvas->palette[AP_COLOR],
        SM_ROUNDNESS);

    size_t thickness = 1;
    canvas_draw_rect_round(
        canvas,
        x - padding_x,
        y - padding_y,
        width,
        height + padding_y * 2,
        thickness,
        canvas->palette[FG_COLOR],
        SM_ROUNDNESS);

    size_t scale = 1;
    canvas_draw_text(canvas, x, y, text, scale, AP_FG_BLENDING);
    text_destroy(&text);
}

static void
draw_wire(player_t *self, canvas_t *canvas, const wire_t *wire) {
    wire_t *opposite = wire->opposite;
    if (!wire->node ||
        !opposite ||
        !opposite->node)
        return;

    uint32_t pixel = canvas->palette[FG_COLOR];
    if (wire_is_principal(wire) && wire_is_principal(opposite))
        pixel = canvas->palette[SL_COLOR];

    node_model_t *node_model1 =
        hash_get(self->node_model_hash, (void *) wire->node->id);
    node_model_t *node_model2 =
        hash_get(self->node_model_hash, (void *) opposite->node->id);

    if (node_model1 && node_model2) {
        canvas_draw_line(
            canvas,
            self->node_physics->x + node_model1->position.x,
            self->node_physics->y + node_model1->position.y,
            self->node_physics->x + node_model2->position.x,
            self->node_physics->y + node_model2->position.y,
            pixel);
    }
}

static void
draw_net_border(player_t *self, canvas_t *canvas, bool really) {
    if (!really) return;

    size_t thickness = 1;
    canvas_draw_rect_round(
        canvas,
        self->node_physics->x,
        self->node_physics->y,
        self->node_physics->width,
        self->node_physics->height,
        thickness,
        canvas->palette[FG_COLOR],
        SM_ROUNDNESS);
}

void
draw_net(player_t *self, canvas_t *canvas) {
    draw_net_border(self, canvas, false);

    node_t *node = hash_first(self->node_hash);
    while (node) {
        for (size_t i = 0; i < node->ctor->arity; i++) {
            value_t *value = node_get_value(node, i);
            if (!is_wire(value)) continue;
            wire_t *wire = as_wire(value);
            draw_wire(self, canvas, wire);
        }

        node = hash_next(self->node_hash);
    }

    node_model_t *node_model = hash_first(self->node_model_hash);
    while (node_model) {
        size_t node_id = (size_t) hash_cursor(self->node_model_hash);
        draw_node(self, canvas, node_id, node_model);
        node_model = hash_next(self->node_model_hash);
    }
}
