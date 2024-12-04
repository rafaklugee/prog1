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

// ARRUMAR ORDEM DOS PARÂMETROS (DEIXAR PADRONIZADO)

void chega (struct evento *ev, struct fprio_t *lef);

void espera (int instante, struct heroi *h, struct base *b, struct fprio_t *lef, struct lista_t *lst);

void desiste (int instante, struct heroi *h, struct base *b, struct mundo *w, struct fprio_t *lef);

void avisa (int instante, struct heroi *h, struct base *b, struct fprio_t *lef, struct lista_t *lst);

void entra (int instante, struct heroi *h, struct base *b, struct fprio_t *lef);

void sai (int instante, struct heroi *h, struct base *b, struct mundo *w, struct fprio_t *lef);

void viaja (int instante, struct heroi *h, struct base *i, struct base *d, struct fprio_t *lef);

void morre (int instante, struct missao *m, struct heroi *h, struct base *b, struct fprio_t *lef);

void missao (int instante, struct missao *m, struct mundo *w, struct fprio_t *lef);

void fim (struct mundo *w);

void eventos_iniciais (struct mundo *w, struct fprio_t *lef);

#endif
