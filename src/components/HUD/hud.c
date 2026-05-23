#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // USLEEP solo su linux
#include "hud.h"
#include "../adt/struct.h"

void intestation() {
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  printf(" # |----------------------------------------- ELENCO SEGNALAZIONI ---------------------------------------| # \n");
  printf(" # |        ID        |        CITTADINO       |     DATA     |     STATO     |     CATEORIA     |   U   | # \n");
  printf(" # |-----------------------------------------------------------------------------------------------------| # \n");
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
    
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  printf(" #   %-47s #   %-49s # \n", " STATISTICHE GENERALI", "CONTEGGIO SEGNALAZIONI PER CATEGORIA");
  printf(" #---------------------------------------------------#-----------------------------------------------------# \n");

  printf(" #   - Numero segnalazioni comune:  %7d          #  - Illuminazione:  %7d                          # \n", 
    getTotalSeg(sistema), getSegCountByCategory(sistema, 1));

  printf(" #   - Segnalazioni aperte:         %7d          #  - Rifiuti:        %7d                          # \n", 
    getTotalAperte(sistema), getSegCountByCategory(sistema, 2));

  printf(" #   - Segnalazioni in risoluzione: %7d          #  - Strade:         %7d                          # \n", 
    getTotalRis(sistema), getSegCountByCategory(sistema, 3));

  printf(" #   - Segnalazioni chiuse:         %7d          #  - Verde Pubblico: %7d                          # \n", 
    getTotalChiuse(sistema), getSegCountByCategory(sistema, 4));

  printf(" #   - Segnalazioni Urgenti (5):    %7d          #  - Incendio:       %7d                          # \n", 
    getMostUrgenti(sistema), getSegCountByCategory(sistema, 0));

  printf(" #---------------------------------------------------#  - Allagamento:    %7d                          # \n", 
    getSegCountByCategory(sistema, 5));

  printf(" #   - Creare una segnalazione:           1          #  - Segnaletica:    %7d                          # \n", 
    getSegCountByCategory(sistema, 6));

  printf(" #   - Rimuovere una segnalazione:        2          #  - Edilizia:       %7d                          # \n", 
    getSegCountByCategory(sistema, 7));

  printf(" #   - Mostra segnalazioni per data:      3          #  - Randagismo:     %7d                          # \n", 
    getSegCountByCategory(sistema, 8));

  printf(" #   - Mostra segnalazioni per id:        4          #  - Inquinamento:   %7d                          # \n", 
    getSegCountByCategory(sistema, 9));
  
  printf(" #   - Modifica stato segnalazione:       5          #  - Sicurezza:      %7d                          # \n", 
    getSegCountByCategory(sistema, 10));
  printf(" #   - Salvare ed uscire:                 0          #  -------------------------------------------------- # \n");

  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  printf(" #   - Scelta: ");
  fflush(stdout);
}

void getSeg(s node) {
  if (node == NULL) return;

  char *dataStr = getData(node);
  const char *statoStr;

  switch(getState(node)) {
    case 0: { 
      statoStr = "Aperta"; 
    } break;

    case 1: { 
      statoStr = "Risoluzione"; 
    } break;

    case 2: {
      statoStr = "Chiusa"; 
    } break;
    default: {
      statoStr = "N/D";
    } break;
  }

  printf(" # | %-16d | %-22.22s | %-12s | %-13s | %-16.16s |   %d   | # \n",
    getID(node),
    getName(node),
    dataStr ? dataStr : "N/D",
    statoStr,
    getCat(node),
    getUrg(node)
  );

  if (dataStr) {
    free(dataStr);
  }
}

void showSeg(Root root) {
  printf("\033[H\033[J");

  intestation();

  s currentSeg = getDataHead(root);
  int limit = 0;

  while(currentSeg != NULL && limit < 20) {
    getSeg(currentSeg); 
    currentSeg = nextForData(currentSeg);
    limit++;
  }

  printf(" # |-----------------------------------------------------------------------------------------------------| # \n");
  printf(" # |-------------- Mostrate le prime 20 segnalazioni in ordine cronologico. Premi INVIO: ----------------| # \n");
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  
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

void modifySegHud(Root root) {
  if (root == NULL) return;

  int32_t idTarget;
  printf(" # --------- MODIFICA SEGNALAZIONE --------- # \n");
  printf(" #   Inserisci ID segnalazione: ");

  if (scanf("%d", &idTarget) != 1) {
    while(getchar() != '\n');
    printf(" # Errore: ID non valido.\n");
    return;
  }
  getchar();

  printf(" # Seleziona il nuovo stato:\n");
  printf(" # 0) Aperta\n");
  printf(" # 1) In Risoluzione\n");
  printf(" # 2) Chiusa\n");
  printf(" # Scelta: ");

  int nuovoStato;
  if (scanf("%d", &nuovoStato) != 1 || nuovoStato < 0 || nuovoStato > 2) {
    while(getchar() != '\n');
    printf(" # Errore: Stato non valido.\n");
    return;
  }
  while(getchar() != '\n');

  modifySeg(root, idTarget, nuovoStato);

  printf(" # Stato modificato in RAM! Sarà permanente all'uscita (0).\n");
  printf(" # Premi INVIO per continuare...");
  getchar();
}