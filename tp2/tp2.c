/* 
 * Tipos Abstratos de Dados - TADs
 * Arquivo do programa principal, que usa o TAD racional.
 * Feito em 24/09/2024 para a disciplina CI1001 - Programação 1.
*/

#include <stdio.h>
#include <stdlib.h>
#include "racional.h"
#define TAM 100

void selection_sort (struct racional r[], long tam);
void remove_invalido (struct racional r[], long *tam);

/* programa principal */
int main () {
  int i;
  long num, den, numIteracoes;
  struct racional v[TAM];
  struct racional soma;

  scanf ("%ld", &numIteracoes);
  if (numIteracoes <= 0 || numIteracoes >= 100) {
    printf ("O numero precisa estar entre 0 e 100!\n");
    return 0;
  }

// Lendo os números do vetor
  for (i = 0; i < numIteracoes; i++) {
    scanf ("%ld", &num);
    scanf ("%ld", &den);
    v[i] = cria_r (num, den);
  }

  printf ("VETOR = ");
  for (i = 0; i < numIteracoes; i++) {
    imprime_r (v[i]);
    printf (" ");
  }

  printf ("\nVETOR = ");
  remove_invalido (v, &numIteracoes); // Removendo os "NaN"
  for (i = 0; i < numIteracoes; i++) {
    imprime_r (v[i]);
    printf (" ");
  }

  printf ("\nVETOR = ");
  selection_sort (v, numIteracoes); // Ordenando o vetor
  for (i = 0; i < numIteracoes; i++) {
    imprime_r (v[i]);
    printf (" ");
  }

  printf ("\nSOMA = ");
  soma = cria_r (0, 1);
  for (i = 0; i < numIteracoes; i++) {
    soma_r (soma, v[i], &soma);
  }
  imprime_r (soma);

  printf ("\n");
  return (0) ;
}

void selection_sort(struct racional r[], long tam) { 
  int i, j, idxmenor;
  struct racional temp;

  for (i = 0; i < tam - 1; i++) {
    idxmenor = i;
    for (j = i + 1; j < tam; j++) {
      if (compara_r(r[j], r[idxmenor]) < 0) {
        idxmenor = j;
      }
    }
      if (i != idxmenor) {
          temp = r[i];
          r[i] = r[idxmenor];
          r[idxmenor] = temp;
      }
  }
}

void remove_invalido (struct racional r[], long *tam) {
  int i = 0;
  while (i < *tam) {
    if (!valido_r(r[i])) {
      r[i] = r[*tam - 1];
      (*tam)--;
    }
    else
      i++;
  }
}