#ifndef UCI_H
#define UCI_H

void uci(int depth);
void uci_send(char* content);
void parseMoves(char* input);
void openLog();
void closeLog();
void logMessage(const char* message);
void uci_send_bestmove(int move);

#endif
