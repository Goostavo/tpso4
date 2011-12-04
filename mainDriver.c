//Trabalho de Sistemas Operacionais
//Gustavo Garcia e Vinícius Lana
//Main para testes do driver

#include <stdio.h>
#include <stdlib.h>
#include "driver.h"

int main(void)
{
    int temp, temp2;
    char* buffer;
    buffer = (char*)malloc(4096);
    temp =driver_init("Teste_log");
    printf("\n\nInicializado com status:%d\n",temp);
    temp=driver_blcksize();
    printf("Block size:%d\n",temp);
    temp=driver_blckcount();
    printf("Block count:%d\n",temp);

    driver_shutdown();
    return 0;
}
