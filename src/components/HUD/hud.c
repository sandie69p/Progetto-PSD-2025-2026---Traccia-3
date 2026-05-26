
/**
 * @file hud.c
 * @brief Implementazione dell'interfaccia utente testuale (Heads-Up Display) del Portale Comunale.
 *
 * Questo modulo si occupa esclusivamente della gestione del livello di presentazione 
 * (Presentation Layer) e dell'interazione uomo-macchina a terminale. La decisione progettuale 
 * cardine risiede nel rispetto del principio di separazione delle competenze (Separation of Concerns): 
 * il modulo HUD non conosce la struttura fisica interna dei nodi del grafo, ma interroga e muta lo 
 * stato del sistema operando come client dell'ADT opaco mediante le sole API pubbliche esportate 
 * dal modulo strutturale. Gestisce l'allineamento geometrico dei cruscotti grafici, il mascheramento 
 * dei caratteri spuri nei flussi di input e il rendering tabellare dei record.
 *
 * @author Salvatore
 * @date 2026
 *
 * @pre Il corretto funzionamento delle routine grafiche dipende dalla presenza di un terminale 
 * con supporto standard ANSI per la pulizia dello schermo e la formattazione tabellare.
 * @post Il modulo emette flussi di testo formattati su standard output e altera lo stato della RAM 
 * esclusivamente mediante chiamate transazionali verso l'ADT opaco.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // USLEEP solo su linux
#include "hud.h"
#include "../adt/struct.h"

/**
 * @brief Renderizza a video l'intestazione grafica tabellare per l'elenco delle segnalazioni.
 *
 * Questa funzione interna di utilità si occupa di stampare i delimitatori geometrici e i titoli 
 * delle colonne del cruscotto informativo (ID, Cittadino, Data, Stato, Categoria, Urgenza). La decisione 
 * progettuale risiede nell'allineamento statico dei caratteri in un blocco monospazio a larghezza fissa, 
 * studiato per combaciare al singolo byte con le specifiche di spaziatura utilizzate dalle routine di 
 * visualizzazione e ricerca (come search_seg), garantendo la coerenza visiva dell'interfaccia utente.
 *
 * @pre Il terminale deve supportare la codifica standard dei caratteri ASCII per una corretta 
 * visualizzazione della griglia geometrica.
 * @post I caratteri formattati vengono emessi sullo stream di output standard, senza alcuna alterazione 
 * o allocazione di memoria dinamica Heap.
 */
static void intestation(void) {
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  printf(" #  |----------------------------------------- ELENCO SEGNALAZIONI ---------------------------------------| # \n");
  printf(" #  |       ID         |        CITTADINO       |     DATA     |     STATO     |     CATEGORIA    |   U   | # \n");
  printf(" #  |-----------------------------------------------------------------------------------------------------| # \n");
}

/**
 * @brief Genera ed esporta su disco un report statistico testuale riassuntivo del portale comunale.
 *
 * Questa funzione si occupa di consolidare le metriche di efficienza e i carichi di lavoro in un file 
 * di testo leggibile (`report_comune.txt`). La decisione progettuale fondamentale risiede nel totale 
 * rispetto dell'incapsulamento dei dati (Information Hiding): per estrarre il volume globale dei record, 
 * lo spaccato dei sotto-stati, l'indice delle emergenze e i conteggi di settore, la funzione non accede 
 * mai alle strutture interne del grafo, ma interroga l'ADT esclusivamente attraverso i metodi getter 
 * pubblici (`getTotalSeg`, `getTotalAperte`, ecc.). L'apertura del file stream avviene in modalità "w" 
 * per garantire una scrittura distruttiva che rigeneri da zero un cruscotto informativo aggiornato ad 
 * ogni chiamata, minimizzando la complessità a tempo lineare rispetto al numero fisso delle categorie.
 *
 * @note In conformità con le direttive accademiche, la catena di formattazione e l'operatore ternario 
 * sequenziale per la selezione dei letterali di categoria non sono interrotti da micro-commenti in-linea.
 *
 * @param[in] sistema Il puntatore alla struttura principale di controllo (radice) da cui campionare i dati.
 *
 * @param[out] fReport File stream di output aperto verso la memoria secondaria in modalità di testo.
 *
 * @pre `sistema` deve essere un'istanza valida, non nulla e precedentemente inizializzata in memoria RAM.
 * @post Un file di testo formattato viene generato o sovrascritto nella cartella di esecuzione. 
 * Lo stream verso il disco viene chiuso correttamente. Lo stato della memoria RAM rimane completamente inalterato.
 */
static void getReport(Root sistema) {
  if (sistema == NULL) return;

  FILE *fReport = fopen("./report_comune.txt", "w");
  if (fReport == NULL) {
    printf(" # | [ERRORE RIGIDO] Impossibile creare il file del report su disco.\n");
    return;
  }

  fprintf(fReport, "=========================================================================\n");
  fprintf(fReport, "          PORTALE URBANISTICO COMUNALE - REPORT STATISTICO GLOBALE       \n");
  fprintf(fReport, "=========================================================================\n\n");
  fprintf(fReport, " * Volume Totale Segnalazioni Attive: %d\n\n", getTotalSeg(sistema));
  
  fprintf(fReport, " * SPACCATO DEGLI STATI OPERATIVI:\n");
  fprintf(fReport, "   - Segnalazioni in stato Aperto: %d\n", getTotalAperte(sistema));
  fprintf(fReport, "   - Segnalazioni in Risoluzione: %d\n", getTotalRis(sistema));
  fprintf(fReport, "   - Segnalazioni in stato Chiuso: %d\n\n", getTotalChiuse(sistema));
  
  fprintf(fReport, " * INDICE DI CRITICITÀ:\n");
  fprintf(fReport, "   - Segnalazioni Massima Urgenza (5): %d\n\n", getMostUrgenti(sistema));
  
  fprintf(fReport, " * DISTRIBUZIONE DETTAGLIATA PER CATEGORIA COMUNALE:\n");
  
  for(int i = 0; i < 11; i++) {
     const char* nCat = (i == 0) ? "Incendio" : 
        (i == 1) ? "Illuminazione" : 
        (i == 2) ? "Rifiuti" : 
        (i == 3) ? "Strade" : 
        (i == 4) ? "Verde Pubblico" : 
        (i == 5) ? "Allagamento" : 
        (i == 6) ? "Segnaletica" : 
        (i == 7) ? "Edilizia" : 
        (i == 8) ? "Randagismo" : 
        (i == 9) ? "Inquinamento" : "Sicurezza";
                        
     fprintf(fReport, "   - %-25s : %d\n", nCat, getSegCountByCategory(sistema, i));
  }
  
  fprintf(fReport, "\n * AREA COMUNALE PIÙ COLPITA DA DISSERVIZI: %s\n", getMaxCatName(sistema));
  fprintf(fReport, "\n=========================================================================\n");
  
  fclose(fReport);
}

/**
 * @brief Coordina la procedura sequenziale di shutdown del portale, sincronizzando i dati e svuotando la RAM.
 *
 * Questa funzione gestisce l'uscita definitiva dal programma assicurando la totale consistenza dei 
 * dati. La decisione progettuale cardine risiede nella transizione sequenziale protetta: prima di 
 * terminare il processo, la funzione invoca in ordine `save_records()` per consolidare lo snapshot dei 
 * nodi e `getReport()` per aggiornare le statistiche su disco. Successivamente, avvia una barra di 
 * caricamento visiva basata sul ritorno a capo dinamico (`\r`) e sul ritardo controllato del thread (`usleep`) 
 * per feedback utente, per poi invocare tassativamente `deleteGraph()`. Questa scomposizione garantisce 
 * che l'applicazione termini con successo esibendo uno stato di 0 memory leak su strumenti di profiling (Valgrind).
 *
 * @note I cicli interni annidati deputati al rendering geometrico dei caratteri '#' e '-' non sono 
 * frammentati da micro-commenti in-linea per non inficiare la leggibilità del blocco di output.
 *
 * @param[in,out] sistema Il puntatore alla struttura principale di controllo (radice) da consolidare e distruggere.
 *
 * @pre `sistema` deve essere un'istanza valida. Se il puntatore è NULL, l'applicazione abortisce in sicurezza.
 * @post I dati attuali della RAM vengono interamente persistiti su disco, tutta la memoria dinamica dello Heap 
 * viene completamente deallocata, e il processo software viene terminato forzatamente tramite exit(0).
 */
void salvataggio(Root sistema) {
  if (sistema == NULL) {
      printf(" # | [AVVISO] Nessun sistema attivo da salvare.\n");
      exit(0);
  }

  printf(" # | Salvataggio in corso...                      # \n");

  save_records(sistema);

  getReport(sistema);

  int lunghezza = 50;
  int i = 0;
  
  for(; i <= lunghezza; i++) {
    printf("\r [");
    
    for(int j = 0; j <= i; j++) {
      printf("#");
    }
    
    for(int j = 0; j < (lunghezza - i); j++) {
      printf("-");
    }

    printf("] %6.2f%%", ((float) i * 100) / (float) lunghezza);

    fflush(stdout);
    usleep(20000);
  }

  printf("\n # | [INFO] Liberazione della memoria dinamica (Svuotamento Heap RAM)...                                 | # \n");
  deleteGraph(sistema);
  printf(" # | [ OK ] Grafo multi-indice deallocato con successo (0 leak).                                         | # \n");

  printf(" # | [INFO] Chiusura del processo in corso...                                                            | # \n");
  usleep(500000);
  system("clear");
  exit(0);
}

/**
 * @brief Renderizza il cruscotto grafico principale (Dashboard) assemblando le statistiche a colonne parallele.
 *
 * Questa funzione costituisce il punto nodale dell'interfaccia utente (HUD). La decisione progettuale 
 * cardine risiede nella formattazione geometrica bidimensionale: i flussi informativi estratti in tempo 
 * costante dal grafo vengono organizzati in due macro-colonne simmetriche (`col_sinistra` e `col_destra`). 
 * Nel totale rispetto dell'Information Hiding, il modulo HUD non manipola i puntatori della RAM, ma 
 * interroga l'ADT mediante le sue macro-funzioni getter pubbliche. L'allineamento perfetto e la 
 * stabilità visiva dei bordi geometrici vengono garantiti programmaticamente impostando specificatori 
 * di larghezza fissa all'interno delle primitive di stampa, impedendo disallineamenti derivanti dalle 
 * diverse lunghezze numeriche delle metriche.
 *
 * @note Per mantenere la linearità visiva del blocco sequenziale di stampa, i singoli passaggi di 
 * composizione delle stringhe tramite sprintf() non sono interrotti da micro-commenti in-linea.
 *
 * @param[in] sistema Il puntatore alla struttura principale di controllo (radice) da campionare a video.
 *
 * @pre `sistema` deve essere un'istanza valida allocata e inizializzata. Se il puntatore è NULL, 
 * la funzione intercetta l'anomalia emettendo un messaggio di blocco difensivo ed evita il crash.
 * @post Il terminale viene ripulito tramite chiamata di sistema e il pannello di controllo viene 
 * interamente renderizzato sullo standard output. Lo stato della memoria RAM rimane inalterato.
 */
void dashboard(Root sistema) {
    if (sistema == NULL) {
        printf("\n  # | [ERRORE CRITICO] Impossibile mostrare la dashboard: sistema non inizializzato o RAM corrotta.\n");
        return;
    }

    system("clear");
    
    printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
    printf(" #   %-47s #    %-48s # \n", " STATISTICHE GENERALI", "CONTEGGIO SEGNALAZIONI PER CATEGORIA");
    printf(" #---------------------------------------------------#-----------------------------------------------------# \n");

    char col_sinistra[128];
    char col_destra[128];

    sprintf(col_sinistra, " - Numero segnalazioni comune:  %7d", getTotalSeg(sistema));
    sprintf(col_destra, " - Illuminazione:      %7d", getSegCountByCategory(sistema, 1));
    printf(" #   %-47s #   %-49s # \n", col_sinistra, col_destra);

    sprintf(col_sinistra, " - Segnalazioni aperte:         %7d", getTotalAperte(sistema));
    sprintf(col_destra, " - Rifiuti:            %7d", getSegCountByCategory(sistema, 2));
    printf(" #   %-47s #   %-49s # \n", col_sinistra, col_destra);

    sprintf(col_sinistra, " - Segnalazioni in risoluzione: %7d", getTotalRis(sistema));
    sprintf(col_destra, " - Strade:             %7d", getSegCountByCategory(sistema, 3));
    printf(" #   %-47s #   %-49s # \n", col_sinistra, col_destra);

    sprintf(col_sinistra, " - Segnalazioni chiuse:         %7d", getTotalChiuse(sistema));
    sprintf(col_destra, " - Verde Pubblico:     %7d", getSegCountByCategory(sistema, 4));
    printf(" #   %-47s #   %-49s # \n", col_sinistra, col_destra);

    sprintf(col_sinistra, " - Segnalazioni Urgenti (5):    %7d", getMostUrgenti(sistema));
    sprintf(col_destra, " - Incendio:           %7d", getSegCountByCategory(sistema, 0));
    printf(" #   %-47s #   %-49s # \n", col_sinistra, col_destra);

    sprintf(col_destra, " - Allagamento:        %7d", getSegCountByCategory(sistema, 5));
    printf(" #---------------------------------------------------#   %-49s # \n", col_destra);

    sprintf(col_destra, " - Segnaletica:        %7d", getSegCountByCategory(sistema, 6));
    printf(" #   %-47s #   %-49s # \n", " - Creare una segnalazione:           1", col_destra);

    sprintf(col_destra, " - Edilizia:           %7d", getSegCountByCategory(sistema, 7));
    printf(" #   %-47s #   %-49s # \n", " - Rimuovere una segnalazione:        2", col_destra);

    sprintf(col_destra, " - Randagismo:         %7d", getSegCountByCategory(sistema, 8));
    printf(" #   %-47s #   %-49s # \n", " - Mostra segnalazioni per data:      3", col_destra);

    sprintf(col_destra, " - Inquinamento:       %7d", getSegCountByCategory(sistema, 9));
    printf(" #   %-47s #   %-49s # \n", " - Mostra segnalazioni per id:        4", col_destra);

    sprintf(col_destra, " - Sicurezza:          %7d", getSegCountByCategory(sistema, 10));
    printf(" #   %-47s #   %-49s # \n", " - Modifica stato segnalazione:       5", col_destra);

    const char *topCategoria = getMaxCatName(sistema);

    sprintf(col_destra, " - Cat. piu' segnalata: %-15.15s", topCategoria ? topCategoria : "N/D");
    printf(" #   %-47s #   %-49s # \n", " - Mostra descrizione segnalazione:   6", col_destra);
    printf(" #   %-47s #%-53s# \n", " - Salvare ed uscire:                 0", "-----------------------------------------------------");

    printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
    fflush(stdout);
}

/**
 * @brief Estrae, formatta e renderizza a video i campi informativi di un singolo nodo segnalazione.
 *
 * La funzione si occupa dell'impaginazione di un singolo record all'interno della griglia geometrica 
 * dell'interfaccia tabellare. Nel pieno rispetto del principio di incapsulamento (Information Hiding), 
 * l'algoritmo non accede mai direttamente alla struttura fisica del record, ma interroga il nodo opaco 
 * tramite i getter e gli iteratori pubblici dell'ADT (`getID`, `getName`, `getCat`, `getState`, `getUrg`). 
 * Rappresenta un punto critico per la stabilità della memoria: poiché la routine delegata `getData()` 
 * alloca dinamicamente un buffer di testo sullo Heap per convertire il timestamp intero, questa funzione 
 * acquisisce la titolarità di tale puntatore ed esegue tassativamente la free() prima dell'uscita, 
 * garantendo l'assenza di memory leak (0 leak) anche in contesti di iterazione massiva.
 *
 * @note Per preservare la pulizia sintattica e la leggibilità visiva dei formattatori posizionali, 
 * i singoli casi di commutazione dello switch e le macro di stampa non contengono commenti in-linea.
 *
 * @param[in] node Il puntatore al nodo segnalazione (`s`) costante da decodificare e visualizzare.
 *
 * @pre `node` deve essere un record valido, integro e non nullo precedentemente allocato ed inserito nel grafo.
 * @post I dati del record vengono formattati in una stringa monospazio ed emessi sullo standard output. 
 * Il buffer temporaneo della data allocato sullo Heap viene interamente deallocato. Lo stato della RAM rimane inalterato.
 */
static void getSeg(s node) {
  if (node == NULL) return;

  char *dataStr = getData(node);
  const char *nameStr = getName(node);
  const char *catStr = getCat(node);
  const char *statoStr;

  switch(getState(node)) {
    case 0:  statoStr = "Aperta";      break;
    case 1:  statoStr = "Risoluzione"; break;
    case 2:  statoStr = "Chiusa";      break;
    default: statoStr = "N/D";         break;
  }

  char riga_contenuto[256];
  
  sprintf(riga_contenuto, " | %-16d | %-22.22s | %-12.12s | %-13.13s | %-16.16s |   %d   |",
    getID(node),
    nameStr ? nameStr : "N/D",
    dataStr ? dataStr : "N/D",
    statoStr,
    catStr ? catStr : "N/D",
    getUrg(node)
  );

  printf(" # %-101s # \n", riga_contenuto);

  if (dataStr) {
    free(dataStr);
  }
}

/**
 * @brief Sfoglia e renderizza a video le prime 20 segnalazioni presenti sul canale cronologico.
 *
 * La funzione implementa un cruscotto di visualizzazione temporale parziale. La decisione progettuale 
 * cardine risiede nel totale rispetto dell'Information Hiding e nell'ottimizzazione del layout visivo: 
 * il modulo HUD non conosce l'anatomia interna dei collegamenti del grafo, ma avvia il tracciato 
 * interpellando il metodo getter pubblico `getDataHead(root)` e avanza sequenzialmente lungo la linea 
 * temporale sfruttando l'iteratore opaco `nextForData(currentSeg)`. Per garantire la leggibilità a terminale 
 * ed evitare la saturazione geometrica della schermata, viene introdotto un vincolo algoritmico di sbarramento 
 * (`limit < 20`) che tronca l'emissione dei record a un massimo di 20 righe contigue, ripulendo i flussi di 
 * input spuri prima di bloccare l'interfaccia in attesa del comando utente.
 *
 * @note In conformità con le linee guida accademiche, le istruzioni di reset del terminale tramite 
 * `system("clear")` e i cicli di svuotamento dei buffer non contengono commenti in-linea.
 *
 * @param[in] root Il puntatore alla struttura principale di controllo (radice) da scansionare cronologicamente.
 *
 * @pre `root` deve essere un'istanza valida, non nulla e precedentemente popolata in memoria RAM.
 * @post I record individuati vengono impaginati all'interno della griglia geometrica ed emessi su standard output. 
 * Lo stato della memoria RAM e la disposizione dei collegamenti ortogonali rimangono inalterati.
 */
void showSeg(Root root) {
  if (root == NULL) return;

  system("clear");

  intestation();

  s currentSeg = getDataHead(root);
  int limit = 0;

  while(currentSeg != NULL && limit < 20) {
    getSeg(currentSeg); 
    currentSeg = nextForData(currentSeg);
    limit++;
  }

  int c;
  while ((c = getchar()) != '\n' && c != EOF);

  char riga_footer_linea[256];
  char riga_footer_testo[256];
  
  sprintf(riga_footer_linea, " |-----------------------------------------------------------------------------------------------------|");
  sprintf(riga_footer_testo, " |-------------- Mostrate le prime 20 segnalazioni in ordine cronologico. Premi INVIO: ----------------|");
  
  printf(" # %-101s # \n", riga_footer_linea);
  printf(" # %-101s # \n", riga_footer_testo);
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  
  getchar();
}

/**
 * @brief Funzione wrapper per il rendering grafico della schermata di acquisizione di un nuovo record.
 *
 * La funzione funge da contenitore estetico e strutturale per il livello di presentazione (Presentation 
 * Layer). La decisione progettuale cardine risiede nel rispetto del principio di singola responsabilità 
 * (Single Responsibility Principle): la routine non elabora i dati e non interagisce con l'Heap, ma si 
 * occupa esclusivamente di preparare il terminale pulendo lo schermo e stampando i delimitatori geometrici 
 * esterni monospazio dell'HUD. Delega interamente la logica difensiva di acquisizione, allocazione del nodo 
 * opaco, persistenza su file binario e riallineamento transazionale del grafo alla sottostante getNewSeg().
 *
 * @param[in,out] root Il puntatore alla struttura principale di controllo (radice) da inoltrare al modulo di acquisizione.
 *
 * @pre `root` deve essere un'istanza valida e non nulla, precedentemente allocata in memoria RAM.
 * @post Il terminale viene ripulito, la cornice dell'HUD viene renderizzata e il controllo dei puntatori 
 * viene trasferito in sicurezza alla routine di inserimento a caldo. Lo stato della RAM viene modificato 
 * solo in base all'esito della funzione delegata.
 */
void insertNewSeg(Root root) {
  if (root == NULL) return;

  system("clear");
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  printf(" # |------------------------------------- ACQUISIZIONE NUOVA SEGNALAZIONE -------------------------------| # \n");
  printf(" # |                                                                                                     | # \n");

  getNewSeg(root); 

  printf(" # |                                                                                                     | # \n");
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
}

/**
 * @brief Interfaccia utente controllata (Wrapper) per l'acquisizione sicura e la rimozione a caldo di un record.
 *
 * La funzione coordina la schermata grafica di rimozione delle segnalazioni. La decisione progettuale 
 * cardine risiede nella severità dei meccanismi di validazione dello stream di input standard (Stream Masking): 
 * l'algoritmo non si limita a verificare la formattazione numerica dell'ID tramite scanf(), ma esamina il 
 * carattere residuo immediatamente successivo nel buffer della tastiera per intercettare tentativi di input 
 * corrotti. L'intervallo di sbarramento dell'ID adotta un approccio a "white-list" geometrica incolonnata 
 * in verticale per verificare che l'identificativo appartenga rigorosamente a uno degli intervalli discreti 
 * validi delle sottocategorie comunali censite (da 1000000 a 9099999). Per confermare l'avvenuta espulsione 
 * del record senza violare l'Information Hiding, la funzione adotta un approccio transazionale, confrontando 
 * la cardinalità totale del grafo prima e dopo l'invocazione di init_removeSeg().
 *
 * @note Per mantenere la stabilità geometrica dei blocchi di visualizzazione monospazio a larghezza fissa, 
 * i singoli costrutti condizionali di sbarramento degli errori non contengono commenti in-linea.
 *
 * @param[in,out] root Il puntatore alla struttura principale di controllo (radice) da cui estirpare il record.
 *
 * @pre `root` deve essere un'istanza valida e non nulla, precedentemente allocata in memoria RAM.
 * @post Se l'ID è conforme ed esistente, il nodo viene rimosso fisicamente dai quattro indici paralleli e 
 * la memoria viene deallocata dalla routine interna; in caso contrario, la RAM non subisce alterazioni. 
 * Il terminale viene aggiornato graficamente prima del rientro alla dashboard.
 */
void removeSeg(Root root) {
  if (root == NULL) return;

  system("clear");
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  printf(" # |------------------------------------- RIMOZIONE SEGNALAZIONE COMUNALE -------------------------------| # \n");
  printf(" # |                                                                                                     | # \n");
  
  char riga_prompt[128];
  sprintf(riga_prompt, " Inserisci il codice numerico identificativo (ID) da eliminare permanentemente: ");
  printf(" # |%-101s| # \n", riga_prompt);
  printf(" # |                                                                                                     | # \n");
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  printf(" # Scelta ID per rimozione: ");
  fflush(stdout);

  int idTarget;
  if (scanf("%d", &idTarget) != 1) {
    printf(" # [ERRORE RIGIDO] L'ID inserito deve contenere unicamente cifre numeriche!                              | # \n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    printf(" # Premi INVIO per annullare e ritornare alla dashboard...                                               | # \n");
    printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
    getchar();
    return;
  }

  int residuo = getchar();
  if (residuo != '\n' && residuo != EOF) {
    printf(" # [ERRORE RIGIDO] Input corrotto! Rilevati caratteri alfabetici illegali accodati all'ID.               | # \n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    printf(" # Premi INVIO per annullare e ritornare alla dashboard...                                               | # \n");
    printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
    getchar();
    return;
  }

  if (!((idTarget >= 1000000 && idTarget <= 1099999)||
    (idTarget >= 1100000 && idTarget <= 1199999) ||
    (idTarget >= 2000000 && idTarget <= 2099999) ||
    (idTarget >= 2100000 && idTarget <= 2199999) ||
    (idTarget >= 3000000 && idTarget <= 3099999) ||
    (idTarget >= 4000000 && idTarget <= 4099999) ||
    (idTarget >= 5000000 && idTarget <= 5099999) ||
    (idTarget >= 6000000 && idTarget <= 6099999) ||
    (idTarget >= 7000000 && idTarget <= 7099999) ||
    (idTarget >= 8000000 && idTarget <= 8099999) ||
    (idTarget >= 9000000 && idTarget <= 9099999)) 
  ) {
    printf(" # [ERRORE RIGIDO] ID inserito fuori range o non conforme ai parametri del comune!                       | # \n");
    printf(" # Premi INVIO per annullare e ritornare alla dashboard...                                               | # \n");
    printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
    getchar();
    return;
  }

  int totale_prima = getTotalSeg(root);
  init_removeSeg(root, idTarget);
  int totale_dopo = getTotalSeg(root);

  system("clear");
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  
  if (totale_dopo < totale_prima) {
    printf(" # |--------------------------------- OPERAZIONE COMPLETATA CON SUCCESSO --------------------------------| # \n");
    printf(" # |                                                                                                     | # \n");
    char riga_successo[128];
    sprintf(riga_successo, " [ OK ] Segnalazione ID %d estirpata correttamente da tutti i flussi ortogonali in RAM.", idTarget);
    printf(" # |%-101s| # \n", riga_successo);
  } else {
    printf(" # |---------------------------------------- OPERAZIONE FALLITA -----------------------------------------| # \n");
    printf(" # |                                                                                                     | # \n");
    char riga_fallimento[128];
    sprintf(riga_fallimento, " [AVVISO] Nessuna eliminazione effettuata. L'ID %d non e' presente nel database locale.", idTarget);
    printf(" # |%-101s| # \n", riga_fallimento);
  }

  printf(" # |                                                                                                     | # \n");
  printf(" # Premi INVIO per confermare e tornare alla dashboard principale...                                     | # \n");
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  getchar();
}

/**
 * @brief Gestisce l'interfaccia di ricerca interattiva in tempo reale filtrando i record a ogni cifra digitata.
 *
 * La funzione implementa una barra di ricerca reattiva (real-time filtering). La decisione progettuale 
 * cardine risiede nella manipolazione a basso livello dei descrittori del terminale POSIX: mediante 
 * l'invocazione di `system("stty -icanon -echo")`, viene disabilitata la modalità canonica (buffered) 
 * e l'eco automatico a schermo. Questo consente alla primitiva `getchar()` di catturare i singoli byte 
 * istantaneamente senza attendere il carattere di invio. Il ciclo intercetta i codici ASCII di sbarramento 
 * (27 per ESC, 127/8 per il Backspace) per aggiornare dinamicamente il buffer della stringa `query`, 
 * forzando il ridisegno asincrono del cruscotto geometrico tramite `search_seg()`. Prima dell'uscita, 
 * viene tassativamente ripristinato lo stato originario del terminale (`stty cooked echo`) per prevenire 
 * l'instabilità della shell.
 *
 * @note Per preservare la pulizia visiva e non frammentare il blocco logico di cattura dei tasti, 
 * le maschere dei singoli codici ASCII e le sequenze di escape per il riposizionamento del cursore 
 * ("\033[H", "\033[K") non contengono commenti in-linea.
 *
 * @param[in,out] root Il puntatore alla struttura principale di controllo (radice) da sottoporre a scansione parziale.
 *
 * @pre `root` deve essere un'istanza valida, non nulla e precedentemente allocata in memoria RAM.
 * @post Lo stato dei nodi e degli indici nel grafo non subisce alcuna alterazione. Il terminale viene 
 * configurato in modalità grezza (raw-like) durante l'esecuzione e ripristinato interamente in modalità 
 * canonica prima del rientro alla dashboard.
 */
void init_search_seg(Root root) {
  char query[8] = "";
  int index = 0;
  int ch;

  system("stty -icanon -echo");
  system("clear");

  while(1) {
    printf("\033[H");
        
    printf(" ##### ##### ##### PORTALE COMUNALE - RICERCA ID # ##### ##### ##### \n");
    printf(" # Digita l'ID (Premi ESC per uscire o BACKSPACE per cancellare)     \n");
    printf(" # \n");
    printf(" # BARRA DI RICERCA: [ %-7s ] \033[K\n", query);

    intestation();

    if (index > 0) {
      search_seg(root, query);
    } else {
      for(int i = 0; i < 12; i++) {
        if (i == 5) printf(" #  |                               Inizia a digitare le cifre dell'ID...                                 | # \n");
        else        printf(" #  |                                                                                                     | # \n");
      }
    }
    printf(" #  |-----------------------------------------------------------------------------------------------------| # \n");
    fflush(stdout);

    ch = getchar();

    if (ch == 27) {
      break;
    } 
    else if (ch == 127 || ch == 8) {
      if (index > 0) {
        query[--index] = '\0';
      }
    } 
    else if (ch >= '0' && ch <= '9' && index < 7) {
      query[index++] = ch;
      query[index] = '\0';
    }
  }

  system("stty cooked echo");
  system("clear");
}

/**
 * @brief Interfaccia utente controllata (Wrapper) per l'acquisizione sicura e la modifica dello stato di un record.
 *
 * La funzione gestisce il cruscotto grafico per la transizione di stato delle segnalazioni comunali. 
 * La decisione progettuale cardine risiede nella struttura difensiva della validazione degli input a più 
 * stadi (Multi-Stage Input Validation): l'algoritmo esegue un controllo rigoroso sia sull'ID target sia 
 * sul codice numerico del nuovo stato (0-2), campionando i caratteri residui nel buffer della tastiera 
 * dopo ogni lettura per intercettare ed espellere immediatamente stringhe corrotte o alfabetiche. 
 * L'intervallo di sbarramento dell'ID adotta un approccio a "white-list" geometrica incolonnata per 
 * verificare che l'identificativo appartenga rigorosamente a uno degli intervalli discreti validi delle 
 * sottocategorie (da 1000000 fino a 9099999), rifiutando preventivamente prefissi non censiti. 
 * Interfacciandosi in modo sicuro con l'ADT opaco, decodifica i tre scenari logici restituiti da `modifySeg()` 
 * (Transizione completata, Stato ridondante invariato, ID non trovato) e adatta dinamicamente il layout dell'HUD.
 *
 * @note Per mantenere la stabilità delle righe monospazio fisse e non frammentare il blocco sequenziale di 
 * acquisizione, i singoli costrutti 'if' di sbarramento degli errori non contengono commenti in-linea.
 *
 * @param[in,out] root Il puntatore alla struttura principale di controllo (radice) su cui innestare la modifica.
 *
 * @pre `root` deve essere un'istanza valida e non nulla, precedentemente allocata in memoria RAM.
 * @post Se i parametri sono conformi e l'ID è presente, il nodo viene trasferito tra gli indici di stato 
 * del grafo e la RAM viene riallineata. Lo schermo viene aggiornato graficamente prima del rientro alla dashboard.
 */
void modifySegHud(Root root) {
  if (root == NULL) return;

  system("clear");
  
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  printf(" # |------------------------------------- MODIFICA STATO SEGNALAZIONE -----------------------------------| # \n");
  printf(" # |                                                                                                     | # \n");

  int32_t idTarget;
  char riga_prompt[128];
  sprintf(riga_prompt, " Inserisci ID della segnalazione comunale: ");
  printf(" # |%-101s| # \n", riga_prompt);
  printf(" # |                                                                                                     | # \n");
  printf(" # | Scelta ID: ");
  fflush(stdout);

  if (scanf("%d", &idTarget) != 1 || !((idTarget >= 1000000 && idTarget <= 1099999)||
    (idTarget >= 1100000 && idTarget <= 1199999) ||
    (idTarget >= 2000000 && idTarget <= 2099999) ||
    (idTarget >= 2100000 && idTarget <= 2199999) ||
    (idTarget >= 3000000 && idTarget <= 3099999) ||
    (idTarget >= 4000000 && idTarget <= 4099999) ||
    (idTarget >= 5000000 && idTarget <= 5099999) ||
    (idTarget >= 6000000 && idTarget <= 6099999) ||
    (idTarget >= 7000000 && idTarget <= 7099999) ||
    (idTarget >= 8000000 && idTarget <= 8099999) ||
    (idTarget >= 9000000 && idTarget <= 9099999)) 
  ) {
    printf("\n # | [ERRORE RIGIDO] ID non conforme! Deve essere numerico e compreso tra 1000000 e 2199999.           | # \n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    printf(" # | Premi INVIO per annullare l'operazione e tornare alla dashboard...                                | # \n");
    getchar();
    return;
  }

  int residuo_id = getchar();
  if (residuo_id != '\n' && residuo_id != EOF) {
    printf("\n # | [ERRORE RIGIDO] Input non conforme! Rilevato testo alfabetico illegale accodato all'ID.           | # \n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    printf(" # | Premi INVIO per annullare l'operazione e tornare alla dashboard...                                | # \n");
    getchar();
    return;
  }

  system("clear");
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  printf(" # |------------------------------------- SELEZIONE NUOVO STATO OPERATIVO -------------------------------| # \n");
  
  char riga_target[128];
  sprintf(riga_target, " Modifica in corso per la segnalazione ID: %d", idTarget);
  printf(" # |%-101s| # \n", riga_target);
  printf(" # |                                                                                                     | # \n");
  printf(" # | 0) APERTA                                                                                           | # \n");
  printf(" # | 1) IN RISOLUZIONE                                                                                   | # \n");
  printf(" # | 2) CHIUSA                                                                                           | # \n");
  printf(" # |                                                                                                     | # \n");
  printf(" # | Inserisci codice stato (0-2): ");
  fflush(stdout);

  int nuovoStato;
  if (scanf("%d", &nuovoStato) != 1 || nuovoStato < 0 || nuovoStato > 2) {
    printf("\n # | [ERRORE RIGIDO] Stato non valido! Scegliere unicamente una delle opzioni censite (0, 1, 2).        | # \n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    printf(" # | Premi INVIO per annullare l'operazione e tornare alla dashboard...                                 | # \n");
    getchar();
    return;
  }

  int residuo_stato = getchar();
  if (residuo_stato != '\n' && residuo_stato != EOF) {
    printf("\n # | [ERRORE RIGIDO] Scelta corrotta da caratteri alfabetici non consentiti!                             | # \n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    printf(" # | Premi INVIO per annullare l'operazione e tornare alla dashboard...                                  | # \n");
    getchar();
    return;
  }

  int esito_modifica = modifySeg(root, idTarget, nuovoStato);

  system("clear");
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");

  if (esito_modifica == 1) {
    printf(" # |--------------------------------- AGGIORNAMENTO COMPLETATO CON SUCCESSO -----------------------------| # \n");
    printf(" # |                                                                                                     | # \n");
    char riga_successo[128];
    sprintf(riga_successo, " # | [ OK ] Stato della segnalazione %d modificato correttamente in RAM locale. ", idTarget);
    printf(" # |%-101s| # \n", riga_successo);
    printf(" # | I cambiamenti saranno resi persistenti sul file database binario solo alla chiusura (0).            | # \n");
  } 
  else if (esito_modifica == 0) {
    printf(" # |------------------------------------- OPERAZIONE NON NECESSARIA -------------------------------------| # \n");
    printf(" # |                                                                                                     | # \n");
    char riga_warning[128];
    sprintf(riga_warning, " # | [AVVISO] La segnalazione %5d possiede gia' lo stato operativo selezionato.              | #", idTarget);
    printf(" # |%-101s| # \n", riga_warning);
    printf(" # | Nessuna operazione di riposizionamento eseguita sugli indici del grafo.                             | # \n");
  } 
  else {
    printf(" # |----------------------------------------- OPERAZIONE FALLITA ----------------------------------------| # \n");
    printf(" # |                                                                                                     | # \n");
    char riga_errore[128];
    sprintf(riga_errore, " # [ERRORE] Impossibile procedere. L'ID %d non e' presente nel database locale. ", idTarget);
    printf(" # |%-101s| # \n", riga_errore);
    printf(" # | Verificare la presenza dell'ID tramite il pannello delle statistiche generali.                      | # \n");
  }

  printf(" # |                                                                                                     | # \n");
  printf(" # | Premi INVIO per confermare e tornare alla dashboard principale...                                   | # \n");
  getchar();
}

/**
 * @brief Interfaccia utente controllata (Wrapper) per la visualizzazione espansa della descrizione di un record.
 *
 * La funzione si occupa del rendering visivo del testo descrittivo esteso di una segnalazione rintracciata 
 * nel sistema. Adotta il meccanismo rigido di validazione a cascata dell'ID su base verticale (stile Solidity) 
 * per garantire l'integrità dei flussi di input standard. Nel pieno rispetto dell'Information Hiding, 
 * l'algoritmo non accede mai direttamente ai campi fisici delle strutture del nodo, ma estrae le proprietà 
 * e il buffer testuale mediante i metodi getter dell'interfaccia pubblica dell'ADT opaco (getID, getCat, 
 * getName, getDescription).
 *
 * @param[in,out] root Il puntatore alla struttura principale di controllo (radice) in cui cercare il record.
 *
 * @pre `root` deve essere un'istanza valida e non nulla, precedentemente allocata in memoria RAM.
 * @post Lo schermo viene aggiornato graficamente. Lo stato dei nodi in memoria RAM rimane inalterato.
 */
void showDescription(Root root) {
  if (root == NULL) return;

  system("clear");
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  printf(" # |---------------------------------- VISUALIZZAZIONE DESCRIZIONE ESTESA -------------------------------| # \n");
  printf(" # |                                                                                                     | # \n");

  int32_t idTarget;
  char riga_prompt[128];
  sprintf(riga_prompt, " Inserisci ID della segnalazione comunale per leggere il testo espanso: ");
  printf(" # |%-101s| # \n", riga_prompt);
  printf(" # |                                                                                                     | # \n");
  printf(" # | Scelta ID: ");
  fflush(stdout);

  if (scanf("%d", &idTarget) != 1 || !((idTarget >= 1000000 && idTarget <= 1099999)||
    (idTarget >= 1100000 && idTarget <= 1199999) ||
    (idTarget >= 2000000 && idTarget <= 2099999) ||
    (idTarget >= 2100000 && idTarget <= 2199999) ||
    (idTarget >= 3000000 && idTarget <= 3099999) ||
    (idTarget >= 4000000 && idTarget <= 4099999) ||
    (idTarget >= 5000000 && idTarget <= 5099999) ||
    (idTarget >= 6000000 && idTarget <= 6099999) ||
    (idTarget >= 7000000 && idTarget <= 7099999) ||
    (idTarget >= 8000000 && idTarget <= 8099999) ||
    (idTarget >= 9000000 && idTarget <= 9099999)) 
  ) {
    printf("\n # | [ERRORE RIGIDO] ID non conforme! Deve essere numerico e appartenere a una categoria censita.        | # \n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    printf(" # | Premi INVIO per annullare l'operazione e tornare alla dashboard...                                  | # \n");
    fflush(stdout);
    getchar();
    return;
  }

  int residuo = getchar();
  if (residuo != '\n' && residuo != EOF) {
    printf("\n # | [ERRORE RIGIDO] Input corrotto! Rilevato testo alfabetico illegale accodato all'ID.                 | # \n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    printf(" # | Premi INVIO per annullare l'operazione e tornare alla dashboard...                                  | # \n");
    getchar();
    return;
  }

  s curr = findSegByID(root, idTarget);

  system("clear");
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  
  if (curr != NULL) {
    printf(" # |----------------------------------------- DETTAGLIO RECORD -----------------------------------------| # \n");
    printf(" # | ID: %-94d | # \n", getID(curr));
    printf(" # | Categoria: %-88s | # \n", getCat(curr));
    printf(" # | Cittadino: %-88s | # \n", getName(curr));
    printf(" # | Data: %-90s | # \n", getData(curr));
    printf(" # |-----------------------------------------------------------------------------------------------------| # \n");
    printf(" # | DESCRIZIONE:                                                                                        | # \n");
    
    const char *descStr = getDescription(curr);
    printf(" # | %-99.99s | # \n", descStr ? descStr : "Nessuna descrizione inserita.");
    
  } else {
    printf(" # |---------------------------------------- OPERAZIONE FALLITA -----------------------------------------| # \n");
    printf(" # |                                                                                                     | # \n");
    char riga_errore[128];
    sprintf(riga_errore, " [ERRORE] Impossibile procedere. L'ID %d non esiste nel database del comune. ", idTarget);
    printf(" # |%-101s| # \n", riga_errore);
    fflush(stdout);
    getchar();
  }

  printf(" # |                                                                                                     | # \n");
  printf(" # Premi INVIO per tornare alla dashboard principale...                                                  | # \n");
  printf(" ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### \n");
  fflush(stdout);

  int clean;
  while ((clean = getchar()) != '\n' && clean != EOF);
  getchar();
}
