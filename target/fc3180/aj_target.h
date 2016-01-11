#ifndef _AJ_TARGET_H
#define _AJ_TARGET_H
/**
 * @file
 */
/******************************************************************************
 * Copyright (c) 2012-2014, AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

#include "aj_status.h"
#include "stdint.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define va_copy(x, y) (x) = (y)

#define TRUE 1
#define FALSE 0

#define max(x, y) ((x) > (y) ? (x) : (y))
#define min(x, y) ((x) < (y) ? (x) : (y))

#define WORD_ALIGN(x) ((x & 0x3) ? ((x >> 2) + 1) << 2 : x)

#define HOST_IS_LITTLE_ENDIAN FALSE
#define HOST_IS_BIG_ENDIAN TRUE
#define HOST_ENDIANESS AJ_BIG_ENDIAN

#include "esputil.h"
#define AJ_Printf(...) console_write(##__VA_ARGS__)

int select(int s, uint32_t timeout);
int selectfrom(int s, uint32_t timeout);

AJ_Status AJ_CreateTask(void (*task)(int, char **), char *name, int argc,
                        char **argv);
AJ_Status AJ_InitSystem();
AJ_Status AJ_InitOS();
AJ_Status AJ_StartOS();
void AJ_Main();

#define AJ_ASSERT(x) assert(x)

/*
 * AJ_Reboot() is a NOOP on this platform
 */
#define AJ_Reboot()

#define AJ_EXPORT

/*
 * Main method allows argc, argv
 */
//#define MAIN_ALLOWS_ARGS

#endif
