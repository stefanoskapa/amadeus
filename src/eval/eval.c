#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "../uci/uci.h"
#include "../../lib/spark.h"
#include "../main.h"
#include "eval.h"
#define CENTER  ((1ULL << e4) | (1ULL << d4) | (1ULL << e5) | (1ULL << d5))
#define W_KNIGHTS  ((1ULL << b1) | (1ULL << g1))
#define B_KNIGHTS  ((1ULL << b8) | (1ULL << g8))
#define W_BISHOPS  ((1ULL << c1) | (1ULL << f1))
#define B_BISHOPS  ((1ULL << c8) | (1ULL << f8))



int piece_value[] = { 
  [P] = 100, [p] = -100,
  [N] = 300, [n] = -300,
  [B] = 300, [b] = -300,
  [R] = 500, [r] = -500,
  [Q] = 900, [q] = -900,
  [K] = 9999, [k] = -9999
};

void show_evaluation() {

  double material = mat_balance();
	double develop = development();
	double center = pawn_structure();
	double total = material + develop + center;

	show_board();
  printf("Material:    %.2f\n", (material/100));
  printf("Development: %.2f\n", (develop/100));
  printf("Pawn Center: %.2f\n", (center/100));
  printf("Grand total: %.2f\n", (total/100));
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
  //return score * 25; //reasonable
	return score * 50;
}

int pawn_structure() {
  int wscore = __builtin_popcountll(pos_pieces[P] & CENTER);
  int bscore = __builtin_popcountll(pos_pieces[p] & CENTER);  
  wscore = wscore * wscore * wscore;
  bscore = bscore * bscore * bscore;;
  //return (wscore - bscore) * 10; //reasonable
  return (wscore - bscore) * 20;
}

int positional_score() {
  int score = development() + pawn_structure();
  return score;
}

int evaluate(moves* m_list, int depth) {
  if (m_list->current_index == 0) {
    if (isKingInCheck(pos_side)){
	//	  printf("Checkmate found\n");
	//		show_board();
      return pos_side ? 99999999 - depth : -99999999 + depth; //checkmate
    }
		else
      return 0; //stalemate
  }

  return mat_balance() + positional_score();

}


