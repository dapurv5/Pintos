#include "lib/kernel/list.h"
#include <stdlib.h>
#include <stdbool.h>
#include "threads/thread.h"
#include "threads/alarm.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include "threads/priority.h"

void alarm_init(void)
{
    list_init(&my_sleeping_list);
}

void set_alarm_at_time(int64_t time){

    intr_disable();
    struct thread *add_thread = thread_current();
    add_thread->time=time;

    
    if (list_empty (&my_sleeping_list)){
       list_push_front (&my_sleeping_list,&add_thread->elem);
    }
    else{
        struct list_elem *e;
        e = list_begin (&my_sleeping_list);
        struct thread *at;

        int flag=0;

        for (e = list_begin (&my_sleeping_list); e != list_end (&my_sleeping_list); e = list_next (e)){
            at = list_entry (e, struct thread, elem);
            if((at->time) >= time){
                list_insert (e, &(add_thread->elem));
                flag=1;
                break;
            }
        }
        if(flag==0){
            list_push_back(&my_sleeping_list,&add_thread->elem);
        }
        // printf("Set alarm called time= %d, size=%d \n",time,list_size(&my_sleeping_list));////////////////////

    }
    thread_sleep();
    intr_enable();
}

void check_alarm_completion(void){
    enum intr_level old_level = intr_disable ();
    if(!list_empty(&my_sleeping_list)) {
        struct list_elem *le=list_front(&my_sleeping_list);
        struct thread *st = list_entry (le,struct thread, elem);
        int64_t time=st->time;
        while(time<=timer_ticks() && !list_empty(&my_sleeping_list)){
            
            list_pop_front(&my_sleeping_list);
            thread_awake(st);
            
            //you should not call thread yield here.
            
            
            if(!list_empty(&my_sleeping_list)){
                le=list_front(&my_sleeping_list);
                st = list_entry (le,struct thread, elem);
                time=st->time;

            }
        }
    }
    intr_set_level (old_level);    
}
