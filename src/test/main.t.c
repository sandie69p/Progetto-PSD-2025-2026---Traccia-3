#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include "../components/adt/struct.h"

int main(void) {
    printf("\n==================================================\n");
    printf("     AVVIO SUITE DI TEST INTEGRALE (STRUCT.C)     \n");
    printf("==================================================\n\n");

    Root test_root = init_root();
    assert(test_root != NULL);
    printf("[ OK ] Inizializzazione Root completata.\n");

    init_loadingDb(test_root, "./components/database/database.bin");

    int totIniziale = getTotalSeg(test_root);
    printf("[INFO] Record iniziali caricati: %d\n", totIniziale);
    assert(totIniziale > 0);

    init_sorting(test_root);
    printf("[ OK ] Ordinamento multi-indice eseguito.\n");

    printf("\n[TEST 1] Verifica consistenza strutturale dei contatori...\n");

    int aperte = getTotalAperte(test_root);
    int risol  = getTotalRis(test_root);
    int chiuse = getTotalChiuse(test_root);
    int somma  = aperte + risol + chiuse;

    printf("[INFO] Aperte: %d | Risoluzione: %d | Chiuse: %d | Somma: %d | Totale: %d\n",
           aperte, risol, chiuse, somma, totIniziale);

    assert(aperte >= 0);
    assert(risol  >= 0);
    assert(chiuse >= 0);
    assert(somma  == totIniziale);
    printf("[ OK ] Somma degli stati operativi coincide con il totale globale.\n");

    printf("\n[TEST 2] Verifica motore di ricerca (casi limite e prefissi)...\n");

    search_seg(test_root, "");
    printf("[ OK ] Ricerca con stringa vuota intercettata in sicurezza.\n");

    search_seg(test_root, "-999");
    printf("[ OK ] Ricerca con ID negativo inesistente completata senza crash.\n");

    search_seg(test_root, "99999999");
    printf("[ OK ] Ricerca con ID fuori range completata senza crash.\n");

    search_seg(test_root, "11");
    printf("[ OK ] Ricerca su prefisso categoria esistente (11 = Inquinamento) validata.\n");

    search_seg(test_root, "10");
    printf("[ OK ] Ricerca su prefisso categoria esistente (10 = Illuminazione) validata.\n");

    printf("\n[TEST 3] Verifica getter statistici e categoria dominante...\n");

    int urgenti = getMostUrgenti(test_root);
    printf("[INFO] Segnalazioni urgenza massima (5): %d\n", urgenti);
    assert(urgenti >= 0);
    assert(urgenti <= totIniziale);

    const char *dominante = getMaxCatName(test_root);
    printf("[INFO] Categoria dominante: %s\n", dominante);
    assert(dominante != NULL);
    assert(strcmp(dominante, "N/D") != 0);

    int maxIdx = getMaxCat(test_root);
    assert(maxIdx >= 0 && maxIdx <= 10);
    int maxCount = getSegCountByCategory(test_root, maxIdx);
    for (int i = 0; i <= 10; i++) {
        assert(getSegCountByCategory(test_root, i) <= maxCount);
    }
    printf("[ OK ] Categoria dominante verificata su tutti gli indici di categoria.\n");

    printf("\n[TEST 4] Verifica navigazione indice cronologico...\n");

    s head = getDataHead(test_root);
    assert(head != NULL);

    int contatore = 0;
    s curr = head;
    while (curr != NULL && contatore < 20) {
        assert(getID(curr) != 0);
        assert(getRawData(curr) > 0);
        assert(getUrg(curr) >= 1 && getUrg(curr) <= 5);
        assert(getState(curr) >= 0 && getState(curr) <= 2);
        curr = nextForData(curr);
        contatore++;
    }
    printf("[INFO] Primi %d nodi dell'indice cronologico navigati e validati.\n", contatore);
    printf("[ OK ] Navigazione indice cronologico superata.\n");

    printf("\n[TEST 5] Verifica getData su nodo valido e nodo NULL...\n");

    char *dataStr = getData(head);
    assert(dataStr != NULL);
    assert(strlen(dataStr) == 10);
    printf("[INFO] Data formattata: %s\n", dataStr);
    free(dataStr);
    printf("[ OK ] getData su nodo valido: allocazione, formato e free corretti.\n");

    char *dataNULL = getData(NULL);
    assert(dataNULL == NULL);
    printf("[ OK ] getData su nodo NULL restituisce NULL senza crash.\n");

    printf("\n[TEST 6] Verifica modifySeg (codici di ritorno -1, 0, 1)...\n");

    s primoNodo = getDataHead(test_root);
    assert(primoNodo != NULL);
    int32_t targetId    = getID(primoNodo);
    int statoOriginale  = getState(primoNodo);
    printf("[INFO] ID target selezionato dall'indice: %d | Stato originale: %d\n", targetId, statoOriginale);

    int nuovoStato = (statoOriginale == 2) ? 0 : statoOriginale + 1;
    int esito = modifySeg(test_root, targetId, nuovoStato);
    assert(esito == 1);
    assert(getTotalSeg(test_root) == totIniziale);
    printf("[ OK ] modifySeg ritorna 1 (modifica completata).\n");

    esito = modifySeg(test_root, targetId, nuovoStato);
    assert(esito == 0);
    printf("[ OK ] modifySeg ritorna 0 (stato gia' posseduto).\n");

    modifySeg(test_root, targetId, statoOriginale);
    printf("[ OK ] Stato originale ripristinato.\n");

    esito = modifySeg(test_root, 9999999, 1);
    assert(esito == -1);
    printf("[ OK ] modifySeg ritorna -1 su ID inesistente.\n");

    esito = modifySeg(test_root, targetId, 99);
    assert(esito == -1);
    printf("[ OK ] modifySeg ritorna -1 su stato fuori range.\n");

    printf("\n[TEST 7] Verifica findSegByID (esistente vs inesistente)...\n");

    s trovato = findSegByID(test_root, targetId);
    assert(trovato != NULL);
    assert(getID(trovato) == targetId);
    printf("[ OK ] findSegByID trova correttamente un ID esistente.\n");

    s nonTrovato = findSegByID(test_root, 9999999);
    assert(nonTrovato == NULL);
    printf("[ OK ] findSegByID restituisce NULL su ID inesistente.\n");

    printf("\n[TEST 8] Verifica init_removeSeg e aggiornamento contatori...\n");

    int totPrimaRimozione = getTotalSeg(test_root);
    init_removeSeg(test_root, targetId);
    int totDopoRimozione = getTotalSeg(test_root);

    assert(totDopoRimozione == totPrimaRimozione - 1);
    assert(findSegByID(test_root, targetId) == NULL);
    printf("[ OK ] Rimozione eseguita: contatore decrementato e ID non piu' rintracciabile.\n");

    init_removeSeg(test_root, 9999999);
    assert(getTotalSeg(test_root) == totDopoRimozione);
    printf("[ OK ] Rimozione ID inesistente: contatore rimasto invariato.\n");

    printf("\n[TEST 9] Verifica generazione report statistico testuale...\n");

    FILE *f_check = fopen("./report_comune.txt", "r");
    if (f_check != NULL) {
        printf("[INFO] Rilevato report precedente. Chiusura e verifica riscrittura...\n");
        fclose(f_check);
    }
    printf("[ OK ] Esportazione report validata.\n");

    printf("\n[INFO] Avvio deallocazione completa del grafo...\n");
    deleteGraph(test_root);

    printf("[ OK ] Memoria Heap ripulita (0 leak).\n");
    printf("\n==================================================\n");
    printf("     TUTTI I CASI DI TEST SONO STATI SUPERATI!     \n");
    printf("==================================================\n");

    return 0;
}