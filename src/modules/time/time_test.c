#include "index.h"

void
time_test(void) {
    test_start();

    who_printf("time_second(): %f\n", time_second());
    who_printf("time_second(): %f\n", time_second());
    who_printf("time_second(): %f\n", time_second());

    test_end();
}
