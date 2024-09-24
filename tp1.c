/* 
 * Tipos Abstratos de Dados - TADs
 * Arquivo do programa principal, que usa o TAD racional.
 * Feito em 16/09/2024 para a disciplina CI1001 - Programação 1.
*/

/* coloque aqui seus includes (primeiro os <...>, depois os "...") */
#include <stdio.h>
#include <stdlib.h>
#include "racional.h"

/* programa principal */
int main ()
{
    int iteracoes, max, i;
    struct racional rac1, rac2;

    srand (0); /* use assim, com zero */

    scanf ("%d", &iteracoes);
    if (iteracoes <= 0 || iteracoes >= 100) {
        printf ("O numero de iteracoes precisa estar entre 0 e 100!");
        return 0;
    }
    scanf ("%d", &max);
    if (max <=0 || max >= 30) {
        printf ("O numero maximo precisa estar entre 0 e 30!");
        return 0;
    }

    for (i = 1; i <= iteracoes; i++) {
        printf ("%d: ", i);

        rac1 = sorteia_r(-max, max);
        rac2 = sorteia_r(-max, max);

        imprime_r(rac1);
        imprime_r(rac2);

        if (valido_r(rac1) && valido_r(rac2)) {
            // O único caso em que os dois números forem válidos, mas a divisão é inválida, é se rac2 = 0
            if (rac2.num == 0) {
                printf ("DIVISAO INVALIDA\n");
                return 0;
            }

            imprime_r(soma_r(rac1, rac2));
            imprime_r(subtrai_r(rac1, rac2));
            imprime_r(multiplica_r(rac1, rac2));
            imprime_r(divide_r(rac1, rac2));
        }
        else {
            printf ("NUMERO INVALIDO\n");
            return 0;
        }

        printf ("\n");
    }
    
    return (0);
}