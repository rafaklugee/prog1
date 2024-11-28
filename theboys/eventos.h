#ifndef ENTIDADES
#define ENTIDADES

void chega (int instante, struct heroi *h, struct base *b, struct fprio_t *LEF);

void espera (int instante, struct heroi *h, struct base *b, struct fprio_t *LEF, struct lista_t *lst);

void desiste (int instante, struct heroi *h, struct fprio_t *LEF);

void avisa (int instante, struct heroi *h, struct base *b, struct fprio_t *LEF);

void entra (int instante, struct heroi *h, struct base *b, struct fprio_t *LEF);

void sai (int instante, struct heroi *h, struct base *b, struct fprio_t *LEF);

void viaja (int instante, struct heroi *h, struct base *i, struct base *d, struct fprio_t *LEF);

void morre (int instante, struct heroi *h, struct base *b, struct fprio_t *LEF);

void missao (int instante, struct missao *m);

void fim (struct mundo *w);
#endif