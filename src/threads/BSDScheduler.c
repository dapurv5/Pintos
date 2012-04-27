#include "lib/kernel/list.h"
#include <stdlib.h>
#include <stdbool.h>
#include "threads/thread.h"
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include "threads/priority.h"
#include "threads/fixed-point.h"

/*******************************************************************************/
void thread_calculate_recent_cpu(struct thread* t){
 ASSERT (is_thread(t));

  int rc = t->recent_cpu;
  int nice_f = int_to_fixed_pt(t->nice);
  
  int load1 = load_avg * 2;
  int load2 = load1 + int_to_fixed_pt(1);

  int load3 = fixed_pt_division(load1, load2);
  int load4 = fixed_pt_multiplication(rc,load3);

  t->recent_cpu = load4 + nice_f;
}

 void thread_current_calculate_recent_cpu(void)
 {
     thread_calculate_recent_cpu(thread_current());
 }

void all_thread_calculate_recent_cpu(void)
{
  struct list_elem* e;
  struct thread* t;

  e = list_begin (&all_list);
  while (e != list_end (&all_list))
  {
     t = list_entry (e, struct thread, allelem);
     thread_calculate_recent_cpu(t);
     e = list_next (e);
  }
}

/*******************************************************************************/
 void thread_calculate_priority(struct thread* t)
 {
   ASSERT (is_thread(t));

   if (t != idle_thread)
   {
     t->priority = PRI_MAX - fixed_pt_to_nearest_int(t->recent_cpu / 4) - 2*t->nice;
          
     if (t->priority > PRI_MAX)
        t->priority = PRI_MAX;
     else if (t->priority < PRI_MIN)
        t->priority = PRI_MIN;
   }
 }

 void thread_current_calculate_priority(void)
 {
    thread_calculate_priority(thread_current());    
 }

void all_thread_calculate_priority (void)
{
  struct list_elem *e;
  struct thread *t;
 
  e = list_begin (&all_list);
  while (e != list_end (&all_list))
    {
      t = list_entry (e, struct thread, allelem);
      thread_calculate_priority(t);
      e = list_next (e);
    }
    list_sort (&ready_list, &priority_more, NULL);
}


/*******************************************************************************/

void thread_calculate_load_avg(void)
{
  int ready_threads;

  if (thread_current() != idle_thread)
    ready_threads = list_size (&ready_list) + 1;
  else
    ready_threads = list_size (&ready_list);

  int load = int_to_fixed_pt(59) / 60 ;
  int load1 = fixed_pt_multiplication(load_avg, load);
  int load2 = (int_to_fixed_pt(1) / 60) * ready_threads;
  load_avg = load1 + load2;
}

/*******************************************************************************/