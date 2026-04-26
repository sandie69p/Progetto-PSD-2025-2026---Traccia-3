#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
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
  int totAperte;
} apertoNode;

typedef struct risNode {
  s head;
  int totRis;
} risNode;

typedef struct chiusoNode {
  s head;
  int totChiuse;
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
  int nCat[allCat];
} idNode;

typedef struct dataNode {
  s head;
} dataNode;

typedef struct urgNode {
  s priority[5];
  int nPrio[5];
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
  Root r = (Root) calloc(1, sizeof(struct root));
  if(r == NULL) return NULL;
  
  // Inizializzo i nodi di controllo
  r->id = (idNode *) calloc(1, sizeof(idNode));
  r->data = (dataNode *) calloc(1, sizeof(dataNode));
  r->urgenza = (urgNode *) calloc(1, sizeof(urgNode));
  r->stato = (statoNode *) calloc(1, sizeof(statoNode));
  
  // Inizializzo i nodi di controllo di secondo livello
  r->stato->aperto = (apertoNode *) calloc(1, sizeof(apertoNode));
  r->stato->risoluzione = (risNode *) calloc(1, sizeof(risNode));
  r->stato->chiuso = (chiusoNode *) calloc(1, sizeof(chiusoNode));

  for (int i = 0; i < allCat; i++) { r->id->cat[i] = NULL; r->id->nCat[i] = 0; }
  r->data->head = NULL;
  for (int i = 0; i < 5; i ++) { r->urgenza->priority[i] = NULL; r->urgenza->nPrio[i] = 0; }
  r->stato->aperto->head = NULL;
  r->stato->risoluzione->head = NULL;
  r->stato->chiuso->head = NULL;
  
  r->totSegnalazioni = 0;

  return r;
}

void insertOnGraph(Root r, s newSeg) {
  if (r == NULL || newSeg == NULL) return;

  int prefisso = newSeg->id / 100000;
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
    default: catIdx = -1; break;
  }

  if (catIdx != -1) {
    newSeg->nextId = r->id->cat[catIdx];
    r->id->cat[catIdx] = newSeg;

    newSeg->nextData = r->data->head;
    r->data->head = newSeg;

    if (newSeg->stato == 0) {
      newSeg->nextStato = r->stato->aperto->head;
      r->stato->aperto->head = newSeg;
      r->stato->aperto->totAperte++;
    } else if (newSeg->stato == 1) {
      newSeg->nextStato = r->stato->risoluzione->head;
      r->stato->risoluzione->head = newSeg;
      r->stato->risoluzione->totRis++;
    } else if (newSeg->stato == 2) {
      newSeg->nextStato = r->stato->chiuso->head;
      r->stato->chiuso->head = newSeg;
      r->stato->chiuso->totChiuse++;
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
    s nuova = (s) calloc(1, sizeof(struct segnalazione));
    if (!nuova) { fclose(f); return; }

    if (fread(&nuova->id, sizeof(int), 1, f) != 1) {
      free(nuova);
      break;
    }

    if (fread(nuova->nome_cittadino, sizeof(char), 64, f) != 64) { free(nuova); break; }
    if (fread(nuova->categoria, sizeof(char), 64, f) != 64) { free(nuova); break; }
    if (fread(nuova->descrizione, sizeof(char), 1024, f) != 1024) { free(nuova); break; }
    if (fread(&nuova->data, sizeof(int), 1, f) != 1) { free(nuova); break; }
    if (fread(&nuova->urgenza, sizeof(int), 1, f) != 1) { free(nuova); break; }
    if (fread(&nuova->stato, sizeof(int), 1, f) != 1) { free(nuova); break; }

    nuova->nextId = NULL;
    nuova->nextData = NULL;
    nuova->nextStato = NULL;
    nuova->nextUrg = NULL;

    insertOnGraph(r, nuova);
  }

  fclose(f);
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
  if (r == NULL) return;

  int n = r->totSegnalazioni;
  if (n == 0) return;

  s *dataSeg = calloc(n, sizeof(s));
  if (dataSeg == NULL) return;

  s curr = r->data->head;
  for (int i = 0; i < n && curr != NULL; i++) {
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

    int prefix = (int) nodo->id / 100000;
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
    r->urgenza->nPrio[i] = 0;
  }

  for (int i = n - 1; i >= 0; i--) {
    s nodo = dataSeg[i];
    int urgIdx = nodo->urgenza - 1;

    if (urgIdx >= 0 && urgIdx < 5) {
      nodo->nextUrg = r->urgenza->priority[urgIdx];
      r->urgenza->priority[urgIdx] = nodo;
      r->urgenza->nPrio[urgIdx]++;
    }
  }

  // Inizio ordinamento stati
  quicksort(dataSeg, 0, n - 1, 4);

  r->stato->aperto->head = NULL;
  r->stato->chiuso->head = NULL;
  r->stato->risoluzione->head = NULL;
  r->stato->aperto->totAperte = 0;
  r->stato->risoluzione->totRis = 0;
  r->stato->chiuso->totChiuse = 0;

  for (int i = n - 1; i >= 0; i--) {
    s nodo = dataSeg[i];

    switch (nodo->stato) {
      case 0: {
        nodo->nextStato = r->stato->aperto->head;
        r->stato->aperto->head = nodo;
        r->stato->aperto->totAperte++;
      } break;
      
      case 1: {
        nodo->nextStato = r->stato->risoluzione->head;
        r->stato->risoluzione->head = nodo;
        r->stato->risoluzione->totRis++;
      } break;

      case 2: {
        nodo->nextStato = r->stato->chiuso->head;
        r->stato->chiuso->head = nodo;
        r->stato->chiuso->totChiuse++;
      } break;
    }
  }

  free(dataSeg);
}

// Section Dashboard getter
int getTotalSeg(Root root) {
  return (root != NULL) ? root->totSegnalazioni : 0;
}

int getTotalAperte(Root root) {
  return (root != NULL) ? root->stato->aperto->totAperte : 0;
}

int getTotalRis(Root root) {
  return (root != NULL) ? root->stato->risoluzione->totRis : 0;
}

int getTotalChiuse(Root root) {
  return (root != NULL) ? root->stato->chiuso->totChiuse : 0;
}

int getMostUrgenti(Root root) {
  return (root != NULL) ? root->urgenza->nPrio[4] : 0;
}

s getDataHead(Root root) {
  return (root != NULL) ? root->data->head : NULL;
}

int getID(s node) {
  return (node != NULL) ? node->id : 0;
}

const char *getName(s node) {
  return (node != NULL) ? node->nome_cittadino : "Not Found";
}

const char *getDesc(s node) {
  return (node != NULL) ? node->descrizione : "Not found";
}

const char *getCat(s node) {
  return (node != NULL) ? node->categoria : "Not found";
}

int getRawData(s node) {
  return (node != NULL) ? node->data : 0;
}

int getUrg(s node) {
  return (node != NULL) ? node->urgenza : 0;
}

int getState(s node) {
  return (node != NULL) ? node->stato : 0;
}

char *getData(s node) {
  char *buffer = (char *) malloc(16 * sizeof(char));

  int data = getRawData(node);

  int anno = (int) data / 10000;
  int mese = ( data % 10000 ) / 100;
  int giorno = data % 100;

  sprintf(buffer, "%02d/%02d/%04d", giorno, mese, anno);

  return buffer;
}

s nextForID(s node) {
  return (node != NULL) ? node->nextId : NULL;
}

s nextForData(s node) {
  return (node != NULL) ? node->nextData : NULL;
}

s nextForUrg(s node) {
  return (node != NULL) ? node->nextUrg : NULL;
}

void deleteGraph(Root root) {
  if (root == NULL || root->data == NULL) return;

  s currNode = root->data->head;
  s nextNode;

  while (currNode != NULL) {
  	nextNode = currNode->nextData;

  	free(currNode);

  	currNode = nextNode;
  }

  free(root->id);
  free(root->data);
  
  if (root->stato) {
  	free(root->stato->aperto);
  	free(root->stato->risoluzione);
  	free(root->stato->chiuso);
  } free(root->stato);
  
  free(root->urgenza);

  free(root);
}

int getRandomId(Root root, int prefix, int catIdx) {
  int maxNum = 100000;
  int id;  
  bool duplicato = false;
  
  do {
    duplicato = 0;
    id = (prefix * 100000) + (rand() % maxNum);

    s currSeg = root->id->cat[catIdx];

    while (currSeg != NULL) {
      if(currSeg->id == id) {
        duplicato = true;
        break;
      }
      currSeg = currSeg->nextId;
    }

  } while (duplicato);

  return id;
}

void appendNewSeg(s newSeg, const char *fileName) {
  if (newSeg == NULL) return;

  FILE *f = fopen(fileName, "ab");
  if (f == NULL) { perror("Errore nell'apertura!"); return; }

  fwrite(&newSeg->id, sizeof(int), 1, f);
  fwrite(newSeg->nome_cittadino, sizeof(char), 64, f);
  fwrite(newSeg->categoria, sizeof(char), 64, f);
  fwrite(newSeg->descrizione, sizeof(char), 1024, f);
  fwrite(&newSeg->data, sizeof(int), 1, f);
  fwrite(&newSeg->urgenza, sizeof(int), 1, f);
  fwrite(&newSeg->stato, sizeof(int), 1, f);

  fclose(f);

  printf("[FILE] Segnalazione registrata con successo!");
}

void getPosition(Root root, s newSeg, int catIdx) {
  if (root == NULL || newSeg == NULL) return;
  if (catIdx < 0 || catIdx >= allCat) return;

  // ordinamento di data
  if(root->data->head == NULL || newSeg->data < root->data->head->data) {
    newSeg->nextData = root->data->head;
    root->data->head = newSeg;
  } else {
    s currSeg = root->data->head;

    while (currSeg->nextData != NULL && currSeg->nextData->data <= newSeg->data) {
      currSeg = currSeg->nextData;
    }

    newSeg->nextData = currSeg->nextData;
    currSeg->nextData = newSeg;
  }

  // ordinameto di id
  s headCat = root->id->cat[catIdx];

  if(headCat == NULL || newSeg->id < headCat->id) {
    newSeg->nextId = headCat;
    root->id->cat[catIdx] = newSeg;
  } else {
    s currSeg = headCat;
    while (currSeg->nextId != NULL && currSeg->nextId->id <= newSeg->id) {
      currSeg = currSeg->nextId;
    }
    newSeg->nextId = currSeg->nextId;
    currSeg->nextId = newSeg;
  }

  if (newSeg->stato == 0) { // Aperto
      newSeg->nextStato = root->stato->aperto->head;
      root->stato->aperto->head = newSeg;
      root->stato->aperto->totAperte++;
  } else if (newSeg->stato == 1) { // In risoluzione
      newSeg->nextStato = root->stato->risoluzione->head;
      root->stato->risoluzione->head = newSeg;
      root->stato->risoluzione->totRis++;
  } else { // Chiuso
      newSeg->nextStato = root->stato->chiuso->head;
      root->stato->chiuso->head = newSeg;
      root->stato->chiuso->totChiuse++;
  }

  int urgIdx = newSeg->urgenza - 1;
  
  if (urgIdx >= 0 && urgIdx < 5) {
    newSeg->nextUrg = root->urgenza->priority[urgIdx];
    root->urgenza->priority[urgIdx] = newSeg;
    root->urgenza->nPrio[urgIdx]++;
  }

  root->totSegnalazioni++;
}

void getNewSeg(Root root) {
  int giorno, mese, anno, scelta;

  s newSeg = (s) calloc(1, sizeof(struct segnalazione));
  if (newSeg == NULL) return;

  printf("Seleziona una Categoria: \n");
  printf(" # 1) Illuminazione\n # 2) Rifiuti\n # 3) Strade\n # 4) Verde\n # 5) Incendio\n # 6) Allagamento\n # 7) Segnaletica\n # 8) Edilizia\n # 9) Randagismo\n # 10) Inquinamento\n # 11) Sicurezza\n");
  printf(" # Scelta: ");
  scanf("%d", &scelta);
  getchar();

  int catIdx = scelta - 1;
  int prefix = 0;

  switch (catIdx) {
    case illuminazione: prefix = 10; strcpy(newSeg->categoria, "Illuminazione"); break;
    case rifiuti: prefix = 20; strcpy(newSeg->categoria, "Rifiuti"); break;
    case strade: prefix = 30; strcpy(newSeg->categoria, "Strade"); break;
    case verde: prefix = 40; strcpy(newSeg->categoria, "Verde"); break;
    case incendio: prefix = 50; strcpy(newSeg->categoria, "Incendio"); break;
    case allagamento: prefix = 60; strcpy(newSeg->categoria, "Allagamento"); break;
    case segnaletica: prefix = 70; strcpy(newSeg->categoria, "Segnaletica"); break;
    case edilizia: prefix = 80; strcpy(newSeg->categoria, "Edilizia"); break;
    case randagismo: prefix = 90; strcpy(newSeg->categoria, "Randagismo"); break;
    case inquinamento: prefix = 11; strcpy(newSeg->categoria, "Inquinamento"); break;
    case sicurezza: prefix = 21; strcpy(newSeg->categoria, "Sicurezza"); break;
    default: prefix = 0; break;
  }

  newSeg->id = getRandomId(root, prefix, catIdx);

  printf("Nome del cittadino: ");
  fgets(newSeg->nome_cittadino, 64, stdin);
  newSeg->nome_cittadino[strcspn(newSeg->nome_cittadino, "\n")] = 0;

  printf("Inserisci descrizione: ");
  fgets(newSeg->descrizione, 1024, stdin);
  newSeg->descrizione[strcspn(newSeg->descrizione, "\n")] = 0;

  printf("Inserisci data (DD/MM/AAAA) : "); // aggiungere placeholder
  scanf("%d/%d/%d", &giorno, &mese, &anno);
  getchar();
  newSeg->data = anno * 10000 + mese * 100 + giorno;

  // momentaneo
  newSeg->urgenza = 3;
  newSeg->stato = 0;

  getPosition(root, newSeg, catIdx);
  appendNewSeg(newSeg, "./components/database/database.bin");
}
