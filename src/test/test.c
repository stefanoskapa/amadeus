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

  opening_best_move_test_1();
  opening_best_move_test_2();
  opening_best_move_test_3();

  dont_do_this_test_1();

}

/* Alekhine Defense: 1.e4 Nf6 2.e5 Nd5 3.d4
  d7d6 is the most aggressive option and probably the best
  move in the position:
  1) Challenges white's impressive pawn center
  2) Enables minor piece development for black
*/
void opening_best_move_test_1() {
  printf("\n [ Opening Best Move Test 1 ]\n");
  parse_fen("rnbqkb1r/pppppppp/8/3nP3/3P4/8/PPP2PPP/RNBQKBNR b KQkq - 0 3");
  show_board();
  int move = find_best_move(5);
  print_move_UCI(move);
  if (strcmp(get_move_UCI(move), "d7d6") == 0) {
    printf("Success: best move found\n");
  } else {
    printf("Failed :(\n");
  }
}

/* St. George Defense: 1.e4 a6
   d2d4 is no doubt the best punishment for such a passive move.
   1) White builds an ideal pawn center for free
   2) Controls the central squares
   3) Can develop his pieces quickly
*/

void opening_best_move_test_2() {
  printf("\n [ Opening Best Move Test 2 ]\n");
  parse_fen("rnbqkbnr/1ppppppp/p7/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2");
  show_board();
  int move = find_best_move(5);
  print_move_UCI(move);
  if (strcmp(get_move_UCI(move), "d2d4") == 0) {
    printf("Success: best move found\n");
  } else {
    printf("Failed :(\n");
  }
}

/* Owen Defense: 1.e4 b6
   d2d4 is again the best move, for the same reasons as in the
   previous test case.
*/
void opening_best_move_test_3() {
  printf("\n [ Opening Best Move Test 3 ]\n");
  parse_fen("rnbqkbnr/p1pppppp/1p6/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2");
   show_board();
  int move = find_best_move(5);
  print_move_UCI(move);
  if (strcmp(get_move_UCI(move), "d2d4") == 0) {
    printf("Success: best move found\n");
  } else {
    printf("Failed :(\n");
  }
}

void dont_do_this_test_1() {
  printf("\n [ \"Don't do this\" Test 1 ]\n");
  parse_fen("r1bqk2r/ppp1bppp/2n1pn2/3p4/3P4/3BPN1P/PPPN1PP1/R1BQK2R b KQkq - 2 6");
  show_board();
  int move = find_best_move(5);
  print_move_UCI(move);
  if (strcmp(get_move_UCI(move), "a7a5") == 0) {
    printf("Failed :(\n");
  } else {
    printf("Success: Bad move avoided\n");
  }

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



