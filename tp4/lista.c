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
    if (!lst)
        return -1;

    // Adicionar condição para lista cheia...
    
    struct item_t *novo;

    novo = malloc (sizeof(struct item_t));
    if (!novo)
        return -1;

    novo->valor = item;
    novo->ant = NULL;
    novo->prox = NULL;

    if (pos == -1 || pos >= lst->tamanho) {
        if (lst->tamanho == 0) {
            lst->prim = novo;
            lst->ult = novo;
        }
        else {
            novo->ant = lst->ult;
            lst->ult->prox = novo;
            lst->ult = novo;
        }
    } else {
        struct item_t *aux = lst->prim;

        // Chegando a posição para inserção
        for (int i = 0; i < pos; i++)
            aux = aux->prox;

        // Ajustando os ponteiros de "novo"
        novo->prox = aux;      
        novo->ant = aux->ant;

        // Se o valor antigo não for o primeiro valor da lista,
        // o valor anterior a ele aponta para o próximo (novo).
        // Caso contrário, o novo valor terá que ser o primeiro da lista; 
        if (aux->ant) { 
        aux->ant->prox = novo;
        } else {
        lst->prim = novo;
        }

        aux->ant = novo;
    }

    lst->tamanho++;
    return lst->tamanho;

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