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
#include "conjunto.h"
#include "lista.h"
#include "fprio.h"
#include "eventos.h"
#include "mundo.h"

// programa principal
int main ()
{
  // iniciar as entidades e atributos do mundo
  struct mundo *w = malloc (sizeof(struct mundo));
    if (w == NULL)
      return -1;
  cria_mundo(w);

  // criar a fila de eventos futuros
  struct fprio_t *LEF = fprio_cria();
    if (LEF == NULL)
      return -1;

  // criar os eventos iniciais
  eventos_iniciais(w, LEF);

  
  //relógio = 0
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

  while (w->relogio != 525600) {
    struct evento *evento_atual = LEF->prim->item; // Tá certo isso ?

    w->relogio = evento_atual->instante;

    // PRECISO ITERAR DE MANEIRA ESPECÍFICA...
    switch (evento_atual->tipo) {
      case EVENTO_DESISTE:
        desiste (evento_atual->instante, w->herois, w->bases, w, LEF);
        break; 
      case EVENTO_ESPERA:
        espera (evento_atual->instante, w->herois, w->bases, LEF, w->bases->lst_espera);
        break;
      case EVENTO_AVISA: 
        avisa (evento_atual->instante, w->herois, w->bases, LEF, w->bases->lst_espera);
        break;
      case EVENTO_VIAJA:
        viaja (evento_atual->instante, w->herois, w->bases, w->bases, LEF);
        break;
      case EVENTO_ENTRA:
        entra (evento_atual->instante, w->herois, w->bases, LEF);
        break;
      case EVENTO_SAI:
        sai (evento_atual->instante, w->herois, w->bases, w, LEF, w->bases->lst_espera);
        break;
      case EVENTO_CHEGA: 
        chega (evento_atual->instante, w->herois, w->bases, LEF);
        break;
      case EVENTO_MORRE: 
        morre (evento_atual->instante, w->herois, w->bases, LEF, w->bases->lst_espera);
        break;
      case EVENTO_MISSAO:
        missao (evento_atual->instante, w->missoes, w, LEF);
        break;
      // Apresentar resultados
      case EVENTO_FIM:
        fim (w);
        break;
    }
    fprio_retira(LEF, &evento_atual->tipo, &evento_atual->instante);

  }

  // Destruir o mundo
  free(w);

  return (0) ;
}

