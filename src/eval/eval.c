#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "../uci/uci.h"
#include "../../lib/spark.h"
#include "eval.h"

int d_weight = 6;
int k_weight = 4;
int p_weight = 10;
int m_weight = 14;

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

void show_evaluation() {
  double material = mat_balance();
  double develop = development();
  double center = pawn_structure();
  double king = king_safety();
  double mobil = mobility();

  double total = material + develop + mobil + center + king;
  show_board();
  printf("Material:    %.2f\n", (material/100));
  printf("Development: %.2f\n", (develop/100));
  printf("Mobility: %.2f\n", (mobil/100));
  printf("Pawn Center: %.2f\n", (center/100));
  printf("King Safety: %.2f\n", (king/100));
  printf("Grand total: %.2f\n", (total/100));
}

int mobility() {
  int score = 0;
  U64 bitboard, attacks;
  int source;

  //white knights
  bitboard = pos_pieces[N];
  while (bitboard) {
    source = __builtin_ctzll(bitboard);
    attacks = knight_attacks[source] & (~pos_occupancies[white]); // don't capture own pieces
    clear_bit(bitboard, source);
    score += __builtin_popcountll(attacks);
  }
  

  //black knights
  bitboard = pos_pieces[n];
  while (bitboard) {
    source = __builtin_ctzll(bitboard);
    attacks = knight_attacks[source] & (~pos_occupancies[black]); // don't capture own pieces
    clear_bit(bitboard, source);
    score -= __builtin_popcountll(attacks);
  }
 

  //white bishops
  bitboard = pos_pieces[B];
  while (bitboard) {
    source = first_set_bit(bitboard);
    attacks = get_bishop_attacks(source, pos_occupancies[both]) & (~pos_occupancies[white]); // don't capture own pieces
    clear_bit(bitboard, source);
    score += __builtin_popcountll(attacks);
  }
  //black bishops
  bitboard = pos_pieces[b];
  while (bitboard) {
    source = first_set_bit(bitboard);
    attacks = get_bishop_attacks(source, pos_occupancies[both]) & (~pos_occupancies[black]); // don't capture own pieces
    clear_bit(bitboard, source);
    score -= __builtin_popcountll(attacks);
  }

  //white rooks
  bitboard = pos_pieces[R];
  while (bitboard) {
    source = first_set_bit(bitboard);
    attacks = get_rook_attacks(source, pos_occupancies[both]) & (~pos_occupancies[white]); // don't capture own pieces
    clear_bit(bitboard, source);
    score += __builtin_popcountll(attacks);
  }
  //black rooks
  bitboard = pos_pieces[r];
  while (bitboard) {
    source = first_set_bit(bitboard);
    attacks = get_rook_attacks(source, pos_occupancies[both]) & (~pos_occupancies[black]); // don't capture own pieces
    clear_bit(bitboard, source);
    score -= __builtin_popcountll(attacks);
  }
  
  return (score) * m_weight;

}


int mat_balance() {
  int sum = 0;
  sum += __builtin_popcountll(pos_pieces[P]) * 100;
  sum += __builtin_popcountll(pos_pieces[N]) * 300;
  sum += __builtin_popcountll(pos_pieces[B]) * 310;
  sum += __builtin_popcountll(pos_pieces[R]) * 500;
  sum += __builtin_popcountll(pos_pieces[Q]) * 900;

  sum += __builtin_popcountll(pos_pieces[p]) * -100;
  sum += __builtin_popcountll(pos_pieces[n]) * -300;
  sum += __builtin_popcountll(pos_pieces[b]) * -310;
  sum += __builtin_popcountll(pos_pieces[r]) * -500;
  sum += __builtin_popcountll(pos_pieces[q]) * -900;
  return sum;
}

int king_safety() {

  int score = 0;
  int ksquare;
  U64 bitboard;

  if (pos_pieces[q]){ // white king score 
    ksquare = __builtin_ctzll(pos_pieces[K]);
    bitboard = get_queen_attacks(ksquare, pos_occupancies[0]);
    score -= __builtin_popcountll(bitboard);
    score += w_king[ksquare];  
  }

  if (pos_pieces[Q]) {
    ksquare = __builtin_ctzll(pos_pieces[k]);
    bitboard = get_queen_attacks(ksquare, pos_occupancies[1]);
    score += __builtin_popcountll(bitboard);
    score -= b_king[ksquare];
  }
  return score * k_weight;
}

int development() {
  int score = 0;
  score -= __builtin_popcountll(pos_pieces[N] & W_KNIGHTS);
  score -= __builtin_popcountll(pos_pieces[B] & W_BISHOPS);
  score -= __builtin_popcountll(pos_pieces[P] & W_P_E2_D2);
  score += __builtin_popcountll(pos_pieces[n] & B_KNIGHTS);
  score += __builtin_popcountll(pos_pieces[b] & B_BISHOPS);
  score += __builtin_popcountll(pos_pieces[p] & B_P_E7_D7);
  return score * d_weight;
}

int pawn_structure() {
  int score = __builtin_popcountll(pos_pieces[P] & CENTER);
  score -= __builtin_popcountll(pos_pieces[p] & CENTER);  
  score += __builtin_popcountll(pos_pieces[P] & W_P_BONUS_7);
  score -= __builtin_popcountll(pos_pieces[p] & B_P_BONUS_2);

  return score * p_weight;
}

int positional_score() {
  int score = /*development() + pawn_structure() +*/ king_safety() + mobility();
  return score;
}

int evaluate() {
  return mat_balance() + positional_score();
}


