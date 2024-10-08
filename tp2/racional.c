/* 
 * Tipos Abstratos de Dados - TADs
 * Arquivo de implementação para TAD racional.
 * Feito em 20/09/2024 para a disciplina CI1001 - Programação 1.
 *
 * Este arquivo deve conter as implementações das funções cujos protótipos
 * foram definidos em racional.h. Neste arquivo também podem ser definidas
 * funções auxiliares para facilitar a implementação daquelas funções.
*/

/* coloque aqui seus includes (primeiro os <...>, depois os "...") */
#include <stdio.h>
#include <stdlib.h>
#include "racional.h"

/*
 * Implemente aqui as funcoes definidas no racionais.h; caso precise,
 * pode definir aqui funcoes auxiliares adicionais, que devem ser usadas
 * somente neste arquivo.
*/

/* retorna um número aleatório entre min e max, inclusive. */
long aleat (long min, long max) {
  long resultado = rand() % (max - min + 1) + min;

  return resultado;
}

/* Máximo Divisor Comum entre a e b      */
/* calcula o MDC pelo método de Euclides */
long mdc (long a, long b) {
  long maior, menor, resto;

// Verifica qual é o maior número para fazer a divisão correta do maior pelo menor.
  if (a >= b) {
    maior = a;
    menor = b;
  }
  else {
    maior = b;
    menor = a;
  }

// Calculando mdc pelo método de Euclides
  while (menor != 0) {
    resto = maior % menor;
    maior = menor; 
    menor = resto; 
  }

  return maior;
}

/* Mínimo Múltiplo Comum entre a e b */
/* mmc = (a * b) / mdc (a, b)        */
long mmc (long a, long b) {
// Usando a fórmula do mmc em relação ao mdc.
  return ((a * b) / mdc(a, b));
}

/* Recebe um número racional e o simplifica.
 * Por exemplo, ao receber 10/8 deve retornar 5/4.
 * Se ambos numerador e denominador forem negativos, deve retornar um positivo.
 * Se o denominador for negativo, o sinal deve migrar para o numerador.
 * Se r for inválido, devolve-o sem simplificar. */
struct racional simplifica_r (struct racional r) {
  if (r.den == 0)
    return r;
  
  long divisor;

  divisor = mdc(r.num, r.den);

  r.num = r.num / divisor;
  r.den = r.den / divisor;

/* Se forem dois negativos, troca o sinal dos dois números;
   Se apenas o denominador for negativo, o sinal vai para o numerador. */
  if (r.den < 0) {
    r.num *= -1;
    r.den *= -1;
  }

  return r;
}

struct racional cria_r (long numerador, long denominador) {
  struct racional rac;

  rac.num = numerador;
  rac.den = denominador;

  return rac;
}

int valido_r (struct racional r) {
  if (r.den == 0)
    return 0;
  else
    return 1;
}

struct racional sorteia_r (long min, long max) {
  struct racional rac;

  rac.num = aleat(min, max);
  rac.den = aleat(min, max);

  rac = simplifica_r(rac);

  return rac;
}

int compara_r (struct racional r1, struct racional r2) {
  simplifica_r (r1);
  simplifica_r (r2);

  // A multiplicação cruzada retornará o racional de maior/menor valor
  long a = r1.num * r2.den;
  long b = r2.num * r1.den;

  if (a < b)
    return -1;
  else if (b > a)
    return 1;

  return 0;
}

struct racional simplifica_r_ptr (struct racional r[]) {
  if (r->den == 0)
    return *r;
  
  long divisor;

  divisor = mdc(r->num, r->den);

  r->num = r->num / divisor;
  r->den = r->den / divisor;

/* Se forem dois negativos, troca o sinal dos dois números;
   Se apenas o denominador for negativo, o sinal vai para o numerador. */
  if (r->den < 0) {
    r->num *= -1;
    r->den *= -1;
  }

  return *r;
}

void imprime_r (struct racional r) {
  simplifica_r_ptr (&r);

  if (r.den == 0) 
    printf ("NaN ");
  else if (r.num == 0)
    printf ("0 ");
  else if (r.den == 1)
    printf ("%ld ", r.num);
  else if (r.den == r.num)
    printf ("%ld ", r.num);
  else
    printf("%ld/%ld ", r.num, r.den);
}

void imprime_r_sem_NaN (struct racional r) {
  simplifica_r_ptr(&r);

  if (r.den == 0) 
    return;
  else if (r.num == 0)
    printf ("0 ");
  else if (r.den == 1)
    printf ("%ld ", r.num);
  else if (r.den == r.num)
    printf ("%ld ", r.num);
  else
    printf("%ld/%ld ", r.num, r.den);
}

void selection_sort(struct racional r[], long tam) { 
  int i, j, idxmenor;
  struct racional temp;

  for (i = 0; i < tam - 1; i++) {
    simplifica_r_ptr (&r[i]);
    idxmenor = i;
    for (j = i + 1; j < tam; j++) {
      simplifica_r_ptr (&r[j]); // Simplifica a fração antes de comparar
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


int soma_r (struct racional r1, struct racional r2, struct racional *r3) {
  if (r3 == NULL)
    return 0;

  long denComum;

  denComum = mmc(r1.den, r2.den);

  r1.num *= (denComum/r1.den);
  r2.num *= (denComum/r2.den);

  r3->num = r1.num + r2.num;
  r3->den = denComum;

  simplifica_r_ptr (r3);

  return 0;
}

int subtrai_r (struct racional r1, struct racional r2, struct racional *r3) {
  if (r3 == NULL)
    return 0;

  long denComum;

  denComum = mmc(r1.den, r2.den);

  r1.num *= (denComum/r1.den);
  r2.num *= (denComum/r2.den);

  r3->num = r1.num - r2.num;
  r3->den = denComum;

  *r3 = simplifica_r(*r3);

  return 0;
}

int multiplica_r (struct racional r1, struct racional r2, struct racional *r3) {
  if (r3 == NULL)
    return 0;
  
  r3->num = r1.num * r2.num;
  r3->den = r1.den * r2.den;

  *r3 = simplifica_r(*r3);

  return 0;
}

int divide_r (struct racional r1, struct racional r2, struct racional *r3) {
  if (r3 == NULL)
    return 0;
  
  r3->num = r1.num * r2.den;
  r3->den = r1.den * r2.num;

  if (r3->den == 0) {
    printf ("DIVISAO INVALIDA\n");
    exit (1);
  }

  *r3 = simplifica_r(*r3);

  return 0;
}