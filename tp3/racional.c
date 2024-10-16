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

/* Maximo Divisor Comum entre a e b      */
/* calcula o mdc pelo metodo de Euclides */
long mdc (long a, long b) {
  long maior, menor, resto;

// Verifica qual é o maior número para fazer a divisão correta do maior pelo menor->
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

/* Minimo Multiplo Comum entre a e b */
/* mmc = (a * b) / mdc (a, b)        */
long mmc (long a, long b) {
// Usando a fórmula do mmc em relação ao mdc.
  return ((a * b) / mdc(a, b));
}

/* Simplifica o número racional indicado no parâmetro.
 * Por exemplo, se o número for 10/8 muda para 5/4.
 * Retorna 1 em sucesso e 0 se r for inválido ou o ponteiro for nulo.
 * Se ambos numerador e denominador forem negativos, o resultado é positivo.
 * Se o denominador for negativo, o sinal deve migrar para o numerador-> */
int simplifica_r (struct racional *r) {
  if (r->den == 0 || !r)
    return 0;
  
  long divisor;

  divisor = mdc(r->num, r->den);

  r->num = r->num / divisor;
  r->den = r->den / divisor;

/* Se forem dois negativos, troca o sinal dos dois números;
   Se apenas o denominador for negativo, o sinal vai para o numerador-> */
  if (r->den < 0) {
    r->num *= -1;
    r->den *= -1;
  }

  return 1;
}

struct racional *cria_r (long numerador, long denominador) {
  struct racional *rac = malloc (sizeof(struct racional));
  if (!rac) {
    printf ("Erro ao alocar memória de rac!\n");
    return NULL;
  }

  rac->num = numerador;
  rac->den = denominador;

  simplifica_r(rac);

  return rac;
}

void destroi_r (struct racional *r) {
  if (r != NULL)
    free(r);
}

int valido_r (struct racional *r) {
  if (r) {
    if (r->den == 0)
      return 0;
    else
      return 1;
  }
  else
    return 0;
}

void imprime_r (struct racional *r) {
  if (!r) {
    printf ("NULL");
    return;
  }
  else if (r->den == 0) 
    printf ("NaN");
  else if (r->num == 0)
    printf ("0");
  else if (r->den == 1)
    printf ("%ld", r->num);
  else if (r->den == r->num)
    printf ("%ld", r->num);
  else {
    printf ("%ld/%ld", r->num, r->den);
  }
}

int compara_r (struct racional *r1, struct racional *r2) {
  if (!valido_r(r1) || !valido_r(r2))
    return -2;

  long a, b, denComum;
  
  // Fazendo a comparação normalizando os denominadores pelo mmc
  denComum = mmc(r1->den, r2->den);

  // Adicionando uma variável temporária para não alterar o valor de r1 e r2
  a = r1->num * (denComum/r1->den);
  b = r2->num * (denComum/r2->den);

  if (a < b) 
    return -1;
  else if (b > a) 
    return 1;

  return 0;
}

int soma_r (struct racional *r1, struct racional *r2, struct racional *r3) {
  if (r3 == NULL || !valido_r(r1) || !valido_r(r2))
    return 0;

  long denComum;

  denComum = mmc(r1->den, r2->den);

  r1->num *= (denComum/r1->den);
  r2->num *= (denComum/r2->den);

  r3->num = r1->num + r2->num;
  r3->den = denComum;

  simplifica_r(r3);

  return 0;
}

int subtrai_r (struct racional *r1, struct racional *r2, struct racional *r3) {
  if (r3 == NULL || !valido_r(r1) || !valido_r(r2))
    return 0;

  long denComum;

  denComum = mmc(r1->den, r2->den);

  r1->num *= (denComum/r1->den);
  r2->num *= (denComum/r2->den);

  r3->num = r1->num - r2->num;
  r3->den = denComum;

  simplifica_r(r3);

  return 0;
}

int multiplica_r (struct racional *r1, struct racional *r2, struct racional *r3) {
  if (r3 == NULL || !valido_r(r1) || !valido_r(r2))
    return 0;
  
  r3->num = r1->num * r2->num;
  r3->den = r1->den * r2->den;

  simplifica_r(r3);

  return 0;
}

int divide_r (struct racional *r1, struct racional *r2, struct racional *r3) {
  if (r3 == NULL || !valido_r(r1) || !valido_r(r2))
    return 0;
  
  r3->num = r1->num * r2->den;
  r3->den = r1->den * r2->num;

  if (r3->den == 0) {
    printf ("NaN\n");
    exit (1);
  }

  simplifica_r(r3);

  return 0;
}