#include "filesys/file.h"
#include <debug.h>
#include "filesys/inode.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include "lib/kernel/list.h"

/* initialize file management data structure */
void file_init()
{
    fd_cnt = 3;
}

/* Get file from open_file_list of current_thread with fd */
struct file* get_file(int fd)
{
  struct list_elem *e;
  struct thread* t = thread_current();

  if (list_empty (&t->open_file_list))
    return NULL;

  for (e = list_begin (&t->open_file_list); e != list_end (&t->open_file_list); e = list_next (e))
  {
     struct file *f = list_entry(e, struct file, open_file_elem);
     if (f->fd == fd)
        return f;
  }

  return NULL;
}

