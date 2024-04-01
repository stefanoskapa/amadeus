#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "../uci/uci.h"
#include "../../lib/spark.h"
#include "../eval/eval.h"
#include "../zobr/zobr.h"
#include "search.h"

#define PACK_MOVE_EVAL(move, evaluation) (((unsigned long long)(move) << 32) | ((evaluation) & 0xFFFFFFFF))
#define UNPACK_MOVE(packed) ((packed) >> 32)
#define UNPACK_EVAL(packed) ((int)((packed) & 0xFFFFFFFF))

long_stack visited = {{0},0};

int find_best_move(int depth) {
  U64 moveEval = minimax(depth);
  int bestMove = UNPACK_MOVE(moveEval);
  if (bestMove == 0) {
    logMessage("No move selected!\n");  
  }
  show_board();
  return bestMove; 
}

U64 minimax(int max_depth) {
  return mini_max_ab(0, max_depth, INT_MIN, INT_MAX);
}


int isThreefold() {
  if (visited.index == 0)
    return 0;
  U64 last = visited.items[visited.index - 1];
  int count = 0;
  for (int i = 0; i < visited.index - 1; i++) {
    if(visited.items[i] == last) {
      count++;
    }

    if (count == 2) {
      return 1; // threefold repetition detected
    }
  }
  return 0;

}

/*
   Stand_pat is the evaluation of the current position and
   is used for the base condition.
   If white to move is  already better than black's lowest guaranteed
   outcome (beta), then white is good enough (as he additionally gets
   to play a move). If stand_pat is better than white's best outcome,
   alpha takes the value of stand_pat. The same logic is repeated
   while backtracking because it will prevent unnecessary recursive
   calls.

 */
U64 q_search(int depth, int alpha, int beta, int max_depth) {

  moves new_moves = generate_moves();

  if (new_moves.current_index == 0) {
    if (isKingInCheck(pos_side)) //checkmate
      return pos_side ? INT_MAX - depth : INT_MIN + depth;
    return 0; //stalemate
  }

  int stand_pat = evaluate(); 

  if (pos_side == white) {
    if (stand_pat >= beta) {
      return beta;
    }

    alpha = max(alpha, stand_pat);

    for (int i = 0; i < new_moves.capture_count; i++) {
      make_move(new_moves.moves[i]);
      int score = q_search(depth + 1, alpha, beta, max_depth);
      takeback();

      if (score >= beta) {
        return beta;
      }

      if (score > alpha)
        alpha = score;
    }
  } else {
    if (stand_pat <= alpha) {
      return alpha;
    }

    if (stand_pat < beta)
      beta = stand_pat;

    for (int i = 0; i<new_moves.capture_count; i++) {

      make_move(new_moves.moves[i]);
      int score = q_search(depth + 1, alpha, beta, max_depth);
      takeback();

      if (score <= alpha) {
        return alpha;
      }

      if (score < beta)
        beta = score;
    }
  }

  return pos_side == white ? alpha : beta;

}

U64 mini_max_ab(int depth,int max_depth, int alpha, int beta) {
  //printf("depth=%d\n", depth);
  //show_board();
  moves new_moves = generate_moves();

  int hasChecks = 0;


  if (new_moves.current_index == 0) {
    if (isKingInCheck(pos_side)) //checkmate
      return pos_side ? INT_MAX - depth : INT_MIN + depth;
    return 0; //stalemate
  } 

  if (isThreefold() == 1)
    return 0;

  if (depth >= max_depth + 5) {
    for (int i = 0; i < new_moves.current_index; i++) {
      if (get_move_check(new_moves.moves[i])) {
        hasChecks = 1;
        break;
      }
    }
  }

  if (depth >= max_depth && !isKingInCheck(pos_side) && !hasChecks) {
    int score = q_search(depth, alpha, beta, max_depth);
    return score;
  }

  int bestMove = 0;
  int bestEval, score;
  if (pos_side == white) {
    bestEval = INT_MIN;
    for (int i = 0; i<new_moves.current_index; i++) {
      //printf("\n%*s%s", depth* 2, "", get_move_UCI(new_moves.moves[i]));

      make_move(new_moves.moves[i]);
      pushl(&visited, get_zobrist());

      score = mini_max_ab(depth + 1, max_depth, alpha, beta);
      takeback();
      popl(&visited);
      //  if (depth == 0) {
      //    printf("move=%s  score=%d\n",get_move_UCI(new_moves.moves[i]), score); 
      //  }
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
      pushl(&visited, get_zobrist());
      score = mini_max_ab(depth + 1, max_depth, alpha, beta);

      takeback();
      popl(&visited);
      // if (depth == 0) {
      //   printf("move=%s  score=%d\n",get_move_UCI(new_moves.moves[i]), score); 
      // }

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



int max(int a, int b) {
  return (a > b) ? a : b;
}

int min(int a, int b) {
  return (a < b) ? a : b;
}

void pushl(long_stack *is, U64 item) { 
  if (is->index > 600) {
    printf("Too many moves, stack full\n");
    exit(1);
  }
  is->items[is->index++] = item; 

}

U64 popl(long_stack *is) {
  is->index--;
  return is->items[is->index];
}

void show_stackl(long_stack *is) {
  printf("Stack contents\n");
  for (int i = 0; i < is->index; i++) {
    printf("%llu\n", is->items[i]);
  }
}
