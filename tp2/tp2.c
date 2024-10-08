/* 
 * Tipos Abstratos de Dados - TADs
 * Arquivo do programa principal, que usa o TAD racional.
 * Feito em 24/09/2024 para a disciplina CI1001 - Programação 1.
*/

/* coloque aqui seus includes (primeiro os <...>, depois os "...") */
#include <stdio.h>
#include <stdlib.h>
#include "racional.h"
#define TAM 100

/* coloque aqui as funções auxiliares que precisar neste arquivo */
void imprime_r_sem_NaN (struct racional r);
void selection_sort(struct racional r[], long tam);

/* programa principal */
int main () {
  int i, num;
  struct racional v[TAM];
  struct racional soma_total;

//Inicialiando a variável da soma total, que será passada como parâmetro em "soma_r"
  soma_total.num = 0;
  soma_total.den = 1;

  scanf ("%d", &num);
  if (num <= 0 || num >= 100) {
    printf ("O numero precisa estar entre 0 e 100!\n");
    return 0;
  }

// Lendo os números do vetor
  for (i = 0; i < num; i++) {
    scanf ("%ld", &v[i].num);
    scanf ("%ld", &v[i].den);
  }

  printf ("VETOR = ");
  for (i = 0; i < num; i++)
    imprime_r (v[i]);

  printf ("\nVETOR = ");
  for (i = 0; i < num; i++)
    imprime_r_sem_NaN (v[i]);

  printf ("\nVETOR = ");
  selection_sort (v, num); // Ordenando o vetor
  for (i = 0; i < num; i++)
    imprime_r_sem_NaN (v[i]);

  printf ("\nSOMA = ");
  for (i = 0; i < num; i++)
    soma_r (soma_total, v[i], &soma_total);
  imprime_r (soma_total);

  printf ("\n");
  return (0) ;
}
