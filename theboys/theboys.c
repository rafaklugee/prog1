// programa principal do projeto "The Boys - 2024/2"
// Autor: Rafael Ribeiro Kluge, GRR 20244439

/*
  Defina as estruturas de dados necessárias
  Implemente a criação e destruição dessas estruturas de dados
  Implemente a criação dos eventos iniciais
  Defina uma função para cada evento, contendo inicialmente apenas um printf do mesmo
  Implemente o laço principal de simulação, que chama as funções dos eventos
  Implemente e teste uma função de evento por vez (deixe a missão por último)
  Analise a saída para ver se a sequência de eventos parece correta
  Use o Valgrind para verificar eventuais problemas de memória
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "conjunto.h"
#include "lista.h"
#include "fprio.h"
#include "eventos.h"
#include "mundo.h"

// programa principal
int main ()
{
  int tipo, tempo;
  struct evento *evento_atual;

  // iniciar as entidades e atributos do mundo
  struct mundo *w = malloc (sizeof(struct mundo));
    if (!w)
      return -1;
  cria_mundo(w);

  // criar a fila de eventos futuros
  struct fprio_t *lef = fprio_cria();
    if (!lef)
      return -1;

  // criar os eventos iniciais
  eventos_iniciais(w, lef);

  w->relogio = 0;

  /*repetir
    ev = 1º evento da lista de eventos futuros
    relógio = tempo (ev)
    caso tipo (ev) seja:
        EV1: executa evento 1
        EV2: executa evento 2
        EV3: executa evento 3
        ...
    fim
  até o fim da simulação
  */

  w->eventos_tratados = 0;

  while (w->relogio != w->tempo_final) {

    evento_atual = fprio_retira (lef, &tipo, &tempo) ;
    if (!evento_atual) 
       return -1;

    //if (tipo != evento_atual->tipo)
    //{
    //  printf ("erro: tipo inconsistente\n");
    //  exit (1) ;
    //}
//
    //if (tempo != evento_atual->instante)
    //{
    //  printf ("erro: tempo inconsistente\n");
    //  exit (1);
    //}

    w->relogio = evento_atual->instante;

    //if (w->relogio >= 9989)
    //{
    //  fprio_imprime (lef) ;
    //  printf ("\n") ;
    //}
    //printf ("### vou tratar evento tipo=%d tempo=%d\n", evento_atual->tipo, evento_atual->instante) ;

    switch (evento_atual->tipo) {
      case EVENTO_DESISTE:
        desiste (evento_atual, w, lef);
        break; 
      case EVENTO_ESPERA:
        espera (evento_atual, lef);
        break;
      case EVENTO_AVISA: 
        avisa (evento_atual, lef);
        break;
      case EVENTO_VIAJA:
        viaja (evento_atual, lef);
        break;
      case EVENTO_ENTRA:
        entra (evento_atual, lef);
        break;
      case EVENTO_SAI:
        sai (evento_atual, w, lef);
        break;
      case EVENTO_CHEGA:
        chega (evento_atual, lef);
        break;
      case EVENTO_MORRE: 
        morre (evento_atual, lef);
        break;
      case EVENTO_MISSAO:
        missao (evento_atual, w, lef);
        break;
      // apresentar resultados
      case EVENTO_FIM:
        fim (w);
        break;
    }
    //sleep(1);
    w->eventos_tratados++;
    free (evento_atual) ;
  }

  // destrói a lef
  fprio_destroi (lef);

  // destrói o mundo
  destroi_mundo(w);

  return (0) ;
}

