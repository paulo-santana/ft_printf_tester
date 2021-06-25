/******************************************************************************
 * test-malloc_count/test.c
 *
 * Small program to test malloc_count hooks and user functions.
 *
 ******************************************************************************
 * Copyright (C) 2013 Timo Bingmann <tb@panthema.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *****************************************************************************/

#include "malloc_count.h"
#include "stack_count.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void function_use_stack()
{
    char data[64*1024];
    memset(data, 1, sizeof(data));
}

int main()
{
    /* allocate and free some memory */
    void* a = malloc(2*1024*1024);
    free(a);

    /* query malloc_count for information */
    printf("our peak memory allocation: %lld\n",
           (long long)malloc_count_peak());

    /* use realloc() */
    void* b = malloc(3*1024*1024);
    malloc_count_print_status();

    b = realloc(b, 2*1024*1024);
    malloc_count_print_status();

    b = realloc(b, 4*1024*1024);
    malloc_count_print_status();

    free(b);

    /* some unusual realloc calls */
    void* c = realloc(NULL, 1*1024*1024);
    c = realloc(c, 0);

    /* show how stack_count works */
    {
        void* base = stack_count_clear();
        function_use_stack();
        printf("maximum stack usage: %lld\n",
               (long long)stack_count_usage(base));
    }

    return 0;
}

/*****************************************************************************/
