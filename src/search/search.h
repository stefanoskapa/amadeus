#ifndef SEARCH_H
#define SEARCH_H

#include "../../lib/spark.h"

BB minimax(int max_depth);
BB mini_max_ab(int depth,int max_depth, int alpha, int beta);
int find_best_move(int depth);
int max(int number1, int number2);
int min(int number1, int number2);

typedef struct long_stack {
  BB items[600];
  int index;
} long_stack;

extern long_stack visited;
void pushl(long_stack*, BB);
BB popl(long_stack*);

#endif
