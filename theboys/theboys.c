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

// programa principal
int main ()
{
  // Iniciar as entidades e atributos do mundo
  // Criar a fila de eventos futuros
  // Criar os eventos iniciais

  /*
  relógio = 0
  repetir
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

  // Apresentar resultados

  // Destruir o mundo

  return (0) ;
}

