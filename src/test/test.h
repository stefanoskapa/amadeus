#ifndef TEST_H
#define TEST_H

#include "../../lib/spark.h"

int find_best_move(int depth);
void play(int depth);
void startpos();

void opening_best_move_test_1();
void opening_best_move_test_2();
void opening_best_move_test_3();
void dont_do_this_test_1();
void dont_do_this_test_2();
void dont_do_this_test_3();
void dont_do_this_test_4();
void dont_do_this_test_5();
void run_tests();
void bench(int depth);

#endif
