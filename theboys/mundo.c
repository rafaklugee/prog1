#include <stdio.h>
#include <stdlib.h>
#include "mundo.h"
#include "conjunto.h"
#include "fprio.h"

void cria_mundo (struct mundo *w) {
    // Inicializando estados do iniciais do mundo
    w->tempo_inicial = 0;
    w->tempo_final = 50000;
    //w->tempo_final = 525600 ;
    w->tam_mundo = 20000;

    w->n_habilidades = 10;
    w->n_herois = 50;
    w->n_bases = 10;
    w->n_missoes = w->tempo_final / 100;
    w->n_cumpridas = 0;

    int tam_cjto_habilidades;

    // Inicializando os heróis
    w->herois = malloc(sizeof(struct heroi) * w->n_herois);
    if (!w->herois)
        return;

    for (int i = 0; i < w->n_herois; i++) {
        w->herois[i].id = i;
        w->herois[i].experiencia = 0;
        w->herois[i].status = 1;
        w->herois[i].paciencia = extrai_aleat(0, 100);
        w->herois[i].velocidade = extrai_aleat(50, 5000);
        w->herois[i].base_id = -1;

        // Criando um conjunto de habilidades
        tam_cjto_habilidades = extrai_aleat(1, 3);
        w->herois[i].habilidades = cjto_cria(w->n_habilidades);
        if (w->herois[i].habilidades == NULL)
            return;

        // Inserindo as habilidades no conjunto
        for (int k = 0; k < tam_cjto_habilidades; k++) {
            int habilidade = extrai_aleat(0, w->n_habilidades - 1);
            cjto_insere(w->herois[i].habilidades, habilidade);
        }
        // Verificando se as habilidades foram inseridas
        //printf ("HEROI %d PAC: %d HABILIDADES: [", i, w->herois[i].paciencia);
        //for (int j = 0; j < w->n_habilidades; j++) {
        //    if (w->herois[i].habilidades->flag[j])
        //        printf (" %d", j);
        //    }
        //printf (" ]\n");
    }

    // Inicializando as bases
    w->bases = malloc(sizeof(struct base) * w->n_bases);
    if (!w->bases)
        return;

    for (int i = 0; i < w->n_bases; i++) {
        w->bases[i].id = i;
        w->bases[i].local_x = extrai_aleat(0, w->tam_mundo - 1);
        w->bases[i].local_y = extrai_aleat(0, w->tam_mundo - 1);
        w->bases[i].lotacao = extrai_aleat(3, 10);
        w->bases[i].n_missoes_base = 0;
        w->bases[i].fila_max = 0;
        w->bases[i].presentes = cjto_cria(w->n_herois);
        w->bases[i].lst_espera = lista_cria();

        if (w->bases[i].presentes == NULL || w->bases[i].lst_espera == NULL)
            return;
    }

    // Inicializando as missões
    w->missoes = malloc(sizeof(struct missao) * w->n_missoes);
    if (!w->missoes)
        return;

    for (int i = 0; i < w->n_missoes; i++) {
        w->missoes[i].id = i;
        w->missoes[i].local_x = extrai_aleat(0, w->tam_mundo - 1);
        w->missoes[i].local_y = extrai_aleat(0, w->tam_mundo - 1);
        w->missoes[i].perigo = extrai_aleat(0, 100);
        w->missoes[i].n_tentativas = 0;
        w->missoes[i].status = 0;

        // Criando um conjunto de habilidades
        tam_cjto_habilidades = extrai_aleat(6, 10);
        w->missoes[i].habilidades = cjto_cria(w->n_missoes);
        if (w->missoes[i].habilidades == NULL)
            return;

        // Inserindo as habilidades no conjunto
        for (int j = 0; j < tam_cjto_habilidades; j++) {
            int habilidade = extrai_aleat(0, w->n_habilidades - 1);
            cjto_insere(w->missoes[i].habilidades, habilidade);
        }
        //printf ("\nMISSAO %d HABS: [", i);
        //cjto_imprime(w->missoes[i].habilidades);
        //printf (" ]\n");

        // Verificando se as habilidades foram inseridas
        //for (int j = 0; j < w->n_missoes; j++) {
        //    if (w->missoes[i].habilidades->flag[j])
        //        printf (" %d", j);
        //    }
        //printf ("\n");
    }
}
