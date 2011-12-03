#include <stdio.h>
#include <stdlib.h>
#include "hwif.h"
#include "driver.h"
#include <conio.h>

extern int   hw_command_reg;
extern int   hw_status_reg;
extern char* hw_block_to_write;
extern char* hw_block_read;


int driver_blcksize(void)

{

    hw_command_reg = HW_OP_BLCKSIZE;
    hw_trigger();
    hw_wait();
    return hw_status_reg;
}

int main()
{
    return 0;
}

