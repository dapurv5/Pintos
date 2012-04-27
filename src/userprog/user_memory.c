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
#include "userprog/user_memory.h"
#include "lib/kernel/list.h"

/* close files the process opened */
void close_open_file_list(struct thread* t)
{
    struct file* f;
    while(list_size(&t->open_file_list) > 0)
    {
        struct list_elem *e = list_pop_front(&t->open_file_list);
         f = list_entry(e, struct file, open_file_elem);
        file_close(f);
    }
}

/* Given tid, return thread pointer */
struct thread* get_thread(tid_t tid)
{
    struct thread* cur=thread_current();
    struct list_elem *e;
    for(e=list_begin(&cur->child_list); e!=list_end(&cur->child_list); e=list_next(e))
    {
            struct thread *t = list_entry(e, struct thread, child_elem);
            if(t->tid==tid)
                    return t;
    }
    return NULL;
}


/* Reads a byte at user virtual address UADDR.
   Returns the byte value if successful, -1 if a segfault
   occurred. */
int get_user (const uint8_t *uaddr)
{
  int result;
  asm ("movl $1f, %0; movzbl %1, %0; 1:"
       : "=&a" (result) : "m" (*uaddr));
  return result;
}

/* Writes BYTE to user address UDST.
   UDST must be below PHYS_BASE.
   Returns true if successful, false if a segfault occurred. */
bool put_user (uint8_t *udst, uint8_t byte)
{
  int error_code;
  asm ("movl $1f, %0; movb %b2, %1; 1:"
       : "=&a" (error_code), "=m" (*udst) : "r" (byte));
  return error_code != -1;
}

void sys_exit(int status){
    struct thread* cur=thread_current();
    cur->exit_status=status;
    printf("%s: exit(%d)\n", cur->name, status);
    thread_exit();
}


bool valid (char *uaddr)
{
  int ch;
  int i=0;
  while ((ch = get_user (uaddr + i++)) != '\0')
    {
      if (ch == -1)
         return false;
    }
  return true;
}