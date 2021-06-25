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
 * Memory manager
 */

#include "mm.h"

/* TODO: 64 bit addresses */

/* mmap as given by grub */
static mmap_entry_t *mmap_begin;

/* mmap as sanity checked by the kernel */
static mmap_entry_t kmmap[256];
static size_t kmmap_size = 0;
static uint32_t mmap_length = 0;

/* RAM info */
static uint32_t total_ram = 0;
static uint32_t highest_addr = 0;
static size_t regions = 0;

/* From linker.ld */
extern uint32_t kstart;
extern uint32_t kend;

/* Dump mmap entry */
void dump_entry(mmap_entry_t *entry)
{
	switch (entry->type) {
		case MEMORY_AVAILABLE:
			printk("base_addr=%ui, length=%uiB - available", entry->base_addr, entry->length);
			break;
		case MEMORY_RESERVED:
			printk("base_addr=%ui, length=%uiB - reserved", entry->base_addr, entry->length);
			break;
		case MEMORY_NVS:
		case MEMORY_ACPI:
			printk("base_addr=%ui, length=%uiB - acpi", entry->base_addr, entry->length);
			break;
	}
}

void kmem_init(multiboot_info_t *info)
{
	mmap_entry_t *mmap = (mmap_entry_t *) (uintptr_t) info->mmap_addr;
	mmap_begin = (mmap_entry_t *) (uintptr_t) info->mmap_addr;
	mmap_length = info->mmap_length;

	/* Validate mmap */
	for (size_t i = 0; (uint32_t) (uintptr_t) mmap < (info->mmap_addr + info->mmap_length); i++) {
		/* 0 length entries */
		if (mmap->length == 0x0) {
			mmap->type = MEMORY_INVALID;
		}

		/* Entry overlaps with kernel code */
		if (mmap->base_addr + mmap->length<= KERN_END) {
			mmap->type = MEMORY_INVALID;
		} else if ((mmap->base_addr <= KERN_END) && mmap->type == MEMORY_AVAILABLE) {
			/* If only a part of it does, keep the part that doesn't */
			mmap->base_addr += KERN_END + 1;
			mmap->length -= KERN_END + 1;
		}

		/* Overlapping entries */
		for (size_t j = 0; i < kmmap_size; j++) {
			/* Entry overlaps with another */
			if (!(mmap->base_addr < kmmap[j].base_addr) && 
				(mmap->base_addr < (kmmap[j].base_addr+ kmmap[j].length))) {
				mmap->type = MEMORY_INVALID;
			}
		}

		/* Append entry to kmmap */
		if (mmap->type != MEMORY_INVALID) {
			if (mmap->type == MEMORY_AVAILABLE) {
				regions++;
			}
			kmmap[kmmap_size] = *mmap;
			++kmmap_size;
		}

		total_ram += mmap->length;
		dump_entry(mmap);
		/* Next entry */
		mmap = (mmap_entry_t *) ((uint32_t) (uintptr_t) mmap + mmap->size + sizeof(mmap->size));
	}

	/* You expect this shit to run with less than 5mb ram? */
	if (total_ram <= MiB(5)) {
		panic("Here's a nickel kid. Go buy yourself a real computer");
	}
}

void kmem_init_paging(void)
{
	#ifdef ARCH_x86_32
	arch_init_paging();
	#endif
}

mmap_entry_t *kmem_get_kernel_mmap(void)
{
	return kmmap;
}

size_t kmem_get_kmmap_size(void)
{
	return kmmap_size;
}

uint32_t kmem_get_installed_memory(void)
{
	return total_ram;
}

uint32_t kmem_get_highest_addr(void)
{
	return highest_addr;
}
