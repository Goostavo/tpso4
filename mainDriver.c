//Trabalho de Sistemas Operacionais
//Gustavo Garcia e Vinícius Lana
//Main para testes do driver

#include <stdio.h>
#include "driver.h"

int main(void)
{
    int temp;
    temp =driver_init("Teste_log");
    printf("\n\nInicializado com status:%d\n",temp);
    temp=driver_blcksize();
    printf("Block size:%d\n",temp);
    temp=driver_blckcount();
    printf("Block count:%d\n",temp);
    return 0;
}
