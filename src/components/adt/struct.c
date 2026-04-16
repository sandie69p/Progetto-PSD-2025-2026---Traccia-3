#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "struct.h"

typedef enum {
  APERTO, 
  RISOLUZIONE, 
  CHIUSO
}

typedef struct segnalazione {
  int id;
  char nome_cittadino[64];
  char categoria[64];
  char descrizione[1024];
  int data;
  int urgenza;
  stato stato_segnalazione;
  struct segnalazione *next;
}
