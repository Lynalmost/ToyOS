/*************************************************************************
  > File Name: entry.c
  > Author: Ukey
  > Mail: gsl110809@gmail.com
  > Created Time: 2017年04月11日 星期二 21时42分47秒
 ************************************************************************/

#include "types.h"
#include "console.h"
int kern_entry()
{
	
	
	console_clear();

	console_write_color("Hello OS World\n", rc_black, rc_red);

	return 0;
}
