#ifndef STRUCT_H
#define STRUCT_H

#include <stdint.h>

/**
 * @file struct.h
 * @brief Definizione dell'ADT opaco per la gestione multi-indice delle segnalazioni comunali.
 * * Il modulo implementa un sistema ortogonale a quattro dimensioni (ID, Data, Stato, Urgenza)
 * per garantire accessi, inserimenti e ricerche in tempi ottimali.
 */

/* Strutture opache per l'Information Hiding */
struct root;
struct segnalazione;

/** * @typedef Root
 * @brief Puntatore alla struttura di controllo principale (radice del sistema).
 */
typedef struct root *Root;

/** * @typedef s
 * @brief Puntatore alla struttura dati del singolo nodo segnalazione.
 */
typedef struct segnalazione *s;


/* =========================================================================
   FUNZIONI DI INIZIALIZZAZIONE E GESTIONE CORE
   ========================================================================= */

/**
 * @brief Inizializza la struttura di controllo principale.
 * @pre Nessuna.
 * @post Restituisce un puntatore Root valido con tutti i sotto-indici azzerati.
 * @return Root allocata sull'Heap, oppure NULL in caso di fallimento della calloc.
 */
Root init_root();

/**
 * @brief Carica le segnalazioni da un file binario e le inserisce nel sistema.
 * @pre r deve essere un puntatore Root valido e inizializzato. fileName != NULL.
 * @post Il sistema viene popolato con i record validi estratti dal file binario.
 * @param r Il sistema Root in cui caricare i dati.
 * @param fileName Percorso del file binario (.bin) da leggere.
 */
void init_loadingDb(Root, const char *);

/**
 * @brief Esegue l'ordinamento multi-indice globale tramite algoritmo Quicksort.
 * @pre r deve essere un puntatore Root valido e non vuoto.
 * @post Tutti i sotto-indici concatenati (Data, ID, Stato, Urgenza) risultano ordinati.
 * @param r Il sistema Root da ordinare.
 */
void init_sorting(Root);

/**
 * @brief Rimuove una segnalazione dal sistema tramite il suo ID.
 * @pre r deve essere valido. idTarget deve essere un identificativo a 32-bit valido.
 * @post Se l'ID esiste, il nodo viene rimosso da tutti gli indici, deallocato e i contatori decrementati.
 * @param r Il sistema Root da cui rimuovere il record.
 * @param idTarget L'ID a 32-bit della segnalazione da eliminare.
 */
void init_removeSeg(Root, int32_t);


/* =========================================================================
   FUNZIONI GETTER GLOBALI (DASHBOARD)
   ========================================================================= */

/**
 * @brief Restituisce il numero totale di segnalazioni nel comune.
 * @pre root può essere anche NULL.
 * @post Restituisce il contatore globale. Se root è NULL restituisce 0.
 */
int getTotalSeg(Root);

/**
 * @brief Restituisce il numero totale di segnalazioni in stato Aperta.
 */
int getTotalAperte(Root);

/**
 * @brief Restituisce il numero totale di segnalazioni in stato Risoluzione.
 */
int getTotalRis(Root);

/**
 * @brief Restituisce il numero totale di segnalazioni in stato Chiusa.
 */
int getTotalChiuse(Root);

/**
 * @brief Restituisce il numero totale di segnalazioni a massima urgenza (Livello 5).
 */
int getMostUrgenti(Root);

/**
 * @brief Restituisce la testa dell'indice cronologico (Data).
 * @return Il puntatore alla prima segnalazione per data, NULL se vuoto.
 */
s getDataHead(Root);

/**
 * @brief Restituisce il numero di segnalazioni associate a una specifica categoria.
 * @pre root valido. catIdx compreso tra 0 e allCat-1.
 * @param catIdx L'indice numerico della categoria da campionare.
 */
int getSegCountByCategory(Root, int);


/* =========================================================================
   FUNZIONI GETTER PER SINGOLA SEGNALAZIONE (ELEMENT)
   ========================================================================= */

/**
 * @brief Estrae l'ID a 32-bit dal nodo segnalazione.
 * @pre node != NULL.
 */
int32_t getID(s);

/**
 * @brief Restituisce il nome del cittadino associato alla segnalazione.
 * @return Stringa costante (const char*), "Not Found" se il nodo è NULL.
 */
const char *getName(s);

/**
 * @brief Restituisce la descrizione testuale del problema.
 */
const char *getDesc(s);

/**
 * @brief Restituisce il nome testuale della categoria.
 */
const char *getCat(s);

/**
 * @brief Identifica la categoria del comune che registra l'affluenza massima di problemi.
 * @pre root deve essere un puntatore Root valido, non vuoto e inizializzato.
 * @post Scansiona l'indice di categoria calcolando il picco massimo di segnalazioni.
 * @param root Il sistema di controllo principale.
 * @return L'indice numerico (catId) della categoria più colpita, -1 se il sistema è vuoto o invalido.
 */
int getMaxCat(Root);

/**
 * @brief Converte l'indice enumerato di una categoria nella sua rispettiva stringa letterale.
 * @pre idx deve essere un valore coerente con l'intervallo dell'enum catId (da 0 a allCat-1).
 * @post Associa l'indice alla stringa testuale statica corrispondente.
 * @param idx L'indice numerico della categoria da convertire.
 * @return Stringa costante (const char*) contenente il nome della categoria (es. "Illuminazione").
 */
const char *getMaxCatName(int);

/**
 * @brief Restituisce la data nel formato raw intero (AAAAMMGG).
 */
int getRawData(s);

/**
 * @brief Restituisce il livello di urgenza (da 1 a 5).
 */
int getUrg(s);

/**
 * @brief Restituisce lo stato numerico della segnalazione (0: Aperta, 1: Risoluzione, 2: Chiusa).
 */
int getState(s);

/**
 * @brief Genera una stringa formattata della data (GG/MM/AAAA).
 * @pre node != NULL.
 * @post Alloca dinamicamente 16 byte sulla memoria Heap contenenti la stringa.
 * @note CRITICO: La memoria restituita DEVE essere liberata dal chiamante tramite free().
 * @return Puntatore alla stringa allocata.
 */
char *getData(s);


/**
 * =========================================================================
 * FUNZIONI DI NAVIGAZIONE DEGLI INDICI (ITERATORI)
 * ========================================================================= 
*/

/**
 * @brief Restituisce il nodo successivo ordinato per ID all'interno della stessa categoria.
 */
s nextForID(s);

/**
 * @brief Restituisce il nodo successivo in ordine cronologico globale (Data).
 */
s nextForData(s);

/**
 * @brief Restituisce il nodo successivo in ordine di urgenza.
 */
s nextForUrg(s);

/** 
 * =========================================================================
 * FUNZIONI DI INTERFACCIA E DISTRUZIONE
 * ========================================================================= 
 */

/**
 * @brief Avvia la procedura interattiva di acquisizione e inserimento di una nuova segnalazione.
 * @post Acquisisce i dati da stdin, genera un ID univoco, inserisce il nodo e lo scrive in append su disco.
 */
void getNewSeg(Root);

/**
 * @brief Salva l'intero stato del database sovrascrivendo il file binario.
 * @post Il file viene riscritto in modalità "wb" basandosi sull'ordine cronologico corrente.
 */
void save_records(Root);

/**
 * @brief Distrugge l'intero grafo multi-indice deallocando tutta la memoria Heap.
 * @pre r può essere anche NULL.
 * @post Tutta la memoria associata a nodi, sotto-nodi e radice viene azzerata. Il sistema è spento.
 */
void deleteGraph(Root);

/**
 * @brief Esegue la ricerca live stampando a schermo i record che matchano la query.
 * @param searchString Stringa parziale digitata dall'utente nella barra di ricerca.
 */
void search_seg(Root, const char *);

/**
 * @brief Modifica lo stato operativo di una segnalazione esistente aggiornando i flussi ortogonali.
 * @pre root deve essere valido. currId deve essere presente nel sistema. newState deve essere compreso tra 0 e 2.
 * @post Individua il nodo in O(1) tramite prefisso, lo sfila chirurgicamente dal vecchio binario di stato,
 * aggiorna i contatori descrittivi di tracciamento e lo reinserisce in testa alla nuova lista di stato.
 * @param root Il sistema di controllo principale.
 * @param currId L'identificativo unico della segnalazione da alterare.
 * @param newState Il codice numerico del nuovo stato (0: Aperta, 1: Risoluzione, 2: Chiusa).
 */
void modifySeg(Root, int32_t, int);

#endif