#ifndef STRUCT_H
#define STRUCT_H

struct root;
struct segnalazione;

typedef struct root *Root;
typedef struct segnalazione *s;

Root init_root();
void init_loadingDb(Root, const char *);
void init_sorting(Root);

// Funzioni di getter
int getTotalSeg(Root);
int getId(s);
const char *getName(s);
const char *getDesc(s);
int getRawData(s);
int getUrg(s);
int getState(s);

// Navigazione nella hud
s nextForID(s);
s nextForData(s);
s nextForUrg(s);

#endif
