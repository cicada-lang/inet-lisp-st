#include "index.h"

#define LIMIT 10000000

static void *
uint_producer(queue_t *queue) {
    size_t count = 0;
    while (true) {
        if (count == LIMIT) return NULL;

        while (queue_is_full(queue)) {}

        queue_enqueue(queue, (void *) count);
        count++;
    }
}

static void *
uint_consumer(queue_t *queue) {
    size_t count = 0;
    while (true) {
        if (count == LIMIT) return NULL;

        while (queue_is_empty(queue)) {}

        assert(((size_t) queue_dequeue(queue)) == count);
        count++;
    }
}

static void *
string_producer(queue_t *queue) {
    size_t count = 0;
    while (true) {
        if (count == LIMIT) return NULL;

        while (queue_is_full(queue)) {}

        queue_enqueue(queue, uint_to_string(count));
        count++;
    }
}

static void *
string_consumer(queue_t *queue) {
    size_t count = 0;
    while (true) {
        if (count == LIMIT) return NULL;

        while (queue_is_empty(queue)) {}

        char *x = queue_dequeue(queue);
        char *y = uint_to_string(count);
        assert(string_equal(x, y));
        string_destroy(&x);
        string_destroy(&y);
        count++;
    }
}

void
queue_test_thread_safe(void) {
    printf("<queue_test_thread_safe>\n");

    queue_t *queue = queue_new(3);

    {
        printf("uint_producer v.s. uint_consumer\n");

        thread_id_t producer_id =
            thread_start((thread_fn_t *) uint_producer, queue);
        thread_id_t consumer_id =
            thread_start((thread_fn_t *) uint_consumer, queue);

        thread_wait(producer_id);
        thread_wait(consumer_id);
    }

    {
        printf("string_producer v.s. string_consumer\n");

        thread_id_t producer_id =
            thread_start((thread_fn_t *) string_producer, queue);
        thread_id_t consumer_id =
            thread_start((thread_fn_t *) string_consumer, queue);

        thread_wait(producer_id);
        thread_wait(consumer_id);
    }

    queue_destroy(&queue);

    printf("</queue_test_thread_safe>\n");
}
