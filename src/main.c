#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "./components/HUD/hud.h"
#include "./components/adt/struct.h"

int main(void) {
  
  Root sistema = init_root();
  if (sistema == NULL) return 1;

  clock_t start = clock();
  printf("Caricamento di 5000 segnalazioni in corso... \n");
  init_loadingDb(sistema, "./components/database/database.bin");
  clock_t end = clock();
  double time = ( (double) (end - start) / CLOCKS_PER_SEC);

  printf("Risultato caricamento ");
  printf("Tempo impiegato %.8f secondi \n", time);
  printf("\nPremi INVIO per entrare nel portale...");
  getchar();

  while(1) {
    
    printf("\033[H\033[J");
    
    //       ##### Interfaccia grafica - Portale consulente  #####
    printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
    printf(" #   - Numero segnalazioni comune:  %7d          # \n", 20);
    printf(" #   - Seagnalazioni aperte:        %7d          # \n", 7);
    printf(" #   - Segnalazioni in risoluzione: %7d          # \n", 3);
    printf(" #   - Segnalazioni in revisione:   %7d          # \n", 2);
    printf(" #   - Segnalazioni chiuse:         %7d          # \n", 8);
    printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
    printf(" #   - Creare una segnalazione:           1          # \n");
    printf(" #   - Rimuovere una segnalazione:        2          # \n");
    printf(" #   - Segnalazioni urgenti:              0          # \n");
    printf(" #   - Salvare ed uscire:                 0          # \n");
    printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  
    int choise;
    scanf("%d", &choise);
    
    switch(choise) {
      case 1: {
        
      } break;
      case 2: {
        
      } break;
      case 3: {
        
      } break;
      case 4: {
        
      } break;
      case 5: {
        
      } break;
      case 6: {
        
      } break;
      case 0: {
        salvataggio();
      } break;
    }  
  }  
}
