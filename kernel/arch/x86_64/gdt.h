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

#ifndef KERNEL_x86_64_GDT
#define KERNEL_x86_64_GDT

#include <stdint.h>
#include <libk/string.h>

struct tss_entry {
	uint16_t length;
	uint16_t base_low16;
	uint8_t base_mid8;
	uint8_t flags1;
	uint8_t flags2;
	uint8_t base_high8;
	uint32_t base_upper32;
	uint32_t reserved;
} __attribute__((packed));

struct gdt_entry {
	uint16_t limit;
	uint16_t base_low;
	uint8_t base_mid;
	uint8_t access;
	uint8_t granularity;
	uint8_t base_hi;
} __attribute__((packed));

struct gdt_desc {
	struct gdt_entry entries[5];
	struct tss_entry tss;
} __attribute__((packed));

struct gdt_ptr {
	uint16_t limit;
	uint64_t base;
} __attribute__((packed));

/**
 * init_gdt
 *   brief: Load the kernel GDT
 */
void init_gdt(void);

#endif
