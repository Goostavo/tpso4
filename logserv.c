#include <stdio.h>
#include <stdlib.h>
#include "hwif.h"
#include "driver.h"



/* logserv_init configura o serviço, antes dele poder ser usado, passando o
 * nome do arquivo a ser utilizado pelo dispositivo
 * Retorna zero se a operação é bem sucedida, outro valor caso contrário.
 */
int logserv_init( char* devname )
{
    int erro;
    erro=driver_init(devname);
    if (erro==0) return 0;
    else return -1;
}
/* termina a utilização do serviço, fechando o dispositivo;
 */
void logserv_shutdown( void )
{
    driver_shutdown();
}

/* logserv_openlog abre um dispositivo com um log já existente,
 * ou cria o log pela primeira vez. O nome do log pode ter até 15
 * caracteres.
 * O modo de operação é leitura, se mode é zero, e escrita caso contrário.
 * No modo de leitura, o cursor do log estará no seu início,
 * no modo de escrita, ele estará ao final do mesmo.
 * Retorna um inteiro que será usado como identificador do log
 * dentro do sistema, ou um valor menor que zero em caso de erro;
 */
int logserv_openlog( char* logname, int mode )
{
    //Rascunho
    return 0;
}
/* logserv_closelog termina a utilização do serviço para
 * aquele log em particular, identificado pelo parâmetro logid;
 */
int logserv_closelog( int logid )
{
    //Rascunho
    return 0;
}
/* logserv_writelog: se o log foi aberto para escrita, escreve a mensagem
 * contida no buffer, até o tamanho buflen, ao final do log, junto com
 * a hora do sistema, como retornada pela função time da biblioteca padrão;
 */
int logserv_writelog( int logid, char* buffer, int buflen)
{
    //Rascunho
    return 0;
}
/* logserv_readlog: se o log foi aberto para leitura, extrai a próxima
 * mensagem do log e a armazena no buffer dado, até o tamanho máximo buflen.
 * Se a mensagem é maior que o buffer, o restante é descartado.
 * Retorna um inteiro que representa o instante em que a mensagem foi
 * registrada no log, como descrito para a operação de escrita;
 * Se já atingiu o fim do log (depois de ler a última mensagem), retorna
 * zero e não atualiza o buffer.
 */
int logserv_readlog( int logid, char* buffer, int buflen)
{
    //Rascunho
    return 0;
}
