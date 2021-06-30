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
 * Kernel main function
 */

#include "kmain.h"

// extern void enter_usermode(void);

/* Kernel main function */
void kmain(efi_info_t *info)
{
	con_plot_px(70, 80, 0xFFFFFF, info->gop->fb_base, info->gop->pps);

	init_tss(); /* Init TSS */
	init_gdt(); /* Init GDT */
	init_idt(); /* Init IDT */

	/* Set kernel mode */
	set_kernel_mode(TTY_MODE);

	#ifdef BUILD_TESTS
		/* Start tests */
		do_tests();
	#endif

	for(;;);
}
