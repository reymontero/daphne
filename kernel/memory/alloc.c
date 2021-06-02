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
 *
 * Malloc
 */

#include "alloc.h"

/* Offset for each mmap entry */
static uint32_t mmap_offs[256];

/* alloc_mem mutex lock */
static mutex_t alloc_mutex = 0;

static uint32_t *alloc_mem(size_t n)
{
	acquire_mutex(&alloc_mutex);
	int entry_found = 0;
	size_t i = 0;
	mmap_entry_t *mmap = kmem_get_kernel_mmap();

	for (; i < kmem_get_kmmap_size(); i++) {
		if (mmap[i].type == MEMORY_AVAILABLE) {
			/* See how much of this entry we have used */
			if (mmap->length_low - mmap_offs[i] >= n) {
				entry_found = 1;
				mmap_offs[i] += n;
				break;
			}
		}
	}

	if (!entry_found)
		panic("alloc_mem: ran out of memory!");

	release_mutex(&alloc_mutex);
	return (uint32_t *) mmap[i].base_addr_low + mmap_offs[i];
}

/* We align memory to 32 bytes for performance gains with bins
   NOT because we like to waste memory (well, most of us don't) */
size_t kmem_align(size_t n)
{
	/* Align n to 32 bits */
	if (n % 32 != 0) {
		if (n <= 32) {
			n = 32;
		} else {
			n = 32 * ((n / 32) + 1);
		}
	}

	return n;
}

void *alloc_mem_aligned(size_t n)
{	
	return (void *) alloc_mem(kmem_align(n));
}