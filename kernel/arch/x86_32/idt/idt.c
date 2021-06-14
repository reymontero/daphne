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
 * IDT
 */

/* TODO: Fix those nasty warnings */

#include "idt.h"

/* IDT structure */
static struct IDT_entry IDT_buffer[256];

/* IRQ handlers */
static void *irq_handlers[129];

/* Install an IRQ handler to the IDT */
void idt_install_irq_handler(void (*handler)(void), int i)
{
	printk("installing IRQ handler %i", i + 32);
	irq_handlers[32 + i] = handler;
}

void idt_install_raw_handler(void (*handler)(void), int i)
{
	irq_handlers[i] = handler;
}

/* Install all handlers */
void idt_install_handlers()
{
	extern void isr0(void);
	extern void isr1(void);
	extern void isr8(void);

	extern void irq0(void);
	extern void irq1(void);
	extern void _syscall(void);
	extern void generic_irq(void);

	idt_install_raw_handler(isr0, 0);
	idt_install_raw_handler(isr1, 1);
	idt_install_raw_handler(isr8, 8);

	/* PIT */
	idt_install_irq_handler(irq0, 0);
	/* Keyboard */
	idt_install_irq_handler(irq1, 1);
	/* syscall */
	idt_install_raw_handler(_syscall, 128);
}

/* Generate & load an IDT */
void init_idt(void)
{
	extern int load_idt();
	uint32_t irq_addresses[129];
	uint32_t idt_address;
	unsigned long idt_ptr[2];

	/* Install the handlers */
	idt_install_handlers();
	/* Remap the PIC */
	remap_pic();

	/* Fill the IDT */ 
	for (int i = 0; i < 129; i++) {
		irq_addresses[i] = (uint32_t) irq_handlers[i];
		IDT_buffer[i].offset_lowerbits = irq_addresses[i] & 0xffff;
		IDT_buffer[i].selector = 0x08;
		IDT_buffer[i].zero = 0;
		IDT_buffer[i].type_attr = 0x8e;
		IDT_buffer[i].offset_higherbits = (irq_addresses[i] & 0xffff0000) >> 16;
	}

	idt_address = (uint32_t) IDT_buffer;
	idt_ptr[0] = (sizeof (struct IDT_entry) * 256) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16;
	load_idt(idt_ptr);
}

void fault_handler(struct regs *r)
{
	const char *exception_messages[] = {
		"division by zero",
		"debug",
		"NMI",
		"breakpoint",
		"invalid",
		"out of bounds",
		"invalid opcode",
		"no coprocessor",
		"double fault",
		"coprocessor segment overrun",
		"bad TSS",
		"segment not present",
		"stack fault",
		"GPF",
		"page fault",
		"unknown interrupt",
		"coprocessor fault",
		"alignment check",
		"machine check",
		"reserved",
		"reserved",
		"reserved",
		"reserved",
		"reserved",
		"reserved",
		"reserved",
		"reserved",
		"reserved",
		"reserved",
		"reserved",
		"reserved",
		"reserved"
	};

	if (r->int_no < 32)
		panic(exception_messages[r->int_no]);
	else
		panic("what the hell?");
}
