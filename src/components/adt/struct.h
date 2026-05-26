#ifndef STRUCT_H
#define STRUCT_H

#include <stdint.h>

/**
 * @file struct.h
 * @brief Interfaccia pubblica dell'ADT opaco per la gestione multi-indice
 *        delle segnalazioni comunali.
 *
 * Il modulo implementa un sistema basato su strutture opache, in cui ogni
 * segnalazione viene mantenuta una sola volta in memoria e collegata tramite
 * piu' indici logici:
 *
 * - indice per categoria/ID;
 * - indice cronologico per data;
 * - indice per stato;
 * - indice per urgenza.
 *
 * Le strutture interne non sono visibili all'esterno del modulo, in modo da
 * garantire Information Hiding e separazione tra interfaccia e implementazione.
 */


/* =========================================================================
   STRUTTURE OPACHE
   ========================================================================= */

/**
 * @brief Struttura opaca principale del sistema.
 *
 * La definizione completa di struct root si trova nel file struct.c.
 */
struct root;

/**
 * @brief Struttura opaca della singola segnalazione.
 *
 * La definizione completa di struct segnalazione si trova nel file struct.c.
 */
struct segnalazione;

/**
 * @typedef Root
 * @brief Puntatore alla struttura principale di controllo del sistema.
 *
 * Rappresenta la radice del grafo multi-indice delle segnalazioni.
 */
typedef struct root *Root;

/**
 * @typedef s
 * @brief Puntatore a un nodo segnalazione.
 *
 * Ogni nodo rappresenta una singola segnalazione comunale.
 */
typedef struct segnalazione *s;


/* =========================================================================
   FUNZIONI DI INIZIALIZZAZIONE E GESTIONE CORE
   ========================================================================= */

/**
 * @brief Inizializza la struttura principale del sistema.
 *
 * Alloca dinamicamente la radice del sistema e i sotto-indici necessari
 * alla gestione delle segnalazioni.
 *
 * @pre Nessuna.
 *
 * @post Se l'allocazione principale ha successo, viene restituito un puntatore
 *       Root utilizzabile dal resto del programma.
 *
 * @return Puntatore alla struttura Root allocata dinamicamente,
 *         oppure NULL se l'allocazione principale fallisce.
 *
 * @note L'implementazione attuale inizializza le strutture tramite calloc,
 *       quindi i campi vengono posti inizialmente a zero/NULL.
 */
Root init_root(void);

/**
 * @brief Carica le segnalazioni da un file binario e le inserisce nel sistema.
 *
 * Legge sequenzialmente i record dal file binario indicato e, per ogni record
 * valido, crea una nuova segnalazione inserendola nei vari indici interni.
 *
 * @pre r dovrebbe essere un puntatore Root valido e inizializzato.
 * @pre fileName dovrebbe essere una stringa valida contenente il percorso
 *      del file binario.
 *
 * @post Le segnalazioni lette correttamente vengono inserite nel sistema.
 *       In caso di errore di apertura del file, la funzione termina senza
 *       modificare il sistema.
 *
 * @param r Sistema Root in cui caricare i dati.
 * @param fileName Percorso del file binario da leggere.
 */
void init_loadingDb(Root, const char *);

/**
 * @brief Ricostruisce e ordina gli indici principali del sistema.
 *
 * La funzione costruisce un array temporaneo di puntatori alle segnalazioni,
 * lo ordina con Quicksort secondo criteri diversi e ricostruisce le liste
 * concatenate usate dagli indici interni.
 *
 * @pre r dovrebbe essere un puntatore Root valido.
 *
 * @post L'indice cronologico viene ordinato per data crescente.
 * @post L'indice per categoria/ID viene ricostruito in ordine crescente di ID.
 * @post Gli indici per stato e urgenza vengono ricostruiti mantenendo le
 *       rispettive liste di appartenenza.
 *
 * @param r Sistema Root da ordinare.
 *
 * @note Se r e' NULL oppure non contiene segnalazioni, la funzione termina
 *       senza effettuare operazioni.
 */
void init_sorting(Root);

/**
 * @brief Rimuove una segnalazione dal sistema tramite il suo ID.
 *
 * Cerca la segnalazione corrispondente all'ID fornito e, se presente,
 * la rimuove da tutti gli indici interni prima di deallocarla.
 *
 * @pre r dovrebbe essere un puntatore Root valido.
 * @pre idTarget dovrebbe rappresentare un ID esistente o potenzialmente
 *      esistente nel sistema.
 *
 * @post Se la segnalazione viene trovata, viene rimossa dagli indici
 *       categoria/ID, data, stato e urgenza.
 * @post I contatori associati vengono decrementati.
 * @post Il nodo rimosso viene deallocato.
 *
 * @param r Sistema Root da cui rimuovere la segnalazione.
 * @param idTarget ID della segnalazione da eliminare.
 *
 * @note Se l'ID non viene trovato, la funzione stampa un messaggio e non
 *       modifica il sistema.
 */
void init_removeSeg(Root, int32_t);


/* =========================================================================
   FUNZIONI GETTER GLOBALI PER DASHBOARD
   ========================================================================= */

/**
 * @brief Restituisce il numero totale di segnalazioni presenti nel sistema.
 *
 * @param root Sistema Root da interrogare.
 *
 * @return Numero totale di segnalazioni.
 * @return 0 se root e' NULL.
 */
int getTotalSeg(Root);

/**
 * @brief Restituisce il numero totale di segnalazioni in stato "Aperta".
 *
 * @param root Sistema Root da interrogare.
 *
 * @return Numero di segnalazioni aperte.
 * @return 0 se root e' NULL.
 */
int getTotalAperte(Root);

/**
 * @brief Restituisce il numero totale di segnalazioni in stato "Risoluzione".
 *
 * @param root Sistema Root da interrogare.
 *
 * @return Numero di segnalazioni in risoluzione.
 * @return 0 se root e' NULL.
 */
int getTotalRis(Root);

/**
 * @brief Restituisce il numero totale di segnalazioni in stato "Chiusa".
 *
 * @param root Sistema Root da interrogare.
 *
 * @return Numero di segnalazioni chiuse.
 * @return 0 se root e' NULL.
 */
int getTotalChiuse(Root);

/**
 * @brief Restituisce il numero di segnalazioni con urgenza massima.
 *
 * L'urgenza massima corrisponde al livello 5.
 *
 * @param root Sistema Root da interrogare.
 *
 * @return Numero di segnalazioni con urgenza pari a 5.
 * @return 0 se root e' NULL.
 */
int getMostUrgenti(Root);

/**
 * @brief Restituisce la testa dell'indice cronologico.
 *
 * @param root Sistema Root da interrogare.
 *
 * @return Puntatore alla prima segnalazione nell'indice per data.
 * @return NULL se root e' NULL oppure se l'indice e' vuoto.
 */
s getDataHead(Root);

/**
 * @brief Restituisce il numero di segnalazioni associate a una categoria.
 *
 * @param root Sistema Root da interrogare.
 * @param catIdx Indice numerico della categoria.
 *
 * @return Numero di segnalazioni nella categoria indicata.
 * @return 0 se root e' NULL, se l'indice delle categorie non esiste
 *         o se catIdx non e' valido.
 *
 * @note L'indice catIdx deve corrispondere all'ordine interno delle categorie
 *       definito in struct.c.
 */
int getSegCountByCategory(Root, int);


/* =========================================================================
   FUNZIONI GETTER PER SINGOLA SEGNALAZIONE
   ========================================================================= */

/**
 * @brief Restituisce l'ID della segnalazione.
 *
 * @param node Nodo segnalazione da interrogare.
 *
 * @return ID della segnalazione.
 * @return 0 se node e' NULL.
 */
int32_t getID(s);

/**
 * @brief Restituisce il nome del cittadino associato alla segnalazione.
 *
 * @param node Nodo segnalazione da interrogare.
 *
 * @return Puntatore a stringa costante contenente il nome del cittadino.
 * @return "Not Found" se node e' NULL.
 */
const char *getName(s);

/**
 * @brief Restituisce la descrizione testuale della segnalazione.
 *
 * @param node Nodo segnalazione da interrogare.
 *
 * @return Puntatore a stringa costante contenente la descrizione.
 * @return "Not found" se node e' NULL.
 */
const char *getDescription(s);

/**
 * @brief Restituisce il nome testuale della categoria.
 *
 * @param node Nodo segnalazione da interrogare.
 *
 * @return Puntatore a stringa costante contenente la categoria.
 * @return "Not found" se node e' NULL.
 */
const char *getCat(s);

/**
 * @brief Restituisce la data nel formato intero raw AAAAMMGG.
 *
 * @param node Nodo segnalazione da interrogare.
 *
 * @return Data in formato AAAAMMGG.
 * @return 0 se node e' NULL.
 */
int getRawData(s);

/**
 * @brief Restituisce il livello di urgenza della segnalazione.
 *
 * @param node Nodo segnalazione da interrogare.
 *
 * @return Livello di urgenza, normalmente compreso tra 1 e 5.
 * @return 0 se node e' NULL.
 */
int getUrg(s);

/**
 * @brief Restituisce lo stato numerico della segnalazione.
 *
 * Gli stati usati dal sistema sono:
 *
 * - 0: Aperta;
 * - 1: In risoluzione;
 * - 2: Chiusa.
 *
 * @param node Nodo segnalazione da interrogare.
 *
 * @return Codice numerico dello stato.
 * @return 0 se node e' NULL.
 */
int getState(s);

/**
 * @brief Genera una stringa formattata della data.
 *
 * Converte la data interna dal formato intero AAAAMMGG al formato testuale
 * GG/MM/AAAA.
 *
 * @param node Nodo segnalazione da interrogare.
 *
 * @return Puntatore a una stringa allocata dinamicamente contenente la data
 *         formattata.
 *
 * @note La memoria restituita viene allocata con malloc e deve essere liberata
 *       dal chiamante tramite free().
 *
 * @warning Se node e' NULL, l'implementazione attuale genera una data basata
 *          sul valore raw 0, quindi "00/00/0000".
 * @warning L'implementazione attuale non controlla esplicitamente il fallimento
 *          della malloc.
 */
char *getData(s);

/**
 * @brief Identifica la categoria con il maggior numero totale di segnalazioni.
 *
 * Scansiona i contatori interni delle categorie e individua quella con il
 * valore piu' alto.
 *
 * @param root Sistema Root da interrogare.
 *
 * @return Indice numerico della categoria piu' frequente.
 * @return -1 se root e' NULL, se l'indice delle categorie non esiste
 *         oppure se non sono presenti segnalazioni.
 *
 * @note Il valore restituito corrisponde all'indice interno della categoria
 *       definito in struct.c.
 */
int getMaxCat(Root);

/**
 * @brief Restituisce il nome della categoria con piu' segnalazioni totali.
 *
 * Invoca internamente getMaxCat() e converte l'indice ottenuto nella relativa
 * stringa descrittiva.
 *
 * @param root Sistema Root da interrogare.
 *
 * @return Nome testuale della categoria piu' frequente.
 * @return "N/D" se il sistema e' vuoto o se non e' possibile determinare
 *         una categoria valida.
 */
const char *getMaxCatName(Root);


/* =========================================================================
   FUNZIONI DI NAVIGAZIONE DEGLI INDICI
   ========================================================================= */

/**
 * @brief Restituisce il nodo successivo nell'indice per ID della stessa categoria.
 *
 * @param node Nodo corrente.
 *
 * @return Nodo successivo nella lista per ID.
 * @return NULL se node e' NULL oppure se non esiste un nodo successivo.
 */
s nextForID(s);

/**
 * @brief Restituisce il nodo successivo nell'indice cronologico.
 *
 * @param node Nodo corrente.
 *
 * @return Nodo successivo nella lista ordinata per data.
 * @return NULL se node e' NULL oppure se non esiste un nodo successivo.
 */
s nextForData(s);

/**
 * @brief Restituisce il nodo successivo nella stessa lista di urgenza.
 *
 * @param node Nodo corrente.
 *
 * @return Nodo successivo nella lista associata allo stesso livello di urgenza.
 * @return NULL se node e' NULL oppure se non esiste un nodo successivo.
 *
 * @note Questa funzione non attraversa automaticamente tutte le urgenze da 5 a 1,
 *       ma segue semplicemente il collegamento nextUrg del nodo corrente.
 */
s nextForUrg(s);


/* =========================================================================
   FUNZIONI DI INSERIMENTO, SALVATAGGIO, RICERCA E DISTRUZIONE
   ========================================================================= */

/**
 * @brief Avvia la procedura interattiva di acquisizione di una nuova segnalazione.
 *
 * La funzione legge da stdin la categoria, il nome del cittadino e la descrizione.
 * Genera automaticamente:
 *
 * - ID univoco basato sulla categoria;
 * - data corrente di sistema;
 * - urgenza iniziale pari a 3;
 * - stato iniziale pari a 0, cioe' "Aperta".
 *
 * @param root Sistema Root in cui inserire la nuova segnalazione.
 *
 * @post Se l'inserimento va a buon fine, la segnalazione viene aggiunta agli
 *       indici interni e scritta in append nel file database.bin.
 *
 * @note La funzione e' pensata per uso interattivo da terminale.
 */
void getNewSeg(Root);

/**
 * @brief Salva tutte le segnalazioni correnti sovrascrivendo il file binario.
 *
 * La funzione apre il database in modalita' "wb" e riscrive tutti i record
 * seguendo l'ordine dell'indice cronologico corrente.
 *
 * @param r Sistema Root da salvare.
 *
 * @post Il file database.bin viene riscritto con le segnalazioni attualmente
 *       presenti in memoria.
 */
void save_records(Root);

/**
 * @brief Dealloca l'intero sistema multi-indice.
 *
 * La funzione percorre l'indice cronologico, libera ogni singola segnalazione
 * una sola volta e poi dealloca le strutture di controllo interne.
 *
 * @param root Sistema Root da distruggere.
 *
 * @post La memoria heap associata al sistema viene liberata.
 *
 * @note Dopo la chiamata, il puntatore root posseduto dal chiamante diventa
 *       non piu' valido e non deve essere riutilizzato.
 */
void deleteGraph(Root);

/**
 * @brief Esegue una ricerca testuale/parziale sulle segnalazioni.
 *
 * La funzione confronta la stringa cercata con il prefisso dell'ID delle
 * segnalazioni e stampa a schermo i record trovati.
 *
 * @param r Sistema Root in cui cercare.
 * @param searchString Stringa parziale digitata dall'utente.
 *
 * @note Se la stringa inizia con un prefisso categoria valido, la ricerca viene
 *       limitata alla lista della categoria corrispondente; altrimenti viene
 *       effettuata sull'indice cronologico globale.
 * @note La funzione stampa direttamente i risultati e non restituisce una lista.
 */
void search_seg(Root, const char *);

/**
 * @brief Modifica lo stato operativo di una segnalazione.
 *
 * Cerca una segnalazione tramite ID e, se trovata, aggiorna il suo stato
 * spostandola dalla lista del vecchio stato alla lista del nuovo stato.
 *
 * @param root Sistema Root contenente la segnalazione.
 * @param currId ID della segnalazione da modificare.
 * @param newState Nuovo stato:
 *                 - 0: Aperta;
 *                 - 1: In risoluzione;
 *                 - 2: Chiusa.
 *
 * @return -1 se root e' NULL, se newState non e' valido o se l'ID non esiste.
 * @return 0 se la segnalazione esiste ma possiede gia' lo stato richiesto.
 * @return 1 se la modifica viene completata con successo.
 */
int modifySeg(Root, int32_t, int);

/**
 * @brief Rintraccia un nodo segnalazione all'interno del grafo partendo dal suo ID.
 * @param Il puntatore alla struttura principale di controllo (radice).
 * @param L'identificativo numerico a 7 cifre da ricercare.
 * @return `s` Il puntatore al nodo opaco trovato, oppure NULL se il record non esiste.
 */
s findSegByID(Root, int32_t);

#endif