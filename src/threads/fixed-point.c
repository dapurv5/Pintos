#include <debug.h>
#include <stddef.h>
#include <random.h>
#include <stdio.h>
#include <string.h>
#include "threads/fixed-point.h"

/*Inspired from Pintos Documentation on Fixed-Point Arithmetic : Section Appendix B.6 on page 101-102 */
/* x, y are fixed point representation while n is a number.*/

static int fraction = 1<<Q;

int int_to_fixed_pt(int n)
{
    return ((n) * fraction);
}

int fixed_pt_to_nearest_int(int x)
{
   if(x >= 0)
    return (((x) + (fraction / 2)) / fraction );
   else
    return (((x) - (fraction / 2)) / fraction );
}

int fixed_pt_to_int(int x)
{
    return ((x) / fraction);
}

int fixed_pt_multiplication(int x,int y)
{
    return (((int64_t)x) * y / fraction);
}

int fixed_pt_division(int x,int y)
{
    return ((((int64_t)(x)) * fraction) / y);
}






