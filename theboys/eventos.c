#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mundo.h"
#include "fprio.h"
#include "lista.h"
#include "eventos.h"
#include "conjunto.h"

// Protótipo de uma função auxiliar para criar um evento
struct evento *cria_evento (int instante, int tipo, struct heroi *h, struct base *b, 
                            struct base *b_aux, struct missao *m);

// Protótipo de uma função de ordenação de struct base
void selection_sort (struct base *b, int tam);

// Variáveis globais para colocar nas estatísticas finais
int soma_missoes = 0;
int max_missao = 0;
int min_missao = __INT_MAX__;

void chega (struct evento *ev, struct fprio_t *lef) {
    if (ev->h->status == 0)
        return;

    int espera = 0;
    int tam_fila = lista_tamanho(ev->b->lst_espera);

    ev->h->base_id = ev->b->id;

    // Se há vagas em B e a fila de espera em B está vazia:
    if (ev->b->lotacao != cjto_card(ev->b->presentes) && tam_fila == 0)
        espera = 1;
    else
        espera = (ev->h->paciencia) > (10 * tam_fila);

    if (espera) {
        // Cria e insere na lef o evento espera;
        struct evento *evento_espera = cria_evento(ev->instante, EVENTO_ESPERA, ev->h, ev->b, NULL, NULL);
            if (!evento_espera)
                return;
        fprio_insere(lef, evento_espera, EVENTO_ESPERA, ev->instante);
        printf ("%d: CHEGA HEROI %2d BASE %d (%2d/%2d) ESPERA\n", 
                    ev->instante, ev->h->id, ev->b->id, cjto_card(ev->b->presentes), ev->b->lotacao);
    }
    else {
        // Cria e insere na lef o evento desiste;
        struct evento *evento_desiste = cria_evento(ev->instante, EVENTO_DESISTE, ev->h, ev->b, NULL, NULL);
            if (!evento_desiste)
                return;
        fprio_insere(lef, evento_desiste, EVENTO_DESISTE, ev->instante);
        printf ("%d: CHEGA HEROI %2d BASE %d (%2d/%2d) DESISTE\n",
                     ev->instante, ev->h->id, ev->b->id, cjto_card(ev->b->presentes), ev->b->lotacao);
    }
}

void espera (struct evento *ev, struct fprio_t *lef) {
    printf ("%d: ESPERA HEROI %2d BASE %d (%2d)\n", 
                ev->instante, ev->h->id, ev->b->id, lista_tamanho(ev->b->lst_espera));
    
    if (lista_tamanho(ev->b->lst_espera) > ev->b->fila_max)
        ev->b->fila_max = lista_tamanho(ev->b->lst_espera);
    
    // Adiciona H ao fim da fila de espera de B
    lista_insere(ev->b->lst_espera, ev->h->id, -1);

    // Cria e insere na lef o evento AVISA (agora, B)
    struct evento *evento_avisa = cria_evento(ev->instante, EVENTO_AVISA, ev->h, ev->b, NULL, NULL);
        if (!evento_avisa)
            return;
    fprio_insere(lef, evento_avisa, EVENTO_AVISA, ev->instante);
}

void desiste (struct evento *ev, struct mundo *w, struct fprio_t *lef) { 
    // Escolhe uma base destino D aleatória
    struct base *b_aleat = &w->bases[extrai_aleat(0, w->n_bases - 1)];

    // Cria e insere na lef o evento VIAJA (agora, H, D)
    struct evento *evento_viaja = cria_evento(ev->instante, EVENTO_VIAJA, ev->h, ev->b, b_aleat, NULL);
        if (!evento_viaja)
            return;
    fprio_insere(lef, evento_viaja, EVENTO_VIAJA, ev->instante);
    printf ("%d: DESISTE HEROI %2d BASE %d\n", ev->instante, ev->h->id, ev->b->id);
}

void avisa (struct evento *ev, struct mundo *w, struct fprio_t *lef) {
    printf ("%d: AVISA PORTEIRO BASE %d (%2d/%2d) FILA [ ",
                         ev->instante, ev->b->id, cjto_card(ev->b->presentes), ev->b->lotacao);
            lista_imprime(ev->b->lst_espera);
            printf (" ]\n");

    // Enquanto houver vaga em B e houver heróis esperando na fila
    while (ev->b->lotacao != cjto_card(ev->b->presentes) && lista_tamanho(ev->b->lst_espera) > 0) {
        int heroi_id;
        lista_retira(ev->b->lst_espera, &heroi_id, 0);           
        cjto_insere(ev->b->presentes, heroi_id);

        // Encontra o herói que está na fila de espera pelo seu id
        struct heroi *h_temp = NULL;
        for (int i = 0; i < w->n_herois; i++) {
            if (w->herois[i].id == heroi_id)
                h_temp = &w->herois[i]; 
        }
        if (!h_temp)
            return;

        struct evento *evento_entra = cria_evento(ev->instante, EVENTO_ENTRA, h_temp, ev->b, NULL, NULL);
        fprio_insere(lef, evento_entra, EVENTO_ENTRA, ev->instante);
        printf ("%d: AVISA PORTEIRO BASE %d ADMITE HEROI %2d\n", 
                    ev->instante, ev->b->id, h_temp->id);
    }
}

void entra (struct evento *ev, struct fprio_t *lef) {
    if (ev->h->status == 0)
        return;

    // Calcula TPB = tempo de permanência na base
    int tpb = 15 + ev->h->paciencia * extrai_aleat(1, 20);

    // Cria e insere na lef o evento SAI (agora + tpb, H, B)
    struct evento *evento_sai = cria_evento(ev->instante + tpb, EVENTO_SAI, ev->h, ev->b, NULL, NULL);
    fprio_insere(lef, evento_sai, EVENTO_SAI, ev->instante + tpb);

    printf ("%d: ENTRA HEROI %2d BASE %d (%2d/%2d) SAI %d\n", ev->instante, ev->h->id,
                 ev->b->id, cjto_card(ev->b->presentes), ev->b->lotacao, ev->instante + tpb);
}

void sai (struct evento *ev, struct mundo *w, struct fprio_t *lef) {
    if (ev->h->status == 0)
        return;

    // Retira H do conjunto de heróis presentes em B
    cjto_retira(ev->b->presentes, ev->h->id);

    // Escolhe uma base destino D aleatória
    struct base *b_aleat = &w->bases[extrai_aleat(0, w->n_bases - 1)];

    // Cria e insere na lef o evento VIAJA (agora, H, D)
    struct evento *evento_viaja = cria_evento(ev->instante, EVENTO_VIAJA, ev->h, ev->b, b_aleat, NULL);
    fprio_insere(lef, evento_viaja, EVENTO_VIAJA, ev->instante);

    // Cria e insere na lef o evento AVISA (agora, B)
    struct evento *evento_avisa = cria_evento(ev->instante, EVENTO_AVISA, NULL, ev->b, NULL, NULL);
    fprio_insere(lef, evento_avisa, EVENTO_AVISA, ev->instante);
    printf ("%d: SAI HEROI %2d BASE %d (%2d/%2d)\n", 
                ev->instante, ev->h->id, ev->b->id, cjto_card(ev->b->presentes), ev->b->lotacao);
}

void viaja (struct evento *ev, struct fprio_t *lef) {
    if (ev->h->status == 0)
        return;

    // Calcula duração da viagem
    int distancia = sqrt (pow(ev->b_aux->local_x - ev->b->local_x, 2) + 
                          pow(ev->b_aux->local_y - ev->b->local_y, 2));
    int duracao = distancia / ev->h->velocidade;

    // Cria e insere na lef o evento CHEGA (agora + duração, H, D)
    struct evento *evento_chega = cria_evento(ev->instante + duracao, EVENTO_CHEGA, ev->h,
                                                 ev->b_aux, NULL, NULL);
    fprio_insere(lef, evento_chega, EVENTO_CHEGA, ev->instante + duracao);
    printf ("%d: VIAJA HEROI %2d BASE %d BASE %d DIST %d VEL %d CHEGA %d\n", ev->instante, ev->h->id, 
                ev->b->id, ev->b_aux->id, distancia, ev->h->velocidade, ev->instante + duracao);
}

void morre (struct evento *ev, struct fprio_t *lef) {
    if (ev->h->status == 0)
        return;

    // Retira H do conjunto de heróis presentes em B
    cjto_retira(ev->b->presentes, ev->h->id);

    // Muda o status de H para morto 
    ev->h->status = 0;

    // Cria e insere na lef o evento AVISA (agora, B)
    struct evento *evento_avisa = cria_evento(ev->instante, EVENTO_AVISA, ev->h, ev->b, NULL, NULL);
    fprio_insere(lef, evento_avisa, EVENTO_AVISA, ev->instante);
    printf ("%d: MORRE HEROI %2d MISSAO %d\n", ev->instante, ev->h->id, ev->m->id);
}

void missao (struct evento *ev, struct mundo *w, struct fprio_t *lef) {
    if (ev->m->status == 1)
        return;

    int distancia;
    struct base *v_distancia = malloc(sizeof(struct base) * w->n_bases);
    struct base *bmp = NULL;

    // Valores para as estatísticas finais
    soma_missoes++;
    ev->m->n_tentativas++;

    printf ("%d: MISSAO %d TENT %d HAB REQ: [ ", ev->instante, ev->m->id, ev->m->n_tentativas);
    cjto_imprime(ev->m->habilidades);
    printf (" ]\n");
    
    // Calcula a distância de cada base ao local da missão M
    for (int i = 0; i < w->n_bases; i++) {
        distancia = sqrt(pow(ev->m->local_x - w->bases[i].local_x, 2) + 
                         pow(ev->m->local_y - w->bases[i].local_y, 2));
        w->bases[i].tmp_dist = distancia;
    }

    // Copia as structs das bases para um vetor de structs
    // v_distância terá as distâncias que foram armazenadas em bases[i].tmp_dist
    for (int i = 0; i < w->n_bases; i++) 
        v_distancia[i] = w->bases[i];

    // Ordena as distâncias (vetor de structs) das bases ao local da missão M
    // ordenará as tmp_dist que estão armazenadas em v_distancia
    for (int i = 0; i < w->n_bases; i++)
        selection_sort (v_distancia, w->n_bases);

    // Atribui às bases as habilidades dos heróis presentes
    for (int i = 0; i < w->n_bases; i++) {
        struct cjto_t *habilidades = cjto_cria(w->n_habilidades); 
        
        for (int j = 0; j < w->n_herois; j++) {
            if (cjto_pertence (v_distancia[i].presentes, j)) {
                struct cjto_t *uniao = cjto_uniao(habilidades, w->herois[j].habilidades);
        
                if (uniao != NULL) {
                    cjto_destroi(habilidades);
                    habilidades = uniao;
                }
            }
        }

        // Verifica a os heróis que certa base tem, tal como sua distância da missão
        printf ("%d: MISSAO %d BASE %d DIST %d HEROIS [", ev->instante, ev->m->id, i, v_distancia[i].tmp_dist);
        for (int j = 0; j < w->n_herois; j++) {
            if (cjto_pertence(v_distancia[i].presentes, j))
                printf (" %d", j);
        }
        printf (" ]\n");

        // Verifica a os heróis que certa base tem, tal como sua distância da missão
        for (int j = 0; j < w->n_herois; j++) {
            if (cjto_pertence(v_distancia[i].presentes, j)) {
                printf ("%d: MISSAO %d HAB HEROI %d [ ", ev->instante, ev->m->id, j);
                cjto_imprime(w->herois[j].habilidades);
                printf (" ]\n");    
            }
        }    

        // Imprime a união das habilidades da base
        printf ("%d: MISSAO %d UNIAO HAB BASE %d [ ", ev->instante, ev->m->id, i);
        cjto_imprime(habilidades);
        printf (" ]\n");

        // Encontra BMP = base mais próxima da missão cujos heróis possam cumpri-la
        if (cjto_contem(habilidades, ev->m->habilidades)) {
                bmp = &v_distancia[i];
                cjto_destroi(habilidades);
                break;
        }
        cjto_destroi(habilidades);
    }

    // Se houver uma BMP
    if (bmp) {
        // Encontra a BMP no vetor de bases do mundo
        for (int i = 0; i < w->n_bases; i++) {
            if (w->bases[i].id == bmp->id) {
                w->bases[i].n_missoes_base++;
                bmp = &w->bases[i];
                break;
            }
        }

        // Valores para as estatísticas finais
        w->n_cumpridas++;
        bmp->n_missoes_base++;
                
        if (ev->m->n_tentativas < min_missao)
            min_missao = ev->m->n_tentativas;
        if (ev->m->n_tentativas > max_missao)
            max_missao = ev->m->n_tentativas;

        ev->m->status = 1; // Missão cumprida
        for (int i = 0; i < w->n_herois; i++) {
            if (cjto_pertence(bmp->presentes, i)) {
                struct heroi *h = &w->herois[i];

                int risco = ev->m->perigo / (w->herois[i].paciencia + w->herois[i].experiencia + 1.0);
                int random = extrai_aleat(0, 30);
                if (risco > random) {
                    struct evento *evento_morre = cria_evento(ev->instante, EVENTO_MORRE, h,
                                                                 bmp, NULL, ev->m);
                    fprio_insere(lef, evento_morre, EVENTO_MORRE, ev->instante);
                }
                else
                    w->herois[i].experiencia++;
            }

        }
        printf ("%d: MISSAO %d CUMPRIDA BASE %d HABS: [ ", ev->instante, ev->m->id, bmp->id);
        cjto_imprime(ev->m->habilidades);
        printf (" ]\n");
    }
    else {
        struct evento *evento_missao = cria_evento(ev->instante + (24*60), EVENTO_MISSAO, NULL,
                                                     NULL, NULL, ev->m);
        fprio_insere(lef, evento_missao, EVENTO_MISSAO, ev->instante + (24*60));
        printf ("%d: MISSAO %d IMPOSSIVEL\n", ev->instante, ev->m->id);
    }

    free(v_distancia);
}

void fim (struct mundo *w) {
    int n_herois_mortos = 0;
    float media_missao = soma_missoes / w->n_missoes;
    float missoes_t_c = (w->n_cumpridas * 100) / w->n_missoes;

    printf ("%d: FIM\n\n", w->relogio);

    // Estatísticas específicas
    for (int i = 0; i < w->n_herois; i++) {
        struct heroi *h = &w->herois[i];
        if (h->status == 1) {
            printf ("HEROI %2d VIVO PAC %4d EXP %4d HABS [ ", i, h->paciencia, h->experiencia);
            cjto_imprime(h->habilidades);
            printf (" ]\n");
        }
        else {
            n_herois_mortos++;
            printf ("HEROI %2d MORTO PAC %4d EXP %4d HABS [ ", i, h->paciencia, h->experiencia);
            cjto_imprime(h->habilidades);
            printf (" ]\n");
        }
    }
    printf ("\n");

    for (int i = 0; i < w->n_bases; i++) {
        struct base *b = &w->bases[i];
        printf ("BASE %2d LOT %2d FILA MAX %2d MISSOES %d\n", 
                    b->id, b->lotacao, b->fila_max, b->n_missoes_base);
    }
    printf ("\n");

    float taxa_mortalidade = (n_herois_mortos * 100) / w->n_herois;

    // Estatísticas gerais
    printf ("EVENTOS TRATADOS: %d\n", w->eventos_tratados);
    printf ("MISSOES CUMPRIDAS: %d/%d (%.1f%%)\n", w->n_cumpridas, w->n_missoes, missoes_t_c);
    printf ("TENTATIVAS/MISSAO: MIN %d, MAX %d, MEDIA %.1f\n", min_missao, max_missao, media_missao);
    printf ("TAXA MORTALIDADE: %.1f%%\n", taxa_mortalidade);
}

void destroi_mundo (struct mundo *w) {
    // Liberando as habilidades dos heróis
    for (int i = 0; i < w->n_herois; i++) 
        cjto_destroi(w->herois[i].habilidades);

    // Liberando os presentes e as habilidades nas bases e a lista de espera
    for (int i = 0; i < w->n_bases; i++) {
        cjto_destroi(w->bases[i].presentes);
        lista_destroi(w->bases[i].lst_espera);
    }

    // Liberando as habilidades das missões
    for (int i = 0; i < w->n_missoes; i++)
        cjto_destroi(w->missoes[i].habilidades);

    // Liberando herois, bases e missoes como um todo
    free (w->herois);
    free (w->bases);
    free (w->missoes);

    // Liberando o mundo
    free(w);
}

void eventos_iniciais (struct mundo *w, struct fprio_t *lef) {
    int instante;

    // Cada herói chegará em uma base aleatória em algum momento dos três primeiros dias de simulação
    for (int i = 0; i < w->n_herois; i++) {
        w->herois[i].base_id = extrai_aleat (0, w->n_bases - 1);
        instante = extrai_aleat (0, 4320);

        struct evento *evento_chega = cria_evento(instante, EVENTO_CHEGA, &w->herois[i], 
                                                &w->bases[w->herois[i].base_id], NULL, NULL);
        if (!evento_chega)
            return;

        fprio_insere(lef, evento_chega, EVENTO_CHEGA, instante);
    }

    // Cada missão deve ser agendada para ocorrer em algum momento da simulação
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

struct evento *cria_evento (int instante, int tipo, struct heroi *h, 
                            struct base *b, struct base *b_aux, struct missao *m) {
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

void selection_sort (struct base *b, int tam) {
    int i, j, idxmenor, temp;
    for (i = 0; i < tam - 1; i++) {
        idxmenor = i;
        for (j = i + 1; j < tam; j++) {
            if (b[i].tmp_dist > b[j].tmp_dist)
                idxmenor = j;
        }
        if (i != idxmenor) {
            // Ordenando distâncias
            temp = b[i].tmp_dist;
            b[i].tmp_dist = b[idxmenor].tmp_dist;
            b[idxmenor].tmp_dist = temp;
        }
    }
}
