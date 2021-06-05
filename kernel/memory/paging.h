/*
 * Copyright (C) 2020 synthels <synthels.me@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef PAGING
#define PAGING

#include <stdint.h>
#include <logger/panic.h>
#include <memory/mm.h>

#include "watermark.h"

typedef uint32_t pte_t;
typedef uint32_t pdir_t;

/**
 * init_paging
 *   brief: Initialise paging
 */
void init_paging(void);

/**
 * kbrk
 *   brief: extend kernel memory space by n
 */
void kbrk(size_t n);

#endif
