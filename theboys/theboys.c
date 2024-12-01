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

  struct evento *evento_atual = LEF->prim->item;
  if (evento_atual->b == NULL) {
    printf("\nBase do evento é NULL\n");
  } else {
      if (evento_atual->b->lst_espera == NULL) {
          printf("\nLista de espera é NULL\n");
      } else {
          printf("\nLista de espera existe\n");
      }
  }

  printf ("\no tamanho eh: %d\n", lista_tamanho(evento_atual->b->lst_espera));

  
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
exit(1);

  while (w->relogio != 525600) {
    struct evento *evento_atual = LEF->prim->item; // Tá certo isso ?

    w->relogio = evento_atual->instante;

    //printf ("\nestou tratando da base: %d com lotacao %d do tipo %d com tamanho %d\n", evento_atual->b->id, evento_atual->b->id, evento_atual->tipo, lista_tamanho(evento_atual->b->lst_espera));

    // PRECISO ITERAR DE MANEIRA ESPECÍFICA...
    switch (evento_atual->tipo) {
      case EVENTO_DESISTE:
        desiste (evento_atual->instante, evento_atual->h, evento_atual->b, w, LEF);
        break; 
      case EVENTO_ESPERA:
        espera (evento_atual->instante, evento_atual->h, evento_atual->b, LEF, w->bases->lst_espera);
        break;
      case EVENTO_AVISA: 
        avisa (evento_atual->instante, evento_atual->h, evento_atual->b, LEF, w->bases->lst_espera);
        break;
      case EVENTO_VIAJA:
        viaja (evento_atual->instante, evento_atual->h, evento_atual->b, evento_atual->b, LEF);
        break;
      case EVENTO_ENTRA:
        entra (evento_atual->instante, evento_atual->h, evento_atual->b, LEF);
        break;
      case EVENTO_SAI:
        sai (evento_atual->instante, evento_atual->h, evento_atual->b, w, LEF, w->bases->lst_espera);
        break;
      case EVENTO_CHEGA:
      printf ("\nbase id: %d ; lista_espera_tamanho: %d", evento_atual->b->id, lista_tamanho(evento_atual->b->lst_espera));
        chega (evento_atual->instante, evento_atual->h, evento_atual->b, LEF);
        break;
      case EVENTO_MORRE: 
        morre (evento_atual->instante, w, evento_atual->h, evento_atual->b, LEF, w->bases->lst_espera);
        break;
      //case EVENTO_MISSAO:
      //  missao (evento_atual->instante, w->missoes, w, LEF);
      //  break;
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

