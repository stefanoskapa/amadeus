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


int main(void) {
  init_attack_tables();
  

  uci(6);
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
    if (isKingInCheck(pos_side))
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

