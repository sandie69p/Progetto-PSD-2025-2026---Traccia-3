#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // USLEEP solo su linux
#include "hud.h"
#include "../adt/struct.h"

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

void salvataggio(Root sistema) {
  deleteGraph(sistema);
  animazione_salvataggio();
  usleep(1000000);
  exit(0);
}

void dashboard(Root sistema) {
  printf("\033[H\033[J");
    
  //       ##### Interfaccia grafica - Portale consulente  #####
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  printf(" #   - Numero segnalazioni comune:  %7d          # \n", getTotalSeg(sistema));
  printf(" #   - Seagnalazioni aperte:        %7d          # \n", getTotalAperte(sistema));
  printf(" #   - Segnalazioni in risoluzione: %7d          # \n", getTotalRis(sistema));
  printf(" #   - Segnalazioni chiuse:         %7d          # \n", getTotalChiuse(sistema));
  printf(" #   - Segnalazioni Urgenti (5)     %7d          # \n", getMostUrgenti(sistema));
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  printf(" #   - Creare una segnalazione:           1          # \n");
  printf(" #   - Rimuovere una segnalazione:        2          # \n");
  printf(" #   - Mostra segnalazioni per data:      3          # \n");
  printf(" #   - Salvare ed uscire:                 0          # \n");
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
}

void getSeg(s node) {
  if (node == NULL) return;

  char *dataStr = getData(node);
  const char *statoStr;

  switch(getState(node)) {
    case 0: statoStr = "Aperta"; break;
    case 1: statoStr = "Risoluzione"; break;
    case 2: statoStr = "Chiusa"; break;
  }

  printf(" # | %-8d | %-15.15s     | %-10s | %-11s   | P: %d | # \n",
    getID(node),
    getName(node),
    dataStr,
    statoStr,
    getUrg(node)
  );

  free(dataStr);
}

void showSeg(Root root) {
  printf("\033[H\033[J");

  printf(" # |---------------------- ELENCO SEGNALAZIONI -------------------------| # \n");
  printf(" # |    ID    |      CITTADINO      |    DATA    |     STATO     | U    | # \n");
  printf(" # |--------------------------------------------------------------------| # \n");

  s currentSeg = getDataHead(root);

  int limit = 0;

  while(currentSeg != NULL && limit < 20/* per dimostrazione*/ ) {
    getSeg(currentSeg);
    currentSeg = nextForData(currentSeg);
    limit++;
  }

  printf(" # |--------------------------------------------------------------------| # \n");
  printf(" # Mostrate le prime 20 segnalazioni. Premi INVIO... # \n");
  getchar(); getchar();
}

void insertNewSeg(Root root) {
  getNewSeg(root);
}