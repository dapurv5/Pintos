#include "userprog/process.h"
#include <debug.h>
#include <inttypes.h>
#include <round.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "userprog/gdt.h"
#include "userprog/pagedir.h"
#include "userprog/tss.h"
#include "filesys/directory.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "threads/flags.h"
#include "threads/init.h"
#include "threads/interrupt.h"
#include "threads/palloc.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/argument_passing.h"


void push_arguments(void **esp, char * cmd_line)
{
    //printf("\n Pushing arguments \n");//////////////////////////
    
    ASSERT(*esp == PHYS_BASE);
    ASSERT(cmd_line != NULL);
    
    char * s = cmd_line;
    char *token;
    char *save_ptr;
    unsigned int args[MAX_ARG];
    char * argv[MAX_ARG];
    int length = 0;
    int argc = 0;//at least the file name

    /* Start Parsing*/
    for (token = strtok_r(s, " ", &save_ptr); token != NULL;
            token = strtok_r(NULL, " ", &save_ptr))
    {
        argv[argc] = token;
        argc++;
    }
    int count;
    for (count = argc - 1; count >= 0; count--)
    {
        int len = strlen(argv[count]);
        length = length + len + 1;
        
        
        args[argc - 1 - count] = push_argv(esp, argv[count], len);
        
    }

    align(esp, 4 - (length % 4));

    /*push 0*/
    (*esp) = (*esp) - 4;

    unsigned int * addr;
    addr = *esp;
    (*addr) = 0;

    //*(int*)(*esp)=0;

    int i;

    for (i = 0; i < argc; i++)
    {
       push_argv_ptr(esp, args[i]);
    }

    unsigned int addr_arg = (unsigned int)(*esp);
    push_argv_ptr(esp, addr_arg);

    push_argc(esp, argc);
    push_retval(esp, 0x0);
}

unsigned int push_argv(void **esp, char * token, int size)
{
    //printf("push_argv token %s \n",token);/////////////////////////////////
   ASSERT(*esp >= PHYS_BASE-PGSIZE);
   ASSERT(token != NULL);
   ASSERT(size > 0);
   int i;
   char * ptr;

   ptr = *esp;
   ptr = ptr - size - 1;
   *esp = ptr;
   ptr = *esp;

   for (i = 0; i < size; i++)
   {
      (*ptr) = token[i];
      
      ptr++;
   }
   *ptr = '\0';
   return (unsigned int)*esp;
}


void push_argv_ptr(void **esp, unsigned int token)
{
    ASSERT(*esp >= PHYS_BASE-PGSIZE);
    *esp = *esp - 4;
    unsigned int * addr;
    addr = *esp;
    (*addr) = token;
}
void push_argc(void **esp, int c)
{
    ASSERT(*esp >= PHYS_BASE-PGSIZE);
    ASSERT(c>=0 );
    *esp = *esp - 4;
    int * ptr;
    ptr = *esp;
    *ptr = c;

    //printf("pushing number of arguments %d\n",c);
    //printf("at address %x\n",ptr);
}
void push_retval(void **esp, unsigned int retval_addr)
{
    ASSERT(*esp >= PHYS_BASE-PGSIZE);
    (*esp) = (*esp) - 4;
    unsigned int * ptr;
    ptr = *esp;
    *ptr = retval_addr;
    //printf("pushing faked return address %x\n",retval_addr);
    //printf("at address %x\n",ptr);
}




void align(void **esp, int length)
{
    ASSERT(*esp >= PHYS_BASE-PGSIZE);
    //printf("esp point to %x ........\n",*esp);
    uint8_t * zero;
    while(length > 0)
    {
    	(*esp)--;
    	zero = *esp;
    	*zero = 0;
        length--;
    }
}


