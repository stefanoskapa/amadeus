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
  play(4);
}

void run_tests() {

  opening_best_move_test_1();
  opening_best_move_test_2();
  opening_best_move_test_3();

  dont_do_this_test_1();
  dont_do_this_test_2();
  dont_do_this_test_3();
  dont_do_this_test_4();
  dont_do_this_test_5();

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

/*
  White sacrificed the knight on e4
*/
void dont_do_this_test_2() {
  printf("\n [ \"Don't do this\" Test 2 ]\n");
  parse_fen("2r2rk1/ppq2pp1/2n1pnp1/1B1p4/3Pp3/1PN1QP1P/P1P3P1/R4RK1 w - - 1 17");
  show_board();
  int move = find_best_move(5);
  print_move_UCI(move);
  if (strcmp(get_move_UCI(move), "c3e4") == 0) {
    printf("Failed :(\n");
  } else {
    printf("Success: Bad move avoided\n");
  }
}

/*
  Engine thinks it can go e3e4 because it 
  doesn't see the Qa5+ tactic after the exchanges
  on e4.
*/
void dont_do_this_test_3() {
  printf("\n [ \"Don't do this\" Test 3 ]\n");
  parse_fen("3rk2r/ppq2pp1/2nppnp1/1B1p4/3P4/1PN1PQ1P/P1P2PP1/R3K2R w KQk - 1 13");
  show_board();
  int move = find_best_move(5);
  print_move_UCI(move);
  if (strcmp(get_move_UCI(move), "e3e4") == 0) {
    printf("Failed :(\n");
  } else {
    printf("Success: Bad move avoided\n");
  }
}
/*
  Plays Bd3 in Rossolimo Variation

*/
void dont_do_this_test_4() {
  printf("\n [ \"Don't do this\" Test 4 ]\n");
  parse_fen("r1bqkbnr/pp1ppppp/2n5/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3");
  show_board();
  int move = find_best_move(5);
  print_move_UCI(move);
  if (strcmp(get_move_UCI(move), "f1d3") == 0) {
    printf("Failed :(\n");
  } else {
    printf("Success: Bad move avoided\n");
  }
}

/*
  Runs into stalemate 
  https://lichess.org/wyrufcKz
  
  Reproducible only with UCI command:

  position startpos moves e2e4 b8c6 d2d4 d7d5 e4d5 d8d5 g1e2 d5f5 c1f4 e7e5 d4e5 c6e5 d1d4 f8d6 b1c3 g8f6 c3b5 e5f3 g2f3 f5b5 f4d6 c7d6 e1c1 e8g8 d4d6 b5a5 a2a3 c8e6 d6d2 a5e5 f1g2 e6c4 e2c3 f8e8 a3a4 e5c5 f3f4 f6g4 d2d4 c5d4 d1d4 g4f2 g2b7 a8b8 h1g1 b8b7 d4c4 b7b6 f4f5 b6f6 c3b5 a7a5 h2h4 g8f8 b2b3 f6f5 b5d6 f2h3 g1d1 f5e5 d6e8 e5e8 d1d5 e8a8 d5h5 h7h6 c4c5 h3f4 c5a5 a8d8 h5b5 f4g2 h4h5 g2e3 c2c3 d8d1 c1b2 d1d2 b2a3 e3c2 a3b2 c2d4 b2c1 d2c2 c1b1 c2c3 a5a8 f8e7 a8a7 e7d6 a7a6 d4c6 b3b4 c3b3 b1a2 b3c3 a6a8 c3e3 a4a5 d6e7 a5a6 e3g3 a6a7 e7f6 b5b8 c6d4 b8c8 g3h3 c8c5 h3h2 a2a3 h2h3 a3a4 h3h2 b4b5 d4e6 c5c8 f6g5 b5b6 h2h4 a4a5 h4h1 b6b7 h1a1 a5b6 a1b1 b6c6 b1c1 c6d6 c1d1 d6e7 e6c7 b7b8q c7a8 e7f7 g5h5 f7g7 d1d7 g7f8 d7d5 b8g3 d5f5 f8e8 a8b6 a7a8q b6a8 c8a8 f5f8

go depth 5
*/
void dont_do_this_test_5() {
  printf("\n [ \"Don't do this\" Test 5 ]\n");
  parse_fen("R3Kr2/8/7p/7k/8/6Q1/8/8 w - - 1 75");
  show_board();
  int move = find_best_move(5);
  print_move_UCI(move);
  if (strcmp(get_move_UCI(move), "e8f8") == 0) {
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



