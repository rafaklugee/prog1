// Programa principal do projeto "The Boys - 2024/2"
// Autor: Rafael Ribeiro Kluge, GRR 20244439

#include <stdio.h>
#include <stdlib.h>
#include "conjunto.h"
#include "lista.h"
#include "fprio.h"
#include "eventos.h"
#include "mundo.h"

// Programa principal
int main ()
{
  int tipo, tempo;
  struct evento *evento_atual;

  // Iniciar as entidades e atributos do mundo
  struct mundo *w = malloc (sizeof(struct mundo));
    if (!w)
      return -1;
  cria_mundo(w);

  // Criar a fila de eventos futuros
  struct fprio_t *lef = fprio_cria();
    if (!lef)
      return -1;

  // Criar os eventos iniciais
  eventos_iniciais(w, lef);

  w->relogio = 0;
  w->eventos_tratados = 0;

  while (w->relogio != w->tempo_final) {

    evento_atual = fprio_retira (lef, &tipo, &tempo) ;
    if (!evento_atual) 
       return -1;

    w->relogio = evento_atual->instante;

    switch (evento_atual->tipo) {
      case EVENTO_DESISTE:
        desiste (evento_atual, w, lef);
        break; 
      case EVENTO_ESPERA:
        espera (evento_atual, lef);
        break;
      case EVENTO_AVISA: 
        avisa (evento_atual, w, lef);
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
      // Apresentar resultados
      case EVENTO_FIM:
        fim (w);
        break;
    }
    w->eventos_tratados++;
    free (evento_atual);
  }

  // Destrói a lef
  fprio_destroi (lef);

  // Destrói o mundo
  destroi_mundo(w);

  return (0) ;
}
