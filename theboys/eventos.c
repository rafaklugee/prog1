#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mundo.h"
#include "fprio.h"
#include "lista.h"
#include "eventos.h"
#include "conjunto.h"

// protótipo de uma função auxiliar para criar um evento
struct evento *cria_evento (int instante, int tipo, struct heroi *h, struct base *b, struct base *b_aux, struct missao *m);

// variáveis globais para colocar nas estatísticas finais
int eventos_tratados = 0;
int soma_missoes = 0;
int max_missao = 0;
int min_missao = __INT_MAX__;

void chega (struct evento *ev, struct fprio_t *lef) {
    cjto_insere(ev->b->presentes, ev->h->id);

    printf ("heroi %d chegou na base %d\n", ev->h->id, ev->b->id);

    int tam_fila = lista_tamanho(ev->b->lst_espera);

    // se há vagas em B e a fila de espera em B está vazia:
    if (ev->b->lotacao != cjto_card(ev->b->presentes) && tam_fila == 0)
        ev->b->espera = 1;
    else
        ev->b->espera = (ev->h->paciencia) > (10 * tam_fila);

    if (ev->b->espera) {
        //cria e insere na lef o evento espera;
        struct evento *evento_espera = cria_evento(ev->instante, EVENTO_ESPERA, ev->h, ev->b, NULL, NULL);
            if (!evento_espera)
                return;
        fprio_insere(lef, evento_espera, EVENTO_ESPERA, ev->instante);
        printf ("%d: CHEGA HEROI %2d BASE %d (%2d/%2d) ESPERA\n", ev->instante, ev->h->id, ev->b->id, cjto_card(ev->b->presentes), ev->b->lotacao);
    }
    else {
        //cria e insere na lef o evento desiste;
        struct evento *evento_desiste = cria_evento(ev->instante, EVENTO_DESISTE, ev->h, ev->b, NULL, NULL);
            if (!evento_desiste)
                return;
        fprio_insere(lef, evento_desiste, EVENTO_DESISTE, ev->instante);
        printf ("%d: CHEGA HEROI %2d BASE %d (%2d/%2d) DESISTE\n", ev->instante, ev->h->id, ev->b->id, cjto_card(ev->b->presentes), ev->b->lotacao);
    }
}

void espera (int instante, struct heroi *h, struct base *b, struct fprio_t *lef, struct lista_t *lst) {
    printf ("%d: ESPERA HEROI %2d BASE %d (%2d)\n", instante, h->id, b->id, lista_tamanho(b->lst_espera));
    
    if (lista_tamanho(b->lst_espera) > b->fila_max)
        b->fila_max = lista_tamanho(b->lst_espera);
    
    // adiciona H ao fim da fila de espera de B
    lista_insere(lst, h->id, -1);

    //cria e insere na lef o evento AVISA (agora, B)
    struct evento *evento_avisa = cria_evento(instante, EVENTO_AVISA, h, b, NULL, NULL);
        if (!evento_avisa)
            return;
    fprio_insere (lef, evento_avisa, EVENTO_AVISA, instante);
}

void desiste (int instante, struct heroi *h, struct base *b, struct mundo *w, struct fprio_t *lef) {
    // escolhe uma base destino D aleatória
    struct base *b_aleat = &w->bases[rand() % w->n_bases];

    // cria e insere na lef o evento VIAJA (agora, H, D)
    struct evento *evento_viaja = cria_evento(instante, EVENTO_VIAJA, h, b, b_aleat, NULL);
        if (!evento_viaja)
            return;
    fprio_insere (lef, evento_viaja, EVENTO_VIAJA, instante);
    printf ("%d: DESISTE HEROI %2d BASE %d\n", instante, h->id, b->id);
}

void avisa (int instante, struct heroi *h, struct base *b, struct fprio_t *lef, struct lista_t *lst) {
    if (lista_tamanho(lst) > 0) {
        // enquanto houver vaga em B e houver heróis esperando na fila:
        while (b->lotacao != cjto_card(b->presentes) && b->espera) {
            lista_retira (lst, &h->id, 0);
            b->espera = 0;
            
            printf ("%d: AVISA PORTEIRO BASE %d (%2d/%2d) FILA [", instante, b->id, cjto_card(b->presentes), b->lotacao);
            struct item_t *atual = lst->prim;
            for (int i = 0; i < lst->tamanho; i++) {
                printf (" %2d", atual->valor);
                atual = atual->prox;
            }
            printf (" ]\n");

            // adiciona H' ao conjunto de heróis presentes em B
            //printf ("HEROIS PRESENTES NA BASE antes de inserir: [");
            //for (int i = 0; i < w->n_herois; i++) {
            //    if (b->presentes->flag[i])
            //        printf (" %d", i);
            //}
            //printf (" ]\n");

            cjto_insere(b->presentes, h->id);

            //printf ("HEROIS PRESENTES NA BASE depois de inserir: [");
            //for (int i = 0; i < w->n_herois; i++) {
            //    if (b->presentes->flag[i])
            //        printf (" %d", i);
            //}
            //printf (" ]\n");

            // cria e insere na lef o evento ENTRA (agora, H', B)
            struct evento *evento_entra = cria_evento(instante, EVENTO_ENTRA, h, b, NULL, NULL);
            fprio_insere (lef, evento_entra, EVENTO_ENTRA, instante);
            printf ("%d: AVISA PORTEIRO BASE %d ADMITE %2d\n", instante, b->id, h->id);
        }
    }
    else
        return;
}

void entra (int instante, struct heroi *h, struct base *b, struct fprio_t *lef) {
    // Extraindo um número aleatório entre [1..20]
    int random = extrai_aleat(1, 20);

    // calcula tpb = tempo de permanência na base
    int tpb = 15 + h->paciencia * random;

    // cria e insere na lef o evento SAI (agora + tpb, H, B)
    struct evento *evento_sai = cria_evento(instante + tpb, EVENTO_SAI, h, b, NULL, NULL);
    fprio_insere (lef, evento_sai, EVENTO_SAI, instante + tpb);

    printf ("%d: ENTRA HEROI %2d BASE %d (%2d/%2d) SAI %d\n", instante, h->id, b->id, cjto_card(b->presentes), b->lotacao, instante + tpb);
}

void sai (int instante, struct heroi *h, struct base *b, struct mundo *w, struct fprio_t *lef) {  
    // retira H do conjunto de heróis presentes em B
    //printf ("HEROIS PRESENTES NA BASE antes de retirar: [");
    //        for (int i = 0; i < w->n_herois; i++) {
    //            if (b->presentes->flag[i])
    //                printf (" %d", i);
    //        }
    //        printf (" ]\n");

    cjto_retira(b->presentes, h->id);

    //printf ("HEROIS PRESENTES NA BASE depois de retirar: [");
    //        for (int i = 0; i < w->n_herois; i++) {
    //            if (b->presentes->flag[i])
    //                printf (" %d", i);
    //        }
    //        printf (" ]\n");

    // escolhe uma base destino D aleatória
    struct base *b_aleat = &w->bases[rand() % w->n_bases];

    // cria e insere na lef o evento VIAJA (agora, H, D)
    struct evento *evento_viaja = cria_evento(instante, EVENTO_VIAJA, h, b, b_aleat, NULL);
    fprio_insere (lef, evento_viaja, EVENTO_VIAJA, instante);

    // cria e insere na lef o evento AVISA (agora, B)
    struct evento *evento_avisa = cria_evento(instante, EVENTO_AVISA, h, b, NULL, NULL);
    fprio_insere (lef, evento_avisa, EVENTO_AVISA, instante);
    printf ("%d: SAI HEROI %2d BASE %d (%2d/%2d)\n", instante, h->id, b->id, cjto_card(b->presentes), b->lotacao);
}

void viaja (int instante, struct heroi *h, struct base *i, struct base *d, struct fprio_t *lef) {
    // calcula duração da viagem
    int distancia = sqrt (pow(d->local_x - i->local_x, 2) + pow(d->local_y - i->local_y, 2));
    int duracao = distancia / h->velocidade;

    // cria e insere na lef o evento CHEGA (agora + duração, H, D)
    struct evento *evento_chega = cria_evento(instante + duracao, EVENTO_CHEGA, h, i, d, NULL);
    fprio_insere (lef, evento_chega, EVENTO_CHEGA, instante + duracao);
    printf ("%d: VIAJA HEROI %2d BASE %d BASE %d DIST %d VEL %d CHEGA %d\n", instante, h->id, i->id, d->id, distancia, h->velocidade, instante + duracao);
}

void morre (int instante, struct missao *m, struct heroi *h, struct base *b, struct fprio_t *lef) {
    // retira H do conjunto de heróis presentes em B
    cjto_retira(b->presentes, h->id);

    // muda o status de H para morto 
    h->status = 0;

    // cria e insere na lef o evento AVISA (agora, B)
    struct evento *evento_avisa = cria_evento(instante, EVENTO_AVISA, h, b, NULL, m);
    fprio_insere (lef, evento_avisa, EVENTO_AVISA, instante);
    printf ("%d: MORRE HEROI %2d MISSAO %d\n", instante, h->id, m->id);
}

void missao (int instante, struct missao *m, struct mundo *w, struct fprio_t *lef) {
    if (m->status == 1)
        return;

    int i, distancia, risco;
    int dis_menor = __INT_MAX__; // VER ISSO AQUI
    struct base *base_proxima = NULL;

    // extraindo um número aleatório entre [0..30]
    int random = extrai_aleat(0, 30);

    // cria um conjunto com as habilidades requeridas para a missão
    struct cjto_t *hab_requeridas = cjto_cria(w->n_habilidades);

    // valores para as estatísticas finais
    m->n_tentativas++;
    soma_missoes += m->n_tentativas;

    printf ("%d: MISSAO %d TENT %d HAB REQ: [", instante, m->id, m->n_tentativas);
    for (int i = 0; i < w->n_habilidades; i++) {
        if (m->habilidades->flag[i]) {
            cjto_insere(hab_requeridas, i); // insere a habilidade no conjunto
            printf (" %d", i);
        }
    }
    printf (" ]\n");

    // calcula a distância de cada base ao local da missão M
    // encontra BMP = base mais próxima da missão cujos heróis possam cumpri-la 
    for (i = 0; i < w->n_bases; i++) {
        struct base *base_atual = &w->bases[i]; // pega o endereço das bases
    
        // cria um conjunto com as habilidades que a base possui
        struct cjto_t *hab_base = cjto_cria(w->n_habilidades);
        
        // insere no conjunto de habilidades da base as habilidades dos heróis presentes
        if (base_atual->presentes) {
            for (int j = 0; j < cjto_card(base_atual->presentes); j++) {
                for (int k = 0; k < w->n_habilidades; k++) {
                    if (w->herois[j].habilidades->flag[k]) {
                        cjto_insere(hab_base, k); // insere a habilidade no conjunto
                    }
                }
            }
        }

        // verifica aptidão da base atual
        if (cjto_contem(hab_base, hab_requeridas)) {
            distancia = sqrt(pow(m->local_x - base_atual->local_x, 2) + pow(m->local_y - base_atual->local_y, 2));

            printf ("%d: MISSAO %d BASE %d DIST %d HEROIS [", instante, m->id, base_atual->id, distancia);
            for (int i = 0; i < cjto_card(base_atual->presentes); i++) {
                printf (" %d", w->herois[i].id);
            }
            printf (" ]\n");

            for (int i = 0; i < cjto_card(base_atual->presentes); i++) {
                printf ("%d: MISSAO %d HAB HEROI %d [", instante, m->id, w->herois[i].id);
                for (int j = 0; j < w->n_habilidades; j++) {
                    if (w->herois[i].habilidades->flag[j])
                        printf (" %d", j);
                }
                printf (" ]\n");
            }

                printf ("%d: MISSAO %d UNIAO HAB BASE %d [", instante, m->id, base_atual->id);
                for (int j = 0; j < w->n_habilidades; j++) {
                    if (hab_base->flag[j])
                        printf (" %d", j);
                }
                printf (" ]\n");

            if (distancia < dis_menor) {
                dis_menor = distancia;
                base_proxima = base_atual;
            }
        }
        cjto_destroi(hab_base);
    }

    // se houver uma BMP
    if (base_proxima) {
        m->status = 1; // missão cumprida
        for (i = 0; i < cjto_card(base_proxima->presentes); i++) {
            struct heroi *h = &w->herois[i];
            risco = m->perigo / ((h->paciencia) + h->experiencia + 1);

            //printf("RISCO = PERIGO: %d/ (PAC_HERÓI %d + EXP_HEROI %d + 1) == %d (RANDOM = %d)\n", m->perigo, h->paciencia, h->experiencia, risco, random);

            if (risco > random) {
                struct evento *evento_morre = cria_evento(instante, EVENTO_MORRE, h, base_proxima, NULL, m);
                fprio_insere(lef, evento_morre, EVENTO_MORRE, instante);
            }
            else {
                h->experiencia++;
            }
        }

        // valores para as estatísticas finais
        w->n_cumpridas++;
        base_proxima->n_missoes_base++;
        if (m->n_tentativas < min_missao)
            min_missao = m->n_tentativas;
        if (m->n_tentativas > max_missao)
            max_missao = m->n_tentativas;

        printf ("%d: MISSAO %d CUMPRIDA BASE %d HABS: [", instante, m->id, base_proxima->id);
            for (int i = 0; i < w->n_habilidades; i++) {
                if (m->habilidades->flag[i])
                printf (" %d", i);
            }
        printf (" ]\n");
    }
    // senão
    else {
        struct evento *evento_missao = cria_evento(instante + (24*60), EVENTO_MISSAO, NULL, NULL, NULL, m);
        fprio_insere(lef, evento_missao, EVENTO_MISSAO, instante + (24*60));
        printf ("%d: MISSAO %d IMPOSSIVEL\n", instante, m->id);
    }

    cjto_destroi(hab_requeridas);
}

void fim (struct mundo *w) {
    int n_herois_mortos = 0;
    float media_missao = soma_missoes / w->n_missoes;
    float missoes_t_c = (w->n_cumpridas * 100) / w->n_missoes;

    // estatísticas específicas
    for (int i = 0; i < w->n_herois; i++) {
        struct heroi *h = &w->herois[i];
        if (h->status == 1) {
            printf ("HEROI %2d VIVO PAC %4d EXP %4d HABS [", h->id, h->paciencia, h->experiencia);
                for (int j = 0; j < w->n_habilidades; j++) {
                    if (h->habilidades->flag[j])
                        printf (" %d", j);
                }
            printf (" ]\n");
        }
        else {
            n_herois_mortos++;
            printf ("HEROI %2d MORTO PAC %4d EXP %4d HABS [", h->id, h->paciencia, h->experiencia);
                for (int k = 0; k < w->n_habilidades; k++) {
                    if (h->habilidades->flag[k])
                        printf (" %d", k);
                }
            printf (" ]\n");
        }
    }

    for (int i = 0; i < w->n_bases; i++) {
        struct base *b = &w->bases[i];
        printf ("BASE %2d LOT %2d FILA MAX %2d MISSOES %d\n", b->id, b->lotacao, b->fila_max, b->n_missoes_base);
    }

    float taxa_mortalidade = (n_herois_mortos * 100) / w->n_herois;

    // estatísticas gerais
        printf ("EVENTOS TRATADOS: %d\n", eventos_tratados);
        printf ("MISSOES CUMPRIDAS: %d/%d (%.1f%%)\n", w->n_cumpridas, w->n_missoes, missoes_t_c);
        printf ("TENTATIVAS/MISSAO: MIN %d, MAX %d, MEDIA %.1f\n", min_missao, max_missao, media_missao);
        printf ("TAXA MORTALIDADE: %.1f%%\n", taxa_mortalidade);
}

void eventos_iniciais (struct mundo *w, struct fprio_t *lef) {
    int instante;

    // cada herói chegará em uma base aleatória em algum momento dos três primeiros dias de simulação
    for (int i = 0; i < w->n_herois; i++) {
        w->herois[i].base_id = extrai_aleat (0, w->n_bases - 1);
        instante = extrai_aleat (0, 4320);
        struct evento *evento_chega = cria_evento(instante, EVENTO_CHEGA, &w->herois[i], &w->bases[w->herois[i].base_id], NULL, NULL);
        if (!evento_chega)
            return;
        fprio_insere(lef, evento_chega, EVENTO_CHEGA, instante);
    }

    //for (int i = 0; i < w->n_herois; i++) {
    //    for (int j = i + 1; j < w->n_herois; j++) {
    //        if (w->herois[i].id == w->herois[j].id) {
    //            printf("Erro: Herói duplicado com ID %d\n", w->herois[i].id);
    //        }
    //    }
    //}

    // cada missão deve ser agendada para ocorrer em algum momento da simulação
    for (int i = 0; i < w->n_missoes; i++) {
        instante = extrai_aleat (0, w->tempo_final);
        struct evento *evento_missao = cria_evento(instante, EVENTO_MISSAO, NULL, NULL, NULL, &w->missoes[i]);
        if (!evento_missao)
            return;
        fprio_insere(lef, evento_missao, EVENTO_MISSAO, instante);
    }

    instante = w->tempo_final;

    struct evento *evento_fim = cria_evento(instante, EVENTO_FIM, w->herois, w->bases, NULL, w->missoes);
    if (!evento_fim)
        return;
    fprio_insere(lef, evento_fim, EVENTO_FIM, instante);

}

struct evento *cria_evento (int instante, int tipo, struct heroi *h, struct base *b, struct base *b_aux, struct missao *m) {
    struct evento *novo_evento = malloc(sizeof(struct evento));
    if (!novo_evento) {
        return NULL; 
    }

    novo_evento->instante = instante;
    novo_evento->tipo = tipo;
    novo_evento->h = h;
    novo_evento->b = b;
    novo_evento->b_aux = b_aux;
    novo_evento->m = m;

    eventos_tratados++;

    return novo_evento;
}

int extrai_aleat (int ini, int cap) {
    if (cap <= 0)
        return -1;

    int valor = ini + rand() % (cap - ini + 1);

    return valor;
}
