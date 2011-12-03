/*
 * Definição da interface de hardware para o dispositivo de
 * memória de bloco.
 */

#ifndef _HWIF_H_
#define _HWIF_H_

/* A interface de um dispositivo normalmente é definida em termos de
 * endereços de memória que definem áreas que têm significado especial
 * para o dispositivo. No caso de nosso dispositivo simples, há dois
 * registradores (inteiros), um para enviar comandos para o HW e outro de
 * onde se pode ler valores de resposta do dispositivo para operações do
 * sistema. Em geral, para o computador, escrever na área de resposta ou
 * ou ler da área de comando são operações com resultados não definidos,
 * logo tais ações não devem ocorrer. Além dos dois registradores, há
 * duas áreas de memória do tamanho de um bloco do dispositivo destinadas
 * ao armazenamento de dos blocos a serem escritos ou lidos do
 * dispositivo. Essas áreas só serão criadas quando o dispositivo receber
 * seu comando de inicialização (que não é o mesmo que a chamada da
 * função hw_initialize).
 */

extern int   hw_command_reg;
extern int   hw_status_reg;
extern char* hw_block_to_write;
extern char* hw_block_read;

/* As operações são identificadas por valores inteiros pré-definidos,
 * segundo a lista a seguir:
 */

#define HWBLCKFIELDLEN            24
#define HW_OP_START     ( 0 << HWBLCKFIELDLEN)
#define HW_OP_BLCKSIZE  ( 1 << HWBLCKFIELDLEN)
#define HW_OP_BLCKCNT   ( 2 << HWBLCKFIELDLEN)
#define HW_OP_WRBLCK    ( 3 << HWBLCKFIELDLEN)
#define HW_OP_RDBLCK    ( 4 << HWBLCKFIELDLEN)
#define HW_OP_SHUTDOWN  ( 5 << HWBLCKFIELDLEN)

/* No caso das operações de escrita e leitura de blocos, é preciso
 * incluir o no. identificador do bloco no código da operação. Para isso,
 * deve-se fazer um OU lógico do opcode com o no. do bloco. Como o campo
 * tem 24 bits, há espaço suficiente para mais que o limite de blocos e
 * uma operação de adição também tem o efeito desejado, uma vez que
 * nenhum opcode tem o bit mais significativo ligado (o que o tornaria
 * negativo).
 */

/* A primeira função inicializa o dispositivo. Essa seria uma
 * tarefa a ser executada pelo kernel durante o processo de boot de um S.O.
 * O parâmetro é o nome de um arquivo a ser usado para armazenar o
 * que seria a memória do dispositivo. Se o arquivo já existir, seu
 * conteúdo será usado; caso contrário, um arquivo será criado e
 * inicializado com zeros.
 *
 * É importante notar que o tamanho da memória não faz parte da
 * inicialização, pois se trata de uma "placa" adicionada ao sistema.
 * Valor de retorno: 0 se sucesso, < 0 se houver erro (vide errno)
 */

void hw_initialize(char* storage_fname);

/* O hardware é desenvolvido de forma que a simples escrita de um valor
 * no registrador de comandos dispara o processamento no dispositivo.
 * No nosso caso, vamos precisar de uma operação extra para disparar o
 * processamento no nardware explicitamente.
 */

void hw_trigger(void);

/* Como a operação do mesmo é assíncrona, o programa pode até fazer
 * outras coisas depois de escrever o comando, mas precisa esperar por um
 * sinal do hardware para indicar quando a operação foi completada. Como
 * visto nas aulas, esse sinal normalmente é uma interrupção que deve ser
 * tratada e transformada em um evento de sincronização. Para
 * simplificar, vamos criar uma função que bloqueia até que o
 * processamento tenha sido completado, como se o programa recebesse o
 * sinal de término.
 */

void hw_wait(void);

#endif /* _DEVIF_H_ */
