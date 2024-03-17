#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "uci/uci.h"
#include "../lib/spark.h"
#include "main.h"
#define CENTER  ((1ULL << e4) | (1ULL << d4) | (1ULL << e5) | (1ULL << d5))
#define W_KNIGHTS  ((1ULL << b1) | (1ULL << g1))
#define B_KNIGHTS  ((1ULL << b8) | (1ULL << g8))
#define W_BISHOPS  ((1ULL << c1) | (1ULL << f1))
#define B_BISHOPS  ((1ULL << c8) | (1ULL << f8))

#define PACK_MOVE_EVAL(move, evaluation) (((unsigned long long)(move) << 32) | ((evaluation) & 0xFFFFFFFF))
#define UNPACK_MOVE(packed) ((packed) >> 32)
#define UNPACK_EVAL(packed) ((int)((packed) & 0xFFFFFFFF))

int go_infinate = 0;
clock_t start, end;
double time_used;

int piece_value[] = { 
  [P] = 10, [p] = -10,
  [N] = 30, [n] = -30,
  [B] = 30, [b] = -30,
  [R] = 50, [r] = -50,
  [Q] = 90, [q] = -90,
  [K] = 999, [k] = -999
};

int main(void) {
  init_attack_tables();
    
  uci(4);
  //starting_pos();
  //show_board();
  //play(7);
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
  //int bestEval = UNPACK_EVAL(moveEval); 
  if (bestMove == 0) {
     logMessage("No move selected!\n");  
  }
  return bestMove; 
}

U64 minimax(int max_depth) {
  return mini_max_ab(0, max_depth, INT_MIN, INT_MAX, 0);
}

U64 mini_max_ab(int depth,int max_depth, int alpha, int beta, int wasCap) {
  moves new_moves = {{0},0};
  generate_moves(&new_moves);
  if ((depth >= max_depth && !wasCap) || new_moves.current_index == 0) {
    return evaluate(&new_moves); //TODO stop if wasCap==false && noOponnentCaptures
  }


  int bestMove = 0;
  int bestEval, score;
  if (pos_side == white) {
    bestEval = INT_MIN;
    for (int i = 0; i<new_moves.current_index; i++) {
      
      //printf("depth %d   move %s\n", depth, get_move_UCI(new_moves.moves[i]));
      make_move(new_moves.moves[i]);
      score = mini_max_ab(depth + 1, max_depth, alpha, beta, get_move_capture(new_moves.moves[i]));
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
      
      //printf("depth %d   move %s\n", depth, get_move_UCI(new_moves.moves[i]));
	    make_move(new_moves.moves[i]);
      score = mini_max_ab(depth + 1, max_depth, alpha, beta, get_move_capture(new_moves.moves[i]));
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


int mat_balance() {
  int sum = 0;
  for (int i = P; i <=k; i++) 
    sum += __builtin_popcountll(pos_pieces[i]) * piece_value[i];
  return sum;
}

int development() {
  int score = 0;
  score -= __builtin_popcountll(pos_pieces[N] & W_KNIGHTS);
  score -= __builtin_popcountll(pos_pieces[B] & W_BISHOPS);
  score += __builtin_popcountll(pos_pieces[n] & B_KNIGHTS);
  score += __builtin_popcountll(pos_pieces[b] & B_BISHOPS);
  return score;
}

int pawn_structure() {
  int wscore = __builtin_popcountll(pos_pieces[P] & CENTER);
  int bscore = __builtin_popcountll(pos_pieces[p] & CENTER);  
  wscore = wscore * 2 * wscore;
  bscore = bscore * 2 * bscore;
  return (wscore - bscore) * 2;
}

int positional_score() {
  int score = development() + pawn_structure();
  return score;
}

int evaluate(moves* m_list) {
  if (m_list->current_index == 0) {
    if (isKingInCheck(pos_side))
      return pos_side ? 9999 : -9999; //checkmate
    else
      return 0; //stalemate
  }

  return mat_balance() + positional_score();

}

inline int max(int a, int b) {
    return (a > b) ? a : b;
}
inline int min(int a, int b) {
    return (a < b) ? a : b;
}

