//Trabalho de Sistemas Operacionais
//Gustavo Garcia e Vinícius Lana
//Main para testes do driver

#include <stdio.h>
#include <stdlib.h>
#include "logserv.h"

//Modos de Operacao
#define LEITURA 0
#define ESCRITA 1
int main(void)
{
    logserv_init("Log1.dat");
    logserv_openlog("Logaritmo",1);
    logserv_shutdown();
    return 0;
}
