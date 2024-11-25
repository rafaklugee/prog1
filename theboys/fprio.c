#include <stdio.h>
#include <stdlib.h>
#include "fprio.h"

struct fprio_t *fprio_cria () {
    struct fprio_t *fila;

    fila = malloc (sizeof(struct fprio_t));
    if (!fila)
        return NULL;

    fila->prim = NULL;
    fila->num = 0;

    return fila;
}

struct fprio_t *fprio_destroi (struct fprio_t *f) {
    if (!f)
        return NULL;
    
    int tipo, prio;
    void *item;

    // Remove os itens da fila.
    while(fprio_tamanho(f) > 0) {
        item = fprio_retira(f, &tipo, &prio);
        free(item);
    }

    // Destrói a fila.
    free(f);

    return NULL;
}

int fprio_insere (struct fprio_t *f, void *item, int tipo, int prio) {
    if (!f || !item)
        return -1;

    struct fpnodo_t *novo;
    struct fpnodo_t *atual;
    struct fpnodo_t *anterior;

    novo = malloc (sizeof(struct fpnodo_t));
    if (!novo)
        return -1;

    novo->item = item;
    novo->tipo = tipo;
    novo->prio = prio;

    novo->prox = NULL;
    atual = f->prim;

    // Verificando se o item já não existe.
    while (atual) {
        if (atual->item == item) {
            free(novo);
            return -1;
        }

        atual = atual->prox;
    }


    // Será inserido na primeira posição, se o tamanho for 0, 
    // ou se tiver menor prioridade.
        if (f->num == 0 || f->prim->prio > prio) {
            novo->prox = f->prim;
            f->prim = novo;
        }
        else {
        // Chegando na posição para fazer a inserção.
            atual = f->prim;

            while (atual != NULL && atual->prio <= prio) {
                anterior = atual;
                atual = atual->prox;
            }

            novo->prox = anterior->prox;
            anterior->prox = novo;
        }

    f->num++;

    return f->num;
}

void *fprio_retira (struct fprio_t *f, int *tipo, int *prio) {
    if (!f || !tipo || !prio)
        return NULL;

    struct fpnodo_t *aux = f->prim;
    void *item = aux->item;

    // Devolvendo o tipo e a prioridade.
    *tipo = aux->tipo;
    *prio = aux->prio;

    // Colocando um novo primeiro valor na lista.
    f->prim = aux->prox;

    f->num--;
    free(aux);

    return item;
}

int fprio_tamanho (struct fprio_t *f) {
    if (!f)
        return -1;
    
    return f->num;
}

void fprio_imprime (struct fprio_t *f) {
    if (!f || fprio_tamanho(f) <= 0)
        return;

    struct fpnodo_t *nodo;

    nodo = f->prim;

    while(nodo) {
        if (nodo->prox)
            printf ("(%d %d) ", nodo->tipo, nodo->prio);
        else
            printf ("(%d %d)", nodo->tipo, nodo->prio);
        
        nodo = nodo->prox;
    }
}