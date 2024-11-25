#ifndef ENTIDADES
#define ENTIDADES

#include "fprio.h"

// Cria estrutura do herói
struct heroi {
  int id;
  int *habilidades;					
  int paciencia;			
  int velocidade;
  int experiencia;
  int base_id;
  int status;
};

struct base {
    int id;
    int lotacao;
    int espera;
    int local_x, local_y;
    int num_presentes;
    struct heroi *presentes;
    struct fprio_t *fila;
};

struct missao {
    int id;
    int *habilidades;
    int perigo;
    int local_x, local_y;
};

struct mundo {
    int n_herois;
    int herois[15];
    int n_bases;
    int bases[3];
    int n_missoes;
    int missoes[5];
    int n_habilidades;
    int tam_mundo;
    int relogio;
};

struct evento {
    int instante;
    int tipo; // espera, desiste, avisa
    struct heroi *h;
    struct base *b;
};

#endif