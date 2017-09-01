//
//  gameLabRedes.c
//  
//
//  Created by Bruno Moretto on 9/1/17.
//
//

#include <stdio.h>
#include <string.h>


int main(){

    char nome[20];
    printf("Bem vindo ao mundo de Erechim, qual é o seu nome, aventureiro(a)\n");
    scanf("%s", nome);
    printf("%s, Existem quatro familias que controlam a terra de Erechim, os Morgs, que lideram o norte, os Badalottis que contralam as Ilhas de Pedra, os Tormen que contrloam o Castelo Branco e os temiveis Lorenzos que controlam o Sul e lideram o mundo livre.\n Voce tem uma oportunidade rara de se juntar a uma dessas familias, qual delas voce gostaria de se juntar?\n", nome);
    printf(" 1 - Morgs\n 2 - Badalottis\n 3 - Tormen\n 4 - Lorenzos\n");
    int familia = 0;
    char suaFamilia[20];
    int loop = 0;
    //scanf("%d", &familia);
    
    while (loop == 0) {
        scanf("%d", &familia);
        switch (familia) {
            case 1:
                printf("Voce se juntou aos Morgs\n");
                strncpy(suaFamilia, "Morgs", 20);
                loop = 1;
                break;
                
            case 2:
                printf("Voce se juntou aos Badalottis\n");
                strncpy(suaFamilia, "Badalottis", 20);
                loop = 1;
                break;
                
            case 3:
                printf("Voce se juntou aos Tormen\n");
                strncpy(suaFamilia, "Tormen", 20);
                loop = 1;
                break;
                
            case 4:
                printf("Voce se juntou aos Lorenzos\n");
                strncpy(suaFamilia, "Lorenzos", 20);
                loop = 1;
                break;
                
            default:
                printf("Valor invalido\n");
                break;
        }

    }
    
    
    return 0;
}
