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
U64 z_pieces[12][64];
U64 z_castling[16];
U64 z_ep[65];
U64 z_side[2];

long_stack visited = {{0},0};

int main(void) {
  init_attack_tables();
  init_zobrist();

//   startpos();
//parse_fen("rnbqkb1r/pppppppp/8/4P3/4n3/3P4/PPP2PPP/RNBQKBNR b KQkq - 0 3");
parse_fen("rnbqkb1r/pppppppp/8/4P3/4n3/3P4/PPP2PPP/RNBQKBNR b KQkq - 0 3");
play(5);

  uci(6);
}

U64 get_zobrist() {

  U64 hash = 0ULL;

  for (int i = 0; i < 64; i++) {
    int piece = pos_occupancy[i];
    if (piece == INT_MAX)
      continue;
    hash ^= z_pieces[piece][i];
  }
  hash ^= z_ep[pos_ep]; //not necessary to have 64 squares
  hash ^= z_castling[pos_castling];
  hash ^= z_side[pos_side];
  return hash;
}

void init_zobrist() {
  for (int i = 0; i < 12; i++) {
    for (int sq = 0; sq < 64; sq++) {
      U64 num = rand_64();
      z_pieces[i][sq] = num;
    }
  }

  for (int i = 0; i < 65; i++) {
    z_ep[i] = rand_64();
  }

  for (int i = 0; i < 16; i++) {
    z_castling[i] = rand_64();
  }

  z_side[0] = rand_64();
  z_side[1] = rand_64();
}

U64 rand_64(void) {
  U64 r = 0;
  for (int i=0; i<64; i++) {
    r = r*2 + rand()%2;
  }
  return r;
}

void startpos() {
  parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

}
void play(int depth) {
  while(1) {

    getchar();
    start = clock();
    int move = find_best_move(depth);
    end = clock();

    if (move == 0)
      break;
    make_move(move);
    pushl(&visited, get_zobrist());
    print_move_UCI(move);
    show_evaluation();
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
U64 q_search(int depth, int alpha, int beta) {

  moves new_moves = {{0},0};
  generate_moves(&new_moves);
  if (new_moves.current_index == 0) {
    if (isKingInCheck(pos_side)) //checkmate
      return pos_side ? INT_MAX - depth : INT_MIN + depth;
    return 0; //stalemate
  } 

  int stand_pat = evaluate(); 

  if (pos_side == white) {
    if (stand_pat >= beta)
      return beta;
    alpha = max(alpha, stand_pat);

    for (int i = 0; i<new_moves.current_index && get_move_capture(new_moves.moves[i]); i++) {
      make_move(new_moves.moves[i]);
      int score = q_search(depth + 1, alpha, beta);
      takeback();

      if (score >= beta)
        return beta;
      if (score > alpha)
        alpha = score;
    }
  } else {
    if (stand_pat <= alpha)
      return alpha;
    if (stand_pat < beta)
      beta = stand_pat;

    for (int i = 0; i<new_moves.current_index && get_move_capture(new_moves.moves[i]); i++) {

      make_move(new_moves.moves[i]);
      int score = q_search(depth + 1, alpha, beta);
      takeback();

      if (score <= alpha)
        return alpha;
      if (score < beta)
        beta = score;
    }
  }

  return pos_side == white ? alpha : beta;
}


U64 mini_max_ab(int depth,int max_depth, int alpha, int beta) {

  moves new_moves = {{0},0};
  generate_moves(&new_moves);

  if (new_moves.current_index == 0) {
    if (isKingInCheck(pos_side)) //checkmate
      return pos_side ? INT_MAX - depth : INT_MIN + depth;
    return 0; //stalemate
  } 

  if (isThreefold() == 1)
    return 0;

  if (depth == max_depth) {
    int score = q_search(depth, alpha, beta);
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
//      if (depth == 0) {
//        printf("move=%s  score=%d\n",get_move_UCI(new_moves.moves[i]), score); 
//      }
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
  //    if (depth == 0) {
  //      printf("move=%s  score=%d\n",get_move_UCI(new_moves.moves[i]), score); 
  //    }

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
