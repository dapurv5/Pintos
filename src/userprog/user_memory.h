#ifndef USER_MEMORY_H
#define USER_MEMORY_H

#define LIMIT 0x100000000     /* 4 GB. */

void close_open_file_list(struct thread*);
struct thread* get_thread(tid_t);

int get_user(const uint8_t*);
bool put_user(uint8_t*, uint8_t);
bool strcopy(uint8_t*, uint8_t*, uint32_t);
void sys_exit(int);
bool valid (char* );

#endif