/*************************************************************************
  > File Name: entry.c
  > Author: Ukey
  > Mail: gsl110809@gmail.com
  > Created Time: 2017年04月11日 星期二 21时42分47秒
 ************************************************************************/
#include "gdt.h"
#include "console.h"
#include "debug.h"
int kern_entry()
{
	
	init_debug();
	init_gdt();

	console_clear();

	printk_color(rc_black, rc_red, "hello os\n");

	panic("test");
	return 0;
}
