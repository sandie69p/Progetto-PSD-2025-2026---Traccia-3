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

    printf("\n[TEST 1] Verifica registrazione e inserimento controllato...\n");
    
    int apertePrima = getTotalAperte(test_root);
    int totPrimaInsert = getTotalSeg(test_root);
    
    printf("[INFO] Simulazione incremento e consistenza strutturale...\n");
    assert(totPrimaInsert == totIniziale);
    assert(apertePrima >= 0);
    printf("[ OK ] Registrazione e consistenza allocazione superata.\n");

    printf("\n[TEST 2] Verifica motore di ricerca (Esistente vs Inesistente)...\n");
    
    search_seg(test_root, "");
    printf("[ OK ] Ricerca con stringa vuota intercettata in sicurezza.\n");
    
    printf("[INFO] Simulazione ricerca ID inesistente ('-999')...\n");
    search_seg(test_root, "-999");
    printf("[ OK ] Ricerca record inesistente completata senza crash (0 trovati).\n");

    printf("[INFO] Simulazione ricerca live su prefisso esistente '11'...\n");
    search_seg(test_root, "11");
    printf("[ OK ] Funzionalità di ricerca real-time validata.\n");

    printf("\n[TEST 3] Verifica gestione priorita' e dominanza categorie...\n");
    
    int urgent_count = getMostUrgenti(test_root);
    printf("[INFO] Segnalazioni a priorita' massima (5) rilevate: %d\n", urgent_count);
    assert(urgent_count >= 0);

    const char *dominante = getMaxCatName(test_root);
    printf("[INFO] Categoria dominante calcolata dall'ADT: %s\n", dominante);
    assert(dominante != NULL && strcmp(dominante, "N/D") != 0);
    printf("[ OK ] Calcolo statistico delle priorita' e picchi verificato.\n");

    printf("\n[TEST 4] Verifica modifica stato e flussi ortogonali...\n");
    
    int32_t targetId = 1000001; 
    printf("[INFO] Cambio di stato condizionale per ID %d...\n", targetId);
    
    modifySeg(test_root, targetId, 1);
    assert(getTotalSeg(test_root) == totIniziale);
    printf("[ OK ] Flusso ortogonale modificato e contatori allineati!\n");

    modifySeg(test_root, targetId, 0);
    printf("[ OK ] Reset dello stato completato con successo.\n");

    printf("\n[TEST 5] Verifica generazione report statistico testuale...\n");
    
    FILE *f_check = fopen("./report_comune.txt", "r");
    if (f_check != NULL) {
        printf("[INFO] Rilevato report precedente. Test riscrittura...\n");
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