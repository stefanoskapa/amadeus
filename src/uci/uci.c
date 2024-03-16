#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "uci.h"
#include "../main.h"
#include "../../lib/spark.h"

FILE* logFile = NULL;

void uci() {
  openLog();

  char line[256];
    while (fgets(line, sizeof(line), stdin)) {
      logMessage(line);    
      if (strncmp(line, "uci", 3) == 0) {
        uci_send("id name Amadeus\n");
        uci_send("id author StefanosKapa\n");
        uci_send("uciok\n");
	fflush(stdout);
      } else if (strncmp(line, "isready", 7) == 0) {
        uci_send("readyok\n");
	fflush(stdout);
      } else if (strncmp(line, "go", 2) == 0) {     
        int move = find_best_move(6);
	uci_send_bestmove(move);
      } else if (strncmp(line, "position", 8) == 0) {
	  if (strstr(line, "startpos")) {
            parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	  }   
	  if (strstr(line, "moves")) {
            parseMoves(line);
          }	
      } else if (strncmp(line, "ucinewgame", 10) == 0) {
            parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
      } else if (strncmp(line, "quit", 4) == 0) {
            break;
      }
      
    }

}
void starting_pos() {
  parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  
}

void uci_send(char* content) {
  
  logMessage(content);
  printf("%s\n", content);

}

void uci_send_bestmove(int move) {

  printf("bestmove ");
  print_move_UCI(move);
  logMessage("bestmove \n");
  fflush(stdout);

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
           
	  char result[5];
	  for (int i = 0; i < temp.current_index; i++) {
	  
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
