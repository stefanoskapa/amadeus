#include <stdio.h>
#include <stdlib.h>
#include "../lib/spark.h"
#include "zobr/zobr.h"
#include "uci/uci.h"

int main(void) {
  init_attack_tables();
  init_zobrist();
  uci(6);
}


