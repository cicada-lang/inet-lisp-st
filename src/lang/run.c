#include "index.h"

void
run_net(vm_t *vm) {
    while (!list_is_empty(vm->activity_list)) {
        step_net(vm);
    }
}

static void
collect_free_wires_from_node(vm_t *vm, node_t *node) {
    for (port_index_t i = 0; i < node->def->arity; i++) {
        if (!wire_is_principal(node->wires[i])) {
            wire_t *wire = node->wires[i];
            wire_free_from_node(wire);
            stack_push(vm->value_stack, wire);
        }
    }

    vm_delete_node(vm, node);
}

void
step_net(vm_t *vm) {
    activity_t *activity = list_shift(vm->activity_list);
    if (!activity) return;

    node_t *first_node = activity->wire->node;
    node_t *second_node = activity->wire->opposite->node;

    if (first_node->def == activity->rule->second_node_def &&
        second_node->def == activity->rule->first_node_def)
    {
        first_node = activity->wire->opposite->node;
        second_node = activity->wire->node;
    }

    collect_free_wires_from_node(vm, first_node);
    collect_free_wires_from_node(vm, second_node);

    vm_delete_wire(vm, activity->wire);
    vm_delete_wire(vm, activity->wire->opposite);

    size_t base_length = stack_length(vm->return_stack);
    frame_t *frame = frame_new(activity->rule->function);

    activity_destroy(&activity);

    stack_push(vm->return_stack, frame);
    run_vm_until(vm, base_length);
}

void
run_vm_until(vm_t *vm, size_t base_length) {
    if (vm->log_level > 0) {
        vm_print(vm, stdout);
        fprintf(stdout, "\n");
    }

    while (stack_length(vm->return_stack) > base_length) {
        step_vm(vm);

        if (vm->log_level > 0) {
            vm_print(vm, stdout);
            fprintf(stdout, "\n");
        }
    }
}

static void
step_operation(vm_t *vm, frame_t *frame, op_t *unknown_op) {
    switch (unknown_op->kind) {
    case CALL_OP: {
        call_op_t *op = (call_op_t *) unknown_op;
        call(vm, op->def);
        return;
    }

    case LITERAL_OP: {
        literal_op_t *op = (literal_op_t *) unknown_op;
        stack_push(vm->value_stack, op->value);
        return;
    }

    case LOCAL_GET_OP: {
        local_get_op_t *op = (local_get_op_t *) unknown_op;
        value_t value = frame_local_get(frame, op->index);
        stack_push(vm->value_stack, value);
        return;
    }

    case LOCAL_SET_OP: {
        local_set_op_t *op = (local_set_op_t *) unknown_op;
        value_t value = stack_pop(vm->value_stack);
        frame_local_set(frame, op->index, value);
        return;
    }
    }
}

void
step_vm(vm_t *vm) {
    if (stack_is_empty(vm->return_stack)) return;

    frame_t *frame = stack_pop(vm->return_stack);
    if (frame_is_finished(frame)) {
        frame_destroy(&frame);
        return;
    }

    op_t *op = frame_fetch_op(frame);

    // proper tail-call = do not push finished frame.
    bool finished = frame_is_finished(frame);
    if (!finished) {
        stack_push(vm->return_stack, frame);
    }

    step_operation(vm, frame, op);

    if (finished) {
        frame_destroy(&frame);
    }
}
