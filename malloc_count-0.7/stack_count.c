/******************************************************************************
 * stack_count.c
 *
 * Header containing two functions to monitor stack usage of a program.
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

#include "stack_count.h"

#include <inttypes.h>

/* default stack size on Linux is 8 MiB, so fill 75% of it. */
static const size_t stacksize = 6*1024*1024;

/* "clear" the stack by writing a sentinel value into it. */
void* stack_count_clear(void)
{
    const size_t asize = stacksize / sizeof(uint32_t);
    uint32_t stack[asize]; /* allocated on stack */
    uint32_t* p = stack;
    while ( p < stack + asize ) *p++ = 0xDEADC0DEu;
    return p;
}

/* checks the maximum usage of the stack since the last clear call. */
size_t stack_count_usage(void* lastbase)
{
    const size_t asize = stacksize / sizeof(uint32_t);
    uint32_t* p = (uint32_t*)lastbase - asize; /* calculate top of last clear. */
    while ( *p == 0xDEADC0DEu ) ++p;
    return ((uint32_t*)lastbase - p) * sizeof(uint32_t);
}

/*****************************************************************************/
