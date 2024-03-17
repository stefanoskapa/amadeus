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
    
  uci(7);
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
  int bestMove = 0;
  int bestEval = pos_side ? INT_MAX : INT_MIN;
  int score = bestEval;
  moves available_moves = {{0},0};
  generate_moves(&available_moves);
  
  for (int i = 0; i < available_moves.current_index; i++) {
      
      make_move(available_moves.moves[i]);
      score = mini_max_ab(depth - 1, INT_MIN, INT_MAX);
      if ( (pos_side && score > bestEval) || (!pos_side && score < bestEval)) {
        bestEval = score;
	bestMove = available_moves.moves[i];
      }
      takeback();
    }
   
   if (bestMove == 0) {
     logMessage("No move selected!\n");
   
  }


  return bestMove; 
}

int mini_max_ab(int depth, int alpha, int beta) {
  moves new_moves = {{0},0};
  generate_moves(&new_moves);

  if (depth == 0 || new_moves.current_index == 0) {
    return evaluate(&new_moves);
  }

  int best, score;
  if (pos_side == white) {
    best = INT_MIN;
    for (int i = 0; i<new_moves.current_index; i++) {
      make_move(new_moves.moves[i]);
      score = mini_max_ab(depth - 1, alpha, beta);
      takeback();
      
      best = max(best,score);
      alpha = max(alpha, best);
        
      if (beta <= alpha)
        break;
    }

  } else {
    best = INT_MAX;
    for (int i = 0; i < new_moves.current_index; i++) {
      make_move(new_moves.moves[i]);
      score = mini_max_ab(depth - 1, alpha, beta);
      takeback();
      
      best = min(best, score); 
      beta = min(beta, best);

      if (beta <= alpha)
        break;
        
    }
  
  }

  return best;
}


int mat_balance() {
  int sum = 0;
  for (int i = P; i <=k; i++) 
    sum += __builtin_popcountll(pos_pieces[i]) * piece_value[i];
  return sum;
}

int development() {
  int score = 0;

  score += __builtin_popcountll(pos_pieces[N] & W_KNIGHTS);
  score += __builtin_popcountll(pos_pieces[B] & W_BISHOPS);
  score -= __builtin_popcountll(pos_pieces[n] & B_KNIGHTS);
  score -= __builtin_popcountll(pos_pieces[b] & B_BISHOPS);

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

int max(int a, int b) {
    return (a > b) ? a : b;
}
int min(int a, int b) {
    return (a < b) ? a : b;
}

