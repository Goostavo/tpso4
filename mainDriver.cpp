//Trabalho de Sistemas Operacionais
//Gustavo Garcia e Vinícius Lana
//Main para testes do driver

#include <iostream>
#include "driver.h"

int main(void)
{
    int temp;
    temp = driver::driver_init("Teste_log");
    std::cout<<"Inicializado com status:"<<temp<<std::endl;
    temp=driver::driver_blcksize();
    std::cout<<"Block Size:"<<temp<<std::endl;
    temp=driver::driver_blckcount();
    std::cout<<"Block Count:"<<temp<<std::endl;
    return 0;
}
