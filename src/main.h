#ifndef MAIN_H
#define MAIN_H

#include "../lib/spark.h"

U64 minimax(int max_depth);
U64 mini_max_ab(int depth,int max_depth, int alpha, int beta, int wasCap);
int find_best_move(int depth);
int max(int number1, int number2);
int min(int number1, int number2);
extern int go_infinate;
void play(int depth);

#endif
