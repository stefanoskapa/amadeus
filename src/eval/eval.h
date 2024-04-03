#ifndef EVAL_H
#define EVAL_H

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

int evaluate();
int king_safety();
int development();
int mat_balance(); 
int pawn_structure();
int positional_score();
void show_evaluation();
#endif
