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
  printf("Caricamento di 25000 segnalazioni in corso... \n");
  init_loadingDb(sistema, "./components/database/database.bin");
  init_sorting(sistema);
  clock_t end = clock();
  double time = ( (double) (end - start) / CLOCKS_PER_SEC);

  printf(" #     Risultato caricamento ");
  printf(" #     Tempo impiegato %.8f secondi \n", time);
  printf(" #     Premi INVIO per entrare nel portale...");
  getchar();

  while(1) {
    dashboard(sistema);
  
    int choise;
    scanf("%d", &choise);
    
    switch(choise) {
      case 1: {
        
      } break;
      case 2: {
        
      } break;
      case 3: {
        showSeg(sistema);
      } break;
      case 4: {
        
      } break;
      case 5: {
        
      } break;
      case 6: {
        
      } break;
      case 0: {
        salvataggio(sistema);
      } break;
    }  
  }  
}
