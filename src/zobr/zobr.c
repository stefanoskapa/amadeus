#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "zobr.h"
#include "../../lib/spark.h"


BB z_pieces[12][64];
BB z_castling[16];
BB z_ep[65];
BB z_side[2];

BB get_zobrist() {

  BB hash = 0ULL;

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
      BB num = rand_64();
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

BB rand_64(void) {
  BB r = 0;
  for (int i=0; i<64; i++) {
    r = r*2 + rand()%2;
  }
  return r;
}
