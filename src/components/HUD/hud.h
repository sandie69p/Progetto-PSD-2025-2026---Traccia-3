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

/**
 * @brief Rendering della Dashboard principale del sistema.
 * @pre sistema deve essere un puntatore Root valido e inizializzato.
 * @post Pulisce interamente lo schermo del terminale ed emette la griglia grafica
 * contenente i contatori globali, i dati per categoria e il menu delle scelte.
 * @param sistema La radice del sistema da cui campionare le statistiche in tempo reale.
 */
void dashboard(Root);

/**
 * @brief Avvia la procedura guidata per l'acquisizione di una nuova segnalazione.
 * @pre root deve essere un puntatore Root valido.
 * @post Invocando internamente getNewSeg, acquisisce da stdin i campi della segnalazione,
 * alloca il nodo, aggiorna gli indici in RAM e scrive il record in append su disco.
 * @param root Il sistema in cui inserire la nuova segnalazione.
 */
void insertNewSeg(Root);

/**
 * @brief Interfaccia interattiva per la rimozione controllata di una segnalazione.
 * @pre root deve essere un puntatore Root valido.
 * @post Richiede l'ID a stdin. Se l'ID è sintatticamente valido, invoca init_removeSeg
 * per estirpare il nodo dai 4 indici ortogonali in memoria e decrementare i contatori.
 * @param root Il sistema da cui eliminare il record.
 */
void removeSeg(Root);

/**
 * @brief Visualizza una tabella ordinata contenente le prime 20 segnalazioni.
 * @pre root deve essere un puntatore Root valido.
 * @post Stampa a schermo l'intestazione allineata e invoca getSeg su ciascuno dei 
 * primi 20 nodi estratti dall'indice cronologico (ordinamento per Data).
 * @note Blocca l'esecuzione con una richiesta di INVIO (getchar) per consentire la lettura.
 * @param root Il sistema da cui campionare la testa cronologica.
 */
void showSeg(Root);

/**
 * @brief Attiva il motore di ricerca live e incrementale a terminale.
 * @pre root deve essere un puntatore Root valido e popolato.
 * @post Altera lo stato del terminale in modalità non-canonica (stty -icanon -echo).
 * Ad ogni carattere digitato, aggiorna dinamicamente la tabella filtrando per ID
 * o prefisso di Categoria in tempo reale. Al termine (tasto ESC), ripristina il terminale (stty cooked echo).
 * @note Sfrutta sequenze di escape ANSI per il posizionamento del cursore senza sfarfallio.
 * @param root Il sistema su cui effettuare il filtraggio dinamico dei nodi.
 */
void init_search_seg(Root);

/**
 * @brief Interfaccia testuale per la modifica dello stato di una segnalazione.
 * @pre root deve essere un puntatore Root valido e popolato.
 * @post Acquisisce ID e nuovo stato da stdin, valida l'input e invoca modifySeg
 * per aggiornare il nodo e reinserirlo nel corretto indice di stato.
 * @param root Il sistema su cui effettuare la modifica.
 */
void modifySegHud(Root);

/**
 * @brief Consolda i dati su disco, dealloca il grafo e spegne l'applicazione.
 * @pre sistema deve essere un puntatore Root valido.
 * @post Invoca save_records per riscrivere il database binario pulito, chiama deleteGraph
 * per azzerare l'Heap (0 leak), esegue un'animazione grafica di caricamento e termina il processo.
 * @note Questa funzione non ritorna mai al chiamante, ma effettua una exit(0) deterministica.
 * @param sistema Il sistema finale da salvare e distruggere.
 */
void salvataggio(Root);

#endif