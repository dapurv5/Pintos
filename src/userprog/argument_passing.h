#ifndef ARGUMENT_PASSING_H
#define ARGUMENT_PASSING_H

#define MAX_ARG 128


void push_arguments(void **, char *);
unsigned int push_argv(void **, char *, int);
void align(void **, int);
void push_argv_ptr(void **, unsigned int);
void push_argc(void **, int);
void push_retval(void **, unsigned int);




void push_args(void **, char *);
#endif