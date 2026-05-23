/**
 *  #############################
 *  # INCLUSIONE DELLE LIBRERIE #
 *  #############################
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "./struct.h"

/**
 * #######################
 * # INIZIO INTERO GRAFO #
 * #######################
*/

struct segnalazione {
  int32_t id;
  char nome_cittadino[64];
  char categoria[64];
  char descrizione[1024];
  int data;
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

/**
 *  #####################
 *  # FINE INTERO GRAFO #
 *  #####################
*/


// Inizializza struttura
Root init_root() {
  Root r = calloc(1, sizeof(struct root));
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

  /**
   * CONTROLLI SULLE STRUTTURE RIMOSSI
   * @note siccome calloc inizializza tutti i bit a 0 e' ridondante assegnare NULL
  */

  return r;
}

// Ottieni categoria tramite prefisso
int getCategoryIndex(int prefisso) {
  switch (prefisso) {
    case 10: return illuminazione;
    case 20: return rifiuti;
    case 30: return strade;
    case 40: return verde;
    case 50: return incendio;
    case 60: return allagamento;
    case 70: return segnaletica;
    case 80: return edilizia;
    case 90: return randagismo;
    case 11: return inquinamento;
    case 21: return sicurezza;
    default: return -1;
  }
} 

// Inserisci segnalazione nel grafo
bool insertOnGraph(Root r, s newSeg) {
  if (r == NULL || newSeg == NULL) return false;

  int prefisso = newSeg->id / 100000;
  int catIdx = -1;

  catIdx = getCategoryIndex(prefisso);

  if (catIdx != -1) {
    newSeg->nextId = r->id->cat[catIdx];
    r->id->cat[catIdx] = newSeg;
    r->id->nCat[catIdx]++;

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
    return true;
  }
  return false;
}

// Loading del database.bin
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

    if (fread(nuova->nome_cittadino, sizeof(char), 64, f) != 64) {
      free(nuova); 
      break; 
    }

    if (fread(nuova->categoria, sizeof(char), 64, f) != 64) { 
      free(nuova); 
      break; 
    }

    if (fread(nuova->descrizione, sizeof(char), 1024, f) != 1024) { 
      free(nuova); 
      break; 
    }

    if (fread(&nuova->data, sizeof(int), 1, f) != 1) { 
      free(nuova); 
      break; 
    }

    if (fread(&nuova->urgenza, sizeof(int), 1, f) != 1) { 
      free(nuova); 
      break; 
    }

    if (fread(&nuova->stato, sizeof(int), 1, f) != 1) { 
      free(nuova); 
      break; 
    }

    if (!insertOnGraph(r, nuova)) free(nuova);
  }

  fclose(f);
}

// Logica di ordinamento
void quicksort(s *arr, int low, int high, int type) {
  if (low < high) {
    s pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
      int condition = 0;

      switch (type) {
        case 1: { 
          condition = (arr[j]->data < pivot->data); 
        } break;

        case 2: {
          condition = (arr[j]->id < pivot->id);
        } break;

        case 3: {
          condition = (arr[j]->urgenza < pivot->urgenza); 
        } break;

        case 4: {
          condition = (arr[j]->stato < pivot->stato); 
        } break;
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

// Inizia ordinamento
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
  }
  dataSeg[n-1]->nextData = NULL;

  quicksort(dataSeg, 0, n - 1, 2);
  for(int i = 0; i < allCat; i++) r->id->cat[i] = NULL;
  
  for(int i = n - 1; i >= 0; i--) {
    s nodo = dataSeg[i];
    int catIdx = getCategoryIndex(nodo->id / 100000);
    if(catIdx != -1) {
      nodo->nextId = r->id->cat[catIdx];
      r->id->cat[catIdx] = nodo;
    }
  }

  quicksort(dataSeg, 0, n - 1, 4);
  r->stato->aperto->head = NULL;
  r->stato->risoluzione->head = NULL;
  r->stato->chiuso->head = NULL;

  for(int i = n - 1; i >= 0; i--) {
    s nodo = dataSeg[i];
    if (nodo->stato == 0) {
      nodo->nextStato = r->stato->aperto->head;
      r->stato->aperto->head = nodo;
    } else if (nodo->stato == 1) {
      nodo->nextStato = r->stato->risoluzione->head;
      r->stato->risoluzione->head = nodo;
    } else if (nodo->stato == 2) {
      nodo->nextStato = r->stato->chiuso->head;
      r->stato->chiuso->head = nodo;
    }
  }

  quicksort(dataSeg, 0, n - 1, 3);
  for(int i = 0; i < 5; i++) r->urgenza->priority[i] = NULL;

  for(int i = n - 1; i >= 0; i--) {
    s nodo = dataSeg[i];
    int urgIdx = nodo->urgenza - 1;
    if (urgIdx >= 0 && urgIdx < 5) {
      nodo->nextUrg = r->urgenza->priority[urgIdx];
      r->urgenza->priority[urgIdx] = nodo;
      r->urgenza->nPrio[urgIdx]++;
    }
  }

  free(dataSeg);
}


/**
 * ###################
 * # GETTER FUNCTION #
 * ###################
 */

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

int32_t getID(s node) {
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

// Elimina grafo
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
  } 

  free(root->stato);
  free(root->urgenza);

  free(root);
}

// Crea un id random
int32_t getRandomId(Root root, int prefix, int catIdx) {
  int maxNum = 100000;
  int id;

  bool duplicato = false;
  
  do {
    duplicato = 0;
    id = (prefix * maxNum) + (rand() % maxNum);

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

// Prendi la quantita' di segnalazioni rispetto alla categoria
int getSegCountByCategory(Root root, int catIdx) {
  if (root == NULL || root->id == NULL) return 0;
  if (catIdx < 0 || catIdx >= allCat) return 0;
  return root->id->nCat[catIdx];
}

// Inserisce nuova segnalazione nel file binario
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

// Acquisisci posizione
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

  root->id->nCat[catIdx]++;

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

// Prendi nuova segnalazione
void getNewSeg(Root root) {
  int giorno, mese, anno, scelta;

  s newSeg = (s) calloc(1, sizeof(struct segnalazione));
  if (newSeg == NULL) return;

  printf(" #   - Seleziona una Categoria: \n");
  printf(" #   - 1) Illuminazione \n");
  printf(" #   - 2) Rifiuti \n");
  printf(" #   - 3) Strade \n");
  printf(" #   - 4) Verde \n");
  printf(" #   - 5) Incendio");
  printf(" #   - 6) Allagamento \n");
  printf(" #   - 7) Segnaletica \n");
  printf(" #   - 8) Edilizia \n");
  printf(" #   - 9) Randagismo \n");
  printf(" #   - 10) Inquinamento \n");
  printf(" #   - 11) Sicurezza \n");
  printf(" #   - Scelta: ");
  scanf("%d", &scelta);
  getchar();

  int catIdx = scelta - 1;
  int prefisso = 0;

  switch (catIdx) {
    case illuminazione: {
      prefisso = 10; strcpy(newSeg->categoria, "Illuminazione"); 
    } break;

    case rifiuti: { 
      prefisso = 20; strcpy(newSeg->categoria, "Rifiuti"); 
    } break;

    case strade: { 
      prefisso = 30; strcpy(newSeg->categoria, "Strade"); 
    } break;

    case verde: { 
      prefisso = 40; strcpy(newSeg->categoria, "Verde"); 
    } break;

    case incendio: { 
      prefisso = 50; strcpy(newSeg->categoria, "Incendio"); 
    } break;

    case allagamento: { 
      prefisso = 60; strcpy(newSeg->categoria, "Allagamento"); 
    } break;

    case segnaletica: { 
      prefisso = 70; strcpy(newSeg->categoria, "Segnaletica"); 
    } break;

    case edilizia: { 
      prefisso = 80; strcpy(newSeg->categoria, "Edilizia"); 
    } break;

    case randagismo: { 
      prefisso = 90; strcpy(newSeg->categoria, "Randagismo"); 
    } break;

    case inquinamento: { 
      prefisso = 11; strcpy(newSeg->categoria, "Inquinamento"); 
    } break;

    case sicurezza: { 
      prefisso = 21; strcpy(newSeg->categoria, "Sicurezza"); 
    } break;

    default: { 
      prefisso = 0; 
    } break;
  }

  newSeg->id = getRandomId(root, prefisso, catIdx);

  printf(" #   - Nome del cittadino: ");
  fgets(newSeg->nome_cittadino, 64, stdin);
  newSeg->nome_cittadino[strcspn(newSeg->nome_cittadino, "\n")] = 0;

  printf(" #   - Inserisci descrizione: ");
  fgets(newSeg->descrizione, 1024, stdin);
  newSeg->descrizione[strcspn(newSeg->descrizione, "\n")] = 0;

  printf(" #   - Inserisci data (DD/MM/AAAA) : "); // aggiungere placeholder
  scanf("%d/%d/%d", &giorno, &mese, &anno);
  getchar();
  newSeg->data = anno * 10000 + mese * 100 + giorno;

  // momentaneo
  newSeg->urgenza = 3;
  newSeg->stato = 0;

  printf(" #   - Nuova segnalazione creata con ID: %d", newSeg->id);
  printf(" #   - Premi INVIO per tornare alla Dashboard: ");
  getchar();

  getPosition(root, newSeg, catIdx);
  appendNewSeg(newSeg, "./components/database/database.bin");
}

// Rimuovi segnalazione
void init_removeSeg(Root r, int32_t idTarget) {
  if (!r) return;

  int prefisso = idTarget / 100000;
  int catIdx = getCategoryIndex(prefisso);

  s curr = r->id->cat[catIdx];
  s prev = NULL;

  while (curr && curr->id != idTarget) {
    prev = curr;
    curr = curr->nextId;
  }

  if (!curr) {
    printf(" #   - Segnalazione %d non trovata.\n", idTarget);
    return;
  }

  if (!prev) r->id->cat[catIdx] = curr->nextId;
  else prev->nextId = curr->nextId;

  s currD = r->data->head;
  s prevD = NULL;
  while (currD && currD->id != idTarget) {
    prevD = currD;
    currD = currD->nextData;
  }
  if (currD) {
    if (!prevD) r->data->head = currD->nextData;
    else prevD->nextData = currD->nextData;
  }

  int p = curr->urgenza - 1; 
  s currU = r->urgenza->priority[p];
  s prevU = NULL;

  while (currU && currU->id != idTarget) {
    prevU = currU;
    currU = currU->nextUrg;
  }

  if (currU) {
    if (!prevU) r->urgenza->priority[p] = currU->nextUrg;
    else prevU->nextUrg = currU->nextUrg;
    r->urgenza->nPrio[p]--;
  }

  s currS = NULL;
  s prevS = NULL;

  if (curr->stato == 0) currS = r->stato->aperto->head;
  else if (curr->stato == 1) currS = r->stato->risoluzione->head;
  else if (curr->stato == 2) currS = r->stato->chiuso->head;

  while (currS && currS->id != idTarget) {
    prevS = currS;
    currS = currS->nextStato;
  }

  if (currS) {
    if (!prevS) {
      if (curr->stato == 0) r->stato->aperto->head = currS->nextStato;
      else if (curr->stato == 1) r->stato->risoluzione->head = currS->nextStato;
      else if (curr->stato == 2) r->stato->chiuso->head = currS->nextStato;
    } else {
      prevS->nextStato = currS->nextStato;
    }
  }

  r->id->nCat[catIdx]--; 
  r->totSegnalazioni--;
  
  if (curr->stato == 0) 
    r->stato->aperto->totAperte--;
  else if (curr->stato == 1) 
    r->stato->risoluzione->totRis--;
  else if (curr->stato == 2) 
    r->stato->chiuso->totChiuse--;

  free(curr);
  printf(" #   - Segnalazione %d rimossa con successo.\n", idTarget);
}

// Salva le segnalazioni nel file binario
void save_records(Root r) {
    FILE *f = fopen("./components/database/database.bin", "wb");
    if (!f) {
        perror("Errore nell'apertura del database in scrittura");
        return;
    }

    s curr = r->data->head;
    while (curr) {
      // Scrittura speculare e precisa nei tipi rispetto alle fread
      fwrite(&(curr->id), sizeof(int32_t), 1, f);
      fwrite(curr->nome_cittadino, sizeof(char), 64, f);
      fwrite(curr->categoria, sizeof(char), 64, f);
      fwrite(curr->descrizione, sizeof(char), 1024, f);
      fwrite(&(curr->data), sizeof(int), 1, f);
      fwrite(&(curr->urgenza), sizeof(int), 1, f);
      fwrite(&(curr->stato), sizeof(int), 1, f);
      
      curr = curr->nextData;
    }
    fclose(f);
}

// Ricerca della segnalazione
void search_seg(Root r, const char *searchString) {
  if (!r || !searchString) return;

  int len = strlen(searchString);
  int catIdx = -1;

  if (len >= 2) {
    char prefixStr[3] = {
      searchString[0],
      searchString[1],
      '\0'
    };
    int prefisso = atoi(prefixStr);

    catIdx = getCategoryIndex(prefisso);
  }

  int trovati = 0;
  char idStr[20];

  s curr = (catIdx == -1) ? r->data->head : r->id->cat[catIdx];

  while (curr && trovati < 12) {
    sprintf(idStr, "%d", curr->id);

    if(strncmp(idStr, searchString, len) == 0) {
      const char *statoStr = (curr->stato == 0) ? "Aperta" : (curr->stato == 1) ? "Risoluzione" : "Chiusa";
      printf(" # | %-8d | %-15.15s | %-12.12s | P: %d | %-11s #\n", 
        curr->id, 
        curr->nome_cittadino, 
        curr->categoria, 
        curr->urgenza, 
        statoStr
      );
      trovati++;
    }

    curr = (catIdx == -1) ? curr->nextData : curr->nextId;
  }

  for (int i = trovati; i < 12; i++) {
    printf(" # |          |                 |              |      |             #\n");
  }
}

int getMaxCat(Root root) {
  if (root == NULL || root->id == NULL) return -1;

  int maxIdx = 0;

  for (int i = 1; i < allCat; i++) {
    if (root->id->nCat[i] > root->id->nCat[maxIdx]) {
      maxIdx = i;
    }
  }

  return maxIdx;
}

const char *getMaxCatName(int catIdx) {
  switch (catIdx) {
    case illuminazione: return "Illuminazione";
    case rifiuti: return "Rifiuti";
    case strade: return "Strade";
    case verde: return "Verde";
    case incendio: return "Incendio";
    case allagamento: return "Allagamento";
    case segnaletica: return "Segnaletica";
    case edilizia: return "Edilizia";
    case randagismo: return "Randagismo";
    case inquinamento: return "Inquinamento";
    case sicurezza: return "Sicurezza";
    default: return "N/D";
  }
}

void modifySeg(Root root, int32_t currId, int newState) {
  if (root == NULL || newState < 0 || newState > 2) return;

  int prefisso = currId / 100000;
  int catIdx = getCategoryIndex(prefisso);

  s curr = root->id->cat[catIdx];
  while (curr != NULL && curr->id != currId) {
    curr = curr->nextId;
  }

  if (curr == NULL || curr->stato == newState) return;

  int vecchioStato = curr->stato;

  s prevStato = NULL;
  s currStato = NULL;

  if (vecchioStato == 0) currStato = root->stato->aperto->head;
  else if (vecchioStato == 1) currStato = root->stato->risoluzione->head;
  else currStato = root->stato->chiuso->head;

  while (currStato != NULL && currStato != curr) {
    prevStato = currStato;
    currStato = currStato->nextStato;
  }

  if (currStato == curr) {
    if (prevStato == NULL) {
      if (vecchioStato == 0) root->stato->aperto->head = curr->nextStato;
      else if (vecchioStato == 1) root->stato->risoluzione->head = curr->nextStato;
      else root->stato->chiuso->head = curr->nextStato;
    } else {
      prevStato->nextStato = curr->nextStato;
    }

    if (vecchioStato == 0) root->stato->aperto->totAperte--;
    else if (vecchioStato == 1) root->stato->risoluzione->totRis--;
    else root->stato->chiuso->totChiuse--;
  }

  curr->stato = newState;

  if (newState == 0) {
    curr->nextStato = root->stato->aperto->head;
    root->stato->aperto->head = curr;
    root->stato->aperto->totAperte++;
  } else if (newState == 1) {
    curr->nextStato = root->stato->risoluzione->head;
    root->stato->risoluzione->head = curr;
    root->stato->risoluzione->totRis++;
  } else if (newState == 2) {
    curr->nextStato = root->stato->chiuso->head;
    root->stato->chiuso->head = curr;
    root->stato->chiuso->totChiuse++;
  }
}