[core] `worker_apply` instead of `apply`
[core] `worker_run_until`
[core] `worker_work`
[core] extract `worker_disconnect_node`

# player

[player] use real physics force
[player] `node_physics_simulate` -- move by `velocity` and clear `force` for every `node_model`
[player] remove `node_physics_fake_simulate`
[player] `draw_net` -- print non-wire value -- test by `list-map.play.lisp`

# arity checker

[lang] [maybe] write arity checker in inet-lisp itself!
[lang] write the arity checker like a type checker -- choose good names in the problem domain
[lang] check arity -- compute arity from exp
[lang] compute `arity` from each exp -- what should be the function names?
[lang] `compile_exp` -- `EXP_AP` -- should not get `arity` from `list_length(exp->ap.arg_list)`
[lang] `apply_function` -- supplement wire if arity is short

# error report

improve undefined-node.error.lisp

# testing

[testing] `(assert)` report position
