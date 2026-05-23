/**
 *  #############################
 *  # INCLUSIONE DELLE LIBRERIE #
 *  #############################
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "./components/HUD/hud.h"
#include "./components/adt/struct.h"


/**
 * @note fuori dal main perche' variabile globale
*/
Root sistema =  NULL;

/**
 * @brief Inizializza l'intero sistema di gestione delle segnalazioni.
 *
 * @pre Il database binario deve esistere ed essere accessibile.
 *
 * @post
 * - Alloca e inizializza la Root principale del sistema.
 * - Carica tutte le segnalazioni dal database binario.
 * - Ricostruisce gli indici ordinati del grafo multi-lista.
 * - Mostra il tempo totale di caricamento del sistema.
 *
 * @note
 * Questa funzione rappresenta il punto di bootstrap del portale comunale
 * ed esegue l'inizializzazione completa dell'infrastruttura dati.
 */
void init();

int main(void) {
  init();

  while(1) {

    dashboard(sistema);
  
    int choise;
    if (scanf("%d", &choise) != 1) {
        while(getchar() != '\n');
        continue;
    }

    switch(choise) {
      case 1: insertNewSeg(sistema); break;
      case 2: removeSeg(sistema); break;
      case 3: showSeg(sistema); break;
      case 4: init_search_seg(sistema); break;
      case 5: modifySegHud(sistema); break;
      case 0: salvataggio(sistema); break;
    }  
  }  
}

void init() {
  sistema = init_root();
  if (sistema == NULL) return;

  clock_t start = clock();
  printf(" #   - Caricamento delle segnalazioni in corso... \n");
  init_loadingDb(sistema, "./components/database/database.bin");
  init_sorting(sistema);
  clock_t end = clock();
  double time = ( (double) (end - start) / CLOCKS_PER_SEC);

  printf(" #   - Risultato caricamento \n");
  printf(" #   - Tempo impiegato %.8f secondi per\n", time);
  printf(" #   - Premi INVIO per entrare nel portale...");
  getchar();
}