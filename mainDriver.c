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

    for (temp=0;temp<511;temp++)
    {
        buffer[temp] = (char)temp;
    }
    buffer[511] = '\0';

    for(temp=1;temp<10;temp++)
    {
        driver_write(temp,buffer);
    }

    for(temp=1;temp<10;temp++)
    {
        for (temp2=0;temp2<512;temp2++)
        {
            buffer[temp] = 0;
        }
        driver_read(temp,buffer);
        printf("%s\n",buffer);
    }

    return 0;
}
