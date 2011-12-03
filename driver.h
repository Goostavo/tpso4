/* declara��o da interface a ser implementada para o acionador de
 * dispositivo que controlar� o dispositivo de mem�ria n�o vol�til
 */

#ifndef _DRIVER_H_
#define _DRIVER_H_

/*
 * inializa o dispositivo associado de armazenamento.
 * Retorna zero se tudo der certo e um valor menor que zero caso
 * contr�rio.
 */

int driver_init(char* dev_name);

/* ``desliga'' o dispositivo, salvando os dados no arquivo associado;
 */
int driver_shutdown(void);

/* Tamanho do bloco e no. de blocos s�o uma caracter�stica do dispositivo
 * e devem ser obtidos atrav�s de comandos para esse fim oferecidos pelo
 * hardware.
 */
int driver_blcksize(void);
int driver_blckcount(void);

/* l� do dispositivo um bloco identificado por blocknum, copiando-o
 * para o buffer apontado por blockbuffer, considerando o tamanho
 * de bloco do dispositivo;
 */
int driver_read(int blocknum, char* blockbuffer);

/* escreve o conte�do do buffer para o bloco de n�mero blocknum,
 * considerando o tamanho de bloco do dispositivo.
 */
int driver_write(int blocknum, char* blockbuffer);

#endif // _DRIVER_H_
