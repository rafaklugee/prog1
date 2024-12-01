#ifndef MUNDO
#define MUNDO

#include "conjunto.h"
#include "fprio.h"
#include "lista.h"

// Cria estrutura do herói
struct heroi {
  int id;			
  int paciencia;			
  int velocidade;
  int experiencia;
  int base_id;
  int status; // 1 = vivo; 0 = morto
  struct cjto_t *habilidades;
};

struct base {
    int id;
    int lotacao;
    int local_x, local_y;
    int n_missoes_base;
    int espera; // 1 = espera; 0 = não espera
    struct cjto_t *presentes;
    struct lista_t *lst_espera;
};

struct missao {
    int id;
    int perigo;
    int local_x, local_y;
    int n_tentativas;
    int status; // 1 = cumprida; 0 = descumprida
    struct cjto_t *habilidades;
};

struct mundo {
    int n_herois;
    struct heroi *herois;
    int n_bases;
    struct base *bases;
    int n_missoes;
    struct missao *missoes;
    int n_habilidades;

    int tempo_inicial;
    int tempo_final;
    int tam_mundo;

    int n_cumpridas;
    int relogio;
};

struct evento {
    int instante;
    int tipo;
    struct heroi *h;
    struct base *b;
};

// Protótipo de uma função auxiliar para extrair um número aleatório entre "ini" e "cap"
int extrai_aleat (int ini, int cap);

// Protótipo de uma função auxiliar para inicializar o mundo
void cria_mundo (struct mundo *w);

#endif