#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // USLEEP solo su linux
#include "hud.h"
#include "../components/adt/struct.h"

void animazione_salvataggio() {
  printf(" #-----Salvataggio in corso...-----------------------# \n");

  int lunghezza = 50;
  int i = 0;
  
  for(; i <= lunghezza; i++) {
    printf("\r [");
    
    for(int j = 0; j <= i; j++) {
      printf("#");
    }
    
    for(int j = 0; j < (lunghezza - i); j++) {
      printf("-");
    }

    printf("] %6.2f%%", ((float) i * 100) / (float) lunghezza);

    fflush(stdout);
    usleep(50000);
  }

  printf("\n #-----Uscendo ...-----------------------------------# \n");
}

void salvataggio() {
  // funzione da implementare free()...
  animazione_salvataggio();
  usleep(1000000);
  exit(1);
}
