#ifndef STRUCT_H
#define STRUCT_H

struct root;
struct segnalazione;

typedef struct root *Root;
typedef struct segnalazione *s;

Root init_root();
void init_loadingDb(Root, const char *);
void init_sorting(Root);

#endif
