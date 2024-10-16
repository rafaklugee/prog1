/* 
 * Tipos Abstratos de Dados - TADs
 * Arquivo do programa principal, que usa o TAD racional.
 * Feito em 24/09/2024 para a disciplina CI1001 - Programação 1.
*/

/* coloque aqui seus includes (primeiro os <...>, depois os "...") */
#include <stdio.h>
#include <stdlib.h>
#include "racional.h"

void selection_sort (struct racional **r, long tam);
void remove_invalido (struct racional **r, long *tam);
void libera_racionais (struct racional **r, long tam);
void imprime_vetor (struct racional **r, long tam);

/* programa principal */
int main ()
{
  int i;
  long num, den, tam;

  struct racional **v;
  struct racional *soma;

  scanf ("%ld", &tam);
  if (tam <= 0 || tam >= 100) {
    printf ("O valor nao compreende as limitacoes!\n");
    return 0;
  }

  // Alocação dinâmica do vetor
  v = malloc (tam * sizeof(struct racional *));

  if (!v) {
    printf ("Erro ao alocar o vetor!\n");
    return 0;
  }

  // Lendo os números do vetor
  for (i = 0; i < tam; i++) {
    scanf ("%ld", &num);
    scanf ("%ld", &den);
    v[i] = cria_r (num, den);
  }

  printf ("VETOR = ");
  imprime_vetor (v, tam);

  printf ("\nVETOR = ");
  remove_invalido (v, &tam); // Removendo os "NaN"
  imprime_vetor (v, tam);

  printf ("\nVETOR = ");
  selection_sort (v, tam); // Ordenando o vetor
  imprime_vetor (v, tam);

  printf ("\nSOMA = ");
  soma = cria_r (0, 1);
  for (i = 0; i < tam; i++) {
    soma_r (soma, v[i], soma);
  }
  imprime_r (soma);

  printf ("\nVETOR = ");
  libera_racionais (v, tam);
  imprime_vetor (v, tam);

  // Liberando o vetor de ponteiros
  free(v);
  v = NULL;

  // Liberando o espaço utilizado por soma
  destroi_r (soma);

  printf ("\n");
  return (0);
}

void remove_invalido (struct racional **r, long *tam) {
  int i = 0;
  while (i < *tam) {
    if (!valido_r(r[i])) {
      free(r[i]);
      r[i] = r[*tam - 1];
      (*tam)--;
    }
    else
      i++;
  }
}

void selection_sort(struct racional **r, long tam) { 
  int i, j, idxmenor;
  struct racional *temp;

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

void libera_racionais (struct racional **r, long tam) {
  int i;
  for (i = 0; i < tam; i++) {
    free(r[i]);
    r[i] = NULL;
  }
}

void imprime_vetor (struct racional **r, long tam) {
  for (int i = 0; i < tam; i++) {
    imprime_r (r[i]);
    if (i < tam - 1) 
    printf (" ");
  }
}