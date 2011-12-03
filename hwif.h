/*
 * Defini��o da interface de hardware para o dispositivo de
 * mem�ria de bloco.
 */

#ifndef _HWIF_H_
#define _HWIF_H_

/* A interface de um dispositivo normalmente � definida em termos de
 * endere�os de mem�ria que definem �reas que t�m significado especial
 * para o dispositivo. No caso de nosso dispositivo simples, h� dois
 * registradores (inteiros), um para enviar comandos para o HW e outro de
 * onde se pode ler valores de resposta do dispositivo para opera��es do
 * sistema. Em geral, para o computador, escrever na �rea de resposta ou
 * ou ler da �rea de comando s�o opera��es com resultados n�o definidos,
 * logo tais a��es n�o devem ocorrer. Al�m dos dois registradores, h�
 * duas �reas de mem�ria do tamanho de um bloco do dispositivo destinadas
 * ao armazenamento de dos blocos a serem escritos ou lidos do
 * dispositivo. Essas �reas s� ser�o criadas quando o dispositivo receber
 * seu comando de inicializa��o (que n�o � o mesmo que a chamada da
 * fun��o hw_initialize).
 */

extern int   hw_command_reg;
extern int   hw_status_reg;
extern char* hw_block_to_write;
extern char* hw_block_read;

/* As opera��es s�o identificadas por valores inteiros pr�-definidos,
 * segundo a lista a seguir:
 */

#define HWBLCKFIELDLEN            24
#define HW_OP_START     ( 0 << HWBLCKFIELDLEN)
#define HW_OP_BLCKSIZE  ( 1 << HWBLCKFIELDLEN)
#define HW_OP_BLCKCNT   ( 2 << HWBLCKFIELDLEN)
#define HW_OP_WRBLCK    ( 3 << HWBLCKFIELDLEN)
#define HW_OP_RDBLCK    ( 4 << HWBLCKFIELDLEN)
#define HW_OP_SHUTDOWN  ( 5 << HWBLCKFIELDLEN)

/* No caso das opera��es de escrita e leitura de blocos, � preciso
 * incluir o no. identificador do bloco no c�digo da opera��o. Para isso,
 * deve-se fazer um OU l�gico do opcode com o no. do bloco. Como o campo
 * tem 24 bits, h� espa�o suficiente para mais que o limite de blocos e
 * uma opera��o de adi��o tamb�m tem o efeito desejado, uma vez que
 * nenhum opcode tem o bit mais significativo ligado (o que o tornaria
 * negativo).
 */

/* A primeira fun��o inicializa o dispositivo. Essa seria uma
 * tarefa a ser executada pelo kernel durante o processo de boot de um S.O.
 * O par�metro � o nome de um arquivo a ser usado para armazenar o
 * que seria a mem�ria do dispositivo. Se o arquivo j� existir, seu
 * conte�do ser� usado; caso contr�rio, um arquivo ser� criado e
 * inicializado com zeros.
 *
 * � importante notar que o tamanho da mem�ria n�o faz parte da
 * inicializa��o, pois se trata de uma "placa" adicionada ao sistema.
 * Valor de retorno: 0 se sucesso, < 0 se houver erro (vide errno)
 */

void hw_initialize(char* storage_fname);

/* O hardware � desenvolvido de forma que a simples escrita de um valor
 * no registrador de comandos dispara o processamento no dispositivo.
 * No nosso caso, vamos precisar de uma opera��o extra para disparar o
 * processamento no nardware explicitamente.
 */

void hw_trigger(void);

/* Como a opera��o do mesmo � ass�ncrona, o programa pode at� fazer
 * outras coisas depois de escrever o comando, mas precisa esperar por um
 * sinal do hardware para indicar quando a opera��o foi completada. Como
 * visto nas aulas, esse sinal normalmente � uma interrup��o que deve ser
 * tratada e transformada em um evento de sincroniza��o. Para
 * simplificar, vamos criar uma fun��o que bloqueia at� que o
 * processamento tenha sido completado, como se o programa recebesse o
 * sinal de t�rmino.
 */

void hw_wait(void);

#endif /* _DEVIF_H_ */
