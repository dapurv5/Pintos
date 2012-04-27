#ifndef THREADS_THREAD_H
#define THREADS_THREAD_H

#ifndef USERPROG
#define USERPROG
#endif 


#include <lib/debug.h>
#include "lib/kernel/list.h"
#include <stdint.h>
#include "threads/synch.h"

/* List of processes in THREAD_READY state, that is, processes
   that are ready to run but not actually running. */
extern struct list ready_list;

/* List of all processes.  Processes are added to this list
   when they are first scheduled and removed when they exit. */
extern struct list all_list;

/* Idle thread. */
extern struct thread *idle_thread;

/* Initial thread, the thread running init.c:main(). */
extern struct thread *initial_thread;

/*Load_avg*/
extern int load_avg;

/* Returns true if T appears to point to a valid thread. */
extern bool is_thread (struct thread*);



/* States in a thread's life cycle. */
enum thread_status
  {
    THREAD_RUNNING,     /* Running thread. */
    THREAD_READY,       /* Not running but ready to run. */
    THREAD_BLOCKED,     /* Waiting for an event to trigger. */
    THREAD_DYING,       /* About to be destroyed. */
    THREAD_SLEEPING
  };

/* Thread identifier type.
   You can redefine this to whatever type you like. */
typedef int tid_t;
#define TID_ERROR ((tid_t) -1)          /* Error value for tid_t. */

/* Thread priorities. */
#define PRI_MIN 0                       /* Lowest priority. */
#define PRI_DEFAULT 31                  /* Default priority. */
#define PRI_MAX 63                      /* Highest priority. */


/* Nice Values */
#define NICE_MAX 20
#define NICE_DEFAULT 0
#define NICE_MIN -20

/* A kernel thread or user process.

   Each thread structure is stored in its own 4 kB page.  The
   thread structure itself sits at the very bottom of the page
   (at offset 0).  The rest of the page is reserved for the
   thread's kernel stack, which grows downward from the top of
   the page (at offset 4 kB).  Here's an illustration:

        4 kB +---------------------------------+
             |          kernel stack           |
             |                |                |
             |                |                |
             |                V                |
             |         grows downward          |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             +---------------------------------+
             |              magic              |
             |                :                |
             |                :                |
             |               name              |
             |              status             |
        0 kB +---------------------------------+

   The upshot of this is twofold:

      1. First, `struct thread' must not be allowed to grow too
         big.  If it does, then there will not be enough room for
         the kernel stack.  Our base `struct thread' is only a
         few bytes in size.  It probably should stay well under 1
         kB.

      2. Second, kernel stacks must not be allowed to grow too
         large.  If a stack overflows, it will corrupt the thread
         state.  Thus, kernel functions should not allocate large
         structures or arrays as non-static local variables.  Use
         dynamic allocation with malloc() or palloc_get_page()
         instead.

   The first symptom of either of these problems will probably be
   an assertion failure in thread_current(), which checks that
   the `magic' member of the running thread's `struct thread' is
   set to THREAD_MAGIC.  Stack overflow will normally change this
   value, triggering the assertion. */
/* The `elem' member has a dual purpose.  It can be an element in
   the run queue (thread.c), or it can be an element in a
   semaphore wait list (synch.c).  It can be used these two ways
   only because they are mutually exclusive: only a thread in the
   ready state is on the run queue, whereas only a thread in the
   blocked state is on a semaphore wait list. */
struct thread
  {
    /* Owned by thread.c. */
    tid_t tid;                          /* Thread identifier. */
    enum thread_status status;          /* Thread state. */
    char name[16];                      /* Name (for debugging purposes). */
    uint8_t *stack;                     /* Saved stack pointer. */
    int priority;                       /* Priority. */
    struct list_elem allelem;           /* List element for all threads list. */

    /* Shared between thread.c and synch.c. */
    struct list_elem elem;              /* List element. */

#ifdef USERPROG
    /* Owned by userprog/process.c. */
    uint32_t *pagedir;                  /* Page directory. */

    /*My implementation*/
    struct list child_list;             /* Storing the child threads. */
    struct list_elem child_elem;        /* Element for child_list. */
    struct thread* parent;              /* Pointer to the parent thread */
    struct file* image_on_disk;         /* Executable image on disk*/
    struct semaphore wait;              /* Semaphore for waiting on a thread to exit. */
    struct semaphore zombie;            /*I understood why this is made*/
    struct list open_file_list;         /* Open files for a thread initialized in init_thread() of 'thread.c'. */
    int load_status;                    /* Load status for a thread (0 successful). */
    int exit_status;                    /* Exit status of a thread initialized during SYS_EXIT.*/

#endif

    /* Owned by thread.c. */
    unsigned magic;                     /* Detects stack overflow. */
    int64_t time;                       /*Awake time if this thread is put to sleep*/

    bool under_donation;                /*True if the thread is under temporary donation of priority*/
    int true_priority;                  /*Stores the true priority of the thread*/
    struct list locks_list;              /*Stores all the locks acquired by this thread*/
    struct lock* blocking_lock;         /*The lock by which this thread was blocked.*/

    int nice;                           /* nice value of a thread */
    int recent_cpu;                     /* recent cpu usage */

  };



/* If false (default), use round-robin scheduler.
   If true, use multi-level feedback queue scheduler.
   Controlled by kernel command-line option "-o mlfqs". */
extern bool thread_mlfqs;

void thread_init (void);
void thread_start (void);

void thread_tick (void);
void thread_print_stats (void);

typedef void thread_func (void *aux);
tid_t thread_create (const char *name, int priority, thread_func *, void *);

void thread_block (void);
void thread_unblock (struct thread *);

struct thread *thread_current (void);
tid_t thread_tid (void);
const char *thread_name (void);

void thread_exit (void) NO_RETURN;
void thread_yield (void);

/* Performs some operation on thread t, given auxiliary data AUX. */
typedef void thread_action_func (struct thread *t, void *aux);
void thread_foreach (thread_action_func *, void *);

int thread_get_priority (void);
void thread_set_priority (int);

int thread_get_nice (void);
void thread_set_nice (int);
int thread_get_recent_cpu (void);
int thread_get_load_avg (void);

#endif /* threads/thread.h */
