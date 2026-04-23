#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "./struct.h"

struct segnalazione {
  int id;                     // SSNNNN
  char nome_cittadino[64];
  char categoria[64];
  char descrizione[1024];
  int data;                   // YYYYMMDD
  int urgenza;
  int stato;
  s nextId;
  s nextData;
  s nextStato;
  s nextUrg;
};

typedef struct apertoNode {
  s head;
} apertoNode;

typedef struct risNode {
  s head;
} risNode;

typedef struct chiusoNode {
  s head;
} chiusoNode;

typedef enum {
  incendio = 0,
  illuminazione,
  rifiuti,
  strade,
  verde,
  allagamento,
  segnaletica,
  edilizia,
  randagismo,
  inquinamento,
  sicurezza,
  allCat
} catId;

typedef struct idNode {
  s cat[allCat];
} idNode;

typedef struct dataNode {
  s head;
} dataNode;

typedef struct urgNode {
  s priority[5];
} urgNode;

typedef struct statoNode {
  apertoNode *aperto;
  risNode *risoluzione;
  chiusoNode *chiuso;
} statoNode;

struct root {
  idNode *id;
  dataNode *data;
  statoNode *stato;
  urgNode *urgenza;
  int totSegnalazioni;
};

Root init_root() {
  Root r = (Root) malloc(sizeof(struct root));
  if(r == NULL) return NULL;
  
  // Inizializzo i nodi di controllo
  r->id = (idNode *) malloc(sizeof(idNode));
  r->data = (dataNode *) malloc(sizeof(dataNode));
  r->urgenza = (urgNode *) malloc(sizeof(urgNode));
  r->stato = (statoNode *) malloc(sizeof(statoNode));
  
  // Inizializzo i nodi di controllo di secondo livello
  r->stato->aperto = (apertoNode *) malloc(sizeof(apertoNode));
  r->stato->risoluzione = (risNode *) malloc(sizeof(risNode));
  r->stato->chiuso = (chiusoNode *) malloc(sizeof(chiusoNode));
  
  for (int i = 0; i < allCat; i++) r->id->cat[i] = NULL;
  r->data->head = NULL;
  for (int i = 0; i < 5; i ++) r->urgenza->priority[i] = NULL;
  r->stato->aperto->head = NULL;
  r->stato->risoluzione->head = NULL;
  r->stato->chiuso->head = NULL;
  
  r->totSegnalazioni = 0;

  return r;
}

void insertOnGraph(Root r, s newSeg) {
  int prefisso = newSeg->id / 10000;
  int catIdx;

  switch (prefisso) {
    case 10: catIdx = illuminazione; break;
    case 20: catIdx = rifiuti; break;
    case 30: catIdx = strade; break;
    case 40: catIdx = verde; break;
    case 50: catIdx = incendio; break;
    case 60: catIdx = allagamento; break;
    case 70: catIdx = segnaletica; break;
    case 80: catIdx = edilizia; break;
    case 90: catIdx = randagismo; break;
    case 11: catIdx = inquinamento; break;
    case 21: catIdx = sicurezza; break;
  }

  if (catIdx != -1) {
    newSeg->nextId = r->id->cat[catIdx];
    r->id->cat[catIdx] = newSeg;

    newSeg->nextData = r->data->head;
    r->data->head = newSeg;

    if (newSeg->stato == 0) {
      newSeg->nextStato = r->stato->aperto->head;
      r->stato->aperto->head = newSeg;
    } else if (newSeg->stato == 1) {
      newSeg->nextStato = r->stato->risoluzione->head;
      r->stato->risoluzione->head = newSeg;
    } else if (newSeg->stato == 2) {
      newSeg->nextStato = r->stato->chiuso->head;
      r->stato->chiuso->head = newSeg;
    }

    int urgIdx = newSeg->urgenza - 1;
    if (urgIdx >= 0 && urgIdx < 5) {
      newSeg->nextUrg = r->urgenza->priority[urgIdx];
      r->urgenza->priority[urgIdx] = newSeg;
    }
    
    r->totSegnalazioni++;
  }
}

void init_loadingDb(Root r, const char *fileName) {
  FILE *f = fopen(fileName, "rb");
  if (f == NULL) { perror("Errore apertura file"); return; }


  while (1) {
    s nuova = (s) malloc(sizeof(struct segnalazione));

    size_t read = fread(&nuova->id, sizeof(int), 1, f);
    if (read < 1) { free(nuova); break; }

    fread(nuova->nome_cittadino, sizeof(char), 64, f);
    fread(nuova->categoria, sizeof(char), 64, f);
    fread(nuova->descrizione, sizeof(char), 1024, f);
    fread(&nuova->data, sizeof(int), 1, f);
    fread(&nuova->urgenza, sizeof(int), 1, f);
    fread(&nuova->stato, sizeof(int), 1, f);

    nuova->nextId = NULL;
    nuova->nextData = NULL;
    nuova->nextStato = NULL;
    nuova->nextUrg = NULL;

    insertOnGraph(r, nuova);
  }
}

void quicksort(s *arr, int low, int high, int type) {
  if (low < high) {
    s pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
      int condition = 0;

      switch (type) {
        case 1: condition = (arr[j]->data < pivot->data); break;
        case 2: condition = (arr[j]->id < pivot->id); break;
        case 3: condition = (arr[j]->urgenza < pivot->urgenza); break;
        case 4: condition = (arr[j]->stato < pivot->stato); break;
      }

      if (condition) {
        i++;
        s temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
      }
    }

    s temp = arr[i+1];
    arr[i+1] = arr[high];
    arr[high] = temp;

    int pi = i + 1;
    quicksort(arr, low, pi - 1, type);
    quicksort(arr, pi + 1, high, type);
  }
}

void init_sorting(Root r) {
  int n = r->totSegnalazioni;

  s *dataSeg = malloc(n * sizeof(s));

  s curr = r->data->head;
  for (int i = 0; i < n; i++) {
    dataSeg[i] = curr;
    curr = curr->nextData;
  }

  quicksort(dataSeg, 0, n - 1, 1);

  r->data->head = dataSeg[0];
  for (int i = 0; i < n - 1; i++) {
    dataSeg[i]->nextData = dataSeg[i+1];
  } dataSeg[n-1]->nextData = NULL;

  // Inizio ordinamento id per categoria
  quicksort(dataSeg, 0, n - 1, 2);

  for(int i = 0; i < allCat; i++) {
    r->id->cat[i] = NULL;
  }

  

  // Inserimento a ritroso perche' e' l'inserimento in LIFO;
  for(int i = n - 1; i >= 0; i--) {
    s nodo = dataSeg[i];

    int prefix = (int) nodo->id / 10000;
    int catIdx = -1;

    switch (prefix) {
      case 10: catIdx = illuminazione; break;
      case 20: catIdx = rifiuti; break;
      case 30: catIdx = strade; break;
      case 40: catIdx = verde; break;
      case 50: catIdx = incendio; break;
      case 60: catIdx = allagamento; break;
      case 70: catIdx = segnaletica; break;
      case 80: catIdx = edilizia; break;
      case 90: catIdx = randagismo; break;
      case 11: catIdx = inquinamento; break;
      case 21: catIdx = sicurezza; break;
    }

    if(catIdx != -1) {
      nodo->nextId = r->id->cat[catIdx];
      r->id->cat[catIdx] = nodo;
    }
  }

  // Inizio ordinamento urgenza
  quicksort(dataSeg, 0, n - 1, 3);
  
  for (int i = 0; i < 5; i++) {
    r->urgenza->priority[i] = NULL;
  }

  for (int i = n - 1; i >= 0; i--) {
    s nodo = dataSeg[i];
    int urgIdx = nodo->urgenza - 1;

    if (urgIdx >= 0 && urgIdx < 5) {
      nodo->nextUrg = r->urgenza->priority[urgIdx];
      r->urgenza->priority[urgIdx] = nodo;
    }
  }

  // Inizio ordinamento stati
  quicksort(dataSeg, 0, n - 1, 4);

  r->stato->aperto->head = NULL;
  r->stato->chiuso->head = NULL;
  r->stato->risoluzione->head = NULL;

  for (int i = n - 1; i >= 0; i--) {
    s nodo = dataSeg[i];

    switch (nodo->stato) {
      case 0: {
        nodo->nextStato = r->stato->aperto->head;
        r->stato->aperto->head = nodo;
      } break;
      
      case 1: {
        nodo->nextStato = r->stato->risoluzione->head;
        r->stato->risoluzione->head = nodo;
      } break;

      case 2: {
        nodo->nextStato = r->stato->chiuso->head;
        r->stato->chiuso->head = nodo;
      } break;
    }
  }

  free(dataSeg);
}