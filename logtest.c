/* logtest.c - programa de teste que gera dois logs independentes no
 * dispositivo e depois os lê de volta para verificação.
 */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include "logserv.h"

#define LOG1 "log.um"
#define LOG2 "log.dois"

#define LEITURA 0
#define ESCRITA 1
int 
main(int argc, char* argv[])
{
    int log1, log2;
    int i,j,l,tstp;
    char logmsg[256];

    /* Primeiro, escreve nos dois logs intercaladamente */

    logserv_init(argv[1]);

    log1 = logserv_openlog(LOG1,ESCRITA);
    log2 = logserv_openlog(LOG2,ESCRITA);

    for (i=0;i<100;++i) {
        sprintf(logmsg, "Mensagem %d do log %s.",i,LOG1);
        fprintf(stderr,"%08d: %s\n",time(NULL), logmsg);
        logserv_writelog(log1,logmsg,strlen(logmsg));
        for (j=0;j<2;++j) {
            sprintf(logmsg, "Mensagem %d do log %s.\n",2*i+j,LOG2);
            fprintf(stderr,"%08d: %s\n",time(NULL), logmsg);
            logserv_writelog(log2,logmsg,strlen(logmsg));
        }
    }

    logserv_shutdown();

    /* Agora, lê os dois logs separadamente. */

    logserv_init(argv[1]);

    l = logserv_openlog(LOG1,LEITURA);
    while (tstp=logserv_readlog(l, logmsg, sizeof(logmsg))) {
        fprintf(stderr,"%08d: %s\n",tstp, logmsg);
    }
    logserv_closelog(l);

    l = logserv_openlog(LOG2,LEITURA);
    while (tstp=logserv_readlog(l, logmsg, sizeof(logmsg))) {
        fprintf(stderr,"%08d: %s\n",tstp, logmsg);
    }
    logserv_closelog(l);

    logserv_shutdown();
}
