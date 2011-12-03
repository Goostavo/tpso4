
/*
 * Programa de emulação do dispositivo de memória em blocos para a
 * disciplina de Sistemas Operacionais.
 *
 **********************************************************************
 * NÃO SE PREOCUPE COM O CONTEÚDO DESTE ARQUIVO. ELE NÃO PODE SER
 * ALTERADO EM HIPÓTESE ALGUMA E SERVE APENAS PARA CRIAR A ESTRUTURA
 * INTERNA DO DISPOSITIVO DE HARDWARE. PARA O TRABALHO, BASTA O USO
 * DAS FUNÇÕES DEFINIDAS NA INTERFACE (hwif.h)
 **********************************************************************
 */

#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "hwif.h"

int   hw_command_reg;
int   hw_status_reg;
char* hw_block_to_write;
char* hw_block_read;

int debug = 0;

/* Funções essenciais à operação do dispositivo.
 * Detalhes na definição de cada uma.
 */
int device_execute(int opcode);
int init_storage(char* fname, int* fd);
int open_storage(char* name);
int create_new_storage(char* fname, int* fd);
int read_storage(char* fname,int* fd, int fsize);
void hw_shutdown(void);

#define DEV_BLCK_SIZE_MAX     (4*1024)
#define DEV_BLCK_SIZE_DEFAULT (DEV_BLCK_SIZE_MAX)
#define DEV_BLCK_CNT_DEFAULT  (256)

#define ENV_DEV_FILENAME   "SODEV_FILENAME"
#define ENV_DEV_BLCK_CNT   "SODEV_BLCK_CNT"
#define ENV_DEV_BLCK_SIZE  "SODEV_BLCK_SIZE"
#define ENV_DEV_DEBUG      "SODEV_DEBUG"

/* Arquivo para armazenamento do dispositivo: nome, descritor e tamanho */
char* storage_fname;
int   storage_fd;
int   storage_file_size;

/* Área de memória do dispositivo: endereço e tamanho (em blocos e bytes) */
char* storage;
int   hw_blck_cnt;
int   hw_blck_size;
int   storage_size;

void
hw_initialize(char* fname)
{
    static int message_shown = 0;
    char* env_var;
    if ( (env_var=getenv(ENV_DEV_DEBUG)) != NULL ) {
        debug = atoi(env_var);
    }
    if (fname==NULL) {
        if ((env_var=getenv(ENV_DEV_FILENAME))==NULL) {
            storage_fname = "storage.dat";
        } else {
            storage_fname = env_var;
        }
    } else {
        storage_fname = fname;
    }

    hw_block_to_write = (char*) malloc(DEV_BLCK_SIZE_MAX);
    hw_block_read     = (char*) malloc(DEV_BLCK_SIZE_MAX);

    if (!message_shown) {
        fprintf(stderr,"Para configurar o dispositivo, defina:\n");
        fprintf(stderr,"   %s para o nome do arquivo;\n", ENV_DEV_FILENAME);
        fprintf(stderr,"   %s para o tamanho do bloco;\n", ENV_DEV_BLCK_SIZE);
        fprintf(stderr,"   %s para o numero de blocos;\n", ENV_DEV_BLCK_CNT);
        fprintf(stderr,"   %s para depuracao.\n", ENV_DEV_DEBUG);
        message_shown = 1;
    }
}

void hw_trigger(void)
{
    if (debug>0) fprintf(stderr,"hw_trigger()\n");
}

void hw_wait(void)
{
    /* Na verdade, ao invés de apenas esperar, já que estamos apenas
     * simulando uma operação assíncrona, agora é a hora de executar o
     * comando requisitado.
     */
    hw_status_reg = device_execute(hw_command_reg);
}

#define TENTHOFASECOND     100000
#define HUNDREDTHOFASECOND  10000
#define OPTIME   HUNDREDTHOFASECOND


#define FILE_HEADER_SIZE (sizeof(hw_blck_cnt)+sizeof(hw_blck_size))
#define SIGNAL_BACK( pid )  kill( pid ,SIGUSR1)

char errmsg[128];

int
open_storage(char* name)
{
    int fd;

    if ((fd=open(name,O_RDWR|O_CREAT,00600)) < 0) {
        sprintf(errmsg,"open(%s)",name);
        perror(errmsg);
        exit(2);
    }
    return fd;
}

/* device_execute(int opcode);
 *
 * Processa cada comando. Considera como globais os apontadores para as
 * áreas de memória compartilhada dos buffers de comunicação,
 * hw_dst_buffer e hw_src_buffer.
 * Cada comando leva pelo menos um décimo de segundo para ser executado e
 * cada um deve ser auto-explicativo. Apenas a parte de inicialização
 * exigem processamento mais complexo e é definida nas funções associadas.
 */
int
device_execute( int opcode )
{
    char* blck_ptr;
    int   blck_num;

    usleep(OPTIME);
    switch( (opcode>>HWBLCKFIELDLEN)<<HWBLCKFIELDLEN) /* & 0xff000000) */ {
        case HW_OP_START:
            if (debug>0) fprintf(stderr,"device_execute(DEV_OP_START)\n");
            return init_storage(storage_fname, &storage_fd);
        case HW_OP_BLCKSIZE:
            if (debug>0) fprintf(stderr,"device_execute(DEV_OP_BLCKSIZE)\n");
            return hw_blck_size;
        case HW_OP_BLCKCNT:
            if (debug>0) fprintf(stderr,"device_execute(DEV_OP_BLCKCNT)\n");
            return hw_blck_cnt;
        case HW_OP_WRBLCK:
            blck_num = opcode & 0x0000ffff;
            blck_ptr = storage + (blck_num*hw_blck_size);
            if (debug>0) fprintf(stderr,"device_execute(DEV_OP_WRBLCK,%d)\n",blck_num);
            memcpy(blck_ptr,hw_block_to_write,hw_blck_size);
            return 0;
        case HW_OP_RDBLCK:
            if (debug>0) fprintf(stderr,"device_execute(DEV_OP_RDBLCK)\n");
            blck_num = opcode & 0x0000ffff;
            blck_ptr = storage + (blck_num*hw_blck_size);
            memcpy(hw_block_read,blck_ptr,hw_blck_size);
            return 0;
        case HW_OP_SHUTDOWN:
            if (debug>0) fprintf(stderr,"device_execute(DEV_OP_SHUTDOWN)\n");
            hw_shutdown();
            return(0);
        break;
        default:
            fprintf(stderr,"Device execute(opcode:%d)?\n",opcode);
            return (-1);
        break;
    }
}

int
init_storage(char* fname, int* fd)
{
    int    retval;
    struct stat fstats;

    /* Abre(ou cria) o arquivo e determina como lidar com ele */
    *fd = open_storage(fname);
    if (fstat(*fd,&fstats)<0) {
        perror("fstat(storage)");
        return(-1);
    } else if (fstats.st_size == 0) {
        if ((retval=create_new_storage(fname,fd)) < 0) {
            return retval;
        }
    } else {
        if ((retval=read_storage(fname,fd,fstats.st_size)) < 0) {
            return retval;
        }
    }
    /* Uma vez que o dispositivo foi configurado, cria a área de memória,
     * lendo-a do arquivo (não tem problema se o arquivo estiver vazio)
     */
    if ((storage = calloc(1,storage_size))==NULL) {
        perror("calloc(1,storage)");
        return(-3);
    } else if (read(*fd,storage,(hw_blck_cnt)*(hw_blck_size)) < 0) {
            perror("read(storage)");
            return(10);
    }
    return(0);
}

int
create_new_storage(char* fname, int* fd)
{
    char* env_var;
    if ((env_var=getenv(ENV_DEV_BLCK_CNT))==NULL) {
        hw_blck_cnt  = DEV_BLCK_CNT_DEFAULT;
    } else {
        hw_blck_cnt  = atoi(env_var);
    }

    if ((env_var=getenv(ENV_DEV_BLCK_SIZE))==NULL) {
        hw_blck_size  = DEV_BLCK_SIZE_DEFAULT;
    } else {
        hw_blck_size  = atoi(env_var);
    }

    if ((hw_blck_size)>DEV_BLCK_SIZE_MAX) {
        fprintf(stderr,"Blocos não podem ser maiores que máx. (%d > %d)\n",
                hw_blck_size, DEV_BLCK_SIZE_MAX);
        return(-9);
    }
    fprintf(stderr,"Dispositivo novo (%s): blck_cnt = %d, blck_size = %d\n",
            fname, hw_blck_cnt, hw_blck_size);
    storage_size = hw_blck_cnt * hw_blck_size;
    storage_file_size = FILE_HEADER_SIZE + storage_size;
    return 0;
}

int
read_storage(char* fname, int* fd, int fsize)
{
    int retval;
#define EXPECTED_SIZE() ( FILE_HEADER_SIZE + (hw_blck_size)*(hw_blck_cnt) )
    retval=read(*fd,&hw_blck_cnt,sizeof(hw_blck_cnt));
    if (retval < sizeof(hw_blck_cnt)) {
        fprintf(stderr,"Dispositivo %s: erro leitura do num. de blocos (%d)\n",
                fname, retval);
        return(-4);
    }  else if ((retval=read(*fd,&hw_blck_size,sizeof(hw_blck_size))) < sizeof(hw_blck_size)) {
        fprintf(stderr,"Dispositivo %s: erro leitura do tam. do bloco (%d)\n",
                fname, retval);
        return(-5);
    } else if ((hw_blck_size)>DEV_BLCK_SIZE_MAX) {
        fprintf(stderr,"Dispositivo %s: blocos maiores que o máx. (%d > %d)\n",
                fname, hw_blck_size, DEV_BLCK_SIZE_MAX);
        return(-6);
    } else if (fsize != EXPECTED_SIZE() ) {
        fprintf(stderr,"Dispositivo %s tem %d bytes, devia ter %d\n",
                       fname, fsize, EXPECTED_SIZE() );
        return(-8);
    }
    fprintf(stderr,"Dispositivo lido (%s): blck_size = %d, blck_cnt = %d\n",
            fname, hw_blck_size, hw_blck_cnt);
    storage_size = hw_blck_cnt * hw_blck_size;
    storage_file_size = FILE_HEADER_SIZE + storage_size;
    return(0);
}

/* hw_shutdown:
 * termina a operação do dispositivo. Salva o conteúdo da área de memória
 * do mesmo no arquivo apropriado. As áreas de memória compartilhada ainda
 * permanecem, pois é preciso avisar ao outro processo que o comando foi
 * entendido e executado.
 */

void
hw_shutdown(void)
{
    if (lseek(storage_fd,0,SEEK_SET) < 0) {
        perror("lseek(storage)");
        exit(6);
    }
    fprintf(stderr,"hw_shutdown: salvando %d bytes\n", storage_file_size);
    if (write(storage_fd,&hw_blck_cnt,sizeof(hw_blck_cnt))<sizeof(hw_blck_cnt)) {
        fprintf(stderr,"Erro ao salvar o no. de blocos do dispositivo\n");
        exit(7);
    }
    if (write(storage_fd,&hw_blck_size,sizeof(hw_blck_size))<sizeof(hw_blck_size)) {
        fprintf(stderr,"Erro ao salvar o tam. dos blocos do dispositivo\n");
        exit(8);
    }
    if (write(storage_fd,storage,storage_size)<storage_size) {
        fprintf(stderr,"Erro ao salvar o conteúdo do dispositivo\n");
        exit(9);
    }
}
