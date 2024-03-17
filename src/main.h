
#include "../lib/spark.h"
#define MAX_DEPTH 5
typedef struct LINE {
    int cmove;              // Number of moves in the line.
    int argmove[MAX_DEPTH];  // The line.
}   LINE;


int evaluate(moves* m_list);
int development();
int mat_balance(); 
int mini_max_ab(int depth, int alpha, int beta);
int find_best_move(int depth);
int positional_score();
int max(int number1, int number2);
int min(int number1, int number2);
extern int go_infinate;
void play(int depth);

