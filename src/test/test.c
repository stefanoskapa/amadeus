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

   startpos();
   //parse_fen("rnbqkb1r/pppppppp/8/4P3/4n3/3P4/PPP2PPP/RNBQKBNR b KQkq - 0 3");
   //parse_fen("rnbqkb1r/pppppppp/8/4P3/4n3/3P4/PPP2PPP/RNBQKBNR b KQkq - 0 3");
   play(5);

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



