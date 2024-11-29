#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mundo.h"
#include "fprio.h"
#include "lista.h"
#include "eventos.h"
#include "conjunto.h"

#define EVENTO_DESISTE 0
#define EVENTO_ESPERA 1
#define EVENTO_AVISA 2
#define EVENTO_VIAJA 3
#define EVENTO_ENTRA 4
#define EVENTO_SAI 5
#define EVENTO_CHEGA 6
#define EVENTO_MORRE 7
#define EVENTO_MISSAO 8

// Protótipo de uma função auxiliar para criar um evento
struct evento *cria_evento (int instante, int tipo, struct heroi *h, struct base *b);

void chega (int instante, struct heroi *h, struct base *b, struct fprio_t *LEF) {
    h->base_id = b->id;

    int tam_fila = lista_tamanho(b->lst_espera);

    if (b->lotacao != b->n_presentes && b->espera == 0)
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

void espera (int instante, struct heroi *h, struct base *b, struct fprio_t *LEF, struct lista_t *lst) {
    // adiciona H ao fim da fila de espera de B
    lista_insere(lst, h->id, -1);

    //cria e insere na LEF o evento AVISA (agora, B)
    struct evento *evento_avisa = cria_evento(instante, EVENTO_AVISA, h, b);
        if (!evento_avisa)
            return;
    fprio_insere (LEF, evento_avisa, EVENTO_AVISA, instante);
}

void desiste (int instante, struct heroi *h, struct base *b, struct mundo *w, struct fprio_t *LEF) {
    // escolhe uma base destino D aleatória
    struct base *b_aleat = &b[rand() % w->n_bases];

    // cria e insere na LEF o evento VIAJA (agora, H, D)
    struct evento *evento_viaja = cria_evento(instante, EVENTO_VIAJA, h, b_aleat);
        if (!evento_viaja)
            return;
    fprio_insere (LEF, evento_viaja, EVENTO_VIAJA, instante);
}

void avisa (int instante, struct heroi *h, struct base *b, struct fprio_t *LEF, struct lista_t *lst) {
    while (b->lotacao != b->n_presentes && b->espera) {
        // retira primeiro herói (H') da fila de B
        lista_retira (lst, &h->id, lst->prim->valor);

        // adiciona H' ao conjunto de heróis presentes em B
        h->base_id = b->id;

        // cria e insere na LEF o evento ENTRA (agora, H', B)
        struct evento *evento_entra = cria_evento(instante, EVENTO_ENTRA, h, b);
        fprio_insere (LEF, evento_entra, EVENTO_ENTRA, instante);
    }
}

void entra (int instante, struct heroi *h, struct base *b, struct fprio_t *LEF) {
    // Extraindo um número aleatório entre [0..20]
    int random = extrai_aleat(20);

    // calcula TPB = tempo de permanência na base
    int TPB = 15 + h->paciencia * random;

    // cria e insere na LEF o evento SAI (agora + TPB, H, B)
    struct evento *evento_sai = cria_evento(instante + TPB, EVENTO_SAI, h, b);
    fprio_insere (LEF, evento_sai, EVENTO_SAI, instante);
}

void sai (int instante, struct heroi *h, struct base *b, struct mundo *w, struct fprio_t *LEF, struct lista_t *lst) {
    // retira H do conjunto de heróis presentes em B
    int pos_heroi = lista_procura(lst, h->id);
        if (pos_heroi == -1)
            return;
    lista_retira(lst, &h->id, pos_heroi);

    // escolhe uma base destino D aleatória
    struct base *b_aleat = &b[rand() % w->n_bases];

    // cria e insere na LEF o evento VIAJA (agora, H, D)
    struct evento *evento_viaja = cria_evento(instante, EVENTO_VIAJA, h, b_aleat);
    fprio_insere (LEF, evento_viaja, EVENTO_VIAJA, instante);

    // cria e insere na LEF o evento AVISA (agora, B)
    struct evento *evento_avisa = cria_evento(instante, EVENTO_AVISA, h, b);
    fprio_insere (LEF, evento_avisa, EVENTO_AVISA, instante);
}

void viaja (int instante, struct heroi *h, struct base *i, struct base *d, struct fprio_t *LEF) {
    // calcula duração da viagem
    int distancia;
    distancia = sqrt (pow(d->local_x - i->local_x, 2) + pow(d->local_y - i->local_y, 2));
    int duracao;
    duracao = distancia / h->velocidade;

    // cria e insere na LEF o evento CHEGA (agora + duração, H, D)
    struct evento *evento_chega = cria_evento(instante + duracao, EVENTO_CHEGA, h, d);
    fprio_insere (LEF, evento_chega, EVENTO_CHEGA, instante);
}

void morre (int instante, struct heroi *h, struct base *b, struct fprio_t *LEF, struct lista_t *lst) {
    // retira H do conjunto de heróis presentes em B
    int pos_heroi = lista_procura(lst, h->id);
        if (pos_heroi == -1)
            return;
    lista_retira(lst, &h->id, pos_heroi);

    // muda o status de H para morto 
    h->status = 0;

    // cria e insere na LEF o evento AVISA (agora, B)
    struct evento *evento_avisa = cria_evento(instante, EVENTO_AVISA, h, b);
    fprio_insere (LEF, evento_avisa, EVENTO_AVISA, instante);
}

void missao (int instante, struct missao *m, struct mundo *w, struct fprio_t *LEF) {
    int i, distancia, risco;
    int dis_menor;
    struct base *base_proxima;

    // Pega o endereço da primeira base e inicializa como dis_menor
    struct base *base_ini = &w->bases[0];
    dis_menor = sqrt (pow(m->local_x - base_ini->local_x, 2) + pow(m->local_y - base_ini->local_y, 2));

    // Extraindo um número aleatório entre [0..30]
    int random = extrai_aleat(30);
    
    // calcula a distância de cada base ao local da missão M
    // encontra BMP = base mais próxima da missão cujos heróis possam cumpri-la 
    for (i = 0; i < w->n_bases; i++) {
        struct base *base_atual = &w->bases[i]; // Pega o endereço das bases
        distancia = sqrt (pow(m->local_x - base_atual->local_x, 2) + pow(m->local_y - base_atual->local_y, 2));

        if (distancia < dis_menor) {
            dis_menor = distancia;
            base_proxima = base_atual;
        }
    }

    // se houver uma BMP
    if (base_proxima) {
        m->status = 1; // missão cumprida
        for (i = 0; i < w->n_herois; i++) {
            struct heroi *h = &w->herois[i];
            risco = m->perigo / ((h->paciencia) + h->experiencia + 1);
            if (risco > random) {
                struct evento *evento_morre = cria_evento(instante, EVENTO_MORRE, h, base_proxima);
                fprio_insere(LEF, evento_morre, EVENTO_MORRE, instante);
            }
            else
                h->experiencia++;
        }
    }
    // senão
    else {
        for (i = 0; i < w->n_herois; i++) {
        struct heroi *h = &w->herois[i];

        struct evento *evento_missao = cria_evento(instante, EVENTO_MISSAO, h, base_proxima);
        fprio_insere(LEF, evento_missao, EVENTO_MISSAO, instante + 24*60);
        }
    }
}

void fim (struct mundo *w) {
    // PRECISA USAR "int instante" NOS PARÂMETROS DA FUNÇÃO

    int i;
    float missoes_t_c = w->n_missoes/w->n_cumpridas;

    // A RESOLVER...
    // VER LOTACAO MAXIMA DA FILA, MIN MISSAO, MAX MISSÃO, MEDIA TENTATIVAS/MISSAO
    // VER CJTO_T DAS HABILIDADES
    int min_missao = 0, max_missao = 0, fila_max = 0;
    int habilidades = 0;
    float taxa_mortalidade = 0, media_missao = 0;

    // estatísticas específicas
    for (i = 0; i < w->n_herois; i++) {
            struct heroi *h = &w->herois[i];
            if (h->status == 1)
                printf ("\nHEROI %2d VIVO PAC %4d EXP %4d HABS [%d]", h->id, h->paciencia, h->experiencia, habilidades);
            else
                printf ("\nHEROI %2d MORTO PAC %4d EXP %4d HABS [%d]", h->id, h->paciencia, h->experiencia, habilidades);

            struct base *b = &w->bases[i];
            // VER LOTACAO MAXIMA DA FILA, MIN MISSAO, MAX MISSÃO, MEDIA TENTATIVAS/MISSAO
            printf ("BASE %2d LOT %2d FILA MAX %2d MISSOES %d", b->id, b->lotacao, fila_max, b->n_missoes_base);
    }
    // estatísticas gerais
        printf ("\nEVENTOS TRATADOS: %d", w->n_missoes);
        printf ("\nMISSOES CUMPRIDAS: %d/%d (%.1f%%)", w->n_missoes, w->n_cumpridas, missoes_t_c);
        printf ("\nTENTATIVAS/MISSAO: MIN %d, MAX %d, MEDIA %.1f", min_missao, max_missao, media_missao);
        printf ("\nTAXA MORTALIDADE: %.1f%%", taxa_mortalidade);
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

int extrai_aleat (int cap) {
    if (cap <= 0)
        return -1;

    int valor = rand() % cap;

    return valor;
}