/*************************************************************************
  > File Name: console.h
  > Author: Ukey
  > Mail: gsl110809@gmail.com
  > Created Time: 2017年04月12日 星期三 13时12分58秒
 ************************************************************************/

#ifndef INCLUDE_CONSOLE_H_
#define INCLUDE_CONSOLE_H_

#include "types.h"

typedef enum real_color
{
	rc_black = 0,
	rc_blue = 1,
	rc_green = 2,
	rc_cyan = 3,
	rc_red = 4,
	rc_magenta = 5,
	rc_brown = 6,
	rc_light_grey = 7,
	rc_dark_grey = 8,
	rc_light_blue = 9,
	rc_light_green = 10,
	rc_light_cyan = 11,
	rc_light_red = 12,
	rc_light_magenta = 13,
	rc_light_brown  = 14,	// yellow
	rc_white = 15
} real_color_t;

//清屏
void console_clear();

//屏幕输出一个字符(自定义颜色)
void console_putc_color(char c, real_color_t back, real_color_t fore);

//屏幕打印字符串
void console_write(char *str);

//屏幕打印字符串(自定义颜色)
void console_write_color(char *str, real_color_t back, real_color_t fore);

//屏幕打印十六进制的整数
void console_write_hex(uint32_t n, real_color_t back, real_color_t fore);

//屏幕输出十进制的整数
void console_write_dec(uint32_t n, real_color_t back, real_color_t fore);

#endif
