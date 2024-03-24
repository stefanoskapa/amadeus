#ifndef SEARCH_H
#define SEARCH_H

#include "../../lib/spark.h"

U64 minimax(int max_depth);
U64 mini_max_ab(int depth,int max_depth, int alpha, int beta);
int find_best_move(int depth);
int max(int number1, int number2);
int min(int number1, int number2);

typedef struct long_stack {
  U64 items[600];
  int index;
} long_stack;

extern long_stack visited;
void pushl(long_stack*, U64);
U64 popl(long_stack*);

#endif
