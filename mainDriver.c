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
    int logid1;
    char TesteBuffer[25];
    logserv_init("Log1.dat");
    logid1 = logserv_openlog("Logaritmo",0);
    logserv_readlog(logid1,TesteBuffer,20);
    printf("%s\n",TesteBuffer);
    logserv_readlog(logid1,TesteBuffer,20);
    printf("%s\n",TesteBuffer);
    logserv_readlog(logid1,TesteBuffer,20);
    printf("%s\n",TesteBuffer);

    logserv_shutdown();
    return 0;
}
