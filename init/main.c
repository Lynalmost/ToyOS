/*************************************************************************
  > File Name: entry.c
  > Author: Ukey
  > Mail: gsl110809@gmail.com
  > Created Time: 2017年04月11日 星期二 21时42分47秒
 ************************************************************************/
#include "gdt.h"
#include "console.h"
#include "debug.h"
#include "idt.h"
#include "timer.h"
int kern_entry()
{
	
	init_debug();
	init_gdt();
	init_idt();

	console_clear();

	printk_color(rc_black, rc_red, "hello os\n");

	init_timer(200);
	asm volatile ("sti");

	return 0;
}
