#ifndef EVAL_H
#define EVAL_H


extern int evaluate(moves* m_list, int depth);
int development();
int mat_balance(); 
int pawn_structure();
int positional_score();

#endif
