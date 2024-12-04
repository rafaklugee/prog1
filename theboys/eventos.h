#ifndef EVENTOS
#define EVENTOS

#include "mundo.h"

#define EVENTO_DESISTE 0
#define EVENTO_ESPERA 1
#define EVENTO_AVISA 2
#define EVENTO_VIAJA 3
#define EVENTO_ENTRA 4
#define EVENTO_SAI 5
#define EVENTO_CHEGA 6
#define EVENTO_MORRE 7
#define EVENTO_MISSAO 8
#define EVENTO_FIM 9

void chega (struct evento *ev, struct fprio_t *lef);

void espera (struct evento *ev, struct fprio_t *lef);

void desiste (struct evento *ev, struct mundo *w, struct fprio_t *lef);

void avisa (struct evento *ev, struct fprio_t *lef);

void entra (struct evento *ev, struct fprio_t *lef);

void sai (struct evento *ev, struct mundo *w, struct fprio_t *lef);

void viaja (struct evento *ev, struct fprio_t *lef);

void morre (struct evento *ev, struct fprio_t *lef);

void missao (struct evento *ev, struct mundo *w, struct fprio_t *lef);

void fim (struct mundo *w);

void eventos_iniciais (struct mundo *w, struct fprio_t *lef);

#endif
