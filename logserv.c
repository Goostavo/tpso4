#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logserv.h"
#include "hwif.h"
#include "driver.h"

Log_s logTable[16];//Vetor de alocação de logs
char tempbuffer[512];
int blocksize=0;
int blockcount=0;

/* logserv_init configura o serviço, antes dele poder ser usado, passando o
 * nome do arquivo a ser utilizado pelo dispositivo
 * Retorna zero se a operação é bem sucedida, outro valor caso contrário.
 */
int logserv_init( char* devname )
{
    int aux,i;
    int blocknum=0;
    int erro;
    erro=driver_init(devname);
    if (erro!=0) return -1;

    //Limpa tabela de logs
    for(aux=0;aux<16;aux++)
    {
        logTable[aux].existe=0;
    }

    //Le parametros do dispositivo
    blocksize=driver_blcksize();
    blockcount=driver_blckcount();

    //Monta tabela de logs
    while(blocknum<blockcount)
    {
        driver_read(blocknum,tempbuffer);
        aux=((int)tempbuffer[0] << 24)|((int)tempbuffer[1] << 16)|((int)tempbuffer[2] << 8)|((int)tempbuffer[3]);
        //Se for espaço livre, termina.
        if (aux==0) break;
        //Verifica (formalmente) se é o primeiro
        if (tempbuffer[0]==-128)
        {
            aux = 0;
            //Guarda na tabela de logs
            while(aux < 16 && logTable[aux].existe==1)
            {
                aux++;
            }
            if (aux >= 16)
            {
                printf("Erro, este arquivo de Log possui mais de 16 logs.\n");
            }
            else
            {
                logTable[aux].existe=1;
                for(i=0;i<16;i++)
                {
                    logTable[aux].nome[i]=tempbuffer[4+i];
                }
                logTable[aux].startBlock = blocknum;
                logTable[aux].mode = -1;              //Desativado para leitura/escrita por padrao
            }
        }
        blocknum++;
    }
    return 0;
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
    int aux, aux2, auxPos=0;
    //Verifica na tablea de logs se este ja existe.
    for (aux =0; aux<16; aux++)
    {
        if (strcmp(logname,logTable[aux].nome) == 0)
            break;
    }
    //Se aux<16 o log ja existe
    if (aux <16)
    {
        printf("Log encontrado, ID:%d, modo: %d \n",aux,mode);
        logTable[aux].mode = mode;
        //Se for modo de leitura pega o primeiro lugar
        if (mode == 0)
        {
            logTable[aux].pointBLK = logTable[aux].startBlock;
            logTable[aux].pointPOS = 20;
        }
        else
        {
            logTable[aux].pointBLK = achaBlocoFinal(logTable[aux].startBlock);
            logTable[aux].pointPOS = achaPosFinal(logTable[aux].pointBLK);
        }
        return aux;
    }
    //Senao cria o log
    for(aux=0;aux<16;aux++)
    {
        if(logTable[aux].existe==0)
        {
            logTable[aux].existe=1;
            strcpy(logTable[aux].nome,logname);
            printf("Log criado, ID:%d, modo: %d \n",aux,mode);
            logTable[aux].mode = mode;
            auxPos = achaBlocoLivre();
            logTable[aux].startBlock = auxPos;
            logTable[aux].pointBLK = auxPos;
            logTable[aux].pointPOS = 20;

            for(aux2=4; aux2<512;aux2++)
            {
                tempbuffer[aux2] = 0;
            }
            //Sobe o bit inicial e seta os outros em FFFFFF
            tempbuffer[0] = -128;
            tempbuffer[1] = -1;
            tempbuffer[2] = -1;
            tempbuffer[3] = -1;
            //Escreve o nome
            for(aux2 = 0; aux2<16;aux2++)
            {
                tempbuffer[4+aux2]=logname[aux2];
                if (logname[aux2]=='\0')
                {
                    aux2++;
                    while (aux2<16)
                    {
                        tempbuffer[4+aux2] = 0;
                        aux2++;
                    }
                }
            }
            //Marca posicao incial
            tempbuffer[20]='\n';
            driver_write(auxPos, tempbuffer);
            return aux;
        }
    }
    return -1;
}
/* logserv_closelog termina a utilização do serviço para
 * aquele log em particular, identificado pelo parâmetro logid;
 */
int logserv_closelog( int logid )
{
    //Como o driver tem suporte a logs concorrentes, para desativar a
    //leitura/escrita basta deixar o modo como invalido.
    logTable[logid].mode = -1;
    return 0;
}
/* logserv_writelog: se o log foi aberto para escrita, escreve a mensagem
 * contida no buffer, até o tamanho buflen, ao final do log, junto com
 * a hora do sistema, como retornada pela função time da biblioteca padrão;
 */
int logserv_writelog( int logid, char* buffer, int buflen)
{
    int contaChar = 0;
    int posicao;
    int aux;
    if (logTable[logid].mode != 1)
    {
        printf("Erro:O log nao esta em modo de escrita!\n");
        return -1;
    }
    //Carrega pagina
    driver_read(logTable[logid].pointBLK,tempbuffer);
    posicao = logTable[logid].pointPOS;
    //Comeca a escrever os caracteres
    while (contaChar < (buflen+1))
    {
        if (posicao < blocksize)
        {
            if (contaChar < buflen)
                tempbuffer[posicao] = buffer[contaChar];
            else
                tempbuffer[posicao]='\n';
            contaChar++; posicao++;
        }
        else
        {
            //Salva bloco
            driver_write(logTable[logid].pointBLK,tempbuffer);
            //Encontra proximo espaco livre
            aux = achaBlocoLivre();
            //Refaz as linkagens
            driver_read(logTable[logid].pointBLK,tempbuffer);
            tempbuffer[1]=(char)((aux << 16));
            tempbuffer[2]=(char)((aux << 8));
            tempbuffer[3]=(char)(aux);
            driver_write(logTable[logid].pointBLK,tempbuffer);
            driver_read(aux,tempbuffer);
            tempbuffer[0]=0;
            tempbuffer[1]=-1;
            tempbuffer[2]=-1;
            tempbuffer[3]=-1;
            logTable[logid].pointBLK = aux;
            driver_write(aux,tempbuffer);
            posicao = 4;
        }
    }
    driver_write(logTable[logid].pointBLK,tempbuffer);
    logTable[logid].pointPOS = posicao;
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
    int contaChar = 0;
    int posicao;
    int aux;
    if (logTable[logid].mode != 0)
    {
        printf("Erro:O log nao esta em modo de leitura!\n");
        return -1;
    }
    //Carrega pagina
    driver_read(logTable[logid].pointBLK,tempbuffer);
    posicao = logTable[logid].pointPOS;
    //Copia a escrever os caracteres
    while (contaChar < buflen)
    {
        if (posicao < blocksize)
        {
            if (contaChar < buflen)
                buffer[contaChar] = tempbuffer[posicao];
            contaChar++; posicao++;
        }
        else
        {
            aux = pegaEndereco();
            if (aux == 0x00FFFFFF)
            {
                printf("Final do log atingido. \n");
                return 0;
            }
            else
            {
                logTable[logid].pointBLK = aux;
                driver_read(aux,tempbuffer);
                posicao = 4;
            }
        }
    }
    logTable[logid].pointPOS = posicao;
    return 0;
}

//Funcao para encontrar o ultimo bloco do arquivo
int achaBlocoFinal(int blocoInicial)
{
    int conta_saltos = 0;
    int aux;
    while (conta_saltos < 0x00FFFFFF)
    {
        driver_read(blocoInicial, tempbuffer);
        aux=pegaEndereco();
        if (aux == 0x00FFFFFF)
        {
            return blocoInicial;
        }
        else
        {
            aux=pegaEndereco();
            blocoInicial = aux;
        }
        conta_saltos++;
    }
    return -1;
}

//Funcao para encontrar a ultima posicao
int achaPosFinal(int ultimoBloco)
{
    int posicao;
    driver_read(ultimoBloco, tempbuffer);
    //Checa se eh o primeiro vetor
    if (tempbuffer[0] == -128)
        posicao = 20;
    else
        posicao = 4;

    while(posicao < blocksize)
    {
        if (tempbuffer[posicao]=='\n')
        {
            break;
        }
        posicao++;
    }
    if (posicao >= blocksize)
    {
        if (tempbuffer[0] == -128)
            posicao = 20;
        else
            posicao = 4;
    }
    return posicao;
}

//Funcao para encontrar o primeiro bloco livre
int achaBlocoLivre(void)
{
    int conta_saltos = 0;
    int aux;
    while (conta_saltos < blockcount)
    {
        driver_read(conta_saltos, tempbuffer);
        aux=pegaEndereco();
        if (aux == 0 )
        {
            return conta_saltos;
        }
        conta_saltos++;
    }
    printf("Memoria esgotada, nao foi possivel alocar espaco para o log\n");
    return -1;
}

//Funcao para extrair endereco do tempBuffer
int pegaEndereco(void)
{
    int abc = 0;
    abc = abc + (int)(tempbuffer[3] & 0x7F);
    abc = abc + (int)(tempbuffer[3] & 0x80);

    abc = abc + ((int)(tempbuffer[2] & 0x7F) << 8);
    abc = abc + ((int)(tempbuffer[2] & 0x80) << 8);

    abc = abc + ((int)(tempbuffer[1] & 0x7F) << 16);
    abc = abc + ((int)(tempbuffer[1] & 0x80) << 16);

    return abc;
}
