#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "entidades.h"
#include "fprio.h"
#include "lista.h"
#include "conjunto.h"

// cria e insere na LEF o evento CHEGA (agora + duração, H, D) -> cria ??
// ver como funcionam as listas (?)
// tirar esses defines

#define EVENTO_DESISTE 0
#define EVENTO_ESPERA 1
#define EVENTO_AVISA 2
#define EVENTO_VIAJA 3
#define EVENTO_ENTRA 4
#define EVENTO_SAI 5
#define EVENTO_CHEGA 6

// Protótipo de uma função auxiliar "cria_evento"
struct evento *cria_evento(int instante, int tipo, struct heroi *h, struct base *b);

void chega (int instante, struct heroi *h, struct base *b, struct fprio_t *LEF) {
    h->base_id = b->id;

    int tam_fila = fprio_tamanho(b->fila);

    if (b->lotacao != b->num_presentes && b->espera == 0)
        b->espera = 1;
    else
        b->espera = (h->paciencia) > (10 * tam_fila);

    if (b->espera) {
        //cria e insere na LEF o evento espera;
        struct evento *evento_espera = cria_evento(instante, EVENTO_ESPERA, h, b);
            if (!evento_espera)
                return;
        fprio_insere(LEF, evento_espera, EVENTO_ESPERA, instante);
    }
    else {
        //cria e insere na LEF o evento desiste;
        struct evento *evento_desiste = cria_evento(instante, EVENTO_DESISTE, h, b);
            if (!evento_desiste)
                return;
        fprio_insere(LEF, evento_desiste, EVENTO_DESISTE, instante);
    }
}

void espera (int instante, struct heroi *h, struct base *b, struct fprio_t *LEF) {
    struct evento *evento_avisa = cria_evento(instante, EVENTO_AVISA, h, b);
        if (!evento_avisa)
            return;
    
    // adiciona H ao fim da fila de espera de B
    fprio_insere (LEF, h, EVENTO_ESPERA, instante);
    //cria e insere na LEF o evento AVISA (agora, B)
    fprio_insere (LEF, evento_avisa, EVENTO_AVISA, instante);
}

void avisa (int instante, struct heroi *h, struct base *b, struct fprio_t *LEF) {
    struct evento *evento_entra = cria_evento(instante, EVENTO_ENTRA, h, b);

    while (b->lotacao != b->num_presentes && b->espera) {
        // retira primeiro herói (H') da fila de B
        fprio_retira (b->fila, h, instante);
        // adiciona H' ao conjunto de heróis presentes em B
        h->base_id = b->id;
        //cria e insere na LEF o evento ENTRA (agora, H', B)
        fprio_insere (LEF, evento_entra, EVENTO_ENTRA, instante);
    }
}

void entra (int instante, struct heroi *h, struct base *b, struct fprio_t *LEF) {
    struct evento *evento_sai = cria_evento(instante, EVENTO_SAI, h, b);
    
    struct cjto_t *aleat = cjto_aleat(1, 20);
    if (!aleat)
        return;

    int random = extrai_aleat(aleat);

    // calcula TPB = tempo de permanência na base
    int TPB = 15 + h->paciencia * random;

    // cria e insere na LEF o evento SAI (agora + TPB, H, B)
    fprio_insere (LEF, evento_sai, EVENTO_SAI, instante);
}

void viaja (int instante, struct heroi *h, struct base *i, struct base *d, struct fprio_t *LEF) {
    //calcula duração da viagem
    int distancia;
    distancia = sqrt (pow(d->local_x - i->local_x, 2) + pow(d->local_y - i->local_y, 2));
    int duracao;
    duracao = distancia / h->velocidade;

    struct evento *evento_chega = cria_evento(instante, EVENTO_CHEGA, h, d);

    //cria e insere na LEF o evento CHEGA (agora + duração, H, D)
    fprio_insere (LEF, evento_chega, EVENTO_CHEGA, instante);
}

void morre (int instante, struct heroi *h, struct base *b, struct fprio_t *LEF) {
    //retira H do conjunto de heróis presentes em B
    lista_retira(b, h, h->base_id);

    // muda o status de H para morto 
    h->status = 0;

    // cria e insere na LEF o evento AVISA (agora, B)
    struct evento *evento_avisa = cria_evento(instante, EVENTO_AVISA, h, b);

    fprio_insere (LEF, evento_avisa, EVENTO_AVISA, instante);
}

void missao (int instante, struct missao *m) {
/*
MISSAO (T, M):

calcula a distância de cada base ao local da missão M
encontra BMP = base mais próxima da missão cujos heróis possam cumpri-la 
se houver uma BMP:
    marca a missão M como cumprida
    para cada herói H presente na BMP:
        risco = perigo (M) / (paciência (H) + experiência (H) + 1.0)
        se risco > aleatório (0, 30):
            cria e insere na LEF o evento MORRE (agora, H)
        senão:
            incrementa a experiência de H
senão:
    cria e insere na LEF o evento MISSAO (T + 24*60, M) para o dia seguinte
*/
}

void fim (int instante) {
/*
encerra a simulação
apresenta estatísticas dos heróis
apresenta estatísticas das bases
apresenta estatísticas das missões
*/
}

struct evento *cria_evento(int instante, int tipo, struct heroi *h, struct base *b) {
    struct evento *novo_evento = malloc(sizeof(struct evento));
    if (!novo_evento) {
        return NULL; 
    }

    novo_evento->instante = instante;
    novo_evento->tipo = tipo;
    novo_evento->h = h;
    novo_evento->b = b;

    return novo_evento;
}