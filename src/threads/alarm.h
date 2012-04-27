#include "devices/timer.h"
#include <lib/kernel/list.h>
#include "threads/interrupt.h"

static struct list my_sleeping_list;

void alarm_init(void);

void set_alarm_at_time(int64_t time);

void check_alarm_completion(void);
