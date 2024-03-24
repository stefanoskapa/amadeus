#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "../uci/uci.h"
#include "../../lib/spark.h"
#include "eval.h"

#define CENTER  ((1ULL << e4) | (1ULL << d4) | (1ULL << e5) | (1ULL << d5))
#define W_KNIGHTS  ((1ULL << b1) | (1ULL << g1))
#define B_KNIGHTS  ((1ULL << b8) | (1ULL << g8))
#define W_BISHOPS  ((1ULL << c1) | (1ULL << f1))
#define B_BISHOPS  ((1ULL << c8) | (1ULL << f8))

#define W_P_E2_D2 ( (1ULL << e2) | (1ULL << d2))
#define B_P_E7_D7 ( (1ULL << e7) | (1ULL << d7))
#define W_P_BONUS_7 ((1ULL << a7 | 1ULL << b7 | 1ULL << c7 | 1ULL << d7 | 1ULL << e7 | 1ULL << f7 | 1ULL << g7 | 1ULL << h7))
#define W_P_BONUS_6 ((1ULL << a6 | 1ULL << b6 | 1ULL << c6 | 1ULL << d6 | 1ULL << e6 | 1ULL << f6 | 1ULL << g6 | 1ULL << h6))
#define W_P_BONUS_5 ((1ULL << a5 | 1ULL << b5 | 1ULL << c5 | 1ULL << d5 | 1ULL << e5 | 1ULL << f5 | 1ULL << g5 | 1ULL << h5))

#define B_P_BONUS_2 ((1ULL << a2 | 1ULL << b2 | 1ULL << c2 | 1ULL << d2 | 1ULL << e2 | 1ULL << f2 | 1ULL << g2 | 1ULL << h2))
#define B_P_BONUS_3 ((1ULL << a3 | 1ULL << b3 | 1ULL << c3 | 1ULL << d3 | 1ULL << e3 | 1ULL << f3 | 1ULL << g3 | 1ULL << h3))
#define B_P_BONUS_4 ((1ULL << a4 | 1ULL << b4 | 1ULL << c4 | 1ULL << d4 | 1ULL << e4 | 1ULL << f4 | 1ULL << g4 | 1ULL << h4))

int b_king[] = {
    5,  10,   9, -15,   0, -15,  10,   5, 
    0, -10, -20, -20, -20, -15, -10,   0,
  -40, -40, -40, -40, -40, -40, -40, -40,
  -40, -40, -40, -40, -40, -40, -40, -40,
  -40, -40, -40, -40, -40, -40, -40, -40,
  -40, -40, -40, -40, -40, -40, -40, -40,
  -40, -40, -40, -40, -40, -40, -40, -40,
  -40, -40, -40, -40, -40, -40, -40, -40
  };

int w_king[] = {
  -40, -40, -40, -40, -40, -40, -40, -40,
  -40, -40, -40, -40, -40, -40, -40, -40,
  -40, -40, -40, -40, -40, -40, -40, -40,
  -40, -40, -40, -40, -40, -40, -40, -40,
  -40, -40, -40, -40, -40, -40, -40, -40,
  -40, -40, -40, -40, -40, -40, -40, -40,
    0, -10, -20, -20, -20, -15, -10,   0,
    5,  10,  9,  -15,   0, -15,  10,   5 
  };

int piece_value[] = { 
  [P] = 100, [p] = -100,
  [N] = 300, [n] = -300,
  [B] = 310, [b] = -310,
  [R] = 500, [r] = -500,
  [Q] = 900, [q] = -900,
  [K] = 9999, [k] = -9999
};

void show_evaluation() {

  double material = mat_balance();
  double develop = development();
  double center = pawn_structure();
  double king = king_safety();
  double total = material + develop + center + king;
  show_board();
  printf("Material:    %.2f\n", (material/100));
  printf("Development: %.2f\n", (develop/100));
  printf("Pawn Center: %.2f\n", (center/100));
  printf("King Safety: %.2f\n", (king/100));
  printf("Grand total: %.2f\n", (total/100));
}

int mat_balance() {
  int sum = 0;
  for (int i = P; i <=k; i++) 
    sum += __builtin_popcountll(pos_pieces[i]) * piece_value[i];
  return sum;
}

int king_safety() {
  
  int wscore = 0;
  int bscore = 0;
  int ksquare;
  U64 bitboard;
  if (pos_pieces[q]){ // white king score 
    ksquare = __builtin_ctzll(pos_pieces[K]);
    bitboard = get_queen_attacks(ksquare, pos_occupancies[0]);
    wscore -= __builtin_popcountll(bitboard);
    wscore += w_king[ksquare];  
  }
  
  if (pos_pieces[Q]) {
    ksquare = __builtin_ctzll(pos_pieces[k]);
    bitboard = get_queen_attacks(ksquare, pos_occupancies[1]);
    bscore -= __builtin_popcountll(bitboard);
    bscore += b_king[ksquare];
  }
  return (wscore - bscore) * 3;
}
int development() {
  int score = 0;
  score -= __builtin_popcountll(pos_pieces[N] & W_KNIGHTS) * 3;
  score -= __builtin_popcountll(pos_pieces[B] & W_BISHOPS) * 3;
  score -= __builtin_popcountll(pos_pieces[P] & W_P_E2_D2);
  score += __builtin_popcountll(pos_pieces[n] & B_KNIGHTS) * 3;
  score += __builtin_popcountll(pos_pieces[b] & B_BISHOPS) * 3;
  score += __builtin_popcountll(pos_pieces[p] & B_P_E7_D7);
 return score * 7;
}

int pawn_structure() {
  int wscore = __builtin_popcountll(pos_pieces[P] & CENTER);
  int bscore = __builtin_popcountll(pos_pieces[p] & CENTER);  
  wscore = wscore * wscore * wscore;
  bscore = bscore * bscore * bscore;;
  
  wscore += __builtin_popcountll(pos_pieces[P] & W_P_BONUS_7) * 3;
  wscore += __builtin_popcountll(pos_pieces[P] & W_P_BONUS_6) * 2;
  wscore += __builtin_popcountll(pos_pieces[P] & W_P_BONUS_5);
 
  
  bscore += __builtin_popcountll(pos_pieces[p] & B_P_BONUS_2) * 3;
  bscore += __builtin_popcountll(pos_pieces[p] & B_P_BONUS_3) * 2;
  bscore += __builtin_popcountll(pos_pieces[p] & B_P_BONUS_4);


  return (wscore - bscore) * 7;
}

int positional_score() {
  int score = development() + pawn_structure() + king_safety();
  return score;
}

int evaluate() {

  return mat_balance() + positional_score();

}


