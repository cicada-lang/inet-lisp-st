#include "index.h"

hash_t *global_mod_hash = NULL;

extern void import_prelude(mod_t *mod);

mod_t *
load_mod(path_t *path) {
    if (!global_mod_hash) {
        global_mod_hash = hash_of_string_key();
    }

    mod_t *found_mod = hash_get(global_mod_hash, path_string(path));
    if (found_mod) {
        path_destroy(&path);
        return found_mod;
    }

    file_t *file = file_open_or_fail(path_string(path), "r");
    char *code = file_read_string(file);
    fclose(file);

    mod_t *mod = mod_new(path, code);
    import_prelude(mod);
    worker_t *worker = worker_new(mod);

    list_t *sexp_list = sexp_parse_list(code) ;
    list_t *stmt_list = parse_stmt_list(sexp_list);
    stmt_t *stmt = list_first(stmt_list);
    while (stmt) {
        execute(worker, stmt);
        stmt = list_next(stmt_list);
    }

    list_destroy(&stmt_list);
    list_destroy(&sexp_list);

    mod->loader_worker = worker;

    char *key = string_copy(path_string(path));
    assert(hash_set(global_mod_hash, key, mod));

    return mod;
}
