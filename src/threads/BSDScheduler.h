#include <lib/kernel/list.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

void thread_calculate_recent_cpu(struct thread*);
void thread_current_calculate_recent_cpu(void);
void all_thread_calculate_recent_cpu(void);

void thread_calculate_priority(struct thread*);
void thread_current_calculate_priority(void);
void all_thread_calculate_priority (void);

void thread_calculate_load_avg(void);