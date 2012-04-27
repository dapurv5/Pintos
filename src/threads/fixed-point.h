#include <debug.h>
#include <stddef.h>
#include <random.h>
#include <stdio.h>
#include <string.h>

#define P 17
#define Q 14

#if P + Q != 31
 #error "FATAL ERROR: P + Q != 31."
#endif

int int_to_fixed_pt(int);
int fixed_pt_to_nearest_int(int);
int fixed_pt_to_int(int);
int fixed_pt_multiplication(int, int);
int fixed_pt_division(int, int);