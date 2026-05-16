#ifndef STRUCT_H
#define STRUCT_H

struct root;
struct segnalazione;

typedef struct root *Root;
typedef struct segnalazione *s;

Root init_root();
void init_loadingDb(Root, const char *);
void init_sorting(Root);
void init_removeSeg(Root, int);

// Funzioni di getter globali
int getTotalSeg(Root);
int getTotalAperte(Root);
int getTotalRis(Root);
int getTotalChiuse(Root);
int getMostUrgenti(Root);
s getDataHead(Root);

// funzioni di getter per segnalazione
int getID(s);
const char *getName(s);
const char *getDesc(s);
int getRawData(s);
int getUrg(s);
int getState(s);
char *getData(s);

// Navigazione nella hud
s nextForID(s);
s nextForData(s);
s nextForUrg(s);

void getNewSeg(Root);
void save_records(Root);
void deleteGraph(Root);
void search_seg(Root, const char *);

#endif
