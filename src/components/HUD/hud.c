#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // USLEEP solo su linux
#include "hud.h"
#include "../adt/struct.h"

static void intestation(void) {
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  printf(" #  |----------------------------------------- ELENCO SEGNALAZIONI ---------------------------------------| # \n");
  printf(" #  |       ID         |        CITTADINO       |     DATA     |     STATO     |     CATEGORIA    |   U   | # \n");
  printf(" #  |-----------------------------------------------------------------------------------------------------| # \n");
}

static void getReport(Root sistema) {
  if (sistema == NULL) return;

  FILE *fReport = fopen("./report_comune.txt", "w");
  if (fReport == NULL) {
    printf(" # [ERRORE RIGIDO] Impossibile creare il file del report su disco.\n");
    return;
  }

  fprintf(fReport, "=========================================================================\n");
  fprintf(fReport, "          PORTALE URBANISTICO COMUNALE - REPORT STATISTICO GLOBALE       \n");
  fprintf(fReport, "=========================================================================\n\n");
  fprintf(fReport, " * Volume Totale Segnalazioni Attive: %d\n\n", getTotalSeg(sistema));
  
  fprintf(fReport, " * SPACCATO DEGLI STATI OPERATIVI:\n");
  fprintf(fReport, "   - Segnalazioni in stato Aperto: %d\n", getTotalAperte(sistema));
  fprintf(fReport, "   - Segnalazioni in Risoluzione: %d\n", getTotalRis(sistema));
  fprintf(fReport, "   - Segnalazioni in stato Chiuso: %d\n\n", getTotalChiuse(sistema));
  
  fprintf(fReport, " * INDICE DI CRITICITÀ:\n");
  fprintf(fReport, "   - Segnalazioni Massima Urgenza (5): %d\n\n", getMostUrgenti(sistema));
  
  fprintf(fReport, " * DISTRIBUZIONE DETTAGLIATA PER CATEGORIA COMUNALE:\n");
  
  for(int i = 0; i < 11; i++) {
     const char* nCat = (i == 0) ? "Incendio" : 
        (i == 1) ? "Illuminazione" : 
        (i == 2) ? "Rifiuti" : 
        (i == 3) ? "Strade" : 
        (i == 4) ? "Verde Pubblico" : 
        (i == 5) ? "Allagamento" : 
        (i == 6) ? "Segnaletica" : 
        (i == 7) ? "Edilizia" : 
        (i == 8) ? "Randagismo" : 
        (i == 9) ? "Inquinamento" : "Sicurezza";
                        
     fprintf(fReport, "   - %-25s : %d\n", nCat, getSegCountByCategory(sistema, i));
  }
  
  fprintf(fReport, "\n * AREA COMUNALE PIÙ COLPITA DA DISSERVIZI: %s\n", getMaxCatName(sistema));
  fprintf(fReport, "\n=========================================================================\n");
  
  fclose(fReport);
}

void salvataggio(Root sistema) {
  if (sistema == NULL) {
      printf(" # [AVVISO] Nessun sistema attivo da salvare.\n");
      exit(0);
  }

  printf(" #-----Salvataggio in corso...-----------------------# \n");

  save_records(sistema);

  getReport(sistema);

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
    usleep(20000);
  }

  printf("\n # [INFO] Liberazione della memoria dinamica (Svuotamento Heap RAM)...\n");
  deleteGraph(sistema);
  printf(" # [ OK ] Grafo multi-indice deallocato con successo (0 leak).\n");

  printf(" # [INFO] Chiusura del processo in corso...\n");
  usleep(500000);

  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  exit(0);
}

void dashboard(Root sistema) {
    if (sistema == NULL) {
        printf("\n [ERRORE CRITICO] Impossibile mostrare la dashboard: sistema non inizializzato o RAM corrotta.\n");
        return;
    }

    printf("\033[H\033[J");
    
    printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
    printf(" #   %-47s #    %-48s # \n", " STATISTICHE GENERALI", "CONTEGGIO SEGNALAZIONI PER CATEGORIA");
    printf(" #---------------------------------------------------#-----------------------------------------------------# \n");

    char col_sinistra[128];
    char col_destra[128];

    sprintf(col_sinistra, " - Numero segnalazioni comune:  %7d", getTotalSeg(sistema));
    sprintf(col_destra, " - Illuminazione:      %7d", getSegCountByCategory(sistema, 1));
    printf(" #   %-47s #   %-49s # \n", col_sinistra, col_destra);

    sprintf(col_sinistra, " - Segnalazioni aperte:         %7d", getTotalAperte(sistema));
    sprintf(col_destra, " - Rifiuti:            %7d", getSegCountByCategory(sistema, 2));
    printf(" #   %-47s #   %-49s # \n", col_sinistra, col_destra);

    sprintf(col_sinistra, " - Segnalazioni in risoluzione: %7d", getTotalRis(sistema));
    sprintf(col_destra, " - Strade:             %7d", getSegCountByCategory(sistema, 3));
    printf(" #   %-47s #   %-49s # \n", col_sinistra, col_destra);

    sprintf(col_sinistra, " - Segnalazioni chiuse:         %7d", getTotalChiuse(sistema));
    sprintf(col_destra, " - Verde Pubblico:     %7d", getSegCountByCategory(sistema, 4));
    printf(" #   %-47s #   %-49s # \n", col_sinistra, col_destra);

    sprintf(col_sinistra, " - Segnalazioni Urgenti (5):    %7d", getMostUrgenti(sistema));
    sprintf(col_destra, " - Incendio:           %7d", getSegCountByCategory(sistema, 0));
    printf(" #   %-47s #   %-49s # \n", col_sinistra, col_destra);

    sprintf(col_destra, " - Allagamento:        %7d", getSegCountByCategory(sistema, 5));
    printf(" #---------------------------------------------------#   %-49s # \n", col_destra);

    sprintf(col_destra, " - Segnaletica:        %7d", getSegCountByCategory(sistema, 6));
    printf(" #   %-47s #   %-49s # \n", " - Creare una segnalazione:           1", col_destra);

    sprintf(col_destra, " - Edilizia:           %7d", getSegCountByCategory(sistema, 7));
    printf(" #   %-47s #   %-49s # \n", " - Rimuovere una segnalazione:        2", col_destra);

    sprintf(col_destra, " - Randagismo:         %7d", getSegCountByCategory(sistema, 8));
    printf(" #   %-47s #   %-49s # \n", " - Mostra segnalazioni per data:      3", col_destra);

    sprintf(col_destra, " - Inquinamento:       %7d", getSegCountByCategory(sistema, 9));
    printf(" #   %-47s #   %-49s # \n", " - Mostra segnalazioni per id:        4", col_destra);

    sprintf(col_destra, " - Sicurezza:          %7d", getSegCountByCategory(sistema, 10));
    printf(" #   %-47s #   %-49s # \n", " - Modifica stato segnalazione:       5", col_destra);

    const char *topCategoria = getMaxCatName(sistema);

    sprintf(col_destra, " - Cat. piu' segnalata: %-15.15s", topCategoria ? topCategoria : "N/D");
    printf(" #   %-47s #   %-49s # \n", " - Salvare ed uscire:                 0", col_destra);

    printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
    fflush(stdout);
}

static void getSeg(s node) {
  if (node == NULL) return;

  char *dataStr = getData(node);
  const char *nameStr = getName(node);
  const char *catStr = getCat(node);
  const char *statoStr;

  switch(getState(node)) {
    case 0:  statoStr = "Aperta";      break;
    case 1:  statoStr = "Risoluzione"; break;
    case 2:  statoStr = "Chiusa";      break;
    default: statoStr = "N/D";         break;
  }

  char riga_contenuto[256];
  
  sprintf(riga_contenuto, " | %-16d | %-22.22s | %-12.12s | %-13.13s | %-16.16s |   %d   |",
    getID(node),
    nameStr ? nameStr : "N/D",
    dataStr ? dataStr : "N/D",
    statoStr,
    catStr ? catStr : "N/D",
    getUrg(node)
  );

  printf(" # %-101s # \n", riga_contenuto);

  if (dataStr) {
    free(dataStr);
  }
}

void showSeg(Root root) {
  if (root == NULL) return;

  printf("\033[H\033[J");

  intestation();

  s currentSeg = getDataHead(root);
  int limit = 0;

  while(currentSeg != NULL && limit < 20) {
    getSeg(currentSeg); 
    currentSeg = nextForData(currentSeg);
    limit++;
  }

  int c;
  while ((c = getchar()) != '\n' && c != EOF);

  char riga_footer_linea[256];
  char riga_footer_testo[256];
  
  sprintf(riga_footer_linea, " |-----------------------------------------------------------------------------------------------------|");
  sprintf(riga_footer_testo, " |-------------- Mostrate le prime 20 segnalazioni in ordine cronologico. Premi INVIO: ----------------|");
  
  printf(" # %-101s # \n", riga_footer_linea);
  printf(" # %-101s # \n", riga_footer_testo);
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  
  getchar();
}

void insertNewSeg(Root root) {
  if (root == NULL) return;

  printf("\033[H\033[J");
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  printf(" # |------------------------------------- ACQUISIZIONE NUOVA SEGNALAZIONE ------------------------------| # \n");
  printf(" #                                                                                                       # \n");
  
  int c;
  while ((c = getchar()) != '\n' && c != EOF);

  getNewSeg(root);
  
  printf(" #                                                                                                       # \n");
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
}

void removeSeg(Root root) {
  if (root == NULL) return;

  printf("\033[H\033[J");
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  printf(" # |------------------------------------- RIMOZIONE SEGNALAZIONE COMUNALE ------------------------------| # \n");
  printf(" #                                                                                                       # \n");
  
  char riga_prompt[128];
  sprintf(riga_prompt, " Inserisci il codice numerico identificativo (ID) da eliminare permanentemente: ");
  printf(" # %-101s # \n", riga_prompt);
  printf(" #                                                                                                       # \n");
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  printf(" # Scelta ID per rimozione: ");
  fflush(stdout);

  int idTarget;
  if (scanf("%d", &idTarget) != 1) {
    printf("\n # [ERRORE RIGIDO] L'ID inserito deve contenere unicamente cifre numeriche!                            # \n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    printf(" # Premi INVIO per annullare e ritornare alla dashboard...                                               # \n");
    printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
    getchar();
    return;
  }

  int residuo = getchar();
  if (residuo != '\n' && residuo != EOF) {
    printf("\n # [ERRORE RIGIDO] Input corrotto! Rilevati caratteri alfabetici illegali accodati all'ID.               # \n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    printf(" # Premi INVIO per annullare e ritornare alla dashboard...                                               # \n");
    printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
    getchar();
    return;
  }

  if (idTarget < 0 || idTarget > 2199999) {
    printf("\n # [ERRORE RIGIDO] ID inserito fuori range o non conforme ai parametri del comune!                     # \n");
    printf(" # Premi INVIO per annullare e ritornare alla dashboard...                                               # \n");
    printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
    getchar();
    return;
  }

  int totale_prima = getTotalSeg(root);
  init_removeSeg(root, idTarget);
  int totale_dopo = getTotalSeg(root);

  printf("\033[H\033[J");
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  
  if (totale_dopo < totale_prima) {
    printf(" # |--------------------------------- OPERAZIONE COMPLETATA CON SUCCESSO -----------------------------| # \n");
    printf(" #                                                                                                       # \n");
    char riga_successo[128];
    sprintf(riga_successo, " [ OK ] Segnalazione ID %d estirpata correttamente da tutti i flussi ortogonali in RAM.", idTarget);
    printf(" # %-101s # \n", riga_successo);
  } else {
    printf(" # |----------------------------------------- OPERAZIONE FALLITA ---------------------------------------| # \n");
    printf(" #                                                                                                       # \n");
    char riga_fallimento[128];
    sprintf(riga_fallimento, " [AVVISO] Nessuna eliminazione effettuata. L'ID %d non e' presente nel database locale.", idTarget);
    printf(" # %-101s # \n", riga_fallimento);
  }

  printf(" #                                                                                                       # \n");
  printf(" # Premi INVIO per confermare e tornare alla dashboard principale...                                     # \n");
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  getchar();
}

void init_search_seg(Root root) {
  char query[8] = "";
  int index = 0;
  int ch;

  system("stty -icanon -echo");
  printf("\033[H\033[J");

  while(1) {
    printf("\033[H");
        
    printf(" ##### ##### ##### PORTALE COMUNALE - RICERCA ID # ##### ##### ##### \n");
    printf(" # Digita l'ID (Premi ESC per uscire o BACKSPACE per cancellare)     \n");
    printf(" # \n");
    printf(" # BARRA DI RICERCA: [ %-7s ] \033[K\n", query);

    intestation();

    if (index > 0) {
      search_seg(root, query);
    } else {
      for(int i = 0; i < 12; i++) {
        if (i == 5) printf(" #  |                               Inizia a digitare le cifre dell'ID...                                 | # \n");
        else        printf(" #  |                                                                                                     | # \n");
      }
    }
    printf(" #  |-----------------------------------------------------------------------------------------------------| # \n");
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

  printf("\033[H\033[J");
  
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  printf(" # |------------------------------------- MODIFICA STATO SEGNALAZIONE -----------------------------------| # \n");
  printf(" # |-----------------------------------------------------------------------------------------------------| # \n");

  int32_t idTarget;
  char riga_prompt[128];
  sprintf(riga_prompt, "|---- Inserisci ID della segnalazione comunale: ------------------------------------------------------|");
  printf(" # %-101s # \n", riga_prompt);
  printf(" # |-----------------------------------------------------------------------------------------------------| # \n");
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  printf(" # Scelta ID: ");
  fflush(stdout);

  if (scanf("%d", &idTarget) != 1) {
    printf("\n # [ERRORE RIGIDO] L'ID deve essere composto esclusivamente da cifre numeriche!                          # \n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    printf(" # Premi INVIO per annullare l'operazione e tornare alla dashboard...                                    # \n");
    printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
    getchar();
    return;
  }

  int residuo_id = getchar();
  if (residuo_id != '\n' && residuo_id != EOF) {
    printf("\n # [ERRORE RIGIDO] Input non conforme! Rilevato testo alfabetico illegale accodato all'ID.               # \n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    printf(" # Premi INVIO per annullare l'operazione e tornare alla dashboard...                                    # \n");
    printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
    getchar();
    return;
  }

  printf("\033[H\033[J");
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  printf(" # |------------------------------------- SELEZIONE NUOVO STATO OPERATIVO ------------------------------| # \n");
  
  char riga_target[128];
  sprintf(riga_target, " Modifica in corso per la segnalazione ID: %d", idTarget);
  printf(" # %-101s # \n", riga_target);
  printf(" #-------------------------------------------------------------------------------------------------------# \n");
  printf(" #  0) APERTA                                                                                            # \n");
  printf(" #  1) IN RISOLUZIONE                                                                                    # \n");
  printf(" #  2) CHIUSA                                                                                            # \n");
  printf(" #                                                                                                       # \n");
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  printf(" # Inserisci codice stato (0-2): ");
  fflush(stdout);

  int nuovoStato;
  if (scanf("%d", &nuovoStato) != 1 || nuovoStato < 0 || nuovoStato > 2) {
    printf("\n # [ERRORE RIGIDO] Stato non valido! Scegliere unicamente una delle opzioni censite (0, 1, 2).            # \n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    printf(" # Premi INVIO per annullare l'operazione e tornare alla dashboard...                                    # \n");
    printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
    getchar();
    return;
  }

  int residuo_stato = getchar();
  if (residuo_stato != '\n' && residuo_stato != EOF) {
    printf("\n # [ERRORE RIGIDO] Scelta corrotta da caratteri alfabetici non consentiti!                               # \n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    printf(" # Premi INVIO per annullare l'operazione e tornare alla dashboard...                                    # \n");
    printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
    getchar();
    return;
  }

  modifySeg(root, idTarget, nuovoStato);

  printf("\033[H\033[J");
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  printf(" # |--------------------------------- AGGIORNAMENTO COMPLETATO CON SUCCESSO -----------------------------| # \n");
  printf(" #                                                                                                       # \n");
  
  char riga_successo[128];
  sprintf(riga_successo, " [ OK ] Stato della segnalazione %d modificato correttamente in RAM locale.", idTarget);
  printf(" # %-101s # \n", riga_successo);
  
  printf(" #   I cambiamenti saranno resi persistenti sul file database binario solo alla chiusura (0).            # \n");
  printf(" #                                                                                                       # \n");
  printf(" # Premi INVIO per confermare e tornare alla dashboard principale...                                     # \n");
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  getchar();
}

