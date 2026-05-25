#ifndef HUD_H
#define HUD_H

#include "../adt/struct.h"

/**
 * @file hud.h
 * @brief Interfaccia grafica testuale (Heads-Up Display) per il Portale Comunale.
 *
 * Il modulo gestisce l'interazione uomo-macchina a terminale attraverso menu 
 * formattati, tabelle geometriche allineate e cattura dell'input in tempo reale.
 */

/* =========================================================================
   INTERFACCIA UTENTE ED EMISSIONE CRUSCOTTI GRAFICI
   ========================================================================= */

/**
 * @brief Rendering della Dashboard principale del sistema.
 * * Pulisce lo schermo del terminale ed emette la griglia grafica contenente
 * i contatori globali aggregati, la ripartizione numerica per singola categoria 
 * e il menu di selezione delle funzionalità principali.
 * * @pre sistema deve essere un puntatore valido alla struttura di controllo Root (non nullo e inizializzato).
 * @post I dati attuali del grafo multi-indice vengono formattati e stampati a video. Lo stato della RAM rimane invariato.
 * @note Invia esplicitamente il flush allo stream di output per garantire l'immediatezza del rendering visivo.
 * @param sistema Il puntatore opaco alla radice del sistema da cui campionare le statistiche live.
 * @return void
 */
void dashboard(Root sistema);


/**
 * @brief Inizializza la schermata visiva per l'acquisizione di una nuova segnalazione.
 * * Esegue la pulizia del terminale e la formattazione dei banni superiori dell'HUD,
 * per poi cedere immediatamente il controllo algoritmico al modulo ADT per l'acquisizione.
 * * @pre root deve essere un puntatore valido alla struttura di controllo Root (non nullo e inizializzato).
 * @post Se l'inserimento ha successo, il grafo in RAM viene espanso, le statistiche aggiornate e il record serializzato su disco.
 * @note La funzione incapsula la chiamata a getNewSeg(Root) per rispettare i vincoli di Information Hiding sulle dimensioni dei nodi.
 * @param root Il sistema di controllo principale in cui inserire e registrare il nuovo record.
 * @return void
 */
void insertNewSeg(Root root);


/**
 * @brief Interfaccia testuale interattiva per la rimozione controllata di una segnalazione.
 * * Richiede l'inserimento dell'ID da tastiera, esegue una validazione difensiva rigida 
 * anti-carattere e, in caso di matching, invoca le routine di sfoltimento dei canali ortogonali.
 * * @pre root deve essere un puntatore valido alla struttura di controllo Root (non nullo e inizializzato).
 * @post Se l'ID esiste, il record viene estirpato da tutti e 4 gli indici logici, l'Heap liberata e i contatori decrementati.
 * @note Svuota preventivamente il buffer dello stream stdin in caso di inserimenti alfabetici non conformi o fuori range.
 * @param root Il sistema di controllo principale da cui sfoltire il record mirato.
 * @return void
 */
void removeSeg(Root root);


/**
 * @brief Visualizza una tabella geometrica formattata contenente le prime 20 segnalazioni.
 * * Scansiona l'indice cronologico globale partendo dalla testa del sotto-nodo temporale
 * e stampa i record formattati simulando un'impaginazione fissa di sistema.
 * * @pre root deve essere un puntatore valido alla struttura di controllo Root (non nullo e inizializzato).
 * @post Genera l'output tabellare a video. Sblocca l'interfaccia solo dopo una conferma cosciente dell'utente.
 * @note Blocca temporaneamente l'esecuzione del thread mediante getchar() per consentire la lettura dei dati da terminale.
 * @param root Il sistema di controllo principale da cui campionare la testa dell'ordinamento per data.
 * @return void
 */
void showSeg(Root root);


/**
 * @brief Attiva il motore di ricerca live, incrementale e asincrono a terminale.
 * * Modifica lo stato del terminale Linux disattivando la modalità canonica e l'eco locale.
 * Filtra istantaneamente l'albero dei nodi ad ogni singolo carattere digitato dall'operatore.
 * * @pre root deve essere un puntatore valido alla struttura di controllo Root, non nullo e popolato.
 * @post Restituisce a video la sotto-tabella filtrata in tempo reale. Al termine, ripristina la modalità canonica di Arch.
 * @note Intercetta le sequenze di escape dei tasti speciali (ESC = 27 per uscire, BACKSPACE = 127/8 per cancellare).
 * @param root Il sistema di controllo principale su cui effettuare la query incrementale per ID o categoria.
 * @return void
 */
void init_search_seg(Root root);


/**
 * @brief Interfaccia testuale per la modifica dello stato operativo di una segnalazione.
 * * Acquisisce l'ID target e il nuovo codice di stato, valida la conformità dei range 
 * ed esegue il re-indirizzamento ortogonale dei puntatori legati alla sottomatrice di stato.
 * * @pre root deve essere un puntatore valido alla struttura di controllo Root, non nullo e popolato.
 * @post Notifica visivamente l'esito della transizione (Successo, Operazione non necessaria, ID Inesistente).
 * @note Si interfaccia direttamente con i codici di ritorno (-1, 0, 1) restituiti dall'algoritmo di splicing dell'ADT.
 * @param root Il sistema di controllo principale su cui invocare la mutazione di stato.
 * @return void
 */
void modifySegHud(Root root);


/**
 * @brief Consolida i dati su disco, azzera la memoria dinamica RAM e spegne il processo.
 * * Invocata come opzione di sblocco finale, forza la riscrittura del database binario, 
 * dealloca le strutture tramite algoritmo di demolizione controllata ed esegue il clean dell'ambiente.
 * * @pre sistema deve essere un puntatore valido alla struttura di controllo Root (non nullo).
 * @post Il file binario viene aggiornato e chiuso. Tutta la memoria Heap associata al grafo viene azzerata (0 leak).
 * @note Questa funzione interrompe drasticamente l'applicazione invocando una exit(0) deterministica; non ritorna mai al chiamante.
 * @param sistema Il sistema finale da serializzare, ripulire e spegnere.
 * @return void
 */
void salvataggio(Root sistema);

#endif