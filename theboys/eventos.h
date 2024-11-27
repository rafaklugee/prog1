#ifndef ENTIDADES
#define ENTIDADES

// Protótipo de uma função auxiliar "cria_evento"
struct evento *cria_evento(int instante, int tipo, struct heroi *h, struct base *b);

void chega (int instante, struct heroi *h, struct base *b, struct fprio_t *LEF);

void espera (int instante, struct heroi *h, struct base *b, struct fprio_t *LEF);

void desiste (int instante, struct heroi *h, struct base *b, struct fprio_t *LEF)

void avisa (int instante, struct heroi *h, struct base *b, struct fprio_t *LEF);

void entra (int instante, struct heroi *h, struct base *b, struct fprio_t *LEF);

void sai (int instante, struct heroi *h, struct base *b, struct fprio_t *LEF);

void viaja (int instante, struct heroi *h, struct base *i, struct base *d, struct fprio_t *LEF);

void morre (int instante, struct heroi *h, struct base *b, struct fprio_t *LEF);

void missao (int instante, struct missao *m);

void fim (int instante);
#endif