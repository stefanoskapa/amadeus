#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "spark.h"
#define CENTER  ((1ULL << e4) | (1ULL << d4) | (1ULL << e5) | (1ULL << d5))


int evaluate(moves* m_list);
int development();
int mat_balance(); 
int mini_max(int depth);
int find_best_move(int depth);
int positional_score();
FILE* logFile = NULL;

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
  openLog();
    

  uci();
  //starting_pos();
  //show_board();
  //play(5);
}



void uci() {


  char line[256];
    while (fgets(line, sizeof(line), stdin)) {
      logMessage(line);    
      if (strncmp(line, "uci", 3) == 0) {
        printf("id name Amadeus\n");
        printf("id author StefanosKapa\n");
        printf("uciok\n");
	fflush(stdout);
      } else if (strncmp(line, "isready", 7) == 0) {
        printf("readyok\n");
	fflush(stdout);
      } else if (strncmp(line, "go", 2) == 0) {     
        int move = find_best_move(4);
	printf("bestmove ");
	print_move_UCI(move);
	fflush(stdout);
      } else if (strncmp(line, "position", 8) == 0) {
            // Handle setting up the board and moves
          
	  if (strstr(line, "startpos")) {
	    starting_pos();
	  }   
	  if (strstr(line, "moves")) {
            parseMoves(line); // Parse and apply the moves
          }	
      } else if (strncmp(line, "ucinewgame", 10) == 0) {
        starting_pos();	      
      } else if (strncmp(line, "quit", 4) == 0) {
            break;
      }
        // Handle other commands here
    }

}
void starting_pos() {
  parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  
}

void parseMoves(char* input) {
    // Assuming input is "position startpos moves e2e4 e7e5 g1f3 ..."
    char* token = strtok(input, " "); // Tokenize the input string by spaces

    // Skip tokens until we find "moves"
    while (token != NULL && strcmp(token, "moves") != 0) {
        token = strtok(NULL, " ");
    }

    // Now, token points to "moves", so we start parsing the actual moves
    while (token != NULL) {
        token = strtok(NULL, " "); // Get the next move
        int move = 0;
	if (token != NULL) {
          //logic
	  //
	  moves temp = {{0},0};
	  generate_moves(&temp);
           
	  char result[5];
	  for (int i = 0; i < temp.current_index; i++) { //iterate over generated moves 
	  
	    move = temp.moves[i];
	    int source = get_move_source(move);
	    int target = get_move_target(move);
	    result[0] = '\0';
	    strcat(result, square_to_coordinates[source]);
	    strcat(result, square_to_coordinates[target]);
	    if (strncmp(token, result,4) == 0) {
		    break;
	    } 
	  }
          if (move == 0)
	    logMessage("could not find move!\n");
	  else { 
            make_move(move);
	  }
	
	}

    }
}


int play(int depth) {
  while(1) {
    int move = find_best_move(depth);
    if (move == 0)
      break;
    make_move(move);
   
   print_move_UCI(move);
    show_board();

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
      score = mini_max(depth - 1);
      if ( (pos_side && score > bestEval) || (!pos_side && score < bestEval)) {
        bestEval = score;
	bestMove = available_moves.moves[i];
      }
      takeback();
    }
  
   return bestMove; 
}

int mini_max(int depth) {
  moves new_moves = {{0},0};
  generate_moves(&new_moves);

  if (depth == 0)
    return evaluate(&new_moves);

  int best, score;
  if (pos_side == white) {
    best = INT_MIN;
    for (int i = 0; i<new_moves.current_index; i++) {
      make_move(new_moves.moves[i]);
      score = mini_max(depth - 1);
      if (score > best){
        best = score;
      }
      takeback();
    }

  } else {
    best = INT_MAX;
    for (int i = 0; i < new_moves.current_index; i++) {
      make_move(new_moves.moves[i]);
      score = mini_max(depth - 1);
      if (score < best) {
        best = score;
      }

      takeback();
    
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

  if (pos_pieces[N] & (1ULL << b1))
    score--; 
  if (pos_pieces[N] & (1ULL << g1))
    score--; 
  if (pos_pieces[B] & (1ULL << c1))
    score--; 
  if (pos_pieces[B] & (1ULL << f1))
    score--; 
  if (pos_pieces[n] & (1ULL << b8))
    score++; 
  if (pos_pieces[n] & (1ULL << g8))
    score++; 
  if (pos_pieces[b] & (1ULL << c8))
    score++; 
  if (pos_pieces[b] & (1ULL << f8))
    score++; 

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
  int sideSign = pos_side ? -1 : 1;
  if (m_list->current_index == 0) {
    if (isKingInCheck(pos_side))
      return pos_side ? INT_MAX : INT_MIN; //checkmate
    else
      return 0; //stalemate
  }

  return mat_balance() + positional_score();

}

void openLog() {
    logFile = fopen("engine_log.txt", "w");
    if (logFile == NULL) {
        perror("Failed to open log file");
        exit(1);
    }
}

void closeLog() {
    if (logFile) {
        fclose(logFile);
    }
}

void logMessage(const char* message) {
    if (logFile) {
        fprintf(logFile, "%s\n", message);
        fflush(logFile); // Ensure the message is written immediately
    }
}
