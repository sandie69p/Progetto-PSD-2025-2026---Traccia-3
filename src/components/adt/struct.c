#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "struct.h"

typedef enum {
  APERTO, 
  RISOLUZIONE, 
  CHIUSO
} Stato;

typedef struct segnalazione {
  int id;                     // SSNNNN
  char nome_cittadino[64];
  char categoria[64];
  char descrizione[1024];
  int data;                   // YYYYMMDD
  int urgenza;
  Stato stato_segnalazione;
  struct segnalazione *next;
} Segnalazione;

typedef struct apertoNode {
  struct segnalazione *head;
} apertoNode;

typedef struct risNode {
  struct segnalazione *head;
} risNode;

typedef struct chiusoNode {
  struct segnalazione *head;
} chiusoNode;

typedef struct idNode {
  struct segnalazione *head;
} idNode;

typedef struct dataNode {
  struct segnalazione *head;
} dataNode;

typedef struct urgNode {
  struct segnalazione *head;
} urgNode;

typedef struct statoNode {
  struct apertoNode *aperto;
  struct risNode *risoluzione;
  struct chiusoNode *chiuso;
} statoNode;

typedef struct root {
  struct idNode *id;
  struct dataNode *data;
  struct statoNode *stato;
  struct urgNode *urgenti;
  int totSegnalazioni;
} Root;

Root init_root() {
  Root r = (Root) malloc(sizeof(struct root));
  if(r == NULL) return NULL;
  
  // Inizializzo i nodi di controllo
  r->id = (struct idNode *) malloc(sizeof(struct idNode));
  r->data = (struct dataNode *) malloc(sizeof(struct dataNode));
  r->urgenti = (struct dataNode *) malloc(sizeof(struct urgNode));
  r->stato = (struct statoNode *) malloc(sizeof(struct statoNode));
  
  // Inizializzo i nodi di controllo di secondo livello
  r->stato->aperto = (struct apertoNode *) malloc(sizeof(struct apertoNode));
  r->stato->risoluzione = (struct risNode *) malloc(sizeof(struct risNode));
  r->stato->chiuso = (struct chiusoNode *) malloc(sizeof(struct chiusoNode));
  
  r->id->head = NULL;
  r->data->head = NULL;
  r->urgenti->head = NULL;
  r->stato->aperto->head = NULL;
  r->stato->risoluzione->head = NULL;
  r->stato->chiuso->head = NULL;
  
  r->totSegnalazioni = 0;
}
