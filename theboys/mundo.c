#include <stdio.h>
#include <stdlib.h>
#include "mundo.h"
#include "conjunto.h"
#include "fprio.h"

void cria_world (struct mundo *w) {
    // Inicializando estados do iniciais do mundo
    w->tempo_inicial = 0;
    w->tempo_final = 525600;
    w->tam_mundo = 20000;

    w->n_habilidades = 3; // Mudar para 10
    w->n_herois = 15; // Mudar para 50
    w->n_bases = 3; // Mudar para 10
    w->n_missoes = w->tempo_final / 100;
    w->n_cumpridas = 0;

    w->herois->habilidades->cap = rand() % 10;
    int qtd_habilidades;
    int tam_cjto_habilidades;

    // Inicializando os heróis
    w->herois = malloc(sizeof(struct heroi) * w->n_herois);
        if (w->herois == NULL)
            return;
    for (int i = 0; i < w->n_herois; i++) {
        w->herois[i].id = i;
        w->herois[i].experiencia = 0;
        w->herois[i].paciencia = extrai_aleat(100);
        w->herois[i].velocidade = 50 + extrai_aleat(4950);
        qtd_habilidades = 1 + extrai_aleat(2);
        w->herois[i].habilidades = cjto_aleat(qtd_habilidades, 
                                            w->herois->habilidades->cap);
    }

    // Inicializando as bases
    w->bases = malloc(sizeof(struct heroi) * w->n_bases);
        if (w->bases == NULL)
            return;
    for (int i = 0; i < w->n_bases; i++) {
        w->bases[i].id = i;
        w->bases[i].local_x = extrai_aleat(w->tam_mundo - 1);
        w->bases[i].local_y = extrai_aleat(w->tam_mundo - 1);
        w->bases[i].lotacao = 3 + extrai_aleat(7);
        w->bases[i].presentes = cjto_cria(15);
        w->bases[i].lst_espera = lista_cria();
    }

    // Inicializando as missões
    w->missoes = malloc(sizeof(struct heroi) * w->n_missoes);
        if (w->missoes == NULL)
            return;
    for (int i = 0; i < w->n_missoes; i++) {
        w->missoes[i].id = i;
        w->missoes[i].local_x = extrai_aleat(w->tam_mundo - 1);
        w->missoes[i].local_y = extrai_aleat(w->tam_mundo - 1);
        w->missoes[i].perigo = extrai_aleat(100);
        tam_cjto_habilidades = 6 + extrai_aleat(4);

        // Criando um conjunto de habilidades
        tam_cjto_habilidades = 6 + extrai_aleat(4);
        w->missoes[i].habilidades = cjto_cria(tam_cjto_habilidades);
        if (w->missoes[i].habilidades == NULL)
            return;

        // Inserindo as habilidades no conjunto
        for (int j = 0; j < tam_cjto_habilidades; j++) {
            int habilidade = extrai_aleat(w->n_habilidades - 1);
            cjto_insere(w->missoes[i].habilidades, habilidade);
        }
    }







}