#include <stdio.h>
#include <stdlib.h>
#include "hwif.h"
#include "driver.h"

extern int   hw_command_reg;
extern int   hw_status_reg;
extern char* hw_block_to_write;
extern char* hw_block_read;

//Funcoes do driver
int driver_init(char* dev_name)
{
    //rascunho
    return 0;
}

/* ``desliga'' o dispositivo, salvando os dados no arquivo associado;
 */
int driver_shutdown(void)
{
    //Rascunho
    return 0;
}

/* Tamanho do bloco e no. de blocos são uma característica do dispositivo
 * e devem ser obtidos através de comandos para esse fim oferecidos pelo
 * hardware.
 */
int driver_blcksize(void)
{

    hw_command_reg = HW_OP_BLCKSIZE;
    hw_trigger();
    hw_wait();
    return hw_status_reg;
}
int driver_blckcount(void)
{
    //Rascunho
    return 0;
}

/* lê do dispositivo um bloco identificado por blocknum, copiando-o
 * para o buffer apontado por blockbuffer, considerando o tamanho
 * de bloco do dispositivo;
 */
int driver_read(int blocknum, char* blockbuffer)
{
    //Rascunho
    return 0;
}

/* escreve o conteúdo do buffer para o bloco de número blocknum,
 * considerando o tamanho de bloco do dispositivo.
 */
int driver_write(int blocknum, char* blockbuffer)
{
    //Rascunho
    return 0;
}
