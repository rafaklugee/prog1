#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mundo.h"
#include "fprio.h"
#include "lista.h"
#include "eventos.h"
#include "conjunto.h"

// Protótipo de uma função auxiliar para criar um evento
struct evento *cria_evento (int instante, int tipo, struct heroi *h, struct base *b);

void chega (int instante, struct heroi *h, struct base *b, struct fprio_t *LEF) {
    fprintf (stderr, "\nONLY BASE ID: %d\n", b->id);

    h->base_id = b->id;

    int tam_fila = lista_tamanho(b->lst_espera);
    // VER ESSA QUESTÃO DE FILA DE ESPERA VAZIA...
    // se há vagas em B e a fila de espera em B está vazia:
    if (b->lotacao != cjto_card(b->presentes))
        b->espera = 1;
    else
        b->espera = (h->paciencia) > (10 * tam_fila);

    if (b->espera) {
        //cria e insere na LEF o evento espera;
        struct evento *evento_espera = cria_evento(instante, EVENTO_ESPERA, h, b);
            if (!evento_espera)
                return;
        fprio_insere(LEF, evento_espera, EVENTO_ESPERA, instante);
        printf ("\nCHEGA HEROI %2d BASE %d (%2d/%2d) ESPERA", h->id, b->id, cjto_card(b->presentes), b->lotacao);
    }
    else {
        //cria e insere na LEF o evento desiste;
        struct evento *evento_desiste = cria_evento(instante, EVENTO_DESISTE, h, b);
            if (!evento_desiste)
                return;
        fprio_insere(LEF, evento_desiste, EVENTO_DESISTE, instante);
        printf ("\nCHEGA HEROI %2d BASE %d (%2d/%2d) DESISTE", h->id, b->id, cjto_card(b->presentes), b->lotacao);
    }
}

void espera (int instante, struct heroi *h, struct base *b, struct fprio_t *LEF, struct lista_t *lst) {
    printf ("\nESPERA HEROI %2d BASE %d (%2d)", h->id, b->id, lista_tamanho(b->lst_espera));
    
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
    printf ("\nDESISTE HEROI %2d BASE %d", h->id, b->id);
}

void avisa (int instante, struct heroi *h, struct base *b, struct fprio_t *LEF, struct lista_t *lst) {
    if (lista_tamanho(lst) > 0) {
        // enquanto houver vaga em B e houver heróis esperando na fila:
        while (b->lotacao != cjto_card(b->presentes) && b->espera) {
            lista_retira (lst, &h->id, 0);
            b->espera = 0;
            
            printf ("\nAVISA PORTEIRO BASE %d (%2d/%2d) FILA [", b->id, b->presentes->num, b->lotacao);
            struct item_t *atual = lst->prim;
            for (int i = 0; i < lst->tamanho; i++) {
                printf (" %2d", atual->valor);
                atual = atual->prox;
            }
            printf ("]");

            // adiciona H' ao conjunto de heróis presentes em B
            h->base_id = b->id;
            
            // cria e insere na LEF o evento ENTRA (agora, H', B)
            struct evento *evento_entra = cria_evento(instante, EVENTO_ENTRA, h, b);
            fprio_insere (LEF, evento_entra, EVENTO_ENTRA, instante);
            printf ("\nAVISA PORTEIRO BASE %d ADMITE %2d", b->id, h->id);
        }
    }
    else
        return;
}

void entra (int instante, struct heroi *h, struct base *b, struct fprio_t *LEF) {
    // Extraindo um número aleatório entre [1..20]
    int random = extrai_aleat(1, 20);

    // calcula TPB = tempo de permanência na base
    int TPB = 15 + h->paciencia * random;

    // cria e insere na LEF o evento SAI (agora + TPB, H, B)
    struct evento *evento_sai = cria_evento(instante + TPB, EVENTO_SAI, h, b);
    fprio_insere (LEF, evento_sai, EVENTO_SAI, instante);
    printf ("\nENTRA HEROI %2d BASE %d (%2d/%2d) SAI %d", h->id, b->id, cjto_card(b->presentes), b->lotacao, instante + TPB);
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
    printf ("\nSAI HEROI %2d BASE %d (%2d/%2d)", h->id, b->id, cjto_card(b->presentes), b->lotacao);
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
    printf ("\nVIAJA HEROI %2d BASE %d BASE %d DIST %d VEL %d CHEGA %d", h->id, i->id, d->id, distancia, h->velocidade, instante + duracao);
}

void morre (int instante, struct mundo *w, struct heroi *h, struct base *b, struct fprio_t *LEF, struct lista_t *lst) {
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
    printf ("\nMORRE HEROI %2d MISSAO %d", h->id, w->missoes->id);
}

void missao (int instante, struct missao *m, struct mundo *w, struct fprio_t *LEF) {
    int i, distancia, risco;
    int dis_menor;
    struct base *base_proxima = NULL;

    // pega o endereço da primeira base e inicializa como dis_menor
    struct base *base_ini = &w->bases[0];
    dis_menor = sqrt (pow(m->local_x - base_ini->local_x, 2) + pow(m->local_y - base_ini->local_y, 2));

    // extraindo um número aleatório entre [0..30]
    int random = extrai_aleat(0, 30);
    
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

    printf ("\nMISSAO %d TENT %d HAB REQ: [", m->id, m->n_tentativas++);
    for (int i = 0; i < m->habilidades->cap; i++) {
        if (m->habilidades->flag[i])
            printf (" %d", i);
    }
    printf ("]");

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
            else {
                h->experiencia++;
                w->n_cumpridas++;
                printf ("\nMISSAO %d CUMPRIDA BASE %d HABS: [", m->id, base_proxima->id);
                for (int i = 0; i < m->habilidades->cap; i++) {
                    if (m->habilidades->flag[i])
                    printf (" %d", i);
                }
                printf ("]");
            }
        }
    }
    // senão
    else {
        for (i = 0; i < w->n_herois; i++) {
        struct heroi *h = &w->herois[i];

        struct evento *evento_missao = cria_evento(instante, EVENTO_MISSAO, h, base_proxima);
        fprio_insere(LEF, evento_missao, EVENTO_MISSAO, instante + 24*60);
        printf ("\nMISSAO %d IMPOSSIVEL", m->id);
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

void eventos_iniciais (struct mundo *w, struct fprio_t *LEF) {
    int instante;

    // cada herói chegará em uma base aleatória em algum momento dos três primeiros dias de simulação
    for (int i = 0; i < w->n_herois; i++) {
        w->herois[i].base_id = extrai_aleat (0, w->n_bases - 1);
        instante = extrai_aleat (0, 4320);
        struct evento *evento_chega = cria_evento(instante, EVENTO_CHEGA, &w->herois[i], &w->bases[i]); // Aqui vai dar pau
        //if (!evento_chega) {
        //    printf ("\nerro");
        //    return;
        //}
        fprio_insere(LEF, evento_chega, EVENTO_CHEGA, instante);
    }

    printf ("\n");

    // cada missão deve ser agendada para ocorrer em algum momento da simulação
    for (int i = 0; i < w->n_missoes; i++) {
        //instante = extrai_aleat (0, w->tempo_final);
        struct evento *evento_missao = cria_evento(8888, EVENTO_MISSAO, &w->herois[i], &w->bases[i]); // Aqui também
        fprio_insere(LEF, evento_missao, EVENTO_MISSAO, instante);
    }

    // o evento FIM deve ser agendado para o instante final da simulação
    instante = w->tempo_final;
    struct evento *evento_fim = cria_evento(instante, EVENTO_FIM, w->herois, w->bases);
    fprio_insere(LEF, evento_fim, EVENTO_FIM, instante);
}

struct evento *cria_evento (int instante, int tipo, struct heroi *h, struct base *b) {
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

int extrai_aleat (int ini, int cap) {
    if (cap <= 0)
        return -1;

    int valor = ini + rand() % (cap - ini + 1);

    return valor;
}