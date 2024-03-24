#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "../uci/uci.h"
#include "../../lib/spark.h"
#include "../eval/eval.h"
#include "../search/search.h"
#include "../zobr/zobr.h"
#include "test.h"

#define PACK_MOVE_EVAL(move, evaluation) (((unsigned long long)(move) << 32) | ((evaluation) & 0xFFFFFFFF))
#define UNPACK_MOVE(packed) ((packed) >> 32)
#define UNPACK_EVAL(packed) ((int)((packed) & 0xFFFFFFFF))

clock_t start, end;
double time_used;

int main(void) {
  init_attack_tables();
  init_zobrist();

  run_tests();
  
  startpos();
  play(5);
}

void run_tests() {
  test1();
  test2();
}
void test1() {
  printf("\n [ Test 1 ]\n");
  parse_fen("rnbqkb1r/pppppppp/8/3nP3/3P4/8/PPP2PPP/RNBQKBNR b KQkq - 0 3");
  show_evaluation();
  int move = find_best_move(5);
  
  make_move(move);
  show_evaluation();
}

void test2() {
  
  printf("\n [ Test 2 ]\n");
  parse_fen("rnbqkb1r/ppp1pppp/5n2/3p4/3P4/2N5/PPP1PPPP/R1BQKBNR w KQkq - 2 3");
  show_evaluation();
  int move = find_best_move(5); 
  make_move(move);
  show_evaluation();

}

void startpos() {
  parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

void play(int depth) {
  while(1) {

    getchar();
    start = clock();
    int move = find_best_move(depth);
    end = clock();

    if (move == 0)
      break;
    make_move(move);
    pushl(&visited, get_zobrist());
    print_move_UCI(move);
    show_evaluation();
    time_used = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
    printf("Time: %f ms\n", time_used);
  } 
}



