/* 
 * Tipos Abstratos de Dados - TADs
 * Arquivo de implementação para TAD racional.
 * Feito em 16/09/2024 para a disciplina CI1001 - Programação 1.
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
long aleat (long min, long max)
{
  long resultado = rand() % (max - min + 1) + min;

  return resultado;
}

/* Máximo Divisor Comum entre a e b      */
/* calcula o MDC pelo método de Euclides */
long mdc (long a, long b)
{
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
long mmc (long a, long b)
{
// Usando a fórmula do mmc em relação ao mdc.
  return ((a * b) / mdc(a, b));
}

/* Recebe um número racional e o simplifica.
 * Por exemplo, ao receber 10/8 deve retornar 5/4.
 * Se ambos numerador e denominador forem negativos, deve retornar um positivo.
 * Se o denominador for negativo, o sinal deve migrar para o numerador.
 * Se r for inválido, devolve-o sem simplificar. */
struct racional simplifica_r (struct racional r)
{
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

void imprime_r (struct racional r) {

  simplifica_r(r);

  if (r.den == 0) 
    printf ("INVALIDO ");
  else if (r.num == 0)
    printf ("0 ");
  else if (r.den == 1)
    printf ("%ld ", r.num);
  else if (r.den == r.num)
    printf ("%ld ", r.num);
  else
    printf("%ld/%ld ", r.num, r.den);
}

struct racional soma_r (struct racional r1, struct racional r2) {
  struct racional rac;
  long denComum;

  denComum = mmc(r1.den, r2.den);

  r1.num *= (denComum/r1.den);
  r2.num *= (denComum/r2.den);

  rac.num = r1.num + r2.num;
  rac.den = denComum;

  rac = simplifica_r(rac);

  return rac;
}

struct racional subtrai_r (struct racional r1, struct racional r2) {
  struct racional rac;
  long denComum;

  denComum = mmc(r1.den, r2.den);

  r1.num *= (denComum/r1.den);
  r2.num *= (denComum/r2.den);

  rac.num = r1.num - r2.num;
  rac.den = denComum;

  rac = simplifica_r(rac);

  return rac;
}

struct racional multiplica_r (struct racional r1, struct racional r2) {
  struct racional rac;

  rac.num = r1.num * r2.num;
  rac.den = r1.den * r2.den;

  rac = simplifica_r(rac);

  return rac;
}

struct racional divide_r (struct racional r1, struct racional r2) {
  struct racional rac;

  rac.num = r1.num * r2.den;
  rac.den = r1.den * r2.num;

  if (rac.den == 0) {
    printf ("DIVISAO INVALIDA\n");
    exit (1);
  }

  rac = simplifica_r(rac);

  return rac;
}