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
    eventos_tratados++;
    cjto_insere(ev->b->presentes, ev->h->id);

    int espera = 0;
    int tam_fila = lista_tamanho(ev->b->lst_espera);

    // se há vagas em B e a fila de espera em B está vazia:
    if (ev->b->lotacao != cjto_card(ev->b->presentes) && tam_fila == 0)
        espera = 1;
    else
        espera = (ev->h->paciencia) > (10 * tam_fila);

    if (espera) {
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

void espera (struct evento *ev, struct fprio_t *lef) {
    eventos_tratados++;
    printf ("%d: ESPERA HEROI %2d BASE %d (%2d)\n", ev->instante, ev->h->id, ev->b->id, lista_tamanho(ev->b->lst_espera));
    
    if (lista_tamanho(ev->b->lst_espera) > ev->b->fila_max)
        ev->b->fila_max = lista_tamanho(ev->b->lst_espera);
    
    // adiciona H ao fim da fila de espera de B
    lista_insere(ev->b->lst_espera, ev->h->id, -1);

    //cria e insere na lef o evento AVISA (agora, B)
    struct evento *evento_avisa = cria_evento(ev->instante, EVENTO_AVISA, ev->h, ev->b, NULL, NULL);
        if (!evento_avisa)
            return;
    fprio_insere (lef, evento_avisa, EVENTO_AVISA, ev->instante);
}

void desiste (struct evento *ev, struct mundo *w, struct fprio_t *lef) {
    eventos_tratados++;
    // escolhe uma base destino D aleatória
    struct base *b_aleat = &w->bases[rand() % w->n_bases];

    // cria e insere na lef o evento VIAJA (agora, H, D)
    struct evento *evento_viaja = cria_evento(ev->instante, EVENTO_VIAJA, ev->h, ev->b, b_aleat, NULL);
        if (!evento_viaja)
            return;
    fprio_insere (lef, evento_viaja, EVENTO_VIAJA, ev->instante);
    printf ("%d: DESISTE HEROI %2d BASE %d\n", ev->instante, ev->h->id, ev->b->id);
}

void avisa (struct evento *ev, struct fprio_t *lef) {
    eventos_tratados++;
    int vagas = ev->b->lotacao - cjto_card(ev->b->presentes);
    if (lista_tamanho(ev->b->lst_espera) > 0) {
        // enquanto houver vaga em B e houver heróis esperando na fila:
        while (vagas && lista_tamanho(ev->b->lst_espera) != 0) {
            lista_retira (ev->b->lst_espera, &ev->h->id, 0);
            
            printf ("%d: AVISA PORTEIRO BASE %d (%2d/%2d) FILA [", ev->instante, ev->b->id, cjto_card(ev->b->presentes), ev->b->lotacao);
            struct item_t *atual = ev->b->lst_espera->prim;
            for (int i = 0; i < ev->b->lst_espera->tamanho; i++) {
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

            cjto_insere(ev->b->presentes, ev->h->id);

            //printf ("HEROIS PRESENTES NA BASE depois de inserir: [");
            //for (int i = 0; i < w->n_herois; i++) {
            //    if (b->presentes->flag[i])
            //        printf (" %d", i);
            //}
            //printf (" ]\n");

            // cria e insere na lef o evento ENTRA (agora, H', B)
            struct evento *evento_entra = cria_evento(ev->instante, EVENTO_ENTRA, ev->h, ev->b, NULL, NULL);
            fprio_insere (lef, evento_entra, EVENTO_ENTRA, ev->instante);
            printf ("%d: AVISA PORTEIRO BASE %d ADMITE %2d\n", ev->instante, ev->b->id, ev->h->id);
        }
    }
    else
        return;
}

void entra (struct evento *ev, struct fprio_t *lef) {
    eventos_tratados++;
    // Extraindo um número aleatório entre [1..20]
    int random = extrai_aleat(1, 20);

    // calcula tpb = tempo de permanência na base
    int tpb = 15 + ev->h->paciencia * random;

    // cria e insere na lef o evento SAI (agora + tpb, H, B)
    struct evento *evento_sai = cria_evento(ev->instante + tpb, EVENTO_SAI, ev->h, ev->b, NULL, NULL);
    fprio_insere (lef, evento_sai, EVENTO_SAI, ev->instante + tpb);

    printf ("%d: ENTRA HEROI %2d BASE %d (%2d/%2d) SAI %d\n", 
                ev->instante, ev->h->id, ev->b->id, cjto_card(ev->b->presentes), ev->b->lotacao, ev->instante + tpb);
}

void sai (struct evento *ev, struct mundo *w, struct fprio_t *lef) {
    eventos_tratados++;
    // retira H do conjunto de heróis presentes em B
    //printf ("HEROIS PRESENTES NA BASE antes de retirar: [");
    //        for (int i = 0; i < w->n_herois; i++) {
    //            if (b->presentes->flag[i])
    //                printf (" %d", i);
    //        }
    //        printf (" ]\n");

    cjto_retira(ev->b->presentes, ev->h->id);

    //printf ("HEROIS PRESENTES NA BASE depois de retirar: [");
    //        for (int i = 0; i < w->n_herois; i++) {
    //            if (b->presentes->flag[i])
    //                printf (" %d", i);
    //        }
    //        printf (" ]\n");

    // escolhe uma base destino D aleatória
    struct base *b_aleat = &w->bases[rand() % w->n_bases];

    // cria e insere na lef o evento VIAJA (agora, H, D)
    struct evento *evento_viaja = cria_evento(ev->instante, EVENTO_VIAJA, ev->h, ev->b, b_aleat, NULL);
    fprio_insere (lef, evento_viaja, EVENTO_VIAJA, ev->instante);

    // cria e insere na lef o evento AVISA (agora, B)
    struct evento *evento_avisa = cria_evento(ev->instante, EVENTO_AVISA, ev->h, ev->b, NULL, NULL);
    fprio_insere (lef, evento_avisa, EVENTO_AVISA, ev->instante);
    printf ("%d: SAI HEROI %2d BASE %d (%2d/%2d)\n", 
                ev->instante, ev->h->id, ev->b->id, cjto_card(ev->b->presentes), ev->b->lotacao);
}

void viaja (struct evento *ev, struct fprio_t *lef) {
    eventos_tratados++;
    // calcula duração da viagem
    int distancia = sqrt (pow(ev->b_aux->local_x - ev->b->local_x, 2) + pow(ev->b_aux->local_y - ev->b->local_y, 2));
    int duracao = distancia / ev->h->velocidade;

    printf ("%d: VIAJA HEROI %2d BASE %d BASE %d DIST %d VEL %d CHEGA %d\n", 
                ev->instante, ev->h->id, ev->b->id, ev->b_aux->id, distancia, ev->h->velocidade, ev->instante + duracao);
    
    // cria e insere na lef o evento CHEGA (agora + duração, H, D)
    struct evento *evento_chega = cria_evento(ev->instante + duracao, EVENTO_CHEGA, ev->h, ev->b_aux, NULL, NULL);
    fprio_insere (lef, evento_chega, EVENTO_CHEGA, ev->instante + duracao);
}

void morre (struct evento *ev, struct fprio_t *lef) {
    eventos_tratados++;
    // retira H do conjunto de heróis presentes em B
    cjto_retira(ev->b->presentes, ev->h->id);

    // muda o status de H para morto 
    ev->h->status = 0;

    // cria e insere na lef o evento AVISA (agora, B)
    struct evento *evento_avisa = cria_evento(ev->instante, EVENTO_AVISA, ev->h, ev->b, NULL, ev->m);
    fprio_insere (lef, evento_avisa, EVENTO_AVISA, ev->instante);
    printf ("%d: MORRE HEROI %2d MISSAO %d\n", ev->instante, ev->h->id, ev->m->id);
}

void missao (struct evento *ev, struct mundo *w, struct fprio_t *lef) {
    eventos_tratados++;
    int distancia[w->n_bases];
    struct base *bmp = NULL;

    // valores para as estatísticas finais
    soma_missoes++;

    printf ("%d: MISSAO %d TENT %d HAB REQ: [", ev->instante, ev->m->id, ev->m->n_tentativas);
    for (int i = 0; i < w->n_habilidades; i++) {
        if (ev->m->habilidades->flag[i]) {
            printf (" %d", i);
        }
    }
    printf (" ]\n");
    

    // calcula a distância de cada base ao local da missão M
    for (int i = 0; i < w->n_bases; i++) {

        struct cjto_t *habilidades = cjto_cria(w->n_habilidades);
        struct cjto_t *uniao = cjto_cria(w->n_habilidades);

        distancia[i] = sqrt(pow(ev->m->local_x - w->bases[i].local_x, 2) + pow(ev->m->local_y - w->bases[i].local_y, 2));
    
        // atualiza habilidades da base com as dos heróis presentes
        for (int j = 0; j < w->n_herois; j++) {
            if (w->bases[i].presentes->flag[j]) {

                uniao = cjto_uniao(habilidades, w->herois[j].habilidades);

                if (uniao != NULL) {
                    cjto_destroi(habilidades);
                    habilidades = uniao;
                    //cjto_destroi(uniao);
                }
            }
        }


        // verifica a os heróis que certa base tem, tal como sua distância da missão
        printf ("%d: MISSAO %d BASE %d DIST %d HEROIS [", ev->instante, ev->m->id, i, distancia[i]);
        for (int j = 0; j < w->n_herois; j++) {
            if (w->bases[i].presentes->flag[j])
            printf (" %d", j);
        }
        printf (" ]\n");

        // verifica a os heróis que certa base tem, tal como sua distância da missão
        for (int j = 0; j < w->n_herois; j++) {
            if (w->bases[i].presentes->flag[j]) {
                printf ("%d: MISSAO %d HAB HEROI %d [ ", ev->instante, ev->m->id, j);
                cjto_imprime (w->herois[j].habilidades);
                printf (" ]\n");    
            }
        }    

        // imprime a união das habilidades da base
        printf ("%d: MISSAO %d UNIAO HAB BASE %d [ ", ev->instante, ev->m->id, i);
        cjto_imprime (habilidades);
        printf (" ]\n");

        // encontra BMP = base mais próxima da missão cujos heróis possam cumpri-la
        if (cjto_contem(habilidades, ev->m->habilidades)) {
            if (!bmp || distancia[i] < distancia[bmp->id])
                bmp = &w->bases[i];
        }

        cjto_destroi(habilidades);
    }

    // se houver uma BMP
    if (bmp) {
        ev->m->status = 1; // missão cumprida
        for (int i = 0; i < w->n_herois; i++) {
            if (bmp->presentes->flag[i]) {
                int risco = ev->m->perigo / (w->herois[i].paciencia + w->herois[i].experiencia + 1.0);
                int random = extrai_aleat(0, 30);
                if (risco > random) {
                    //printf ("risco foi maior (risco = %d e random = %d)\n", risco, random);
                    struct evento *evento_morre = cria_evento(ev->instante, EVENTO_MORRE, &w->herois[i], bmp, NULL, ev->m);
                    fprio_insere(lef, evento_morre, EVENTO_MORRE, ev->instante);
                }
                else {
                    //printf ("risco foi menor (risco = %d e random = %d)\n", risco, random);
                    w->herois[i].experiencia++;
                }
            }
        }

        // valores para as estatísticas finais
        w->n_cumpridas++;
        bmp->n_missoes_base++;

        printf ("%d: MISSAO %d CUMPRIDA BASE %d HABS: [", ev->instante, ev->m->id, bmp->id);
        for (int i = 0; i < w->n_habilidades; i++) {
            if (ev->m->habilidades->flag[i])
                printf (" %d", i);
        }
        printf (" ]\n");
    }
    else {
        struct evento *evento_missao = cria_evento(ev->instante + (24*60), EVENTO_MISSAO, NULL, NULL, NULL, ev->m);
        fprio_insere(lef, evento_missao, EVENTO_MISSAO, ev->instante + (24*60));
        printf ("%d: MISSAO %d IMPOSSIVEL\n", ev->instante, ev->m->id);
    }

    // valores para as estatísticas finais
    ev->m->n_tentativas++;
    if (ev->m->n_tentativas < min_missao)
        min_missao = ev->m->n_tentativas;
    if (ev->m->n_tentativas > max_missao)
        max_missao = ev->m->n_tentativas;
}

/*
void missao (struct evento *ev, struct mundo *w, struct fprio_t *lef) {
    if (ev->m->status == 1)
        return;

    int i, distancia, risco;
    int dis_menor = __INT_MAX__; // VER ISSO AQUI
    struct base *base_proxima = NULL;

    // extraindo um número aleatório entre [0..30]
    int random ;

    // cria um conjunto com as habilidades requeridas para a missão
    struct cjto_t *hab_requeridas = cjto_cria(w->n_habilidades);

    // valores para as estatísticas finais
    ev->m->n_tentativas++;
    soma_missoes += ev->m->n_tentativas;

    printf ("%d: MISSAO %d TENT %d HAB REQ: [", ev->instante, ev->m->id, ev->m->n_tentativas);
    for (int i = 0; i < w->n_habilidades; i++) {
        if (ev->m->habilidades->flag[i]) {
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
        base_atual->habilidades = cjto_cria(w->n_habilidades);
        
        // insere no conjunto de habilidades da base as habilidades dos heróis presentes
        for (int j = 0; j < w->n_herois; j++) {
            if (base_atual->presentes->flag[j]) {
                for (int k = 0; k < w->n_habilidades; k++) {
                    if (w->herois[j].habilidades->flag[k])
                        cjto_insere (base_atual->habilidades, k);
                }
            }
        }

        // verifica aptidão da base atual
        if (cjto_contem(base_atual->habilidades, hab_requeridas)) {
            distancia = sqrt(pow(ev->m->local_x - base_atual->local_x, 2) + pow(ev->m->local_y - base_atual->local_y, 2));

            // verifica a os heróis que certa base tem, tal como sua distância da missão
            printf ("%d: MISSAO %d BASE %d DIST %d HEROIS [", ev->instante, ev->m->id, base_atual->id, distancia);
            for (int i = 0; i < w->n_herois; i++) {
                if (base_atual->presentes->flag[i])
                printf (" %d", i);
            }
            printf (" ]\n");

            // verifica as habilidades dos heróis de certa base
            for (int i = 0; i < cjto_card(base_atual->presentes); i++) {
                if (base_atual->presentes->flag[i]) {
                    printf ("%d: MISSAO %d HAB HEROI %d [", ev->instante, ev->m->id, i);
                    for (int j = 0; j < w->n_habilidades; j++) {
                        if (w->herois[i].habilidades->flag[j])
                            printf (" %d", j);
                    }
                    printf (" ]\n");    
                }
            }

            // faz a união das habilidades da base
            printf ("%d: MISSAO %d UNIAO HAB BASE %d [", ev->instante, ev->m->id, base_atual->id);
            for (int j = 0; j < w->n_habilidades; j++) {
                if (base_atual->habilidades->flag[j])
                    printf (" %d", j);
            }
            printf (" ]\n");

            if (distancia < dis_menor) {
                dis_menor = distancia;
                base_proxima = base_atual;
            }
        }
        cjto_destroi(base_atual->habilidades);
    }

    // se houver uma BMP
    if (base_proxima) {
        ev->m->status = 1; // missão cumprida
        for (i = 0; i < cjto_card(base_proxima->presentes); i++) {
            struct heroi *h = &w->herois[i];
            risco = ev->m->perigo / (h->paciencia + h->experiencia + 1.0);
	    random = extrai_aleat (0, 30) ;
            if (risco > random) {
                struct evento *evento_morre = cria_evento(ev->instante, EVENTO_MORRE, h, base_proxima, NULL, ev->m);
                fprio_insere(lef, evento_morre, EVENTO_MORRE, ev->instante);
            }
            else {
                h->experiencia++;
            }
        }

        // valores para as estatísticas finais
        w->n_cumpridas++;
        base_proxima->n_missoes_base++;
        if (ev->m->n_tentativas < min_missao)
            min_missao = ev->m->n_tentativas;
        if (ev->m->n_tentativas > max_missao)
            max_missao = ev->m->n_tentativas;

        printf ("%d: MISSAO %d CUMPRIDA BASE %d HABS: [", ev->instante, ev->m->id, base_proxima->id);
            for (int i = 0; i < w->n_habilidades; i++) {
                if (ev->m->habilidades->flag[i])
                printf (" %d", i);
            }
        printf (" ]\n");
    }
    // senão
    else {
        struct evento *evento_missao = cria_evento(ev->instante + (24*60), EVENTO_MISSAO, NULL, NULL, NULL, ev->m);
        fprio_insere(lef, evento_missao, EVENTO_MISSAO, ev->instante + (24*60));
        printf ("%d: MISSAO %d IMPOSSIVEL\n", ev->instante, ev->m->id);
    }

    cjto_destroi(hab_requeridas);
}
*/


void fim (struct mundo *w) {
    eventos_tratados++;
    int n_herois_mortos = 0;
    float media_missao = soma_missoes / w->n_missoes;
    float missoes_t_c = (w->n_cumpridas * 100) / w->n_missoes;

    printf ("%d: FIM\n\n", w->relogio);

    //for (int i = 0; i < w->n_herois; i++) {
    //    struct heroi *h = &w->herois[i];
    //    printf ("HEROI %d PAC: %d HABILIDADES: [", i, h->paciencia);
    //    for (int j = 0; j < w->n_habilidades; j++) {
    //        if (w->herois[i].habilidades->flag[j])
    //            printf (" %d", j);
    //    }
    //    printf (" ]\n");
    //}

    // estatísticas específicas
    for (int i = 0; i < w->n_herois; i++) {
        struct heroi *h = &w->herois[i];
        if (h->status == 1) {
            printf ("HEROI %2d VIVO PAC %4d EXP %4d HABS [", i, h->paciencia, h->experiencia);
                for (int j = 0; j < w->n_habilidades; j++) {
                    if (h->habilidades->flag[j])
                        printf (" %d", j);
                }
            printf (" ]\n");
        }
        else {
            n_herois_mortos++;
            printf ("HEROI %2d MORTO PAC %4d EXP %4d HABS [", i, h->paciencia, h->experiencia);
                for (int k = 0; k < w->n_habilidades; k++) {
                    if (h->habilidades->flag[k])
                        printf (" %d", k);
                }
            printf (" ]\n");
        }
    }
    printf ("\n");

    for (int i = 0; i < w->n_bases; i++) {
        struct base *b = &w->bases[i];
        printf ("BASE %2d LOT %2d FILA MAX %2d MISSOES %d\n", b->id, b->lotacao, b->fila_max, b->n_missoes_base);
    }
    printf ("\n");

    float taxa_mortalidade = (n_herois_mortos * 100) / w->n_herois;
    // estatísticas gerais
        printf ("EVENTOS TRATADOS: %d\n", eventos_tratados);
        printf ("MISSOES CUMPRIDAS: %d/%d (%.1f%%)\n", w->n_cumpridas, w->n_missoes, missoes_t_c);
        printf ("TENTATIVAS/MISSAO: MIN %d, MAX %d, MEDIA %.1f\n", min_missao, max_missao, media_missao);
        printf ("TAXA MORTALIDADE: %.1f%%\n", taxa_mortalidade);
}

void destroi_mundo (struct mundo *w) {
    // liberando as habilidades dos heróis
    for (int i = 0; i < w->n_herois; i++) 
        cjto_destroi (w->herois[i].habilidades);

    // liberando os presentes e as habilidades nas bases e a lista de espera
    for (int i = 0; i < w->n_bases; i++) {
        cjto_destroi (w->bases[i].presentes);
        lista_destroi (w->bases[i].lst_espera);
    }

    // liberando as habilidades das missões
    for (int i = 0; i < w->n_missoes; i++)
        cjto_destroi (w->missoes[i].habilidades);

    // liberando herois, bases e missoes como um todo
    free (w->herois);
    free (w->bases);
    free (w->missoes);

    // liberando o mundo
    free(w);
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

    //for (int i = 0; i < w->n_herois; i++) {
    //    printf ("HEROI %d TEM AS HAB [", w->herois[i].id);
    //    for (int k = 0; k < w->n_habilidades; k++) {
    //        if (w->herois[i].habilidades->flag[k]) {
    //            printf (" %d", k);
    //        }
    //        printf (" ]\n");
    //    }
    //}

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

    return novo_evento;
}

int extrai_aleat (int ini, int cap) {
    if (cap <= 0)
        return -1;

    int valor = ini + rand() % (cap - ini + 1);

    return valor;
}
