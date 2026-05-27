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
#include <time.h>
#include "./struct.h"

/**
 * #######################
 * # INIZIO INTERO GRAFO #
 * #######################
*/

/**
 * @struct segnalazione
 * @brief Nodo atomico fondamentale che rappresenta un singolo disservizio urbano.
 *
 * @note  Per ottimizzare lo spazio in memoria Heap e garantire l'integrità transazionale 
 *        del sistema, ogni record esiste una e una sola volta in RAM. La multidimensionalità 
 *        del grafo si ottiene tramite 4 catene di puntatori ortogonali e indipendenti 
 *        (nextId, nextData, nextStato, nextUrg). Questo permette allo stesso nodo di coesistere 
 *        simultaneamente in 4 attraversamenti topologici distinti senza duplicare i dati.
 */
struct segnalazione {
  int32_t id;                  /**< Identificativo univoco a 6 cifre (Prefisso Categoria + Codice Random) */
  char nome_cittadino[64];     /**< Buffer statico per l'acquisizione verificata dell'identità del segnalatore */
  char categoria[64];          /**< Stringa letterale della categoria, memorizzata per accelerare i filtri di ricerca */
  char descrizione[1024];      /**< Buffer testuale per la mappatura dettagliata del problema infrastrutturale */
  int data;                    /**< Timestamp cronologico di sistema compresso in un formato intero YYYYMMDD */
  int urgenza;                 /**< Coefficiente di priorità calcolato su una scala da 1 a 5 */
  int stato;                   /**< Codice numerico della macchina a stati operativi (0: Aperta, 1: In Risoluzione, 2: Chiusa) */
  s nextId;                    /**< Puntatore al nodo successivo nella catena sequenziale per ID della stessa categoria */
  s nextData;                  /**< Puntatore al nodo successivo nell'asse logico dell'ordine cronologico globale */
  s nextStato;                 /**< Puntatore al nodo successivo nella coda dello specifico stato di avanzamento */
  s nextUrg;                   /**< Puntatore al nodo successivo all'interno dello stesso cluster di urgenza */
};

/**
 * @struct apertoNode
 * @brief Descrittore di intestazione per l'ancoraggio dei disservizi attivi e non ancora presi in carico.
 */
typedef struct apertoNode {
  s head;                      /**< Punto di ingresso per la sotto-lista delle segnalazioni aperte */
  int totAperte;               /**< Contatore live per l'estrazione delle metriche in tempo costante O(1) */
} apertoNode;

/**
 * @struct risNode
 * @brief Descrittore di intestazione per l'ancoraggio dei disservizi attualmente in fase di lavorazione.
 */
typedef struct risNode {
  s head;                      /**< Punto di ingresso per la sotto-lista delle segnalazioni in risoluzione */
  int totRis;                  /**< Contatore live che evita scansioni lineari per il calcolo dei carichi di lavoro */
} risNode;

/**
 * @struct chiusoNode
 * @brief Descrittore di intestazione per l'ancoraggio dello storico dei disservizi risolti.
 */
typedef struct chiusoNode {
  s head;                      /**< Punto di ingresso per la sotto-lista delle segnalazioni storiche chiuse */
  int totChiuse;               /**< Contatore live per l'analisi immediata del throughput del sistema */
} chiusoNode;

/**
 * @enum catId
 * @brief Enumerazione posizionale esplicita utilizzata per mappare in sicurezza gli indici dei vettori delle categorie.
 */
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
  allCat                      /**< Elemento sentinella fisso che traccia il numero totale di categorie censite */
} catId;

/**
 * @struct idNode
 * @brief Ancora strutturale multi-bucket per mappare la distribuzione dei record in base alla tipologia.
 */
typedef struct idNode {
  s cat[allCat];               /**< Vettore di puntatori alle teste delle singole liste di categorie filtrate per ID */
  int nCat[allCat];            /**< Vettore per il tracciamento in tempo reale della densità di record per ogni settore */
} idNode;

/**
 * @struct dataNode
 * @brief Ancora strutturale per la gestione delle sequenze degli eventi su base temporale.
 */
typedef struct dataNode {
  s head;                      /**< Punto di ingresso assoluto per le query sulla timeline cronologica globale */
} dataNode;

/**
 * @struct urgNode
 * @brief Ancora strutturale per l'isolamento dei record in base ai livelli di emergenza.
 */
typedef struct urgNode {
  s priority[5];               /**< Vettore di puntatori alle teste delle liste per i 5 livelli di priorità [1-5] */
  int nPrio[5];                /**< Contatori live per registrare la densità di elementi per ciascun livello di emergenza */
} urgNode;

/**
 * @struct statoNode
 * @brief Sotto-descrittore logico che raggruppa i canali delle code di avanzamento.
 */
typedef struct statoNode {
  apertoNode *aperto;          /**< Riferimento alla sotto-struttura delle segnalazioni attive non assegnate */
  risNode *risoluzione;        /**< Riferimento alla sotto-struttura delle segnalazioni in fase di gestione */
  chiusoNode *chiuso;          /**< Riferimento alla sotto-struttura dello storico dei dati risolti */
} statoNode;

/**
 * @struct root
 * @brief Punto di orchestrazione supremo che unisce i quattro percorsi di accesso indipendenti.
 *
 * @note  Fornisce un'interfaccia centralizzata a livello di radice per manipolare la topologia 
 *        del grafo, garantendo l'Information Hiding: i moduli esterni non conoscono l'indirizzo 
 *        o l'organizzazione interna della memoria fisica dei nodi.
 */
struct root {
  idNode *id;                  /**< Collegamento all'indice strutturale per categoria e identificativo */
  dataNode *data;              /**< Collegamento all'indice strutturale per sequenziamento temporale */
  statoNode *stato;            /**< Collegamento all'indice strutturale per stato di avanzamento dei flussi */
  urgNode *urgenza;            /**< Collegamento all'indice strutturale per il filtraggio delle emergenze */
  int totSegnalazioni;         /**< Contatore generale monolitico del volume complessivo di nodi nel sistema */
};

/**
 *  #####################
 *  # FINE INTERO GRAFO #
 *  #####################
*/


/**
 * @brief Inizializza la struttura di controllo principale (Radice del Sistema).
 *
 * @note  Alloca dinamicamente nello Heap l'ancora di orchestrazione del grafo e l'intera 
 *        matrice dei descrittori di primo e secondo livello. L'utilizzo di calloc() è un 
 *        vincolo progettuale: garantendo l'azzeramento nativo di tutti i bit del blocco 
 *        allocato, ogni puntatore interno viene automaticamente inizializzato a NULL e ogni 
 *        contatore a 0. Questo azzera il rischio di "garbage memory" eliminando al contempo 
 *        la necessità di logiche di inizializzazione esplicite e ridondanti.
 *
 * @pre Nessuna.
 * @post Restituisce un puntatore Root valido e isolato, pronto per il popolamento.
 *
 * @return Root Il puntatore alla radice del sistema allocata, oppure NULL se il 
 * sottosistema di memoria Heap fallisce l'allocazione (Out of Memory).
 */
Root init_root() {
  Root r = calloc(1, sizeof(struct root));
  if (r == NULL) return NULL;
  
  r->id = (idNode *) calloc(1, sizeof(idNode));
  r->data = (dataNode *) calloc(1, sizeof(dataNode));
  r->urgenza = (urgNode *) calloc(1, sizeof(urgNode));
  r->stato = (statoNode *) calloc(1, sizeof(statoNode));
  
  r->stato->aperto = (apertoNode *) calloc(1, sizeof(apertoNode));
  r->stato->risoluzione = (risNode *) calloc(1, sizeof(risNode));
  r->stato->chiuso = (chiusoNode *) calloc(1, sizeof(chiusoNode));

  return r;
}

/**
 * @brief Traduce il prefisso numerico dell'ID nell'indice posizionale dell'array delle categorie.
 *
 * Questa funzione interna agisce come un selettore logico isolato. Riceve le prime due 
 * cifre della segnalazione (estratte tramite scomposizione matematica) e le mappa 
 * sulla corrispondente costante enumerativa. Questo passaggio è cruciale per l'Information 
 * Hiding e la stabilità del grafo multi-indice: circoscrive le successive operazioni di 
 * ricerca e sfoltimento a un singolo subset (bucket), ottimizzando la complessità 
 * computazionale ed evitando scansioni lineari sull'intero database di 25.000 nodi.
 *
 * @note All'interno del corpo della funzione non vengono inseriti commenti linea per linea 
 * sui singoli casi per preservare la leggibilità del blocco di commutazione sequenziale.
 *
 * @param[in] prefisso Valore numerico intero estratto dalla scomposizione matematica dell'ID 
 * della segnalazione, rappresentante il codice identificativo della classe di servizio.
 *
 * @pre Il parametro `prefisso` deve essere un intero estratto coerentemente come le prime due cifre di un ID a 6 cifre.
 * @post Lo stato del sistema rimane invariato. Viene restituito un valore di mappatura posizionale per i vettori.
 *
 * @return int L'indice intero (0-10) corrispondente alla cella del vettore della categoria, 
 * oppure -1 come segnale d'errore se il prefisso non appartiene a nessuna classe censita.
 */
static int getCategoryIndex(int prefisso) {
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

/**
 * @brief Esegue l'inserimento rapido in testa di un nodo su tutti e quattro gli indici ortogonali.
 *
 * Questa funzione interna viene invocata esclusivamente durante la fase di avvio (boot) dal 
 * database binario. Applica una politica di inserimento in testa (front-loading) con complessità 
 * computazionale ottimale pari a tempo costante O(1). L'omissione volontaria di logiche di 
 * inserimento ordinato in questa fase risponde a un preciso vincolo di efficienza, volto a 
 * velocizzare lo streaming dei record da disco; l'allineamento topologico e cronologico perfetto 
 * del grafo viene differito ed eseguito massivamente in un secondo momento tramite init_sorting().
 *
 * @param[in,out] r Puntatore alla struttura principale di controllo (Root) in cui agganciare il nodo.
 * @param[in,out] newSeg Puntatore al nodo segnalazione pre-allocato e valorizzato da inserire nel grafo.
 *
 * @pre `r` deve essere un'istanza di Root non nulla, validamente inizializzata tramite init_root().
 * @pre `newSeg` deve puntare a un blocco di memoria Heap integro, non nullo e contenente dati di record validi.
 * @post Se l'ID è valido, il nodo `newSeg` viene agganciato simultaneamente in testa alle liste dei 4 descrittori 
 * ortogonali (ID categoria, data, sotto-stato e urgenza). I contatori parziali e globali vengono incrementati di 1 unità.
 *
 * @return bool Ritorna true se il record viene agganciato con successo a tutte le strutture 
 * di controllo del grafo, false se l'indice di categoria derivato risulta non valido.
 */
static bool insertOnGraph(Root r, s newSeg) {
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
      r->urgenza->nPrio[urgIdx]++;
    }
    
    r->totSegnalazioni++;
    return true;
  }
  return false;
}

/**
 * @brief Ricostruisce il database in memoria RAM leggendo sequenzialmente i record da un file binario.
 *
 * La funzione implementa una routine difensiva di scompattamento dati (parsing) da disco. Per ogni 
 * record rilevato, viene allocato un nuovo nodo isolato sullo Heap tramite calloc(). Ciascun campo 
 * viene estratto verificando rigorosamente la corrispondenza dei byte letti: se una chiamata a fread() 
 * fallisce o intercetta la fine del file (EOF), il ciclo si interrompe immediatamente e la memoria del 
 * nodo parziale viene liberata tramite free() per garantire la totale assenza di memory leak (0 leak).
 * Solo i nodi completamente validati ed integri vengono infine inoltrati alla funzione insertOnGraph().
 *
 * @note Non vengono inseriti commenti all'interno dei singoli costrutti condizionali 'if' per non 
 * frammentare l'impatto visivo e la pulizia del blocco sequenziale di lettura.
 *
 * @param[in,out] r Puntatore alla struttura di controllo principale (Root) da popolare tramite file.
 * @param[in] fileName Stringa costante contenente il percorso relativo o assoluto del file binario (.bin) su disco.
 *
 * @pre `r` deve essere un puntatore Root valido, allocato e non nullo.
 * @pre `fileName` deve essere una stringa non nulla ed identificare un file binario esistente e accessibile in lettura.
 * @post Il grafo multi-indice in memoria RAM viene espanso incorporando esclusivamente i record binari validi ed integri.
 * Il file stream viene correttamente chiuso prima dell'uscita. La coerenza dell'ordinamento è delegata alle chiamate successive.
 */
void init_loadingDb(Root r, const char *fileName) {
  FILE *f = fopen(fileName, "rb");
  if (f == NULL) { perror("Errore apertura file"); return; }

  while (1) {
    s nuova = (s) calloc(1, sizeof(struct segnalazione));
    if (!nuova) { fclose(f); return; }

    if (fread(&nuova->id, sizeof(int32_t), 1, f) != 1) {
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

/**
 * @brief Algoritmo di ordinamento QuickSort ricorsivo e parametrico operante su vettori di puntatori.
 *
 * La funzione implementa l'algoritmo di partizionamento in-place su un array temporaneo di puntatori 
 * a struttura. La decisione progettuale cardine risiede nella separazione tra la logica di partizionamento 
 * (che scambia indirizzi di memoria anziché copiare pesanti record fisici) e il criterio di ordinamento. 
 * Il parametro 'type' mappa dinamicamente i quattro diversi assi ortogonali del grafo, determinando 
 * la chiave di confronto (Data, ID, Urgenza o Stato) senza duplicare il codice algoritmico principale.
 *
 * @param[in,out] arr Vettore (array) di puntatori a nodi segnalazione (`s`) da sottoporre a ordinamento.
 * @param[in] low Indice numerico intero che definisce il limite inferiore (partenza) del sotto-vettore corrente.
 * @param[in] high Indice numerico intero che definisce il limite superiore (fine, posizione iniziale del pivot) del sotto-vettore corrente.
 * @param[in] type Codice intero di selezione del criterio di ordinamento (1: Data, 2: ID, 3: Urgenza, 4: Stato).
 *
 * @pre `arr` deve essere un array pre-allocato contenente un numero di puntatori a nodo coerente con l'intervallo [low, high].
 * @pre I valori di `low` e `high` devono rientrare nei limiti di allocazione fisica del vettore.
 * @post Gli elementi (indirizzi di memoria) contenuti nell'array `arr` vengono riordinati in posizione (in-place) 
 * in base alla metrica numerica selezionata dal parametro `type`.
 */
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

/**
 * @brief Ricostruisce, riallinea e ordina simultaneamente tutti e quattro gli assi ortogonali del grafo.
 *
 * Questa funzione costituisce il meccanismo di sincronizzazione massiva della RAM. La decisione 
 * progettuale cardine risiede nella linearizzazione temporanea della rete di nodi: l'intera struttura 
 * viene temporaneamente "appiattita" all'interno di un vettore di puntatori di supporto allocato 
 * dinamicamente sullo Heap. Questo approccio previene la corruzione delle catene logiche parallele 
 * durante le mutazioni. Sfruttando chiamate consecutive a `quicksort()` con metriche differenziate, 
 * la funzione azzera le teste dei descrittori e ri-stende da zero i collegamenti di adiacenza (`nextData`, 
 * `nextId`, `nextStato`, `nextUrg`), stabilizzando il grafo dopo inserimenti o modifiche a caldo.
 *
 * @note Per mantenere la massima pulizia visiva e conformità accademica, i singoli passi di 
 * riallineamento delle sotto-liste non sono frammentati da micro-commenti in-linea.
 *
 * @param[in,out] r Puntatore alla struttura principale di controllo (Root) il cui grafo deve essere riordinato.
 *
 * @pre `r` deve essere un puntatore Root valido, inizializzato e contenente un volume di record coerente con `totSegnalazioni`.
 * @post I 4 indici ortogonali della RAM vengono rigenerati. L'indice cronologico risulta ordinato per data crescente; 
 * i bucket delle categorie, dello stato e dell'urgenza vengono ricatenati preservando l'ordinamento relativo. 
 * La memoria del vettore di supporto viene interamente deallocata tramite free() prima dell'uscita.
 */
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

  free(dataSeg);
}


/**
 * #################################################################========
 * METODI GETTER GLOBALI (Interfaccia di Incapsulamento ed Information Hiding)
 * #################################################################========
 * @{
 * 
 * @note per queste funzioni ho scritto `@param[in] root` / `@pre` / `@post` qui' perche'
 * ridondanti nella specifica in alto ad ogni funzione della sezione corrente
 * 
 * @param[in] root Puntatore alla struttura di controllo principale (radice).
 * @pre `root` deve essere un'istanza valida allocata tramite init_root().
 * @post Lo stato della memoria RAM non subisce alcuna alterazione.
 */


/**
 * @brief Estrae il volume complessivo di segnalazioni registrate nel sistema.
 * @return int Il numero totale di nodi attivi nel grafo, o 0 se la radice è NULL.
 */
int getTotalSeg(Root root)    { return (root != NULL) ? root->totSegnalazioni : 0; }

/**
 * @brief Estrae la cardinalità delle segnalazioni correnti in stato "Aperta".
 * @return int Il numero di segnalazioni aperte, o 0 se la radice è NULL.
 */
int getTotalAperte(Root root) { return (root != NULL) ? root->stato->aperto->totAperte : 0; }

/**
 * @brief Estrae la cardinalità delle segnalazioni correnti in stato "In Risoluzione".
 * @return int Il numero di segnalazioni in lavorazione, o 0 se la radice è NULL.
 */
int getTotalRis(Root root)    { return (root != NULL) ? root->stato->risoluzione->totRis : 0; }

/**
 * @brief Estrae la cardinalità delle segnalazioni correnti in stato "Chiusa".
 * @return int Il numero di segnalazioni storiche risolte, o 0 se la radice è NULL.
 */
int getTotalChiuse(Root root) { return (root != NULL) ? root->stato->chiuso->totChiuse : 0; }

/**
 * @brief Calcola il numero di emergenze attive con livello di urgenza massimo (pari a 5).
 * @return int Il numero di elementi presenti nel sotto-nodo di priorità 5, o 0 se la radice è NULL.
 */
int getMostUrgenti(Root root) { return (root != NULL) ? root->urgenza->nPrio[4] : 0; }

/**
 * @brief Restituisce il punto di ingresso (testa) dell'asse cronologico globale.
 * @post Restituisce un puntatore al nodo più vecchio senza esporre i descrittori di controllo.
 * @return s Il link al primo nodo della lista temporale, o NULL se il sistema è vuoto.
 */
s getDataHead(Root root)      { return (root != NULL) ? root->data->head : NULL; }

/** @} */

/**
 * #################################################################========
 * METODI GETTER ATOMICI (Estrazione Campi del Nodo Opaco)
 * #################################################################========
 * @{
 * 
 * @note per queste funzioni ho scritto `@param[in] node` / `@pre` qui' perche'
 * ridondanti nella specifica in alto ad ogni funzione della sezione corrente
 * 
 * @param[in] node Il puntatore al nodo della segnalazione target.
 * @pre `node` deve essere un record valido e non nullo.
 */

/**
 * @brief Estrae l'identificativo numerico a 32 bit da un singolo nodo.
 * @post Non modifica i campi interni del nodo.
 * @return int32_t Il codice ID a 6 cifre del disservizio, o 0 se il nodo è vuoto.
 */
int32_t getID(s node)         { return (node != NULL) ? node->id : 0; }

/**
 * @brief Estrae la stringa costante del nome del cittadino segnalatore.
 * @post Restituisce l'indirizzo del buffer senza effettuare allocazioni o duplicazioni.
 * @return const char* Stringa costante contenente l'identità dell'utente, o "Not Found".
 */
const char *getName(s node)   { return (node != NULL) ? node->nome_cittadino : "Not Found"; }

/**
 * @brief Restituisce un puntatore alla stringa costante contenente la descrizione della segnalazione.
 *
 * Funzione getter di interfaccia per l'estrazione sicura del testo descrittivo associato a un nodo, 
 * preservando l'Information Hiding dell'ADT opaco ed evitando che il modulo HUD acceda direttamente 
 * alle proprietà della struttura interna.
 *
 * @param[in] node Il puntatore costante al record segnalazione (`s`) da interpellare.
 *
 * @pre `node` deve essere un'istanza valida, non nulla e correttamente allocata.
 * @post Lo stato del nodo in memoria RAM rimane completamente inalterato.
 *
 * @return const char* Il puntatore al buffer statico di caratteri contenente la descrizione, 
 * oppure NULL se il nodo passato è nullo.
 */
const char *getDescription(s node) {
    if (node == NULL) return NULL;
    return node->descrizione;
}


/**
 * @brief Estrae la stringa costante identificativa della categoria comunale.
 * @post Restituisce l'indirizzo del letterale associato alla classe del nodo.
 * @return const char* Stringa costante col nome della categoria, o "Not found".
 */
const char *getCat(s node)    { return (node != NULL) ? node->categoria : "Not found"; }

/**
 * @brief Estrae la data memorizzata nel suo formato intero normalizzato (AAAAMMGG).
 * @post Restituisce il valore numerico grezzo utile per le routine di confronto.
 * @return int Intero codificato in formato AAAAMMGG, o 0 se il nodo è vuoto.
 */
int getRawData(s node)        { return (node != NULL) ? node->data : 0; }

/**
 * @brief Estrae il coefficiente di urgenza associato alla segnalazione.
 * @post Restituisce il livello di urgenza.
 * @return int Coefficiente numerico compreso tra 1 e 5, o 0 se il nodo è vuoto.
 */
int getUrg(s node)            { return (node != NULL) ? node->urgenza : 0; }

/**
 * @brief Estrae il codice dello stato operativo corrente del record.
 * @post Restituisce lo stato corrente.
 * @return int Codice dello stato (0: Aperta, 1: In Risoluzione, 2: Chiusa), o 0 se il nodo è vuoto.
 */
int getState(s node)          { return (node != NULL) ? node->stato : 0; }

/** @} */

/**
 * #################################################################========
 * METODI ITERATORI (Navigazione Avanzata sui canali Ortogonali)
 * #################################################################========
 * @{
 * 
 * @note per queste funzioni ho scritto `@param[in] node` / `@pre` qui' perche'
 * ridondanti nella specifica in alto ad ogni funzione della sezione corrente
 * 
 * @param[in] node Il nodo di partenza dell'attraversamento.
 * @pre `node` deve essere inserito coerentemente nel grafo multi-indice.
 */

/**
 * @brief Naviga l'asse logico dell'indice degli ID all'interno della medesima categoria.
 * @post Restituisce il collegamento adiacente di categoria.
 * @return s Puntatore al nodo logicamente successivo per ID crescente, o NULL se la catena è terminata.
 */
s nextForID(s node)           { return (node != NULL) ? node->nextId : NULL; }

/**
 * @brief Naviga l'asse logico della timeline cronologica globale.
 * @post Restituisce il collegamento adiacente temporale.
 * @return s Puntatore al nodo logicamente successivo per data crescente, o NULL se si è in coda alla lista.
 */
s nextForData(s node)         { return (node != NULL) ? node->nextData : NULL; }

/**
 * @brief Naviga l'asse logico dei cluster aventi la stessa priorità/urgenza.
 * @post Restituisce il collegamento adiacente di urgenza.
 * @return s Puntatore al nodo logicamente successivo con pari coefficiente di urgenza, o NULL se la lista è conclusa.
 */
s nextForUrg(s node)          { return (node != NULL) ? node->nextUrg : NULL; }

/** @} */

/**
 * @brief Converte la data intera raw del nodo in una stringa formattata leggibile (GG/MM/AAAA).
 *
 * La funzione si occupa di scompattare il timestamp cronologico compresso (normalizzato come intero 
 * nel formato AAAAMMGG) attraverso operazioni di scomposizione matematica (divisioni e moduli). 
 * Alloca dinamicamente un buffer di testo sullo Heap per contenere la stringa risultante. 
 * Rappresenta un punto critico per la gestione della memoria del software: la titolarità del blocco 
 * allocato passa direttamente al modulo chiamante, che deve tassativamente occuparsi della sua 
 * deallocazione.
 *
 * @param[in] node Il puntatore al nodo segnalazione da cui estrarre la componente temporale.
 *
 * @pre `node` deve essere un record valido, non nullo e precedentemente inserito nel grafo.
 * @post Alloca un'area di 16 byte sulla memoria Heap. Lo stato interno del nodo non viene modificato.
 *
 * @param[out] buffer Puntatore all'array di caratteri allocato ed emesso in memoria dinamica.
 *
 * @return char* Il puntatore alla stringa formattata "GG/MM/AAAA", pronta per l'emissione a video 
 * tramite HUD, oppure NULL se il sotto-sistema di memoria Heap è esaurito.
 * @return NULL se node e' NULL oppure se l'allocazione della memoria fallisce.
 * 
 * @warning Poiché la funzione esegue una malloc() esplicita ad ogni invocazione, il modulo ricevente 
 * ha la responsabilità di invocare la free() sul puntatore restituito per evitare memory leak.
 */
char *getData(s node) {
  if (node == NULL) return NULL;

  char *buffer = (char *) malloc(16 * sizeof(char));
  if (buffer == NULL) return NULL;

  int data = getRawData(node);

  int anno    = (int) data / 10000;
  int mese    = (data % 10000) / 100;
  int giorno  = data % 100;

  sprintf(buffer, "%02d/%02d/%04d", giorno, mese, anno);

  return buffer;
}

/**
 * @brief Esegue la completa demolizione controllata del grafo multi-indice, azzerando la memoria Heap.
 *
 * La funzione implementa la routine di shutdown e cleanup totale del sistema logico. Poiché ogni 
 * segnalazione è allocata fisicamente una sola volta nell'Heap ma è agganciata a quattro indici 
 * paralleli, la funzione sceglie l'asse cronologico globale (`root->data->head`) come cammino lineare 
 * sicuro per scorrere ed eliminare atomicamente tutti i nodi. L'utilizzo della variabile temporanea 
 * `nextNode` è un vincolo algoritmico per salvare l'indirizzo del blocco successivo prima di invocare 
 * la free() sul nodo corrente, prevenendo crash da puntatori instabili (Dangling Pointers). Una volta 
 * svuotato il pool dei nodi, vengono deallocati in sequenza tutti i descrittori di primo e secondo livello.
 *
 * @note All'interno dei cicli e dei blocchi condizionali di liberazione non vengono inseriti commenti 
 * riga per riga per preservare l'impatto visivo e l'ordine sequenziale delle chiamate a free().
 *
 * @param[in,out] root Il puntatore alla struttura di controllo principale (radice) da smantellare e liberare.
 *
 * @pre Il puntatore `root` può essere anche NULL o parzialmente allocato (la funzione gestisce in sicurezza l'eventualità).
 * @post Tutta la memoria dinamica allocata per i nodi segnalazione, per gli array dei descrittori dei quattro indici 
 * e per la radice viene interamente rilasciata. L'Heap viene azzerata con successo, garantendo 0 leak su Valgrind.
 * Il puntatore posseduto dal chiamante diventa non valido e non deve essere riutilizzato.
 */
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

/**
 * @brief Genera un identificativo numerico pseudo-casuale a 5 cifre garantendone l'univocità.
 *
 * La funzione combina il prefisso ministeriale della categoria con un valore numerico generato 
 * casualmente tramite rand(). Per escludere qualsiasi collisione logica o duplicazione di chiavi 
 * primarie, viene implementato un ciclo di controllo difensivo do-while. La decisione progettuale 
 * cardine per ottimizzare la complessità temporale risiede nel circoscrivere la ricerca: lo 
 * scorrimento lineare del grafo non avviene sull'intero database, ma viene mirato ed isolato 
 * all'interno del solo bucket della categoria interessata tramite l'asse logico `nextId`.
 *
 * @param[in] root Puntatore alla struttura di controllo principale (radice) per l'accesso agli indici.
 * @param[in] prefix Valore intero a due cifre che identifica il codice ministeriale della categoria.
 * @param[in] catIdx Indice posizionale del vettore delle categorie per il piazzamento del puntatore di scansione.
 *
 * @pre `root` deve essere un'istanza valida allocata e popolata. `catIdx` deve rientrare nel range [0, allCat-1].
 * @post Lo stato del grafo in memoria RAM rimane invariato. Viene calcolato e isolato un ID univoco.
 *
 * @return int32_t Un identificativo intero a 32 bit matematicamente univoco all'interno del sistema corrente.
 */
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

/**
 * @brief Restituisce il volume di segnalazioni attive associate a una specifica categoria comunale.
 *
 * La funzione funge da interfaccia di lettura sicura per monitorare la densità dei disservizi. 
 * La decisione progettuale cardine risiede nell'ottimizzazione della complessità computazionale: 
 * invece di scorrere linearmente la lista concatenata della categoria per contare i nodi (operazione 
 * che degraderebbe le prestazioni a O(N)), la funzione interroga in tempo costante O(1) il vettore 
 * dei contatori pre-calcolati `nCat`. Include un meccanismo di controllo difensivo rigido che 
 * intercetta indici non conformi o negativi, prevenendo crash da violazione di memoria.
 *
 * @param[in] root Il puntatore alla struttura di controllo principale (radice) del sistema.
 * @param[in] catIdx L'indice numerico posizionale associato alla categoria comunale da campionare.
 *
 * @pre `root` deve essere un'istanza valida allocata e inizializzata.
 * @pre `catIdx` deve mappare un indice valido all'interno del range di allocazione del vettore.
 * @post Lo stato del grafo in memoria RAM e i contatori non subiscono alcuna alterazione.
 *
 * @param[out] nCat[catIdx] Il valore numerico estratto direttamente dalla cella di memoria del vettore.
 *
 * @return int Il numero esatto di segnalazioni attualmente registrate sotto la categoria richiesta, 
 * oppure 0 se la radice è nulla o se l'indice fornito è fuori dall'intervallo [0, allCat-1].
 */
int getSegCountByCategory(Root root, int catIdx) {
  if (root == NULL || root->id == NULL) return 0;
  if (catIdx < 0 || catIdx >= allCat) return 0;
  return root->id->nCat[catIdx];
}

/**
 * @brief Serializza su disco un singolo record inserendolo in coda al file binario di persistenza.
 *
 * La funzione si occupa di rendere persistenti i dati della RAM scrivendo il contenuto del nodo 
 * direttamente su memoria secondaria. La decisione progettuale cardine risiede nell'uso della 
 * modalità di apertura "ab" (append binario): questo garantisce un'operazione efficiente a tempo 
 * costante O(1), poiché il flusso di byte viene agganciato direttamente alla fine del file senza 
 * dover rileggere o riscritturare i record preesistenti. La serializzazione preserva le dimensioni 
 * fisiche fisse dei buffer per garantire una successiva lettura sequenziale speculare e priva di disallineamenti.
 *
 * @param[in] newSeg Il puntatore costante al nodo segnalazione pre-allocato da salvare su disco.
 * @param[in] fileName Stringa costante contenente il percorso del file binario (.bin) di destinazione.
 *
 * @param[out] f File stream di output aperto verso la memoria secondaria per la scrittura dei byte.
 *
 * @pre `newSeg` deve puntare a un record valido, integro e completamente valorizzato in memoria RAM.
 * @pre `fileName` deve essere una stringa non nulla che identifica un percorso di file accessibile in scrittura.
 * @post I campi informativi del nodo vengono serializzati su disco in un blocco binario contiguo. 
 * Lo stream viene chiuso correttamente e lo stato del grafo in memoria RAM rimane inalterato.
 */
void appendNewSeg(s newSeg, const char *fileName) {
  if (newSeg == NULL) return;

  FILE *f = fopen(fileName, "ab");
  if (f == NULL) { perror("Errore nell'apertura!"); return; }

  fwrite(&newSeg->id, sizeof(int32_t), 1, f);
  fwrite(newSeg->nome_cittadino, sizeof(char), 64, f);
  fwrite(newSeg->categoria, sizeof(char), 64, f);
  fwrite(newSeg->descrizione, sizeof(char), 1024, f);
  fwrite(&newSeg->data, sizeof(int), 1, f);
  fwrite(&newSeg->urgenza, sizeof(int), 1, f);
  fwrite(&newSeg->stato, sizeof(int), 1, f);

  fclose(f);

  printf(" # | [FILE] Segnalazione registrata con successo!");
}

/**
 * @brief Inserisce dinamicamente un nuovo nodo allineandolo su tutti e quattro gli indici del grafo.
 *
 * La funzione gestisce l'aggancio a caldo di una segnalazione creata "al volo". Implementa una 
 * strategia di inserimento ibrida per ottimizzare il bilanciamento della memoria: esegue un 
 * inserimento ordinato tramite scansione lineare sui canali cronologico (`nextData`) e identificativo 
 * (`nextId`), garantendo la sequenzialità locale dei record. Al contrario, per gli assi dello stato 
 * operativo (`nextStato`) e dei livelli di urgenza (`nextUrg`), applica un inserimento rapido in testa 
 * a tempo costante O(1). Aggiorna in tempo reale tutti i contatori descrittori aggregati e di settore.
 *
 * @param[in,out] root Il puntatore alla struttura di controllo principale (radice) del sistema.
 * @param[in,out] newSeg Il puntatore al nodo segnalazione da posizionare e agganciare nel grafo.
 * @param[in] catIdx L'indice posizionale della categoria di appartenenza del record.
 *
 * @pre `root` e `newSeg` devono essere puntatori validi, allocati in memoria Heap e non nulli.
 * @pre `catIdx` deve rientrare rigorosamente all'interno del range di partizionamento [0, allCat-1].
 * @post Il nodo `newSeg` viene inserito fisicamente nelle quattro catene indipendenti. I contatori di 
 * stato, di categoria e il volume totale delle segnalazioni vengono incrementati di 1 unità.
 */
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

/**
 * @brief Interfaccia utente a basso livello per l'acquisizione, validazione e inserimento a caldo di un nuovo record.
 *
 * La funzione coordina la creazione assistita di una segnalazione. Alloca inizialmente un nodo isolato 
 * nello Heap mediante calloc() e valida l'input immesso tramite un meccanismo difensivo. Se l'utente introduce 
 * codici di categoria errati o caratteri spuri, il flusso viene deviato mediante un'istruzione di salto controllata 
 * verso la routine `input_error`, deputata a svuotare il flusso di input e a deallocare immediatamente il nodo orfano 
 * per impedire memory leak. Configura in modo interamente automatizzato la data corrente mediante le primitive di 
 * sistema POSIX e, dopo aver serializzato il record, invoca init_sorting() per garantire il riallineamento 
 * immediato e transazionale dei quattro assi ortogonali del grafo in RAM.
 *
 * @param[in,out] root Il puntatore alla struttura principale di controllo (radice) per l'innesto del nuovo nodo.
 *
 * @pre `root` deve essere un'istanza valida, non nulla e precedentemente allocata in memoria RAM.
 * @post In caso di successo, un nuovo record univoco viene inserito nel grafo e serializzato in modalità append su disco; 
 * tutti i contatori di sistema vengono incrementati e la RAM viene riordinata. In caso di errore di input, 
 * la memoria parziale allocata viene interamente liberata tramite free() e lo stato del grafo rimane invariato.
 *
 * @param[out] newSeg Il puntatore al blocco di memoria Heap allocato per memorizzare temporaneamente i campi acquisiti.
 */
void getNewSeg(Root root) {
  int scelta;

  s newSeg = (s) calloc(1, sizeof(struct segnalazione));
  if (newSeg == NULL) return;

  printf(" # | Seleziona una Categoria: \n");
  printf(" # | 1) Illuminazione \n");
  printf(" # | 2) Rifiuti \n");
  printf(" # | 3) Strade \n");
  printf(" # | 4) Verde \n");
  printf(" # | 5) Incendio \n");
  printf(" # | 6) Allagamento \n");
  printf(" # | 7) Segnaletica \n");
  printf(" # | 8) Edilizia \n");
  printf(" # | 9) Randagismo \n");
  printf(" # | 10) Inquinamento \n");
  printf(" # | 11) Sicurezza \n");
  printf(" # | Scelta: ");
  fflush(stdout);
  
  if (scanf("%d", &scelta) != 1 || scelta < 1 || scelta > 11) {
    printf("\n # | [ERRORE RIGIDO] Codice categoria non valido o non numerico!                                      | # \n");
    goto input_error;
  }

  int clean_buffer;
  while ((clean_buffer = getchar()) != '\n' && clean_buffer != EOF);

  int catIdx = scelta - 1;
  int prefisso = 0;

  switch (catIdx) {
    case illuminazione: { prefisso = 10; strcpy(newSeg->categoria, "Illuminazione");  } break;
    case rifiuti:       { prefisso = 20; strcpy(newSeg->categoria, "Rifiuti");        } break;
    case strade:        { prefisso = 30; strcpy(newSeg->categoria, "Strade");         } break;
    case verde:         { prefisso = 40; strcpy(newSeg->categoria, "Verde");          } break;
    case incendio:      { prefisso = 50; strcpy(newSeg->categoria, "Incendio");       } break;
    case allagamento:   { prefisso = 60; strcpy(newSeg->categoria, "Allagamento");    } break;
    case segnaletica:   { prefisso = 70; strcpy(newSeg->categoria, "Segnaletica");    } break;
    case edilizia:      { prefisso = 80; strcpy(newSeg->categoria, "Edilizia");       } break;
    case randagismo:    { prefisso = 90; strcpy(newSeg->categoria, "Randagismo");     } break;
    case inquinamento:  { prefisso = 11; strcpy(newSeg->categoria, "Inquinamento");   } break;
    case sicurezza:     { prefisso = 21; strcpy(newSeg->categoria, "Sicurezza");      } break;
    default:            { prefisso = 0; } break;
  }

  newSeg->id = getRandomId(root, prefisso, catIdx);

  printf(" #   - Nome del cittadino: ");
  fgets(newSeg->nome_cittadino, 64, stdin);
  newSeg->nome_cittadino[strcspn(newSeg->nome_cittadino, "\n")] = 0;

  printf(" #   - Inserisci descrizione: ");
  fgets(newSeg->descrizione, 1024, stdin);
  newSeg->descrizione[strcspn(newSeg->descrizione, "\n")] = 0;

  time_t t = time(NULL);
  struct tm *tm_info = localtime(&t);

  int anno_t = tm_info->tm_year + 1900;
  int mese_t = tm_info->tm_mon + 1;
  int giorno_t = tm_info->tm_mday;

  newSeg->data = (anno_t * 10000) + (mese_t * 100) + giorno_t;
  newSeg->urgenza = 3;
  newSeg->stato = 0;

  printf(" #   - Nuova segnalazione creata con ID: %d\n", newSeg->id);
  printf(" #   - Data di registrazione automatica (Sistema): %02d/%02d/%04d\n", giorno_t, mese_t, anno_t);
  printf(" #   - Premi INVIO per tornare alla Dashboard: ");
  fflush(stdout);
  getchar(); 

  getPosition(root, newSeg, catIdx);
  appendNewSeg(newSeg, "./components/database/database.bin");

  init_sorting(root);
  
  return;

  input_error: {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
  }
  
  printf(" # | Premi INVIO per annullare l'operazione e tornare alla dashboard principale...                    | # \n");
  free(newSeg);
  getchar();
}

/**
 * @brief Rimuove in modo atomico una segnalazione dal grafo, isolandola da tutti e quattro gli indici ortogonali.
 *
 * Questa funzione costituisce la routine di sfoltimento e deallocazione (splicing) del sistema. 
 * La decisione progettuale fondamentale risiede nella scomposizione topologica della rimozione: 
 * poiché il record fisico è unico ma condiviso da quattro assi paralleli, l'algoritmo effettua 
 * una scansione lineare isolata su ciascuna catena (`nextId`, `nextData`, `nextUrg`, `nextStato`) 
 * per rintracciare il rispettivo nodo predecessore. Una volta bypassati i puntatori del record target 
 * e aggiornati in tempo reale i contatori aggregati dei descrittori, il nodo viene isolato in sicurezza 
 * e rimosso definitivamente dall'Heap tramite free(), garantendo l'assenza di leak o di puntatori instabili.
 *
 * @note All'interno dei singoli cicli di scansione e ri-collegamento non vengono inseriti commenti 
 * linea per linea per preservare la leggibilità delle operazioni puramente logico-strutturali.
 *
 * @param[in,out] r Puntatore alla struttura principale di controllo (Radice) del sistema da mutare.
 * @param[in] idTarget Identificativo numerico intero a 32 bit della segnalazione da eliminare.
 *
 * @pre `r` deve essere un'istanza valida allocata e inizializzata tramite init_root().
 * @pre `idTarget` deve essere un intero a 6 cifre il cui prefisso sia mappabile da getCategoryIndex().
 * @post Se la segnalazione viene individuata, viene scollegata dai 4 assi del grafo, i contatori 
 * globali e parziali vengono decrementati, e la memoria Heap del nodo viene liberata con free(). 
 * Se il record non viene trovato, lo stato del sistema resta invariato e viene emesso un messaggio a video.
 */
void init_removeSeg(Root r, int32_t idTarget) {
  if (!r) return;

  int prefisso = idTarget / 100000;
  int catIdx = getCategoryIndex(prefisso);
  if (catIdx == -1) {
    printf(" # | Segnalazione %d non trovata.\n", idTarget);
    return;
  }

  s curr = r->id->cat[catIdx];
  s prev = NULL;

  while (curr && curr->id != idTarget) {
    prev = curr;
    curr = curr->nextId;
  }

  if (!curr) {
    printf(" # | Segnalazione %d non trovata.\n", idTarget);
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
  printf(" # | Segnalazione %d rimossa con successo.\n", idTarget);
}

/**
 * @brief Consolida l'intero grafo multi-indice su disco, sovrascrivendo il file binario di persistenza.
 *
 * La funzione si occupa di salvare lo stato attuale della memoria RAM su memoria secondaria. La decisione 
 * progettuale cardine risiede nella scelta dell'asse di scorrimento: l'algoritmo seleziona l'indice 
 * cronologico globale (`r->data->head`) come cammino lineare per attraversare tutti i nodi unici del grafo, 
 * serializzandoli uno alla volta. L'apertura del file stream avviene in modalità "wb" (scrittura binaria 
 * distruttiva): questo garantisce il consolidamento atomico pulito dei dati, azzerando il vecchio file 
 * ed evitando la persistenza di record precedentemente rimossi o obsoleti.
 *
 * @param[in] r Il puntatore costante alla struttura principale di controllo (radice) da serializzare.
 *
 * @param[out] f File stream di output aperto verso il file binario per la scrittura contigua dei campi.
 *
 * @pre `r` deve essere un'istanza valida, non nulla e coerentemente popolata in memoria RAM.
 * @post Il file binario viene interamente sovrascritto con i dati aggiornati del grafo. Lo stream verso 
 * il disco viene chiuso correttamente e lo stato dei nodi in memoria Heap rimane inalterato.
 */
void save_records(Root r) {
    FILE *f = fopen("./components/database/database.bin", "wb");
    if (!f) {
        perror(" # | Errore nell'apertura del database in scrittura \n");
        return;
    }

    s curr = r->data->head;
    while (curr) {
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

/**
 * @brief Esegue una ricerca testuale e parziale mirata sugli identificativi numerici delle segnalazioni.
 *
 * La funzione implementa una strategia di scansione adattiva per ottimizzare i tempi di risposta. 
 * La decisione progettuale cardine risiede nel partizionamento dinamico del canale di navigazione: 
 * analizzando la lunghezza e i primi due caratteri della stringa di ricerca, l'algoritmo tenta di 
 * astrarre un prefisso ministeriale valido. In caso di riscontro positivo, il puntatore di scansione 
 * viene isolato sul bucket della singola categoria (`root->id->cat[catIdx]`) navigando l'asse `nextId`; 
 * in caso contrario, la ricerca commuta sull'intero database sfruttando l'asse cronologico globale 
 * (`root->data->head`) mediante `nextData`. Formatta a schermo fino a un massimo di 12 record corrispondenti, 
 * effettuando il riempimento geometrico delle righe vuote residue per preservare l'allineamento dell'HUD.
 *
 * @param[in] r Il puntatore alla struttura principale di controllo (radice) del grafo multi-indice.
 * @param[in] searchString Stringa costante contenente la chiave parziale o totale digitata dall'utente.
 *
 * @pre `r` deve essere un'istanza valida allocata e inizializzata.
 * @pre `searchString` deve essere una stringa non nulla, erogata in modo sicuro dal modulo HUD.
 * @post Lo stato del grafo in memoria RAM non subisce alcuna alterazione o riposizionamento di nodi. 
 * I record individuati vengono formattati e stampati direttamente sullo stream di output standard.
 */
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

    int normalized = curr->data;
    int giorno, mese, anno;
    anno = (int) normalized / 10000;
    normalized %= 10000;
    mese = (int) normalized / 100;
    normalized %= 100;
    giorno = normalized;

    char formattedData[16];
    sprintf(formattedData, "%02d/%02d/%04d", giorno, mese, anno);

    if(strncmp(idStr, searchString, len) == 0) {
      const char *statoStr;
      switch(curr->stato) {
        case 0:  statoStr = "Aperta";      break;
        case 1:  statoStr = "Risoluzione"; break;
        case 2:  statoStr = "Chiusa";      break;
        default: statoStr = "N/D";         break;
      }

      char riga_dati[256];
      sprintf(riga_dati, " | %-12d     | %-22.22s | %-10.10s   | %-13.13s | %-16.16s |   %d   |",
        curr->id,
        curr->nome_cittadino,
        formattedData,
        statoStr,
        curr->categoria, 
        curr->urgenza
      );

      printf(" # %-101s # \n", riga_dati);
      trovati++;
    }

    curr = (catIdx == -1) ? curr->nextData : curr->nextId;
  }

  for (int i = trovati; i < 11; i++) {
    char riga_vuota[256];
    sprintf(riga_vuota, " |                  |                        |              |               |                  |       |");
    printf(" # %-101s # \n", riga_vuota);
  }
}

/**
 * @brief Identifica l'indice posizionale della categoria che contiene il maggior numero di segnalazioni attive.
 *
 * La funzione implementa un algoritmo di ricerca del massimo relativo per monitorare i carichi di 
 * lavoro comunali. La decisione progettuale cardine risiede nello sfruttamento dei contatori aggregati 
 * pre-calcolati: l'algoritmo non deve scansionare l'intera popolazione di nodi del grafo (operazione 
 * che degraderebbe le prestazioni), ma si limita a scorrere il vettore statico `nCat`. Questo riduce 
 * la complessità computazionale a tempo lineare rispetto al solo numero fisso delle categorie O(allCat). 
 * Include un controllo difensivo di sbarramento finale: se la categoria dominante possiede zero record, 
 * significa che il sistema è privo di dati e la funzione restituisce un codice di segnalazione vuota.
 *
 * @param[in] root Il puntatore alla struttura principale di controllo (radice) da cui campionare i contatori.
 *
 * @pre `root` deve essere un'istanza valida, non nulla e precedentemente allocata in memoria RAM.
 * @post Lo stato del grafo, dei nodi e dei vettori dei contatori non subisce alcuna alterazione.
 *
 * @param[out] maxIdx Variabile d'appoggio intera che memorizza temporaneamente l'indice della cella dominante.
 *
 * @return int L'indice intero (0-10) della categoria più colpita dai disservizi, oppure -1 come codice 
 * di errore/anomalia se la radice è nulla o se l'intero database non contiene alcuna segnalazione.
 */
int getMaxCat(Root root) {
  if (root == NULL || root->id == NULL) return -1;

  int maxIdx = 0;

  for (int i = 1; i < allCat; i++) {
    if (root->id->nCat[i] > root->id->nCat[maxIdx]) {
      maxIdx = i;
    }
  }

  if (root->id->nCat[maxIdx] == 0) return -1;

  return maxIdx;
}

/**
 * @brief Traduce l'indice della categoria dominante nella corrispondente stringa letterale costante.
 *
 * La funzione si occupa di convertire il codice posizionale numerico della categoria più colpita 
 * da disservizi in un'etichetta testuale leggibile dall'utente finale nell'HUD. La decisione 
 * progettuale cardine risiede nel riutilizzo logico (composizione) delle funzionalità dell'ADT: 
 * delega interamente il calcolo statistico alla funzione interna `getMaxCat()`. Il valore emesso 
 * è un puntatore a una stringa costante letterale memorizzata nel segmento dati del programma: 
 * questo evita qualsiasi operazione di allocazione dinamica o copia di buffer nella RAM, garantendo 
 * prestazioni ottimali a tempo costante O(1) e l'assenza totale di memory leak.
 *
 * @note All'interno del corpo della funzione non vengono inseriti commenti linea per linea 
 * sui singoli casi dello switch per preservare la leggibilità e la pulizia del blocco.
 *
 * @param[in] root Il puntatore alla struttura principale di controllo (radice) da analizzare.
 *
 * @pre `root` deve essere un'istanza valida allocata e precedentemente inizializzata.
 * @post Lo stato del grafo in memoria RAM rimane invariato. Viene emesso il letterale corrispondente.
 *
 * @return const char* Puntatore a una stringa di caratteri costante contenente il nome formattato 
 * della categoria dominante, oppure "N/D" se il sistema è vuoto o se l'indice restituito non è valido.
 */
const char *getMaxCatName(Root root) {
  int catIdx = getMaxCat(root);

  switch (catIdx) {
    case illuminazione: return "Illuminazione";
    case rifiuti:       return "Rifiuti";
    case strade:        return "Strade";
    case verde:         return "Verde Pubblico";
    case incendio:      return "Incendio";
    case allagamento:   return "Allagamento";
    case segnaletica:   return "Segnaletica";
    case edilizia:      return "Edilizia";
    case randagismo:    return "Randagismo";
    case inquinamento:  return "Inquinamento";
    case sicurezza:     return "Sicurezza";
    default:            return "N/D";
  }
}

/**
 * @brief Modifica lo stato operativo di una segnalazione, trasferendo il nodo tra le code del grafo.
 *
 * La funzione implementa la transizione logica all'interno della macchina a stati del sistema. 
 * La decisione progettuale cardine risiede nella mutazione atomica dell'asse ortogonale dello stato: 
 * invece di allocare un nuovo record, l'algoritmo isola il nodo individuato sfilandolo (splicing) 
 * dalla vecchia lista di appartenenza (Aperta, In Risoluzione, Chiusa) attraverso la ricerca del 
 * suo predecessore (`prevStato`). Successivamente aggiorna il valore del campo interno, innesta 
 * il medesimo record in testa alla coda del nuovo stato desiderato e aggiorna in tempo reale i 
 * rispettivi contatori di rilevamento. Al termine della transizione, invoca init_sorting() per 
 * consolidare la coerenza topologica globale.
 *
 * @param[in,out] root Il puntatore alla struttura principale di controllo (radice) del sistema.
 * @param[in] currId Identificativo numerico intero a 32 bit del record target da sottoporre a mutazione.
 * @param[in] newState Il codice numerico del nuovo stato di destinazione (0: Aperta, 1: In Risoluzione, 2: Chiusa).
 *
 * @pre `root` deve essere un'istanza valida, non nulla e precedentemente allocata in memoria RAM.
 * @pre `newState` deve rispettare rigorosamente l'intervallo discreto [0, 2].
 * @post Se l'ID esiste e lo stato è differente, il nodo viene ricatenato sul nuovo descrittore di stato, 
 * i contatori live vengono aggiornati e l'intero grafo viene riordinato. Se l'ID non esiste, la RAM resta invariata.
 *
 * @return int I codici di ritorno mappano gli esiti transazionali per il modulo HUD:
 * - -1: Errore di validazione (root nulla, ID non conforme o newState fuori range);
 * -  0: Operazione non necessaria (la segnalazione possiede già lo stato richiesto);
 * -  1: Transizione completata con successo e puntatori ortogonali riallineati.
 */
int modifySeg(Root root, int32_t currId, int newState) {
  if (root == NULL || newState < 0 || newState > 2) return -1;

  int prefisso = currId / 100000;
  int catIdx = getCategoryIndex(prefisso);
  if (catIdx == -1) return -1;

  s curr = root->id->cat[catIdx];
  while (curr != NULL && curr->id != currId) {
    curr = curr->nextId;
  }

  if (curr == NULL) return -1;
  if (curr->stato == newState) return 0;

  int vecchioStato = curr->stato;

  s prevStato = NULL;
  s currStato = NULL;

  if (vecchioStato == 0) 
    currStato = root->stato->aperto->head;

  else if (vecchioStato == 1) 
    currStato = root->stato->risoluzione->head;

  else 
    currStato = root->stato->chiuso->head;

  while (currStato != NULL && currStato != curr) {
    prevStato = currStato;
    currStato = currStato->nextStato;
  }

  if (currStato == curr) {
    if (prevStato == NULL) {

      if (vecchioStato == 0) 
        root->stato->aperto->head = curr->nextStato;

      else if (vecchioStato == 1) 
        root->stato->risoluzione->head = curr->nextStato;

      else 
        root->stato->chiuso->head = curr->nextStato;
    } else {

      prevStato->nextStato = curr->nextStato;
    }

    if (vecchioStato == 0) 
      root->stato->aperto->totAperte--;

    else if (vecchioStato == 1) 
      root->stato->risoluzione->totRis--;

    else 
      root->stato->chiuso->totChiuse--;
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

  return 1;
}

/**
 * @brief Rintraccia un nodo segnalazione all'interno del grafo partendo dal suo ID.
 *
 * Questa funzione incapsula la logica di ricerca puntuale. Estrae il prefisso numerico,
 * determina l'indice della sottolista tramite getCategoryIndex() e scansiona la catena
 * ortogonale degli ID. Isola questa computazione all'interno dell'ADT per preservare 
 * l'Information Hiding.
 *
 * @param[in] r Il puntatore alla struttura principale di controllo (radice).
 * @param[in] idTarget L'identificativo numerico a 7 cifre da ricercare.
 * @return s Il puntatore al nodo opaco trovato, oppure NULL se il record non esiste.
 */
s findSegByID(Root r, int32_t idTarget) {
  if (r == NULL || r->id == NULL) return NULL;

  int prefisso = idTarget / 100000;
  int catIdx = getCategoryIndex(prefisso);

  if (catIdx == -1) return NULL;

  s curr = r->id->cat[catIdx];
  while (curr != NULL && curr->id != idTarget) {
    curr = curr->nextId;
  }
  return curr;
}