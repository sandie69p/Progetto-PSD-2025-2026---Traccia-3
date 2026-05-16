#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // USLEEP solo su linux
#include "hud.h"
#include "../adt/struct.h"

void intestation() {
  printf(" # |---------------------- ELENCO SEGNALAZIONI -------------------------| # \n");
  printf(" # |    ID    |      CITTADINO      |    DATA    |     STATO     | U    | # \n");
  printf(" # |--------------------------------------------------------------------| # \n");
}

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
  save_records(sistema);
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
  printf(" #   - Mostra segnalazioni per id:        4          # \n");
  printf(" #   - Salvare ed uscire:                 0          # \n");
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
}

void getSeg(s node) {
  if (node == NULL) return;

  char *dataStr = getData(node);
  const char *statoStr;

  switch(getState(node)) {
    case 0: { statoStr = "Aperta"; 
    } break;

    case 1: { 
      statoStr = "Risoluzione"; 
    } break;

    case 2: {
      statoStr = "Chiusa"; 
    } break;
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

  intestation();

  s currentSeg = getDataHead(root);

  int limit = 0;

  while(currentSeg != NULL && limit < 20/* per dimostrazione*/ ) {
    getSeg(currentSeg);
    currentSeg = nextForData(currentSeg);
    limit++;
  }

  printf(" # |--------------------------------------------------------------------| # \n");
  printf(" # |        - Mostrate le prime 20 segnalazioni. Premi INVIO...         | # \n");
  getchar(); getchar();
}

void insertNewSeg(Root root) {
  getNewSeg(root);
}

void removeSeg(Root root) {
  printf(" #   - Rimuovi la segnalazione usando l'id: ");
  
  int idTarget;
  scanf("%d", &idTarget);
  getchar();

  if (idTarget > 2199999) {
    printf(" #   - Segnalazione non valida, ritorno alla dashboard!");
    return;
  } else {
    printf(" #   - Rimozione segnalazione effettuata! \n");
    init_removeSeg(root, idTarget);
  }
}

void init_search_seg(Root root) {
  char query[8] = "";
  int index = 0;
  int ch;

  system("stty -icanon -echo");
  printf("\033[H\033[J");

  while(1) {
    printf("\033[H");
        
    printf(" ##### ##### ##### PORTALE COMUNALE - RICERCA LIVE ##### ##### ##### \n");
    printf(" # Digita l'ID (Premi ESC per uscire o BACKSPACE per cancellare)     \n");
    printf(" # \n");
    printf(" # BARRA DI RICERCA: [ %-7s ] \033[K\n", query);

    intestation();

    if (index > 0) {
      search_seg(root, query);
    } else {
      for(int i = 0; i < 12; i++) {
        if (i == 5) printf(" # |              Inizia a digitare le cifre dell'ID...            | # \n");
        else        printf(" # |                                                               | # \n");
      }
    }
    printf(" # ----------------------------------------------------------------- # \n");
    fflush(stdout);

    ch = getchar();

    if (ch == 27) {
      break;
    } 
    else if (ch == 127 || ch == 8) {
      if (index > 0) {
        query[--index] = '\0';
      }
    } 
    else if (ch >= '0' && ch <= '9' && index < 7) {
      query[index++] = ch;
      query[index] = '\0';
    }
  }

  system("stty cooked echo");
  printf("\033[H\033[J");
}