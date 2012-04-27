#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

#include "threads/init.h"
#include "filesys/file_supplement.h"
#include "userprog/user_memory.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "filesys/inode.h"
#include "lib/string.h"
#include "threads/malloc.h"
#include "devices/input.h"
#include "devices/shutdown.h"
#include "userprog/process.h"
#include "threads/vaddr.h"


static struct lock file_lock;
static void syscall_handler (struct intr_frame *);

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  lock_init(&file_lock);
}

static void
syscall_handler (struct intr_frame* f)
{
  /*f->esp is pointer to syscall_num. Now sp is also a pointer to syscall_num*/
  uint32_t* sp = f->esp;

  /*Check if sp is a valid user virtual address*/
  if ( !sp || !is_user_vaddr(sp) || !get_user(sp))
    sys_exit(-1);
  
  
  if (! ( is_user_vaddr (sp + 1) )  )
      sys_exit(-1);

  
   int syscall_num = *(sp);
   /*Check if syscall_num is a valid syscall_num*/
   if (syscall_num < SYS_HALT || syscall_num > SYS_INUMBER)
     sys_exit(-1);




  switch(syscall_num)
  {
        case SYS_HALT:
        {
            shutdown_power_off();
            break;
        }
        case SYS_EXIT:
        {
            struct thread* cur=thread_current();
            int status = *(int *)(sp + 1);
            printf("%s: exit(%d)\n", cur->name, status);
            thread_exit();
            break;
        }

        case SYS_EXEC:
        {
            char* cmd_line = *(sp + 1);
            if (!cmd_line){
                sys_exit(-1);
            }
            if (!valid (cmd_line))
            {
                    sys_exit(-1);
            }
            else{
                f->eax= process_execute (cmd_line);
            }
            break;
        }

        case SYS_WAIT:
        {
           //Process ID and Thread ID are used interchangeably since all processes in Pintos are single-threaded.
           tid_t tid =(tid_t) *(sp + 1);
           f->eax = process_wait (tid);
           break;        
        }

        case SYS_CREATE:
        {
           if (! ( is_user_vaddr (sp + 2) )  )
               sys_exit(-1);


           char* filename =(sp + 1);
           off_t initial_size = (off_t) *(sp + 2);

           if (!filename){
               sys_exit(-1);
               break;
           }

           if (!valid (filename))
           {
                    sys_exit(-1);
           }


           f->eax=filesys_create (filename, initial_size);

           break;
        }

        case SYS_REMOVE:
        {
           
           char* filename = (char *)(sp + 1);

           if (!filename){
                f->eax= false;
                sys_exit(-1);
                break;
           }
           if (!valid (filename))
           {
                    sys_exit(-1);
           }

           else
               lock_acquire(&file_lock);
               f->eax = filesys_remove(filename);
               lock_release(&file_lock);
           break;
        }

        case SYS_OPEN:
        {
           
           char* filename = *(sp + 1);
           struct file* file;
           
           if (!filename){ /* file == NULL */
                f->eax=-1;
                break;
           }
           else if (!valid (filename))
           {
                    sys_exit(-1);
           }
           else
               file = filesys_open (filename);
           
           if (!file){ /* Bad file name */
                f->eax=-1;
                break;
           }
           else
           {
              file->fd = fd_cnt++;	//assign file descriptor
              struct thread *t = thread_current();
              list_push_back(&t->open_file_list, &file->open_file_elem);// add this file to the process's open_file_list
              f->eax = file->fd;
           }
           break;
         }

        case SYS_CLOSE:
        {
           int fd = (int)*(sp + 1);

           struct file* file;
                    
           file = get_file(fd);
           if (!file){ /* file == NULL */
                f->eax=-1;
                break;
           }
           else
           {
              file_close(file);
              f->eax=0;
              struct list_elem* e;
              e = &file->open_file_elem;
              list_remove(e);
           }
          break;
        }

       case SYS_FILESIZE:
       {          
          int fd = *(sp + 1);                

          struct file* file;
          file = get_file(fd);

          if(!file){
              f->eax=-1;
              break;
          }

          else
              f->eax = file_length(file);
         break;
       }

       case SYS_READ :
       {
          if (! ( is_user_vaddr (sp + 2) && is_user_vaddr (sp + 3))  )
              sys_exit(-1);

          
          int fd = *(sp + 1);
          uint8_t* u_buffer = *(sp + 2);
          unsigned size = *(sp + 3);

          if(!u_buffer){
              sys_exit(-1);
          }

          if (size <= 0)
           {
             sys_exit(-1);
           }

          struct file * file;
          unsigned i;

          if (fd == STDIN_FILENO) /* stdin */
          {
            for (i = 0; i != size; ++i)
            *(uint8_t *)(u_buffer + i) = input_getc ();
            f->eax= size;
          }
          else if (fd == STDOUT_FILENO){ /* stdout */
            f->eax= -1;
            break;
          }
          else
          {
            file = get_file(fd);
            if (!file){
                f->eax=-1;
                break;
            }
            lock_acquire(&file_lock);
            f->eax= file_read (file, u_buffer, size);
            lock_release(&file_lock);
          }

          break;
       }

       case SYS_WRITE:
        {
          if (! ( is_user_vaddr (sp + 2) && is_user_vaddr (sp + 3))  )
              sys_exit(-1);
          
          int fd = *(sp + 1);
          char *buffer =  *(sp + 2);
          unsigned int size = *(sp + 3);

          if(!buffer){
              sys_exit(-1);
          }

          if (size <= 0)
           {
             sys_exit(-1);
           }

          if (!valid (buffer))
           {
              sys_exit(-1);
           }
          
          struct file * file;

          if (fd == STDOUT_FILENO) /* stdout */
              putbuf (buffer, size);
          else if (fd == STDIN_FILENO) /* stdin */
          {
              f->eax= -1;
              break;
          }
          else
          {
              file = get_file(fd);
              if (!file){
                    f->eax= -1;
                    break;
              }
              else{
                   lock_acquire(&file_lock);
                   f->eax=file_write (f, buffer, size);
                   lock_release(&file_lock);
              }
          }
          break;
       }
 

        case SYS_SEEK:
        {
          if (! ( is_user_vaddr (sp + 2) )  )
               sys_exit(-1);

          int fd = *(sp + 1);
          unsigned position = *(sp + 2);
          
          struct file* file;
          file = get_file(fd);

          if (!file){
             f->eax=-1;
             break;
          }
          file_seek (file, position);
          f->eax=0;
          break;
        }

        case SYS_TELL:
        {
          int fd = *(sp + 1);

          struct file* file;
          file = get_file(fd);

          if(!file)
              f->eax=-1;

          else
              f->eax = file_tell(file);

          break;
        }

      default:
          sys_exit(-1);
  }
}
