#include "index.h"

task_t *
task_from_rule(const rule_t *rule, net_matcher_t *net_matcher) {
    task_t *self = new(task_t);
    self->rule = rule;
    self->net_matcher = net_matcher;
    return self;
}

task_t *
task_from_primitive_node(node_t *primitive_node) {
    task_t *self = new(task_t);
    assert(node_is_primitive(primitive_node));
    self->primitive_node = primitive_node;
    return self;
}

void
task_destroy(task_t **self_pointer) {
    assert(self_pointer);
    if (*self_pointer == NULL) return;

    task_t *self = *self_pointer;
    net_matcher_destroy(&self->net_matcher);
    free(self);
    *self_pointer = NULL;
}

bool
task_is_primitive(const task_t *self) {
    return self->primitive_node != NULL;
}

static void
by_primitive_node(worker_t *worker, node_t *node) {
    assert(node_is_primitive(node));

    for (size_t i = 0; i < node->ctor->arity; i++) {
        if (!node->ctor->port_infos[i]->is_principal)
            continue;
            
        value_t value = node_get(node, i);
        // NOTE be careful that there are two cases:
        // - (1) `[value]`   -- ready
        // - (2) `-<[value]` -- ready
        // - (3) `-<>-`      -- NOT ready
        if (is_wire(value) && is_wire(as_wire(value)->opposite)) {
            // do nothing when NOT ready
            return;
        }
    }

    worker_return_task(worker, task_from_primitive_node(node));
    node->is_matched = true;
}

static void
by_matched_node(worker_t *worker, node_t *node) {
    size_t length = array_length(node->ctor->rule_array);
    for (size_t i = 0; i < length; i++) {
        rule_t *rule = array_get(node->ctor->rule_array, i);
        net_matcher_t *net_matcher =
            match_net(rule->net_pattern, rule->starting_index, node);
        if (net_matcher) {
            worker_return_task(worker, task_from_rule(rule, net_matcher));
            size_t length = net_pattern_length(rule->net_pattern);
            for (size_t i = 0; i < length; i++)
                net_matcher->matched_nodes[i]->is_matched = true;
            return;
        }
    }
}

void
maybe_return_task_by_node(worker_t *worker, node_t *node) {
    assert(node);

    if (node->is_matched)
        return;

    if (node_is_primitive(node))
        by_primitive_node(worker, node);
    else
        by_matched_node(worker, node);
}

void
maybe_return_task_by_node_and_neighbor(worker_t *worker, node_t *node) {
    maybe_return_task_by_node(worker, node);

    // NOTE for imported node ctor,
    // if is not enough to activate the new node only,
    // for example, i import `add`, but apply `add1`,
    // we can not find the def of `add1` in the current mod,
    // thus can not find the rules to activate it.
    // we can fix this problem by
    // also activate the neighboring nodes.

    for (size_t i = 0; i < node->ctor->arity; i++) {
        value_t value = node_get(node, i);
        if (!is_wire(value)) continue;
        wire_t *wire = as_wire(value);
        if (is_wire(wire->opposite) && as_wire(wire->opposite)->node)
            maybe_return_task_by_node(worker, as_wire(wire->opposite)->node);
    }
}

void
task_print(task_t *self, file_t *file) {
    fprintf(file, "<task>\n");
    rule_print(self->rule, file);
    net_matcher_print(self->net_matcher, file);
    fprintf(file, "</task>\n");
}
