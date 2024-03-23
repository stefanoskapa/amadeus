#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "uci/uci.h"
#include "../lib/spark.h"
#include "main.h"
#include "eval/eval.h"

#define PACK_MOVE_EVAL(move, evaluation) (((unsigned long long)(move) << 32) | ((evaluation) & 0xFFFFFFFF))
#define UNPACK_MOVE(packed) ((packed) >> 32)
#define UNPACK_EVAL(packed) ((int)((packed) & 0xFFFFFFFF))

int go_infinate = 0;
clock_t start, end;
double time_used;
U64 z_pieces[12][64];
U64 z_castling[16];
U64 z_ep[65];
U64 z_side[2];

int main(void) {
  init_attack_tables();
  init_zobrist();
  //startpos();

  //printf("%llu\n", get_zobrist());

  uci(6);
}

U64 get_zobrist() {

  U64 hash = 0ULL;

  for (int i = 0; i < 64; i++) {
    int piece = pos_occupancy[i];
    if (piece == INT_MAX)
      continue;
    hash ^= z_pieces[piece][i];
  }
  hash ^= z_ep[pos_ep];
  hash ^= z_castling[pos_castling];
  hash ^= z_side[pos_side];
  return hash;
}

void init_zobrist() {
  for (int i = 0; i < 12; i++) {
    for (int sq = 0; sq < 64; sq++) {
      U64 num = rand_64();
      z_pieces[i][sq] = num;
    }
  }

  for (int i = 0; i < 65; i++) {
    z_ep[i] = rand_64();
  }

  for (int i = 0; i < 16; i++) {
    z_castling[i] = rand_64();
  }

  z_side[0] = rand_64();
  z_side[1] = rand_64();
}

U64 rand_64(void) {
  U64 r = 0;
  for (int i=0; i<64; i++) {
    r = r*2 + rand()%2;
  }
  return r;
}

void startpos() {
  parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

}
void play(int depth) {
  while(1) {
    start = clock();
    int move = find_best_move(depth);
    end = clock();

    if (move == 0)
      break;

    make_move(move);

    print_move_UCI(move);
    show_board();
    time_used = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
    printf("Time: %f ms\n", time_used);
  } 
}

int find_best_move(int depth) {
  U64 moveEval = minimax(depth);
  int bestMove = UNPACK_MOVE(moveEval);
  if (bestMove == 0) {
    logMessage("No move selected!\n");  
  }

  return bestMove; 
}

U64 minimax(int max_depth) {
  return mini_max_ab(0, max_depth, INT_MIN, INT_MAX);
}

U64 mini_max_ab(int depth,int max_depth, int alpha, int beta) {
  moves new_moves = {{0},0};
  generate_moves(&new_moves);

  if (new_moves.current_index == 0) {
    if (isKingInCheck(pos_side)) //checkmate
      return pos_side ? INT_MAX - depth : INT_MIN + depth;
    return 0; //stalemate
  } 

  if (depth == max_depth) {
    int score = evaluate();
    return score;
  }

  int bestMove = 0;
  int bestEval, score;
  if (pos_side == white) {
    bestEval = INT_MIN;
    for (int i = 0; i<new_moves.current_index; i++) {
      //printf("\n%*s%s", depth* 2, "", get_move_UCI(new_moves.moves[i]));

      make_move(new_moves.moves[i]);
      score = mini_max_ab(depth + 1, max_depth, alpha, beta);
      takeback();

      bestEval = max(bestEval,score);

      if (bestEval > alpha) {
        bestMove = new_moves.moves[i];
        alpha = bestEval;
      }  

      if (beta <= alpha)
        break;
    }

  } else {
    bestEval = INT_MAX;
    for (int i = 0; i < new_moves.current_index; i++) {
      //printf("\n%*s%s", depth *2, "", get_move_UCI(new_moves.moves[i]));

      make_move(new_moves.moves[i]);
      score = mini_max_ab(depth + 1, max_depth, alpha, beta);
      takeback();

      bestEval = min(bestEval, score); 

      if (bestEval < beta) {
        bestMove = new_moves.moves[i];
        beta = bestEval;
      } 

      if (beta <= alpha)
        break;

    }

  }

  return PACK_MOVE_EVAL(bestMove, bestEval);
}



inline int max(int a, int b) {
  return (a > b) ? a : b;
}

inline int min(int a, int b) {
  return (a < b) ? a : b;
}

