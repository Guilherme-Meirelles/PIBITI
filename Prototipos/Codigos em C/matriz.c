#include <stdlib.h>
#include <stdio.h>
int main(){
    int **matriz;
    int numeros[] = {1,2,3,4,5,6,7,8,9,0};
    
    matriz = malloc(8 * sizeof(int*));
    for (int i = 0; i < 8; i++){
        matriz[i] = malloc(10* sizeof(int*));
        for (int j = 0; j< 10; j++){
            matriz[i][j] = numeros[j];
        
    }
    }
    for (int i = 0; i< 8; i++){
        printf("[");
        for (int j = 0; j < 10; j++){
            printf(" %d,", matriz[i][j]);
        }
        printf("]\n");
    }
    printf("\n");
    int numeros2[] = {0,9,8,7,6,5,4,3};
    int len = sizeof(numeros2)/sizeof(int);
    for (int i = 0; i < 8; i++){
        matriz[i] = realloc(matriz[i], len * sizeof(int*));
        for (int j = 0; j< len; j++){
            matriz[i][j] = numeros2[j];
        
    }
    }
    for (int i = 0; i< 8; i++){
        printf("[");
        for (int j = 0; j < len; j++){
            printf(" %d,", matriz[i][j]);
        }
        printf("]\n");
    }

    return 0;
}