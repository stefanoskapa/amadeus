#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "uci.h"
#include "../main.h"
#include "../../lib/spark.h"

FILE* logFile = NULL;
int inf_move = 0;

void uci(int depth) {
  openLog();

  char line[4096];
  while (fgets(line, sizeof(line), stdin)) {
    logMessage(line);    
    if (strncmp(line, "uci", 3) == 0) {
      uci_send("id name Amadeus");
      uci_send("id author StefanosKapa");
      uci_send("uciok");
    } else if (strncmp(line, "isready", 7) == 0) {
      uci_send("readyok");
    } else if (strncmp(line, "go", 2) == 0) {     
      if (strncmp(line, "go depth", 8) == 0) {
        int custom_depth;
        sscanf(line, "go depth %d", &custom_depth);
        int move = find_best_move(custom_depth);
        uci_send_bestmove(move);
      } else if (strstr(line,"infinite")) {
        inf_move = find_best_move(depth);
        go_infinate = 1;
      } else {
        int move = find_best_move(depth);
        uci_send_bestmove(move);
      }
    } else if (strncmp(line, "stop", 4) == 0) {
      if (go_infinate) {
        uci_send_bestmove(inf_move);
        go_infinate = 0;
      } else {
        logMessage("Stop was sent but not in infinate mode\n");
      }
    } else if (strncmp(line, "position", 8) == 0) {
      if (strstr(line, "startpos")) {
        parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
      }   
      if (strstr(line, "moves")) {
        parseMoves(line);
      }	
    } else if (strncmp(line, "ucinewgame", 10) == 0) {
      parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    } else if (strncmp(line, "show",4) == 0) {
      show_board();
      print_bitboard(pos_occupancies[0]);
      print_bitboard(pos_occupancies[1]);
      print_bitboard(pos_occupancies[2]);
    } else if (strncmp(line, "quit", 4) == 0) {
      closeLog();    
      break;
    }

  }

}

void uci_send(char* content) {

  logMessage(content);
  printf("%s\n", content);

  fflush(stdout);
}

void uci_send_bestmove(int move) {
  const char *str1 = "bestmove ";
  const char *str2 = get_move_UCI(move);
  char result[16]; 
  sprintf(result, "%s%s", str1, str2);
  uci_send(result);
}

void parseMoves(char* input) {

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

      moves temp = {{0},0};
      generate_moves(&temp);

      char* result;
      for (int i = 0; i < temp.current_index; i++) {

        move = temp.moves[i];
        result = get_move_UCI(move);
        int j = get_move_ep(move) ? 5 : 4;
        if (strncmp(token, result, j) == 0) {
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


void openLog() {
  logFile = fopen("engine.log", "w");
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
