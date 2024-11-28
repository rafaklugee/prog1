#ifndef ENTIDADES
#define ENTIDADES

#include "fprio.h"

// VER CJTO_T DE HABILIDADES...

// Cria estrutura do herói
struct heroi {
  int id;
  int *habilidades;					
  int paciencia;			
  int velocidade;
  int experiencia;
  int base_id;
  int status; // 1 = vivo; 0 = morto
};

struct base {
    int id;
    int lotacao;
    int espera;
    int local_x, local_y;
    int n_presentes;
    int n_missoes_base;
    struct heroi *presentes;
    struct fprio_t *fila;
};

struct missao {
    int id;
    int *habilidades;
    int perigo;
    int local_x, local_y;
    int status; // 1 = cumprida; 0 = descumprida
};

struct mundo {
    int n_herois;
    struct heroi *herois;
    int n_bases;
    struct base *bases;
    int n_missoes;
    struct missao *missoes;

    int n_cumpridas;
    int n_habilidades;
    int tam_mundo;
    int relogio;
};

struct evento {
    int instante;
    int tipo;
    struct heroi *h;
    struct base *b;
};

#endif