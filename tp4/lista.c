// TAD lista de números inteiros
// Carlos Maziero - DINF/UFPR, Out 2024
//
// Implementação do TAD - a completar
//
// Implementação com lista encadeada dupla não-circular

#include <stdio.h>
#include <stdlib.h>
#include "lista.h"

struct lista_t *lista_cria () {
    struct lista_t *l;

    l = malloc (sizeof(struct lista_t));
    if (!l)
        return NULL;

    l->prim = NULL;
    l->ult = NULL;
    l->tamanho = 0;

    return l;
}

struct lista_t *lista_destroi (struct lista_t *lst) {
    if (!lst)
        return NULL;

    int item, pos;

    // Remove os itens da lista
    while (lista_tamanho(lst) > 0)
        lista_retira(lst, &item, pos);

    // Destroi a lista
    free(lst);

    return NULL;    
}

int lista_insere (struct lista_t *lst, int item, int pos) {

}

int lista_retira (struct lista_t *lst, int *item, int pos) {

}

int lista_consulta (struct lista_t *lst, int *item, int pos) {

}

int lista_procura (struct lista_t *lst, int valor) {

}

int lista_tamanho (struct lista_t *lst) {
    if (!lst)
        return -1;

    return (lst->tamanho);
}

void lista_imprime (struct lista_t *lst) {
    if (!lst)
        return;
    if (lista_tamanho(lst) <= 0)
        return;

    struct item_t *item;

    item = lst->prim;

    // Passa item por item, começando do primeiro valor, até acabar
    while (item) {
        printf ("%d ", item->valor);
        item = item->prox;
    }
}