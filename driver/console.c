/*************************************************************************
  > File Name: console.c
  > Author: Ukey
  > Mail: gsl110809@gmail.com
  > Created Time: 2017年04月12日 星期三 13时29分39秒
 ************************************************************************/

#include "console.h"
#include "io.h"

/*
 *内核可以通过VGA来控制屏幕上字符或图形的显示,在文本模式下,可以通过VGA控制器保留的一块内存(0x8b000~0x8bfa0)作为屏幕上字符显示的缓冲区
 *如果想要改变屏幕上显示的字符,那么直接修改缓冲区的内容,再输出就行了
 *
 */

//屏幕的宽度和长度是80和25
#define WIDTH 80
#define HIGH  25

//由于VGA内部的寄存器很多,无法全部都映射到I/O端口的地址空间上
//所以采用的是将一个端口作为VGA内部寄存器的索引:0x3D4
//再通过0x3D5端口来设置相应寄存器的值
#define VGA_INDEX 0x3D4
#define VGA_SETVL 0x3D5

//这里会用到那众多寄存器中的两个,根据索引再加编号确定VGA上的寄存器,分别表示光标位置的高8位和低8位
#define CURSOR_HIGH	0xE
#define CURSOR_LOW	0xF

//VGA的显示缓冲起点为0xb8000
static uint16_t *vga_buffer = (uint16_t *)0xB8000;

//光标的坐标
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

//定义黑底白字的颜色和空格符
uint16_t  bblack_fwhite = ((0 << 4 ) | (15 & 0x0F)) << 8;


//移动光标
static void move_cursor()
{
	uint16_t cursor_now = cursor_x + cursor_y * WIDTH;

	//设置光标的高8位
	outb(CURSOR_HIGH, VGA_INDEX);
	outb(cursor_now >> 8, VGA_SETVL);

	//设置光标的低8位
	outb(CURSOR_LOW, VGA_INDEX);
	outb(cursor_now, VGA_SETVL);
}

//清屏,需要做的仅仅是用黑底白字的空格符覆盖整块屏幕就行了
void console_clear()
{
	uint16_t  blank = 0x20 | bblack_fwhite;
	int i;

	for(i = 0; i < WIDTH * HIGH; i++)
	{
		vga_buffer[i] = blank;
	}

	cursor_x = 0;
	cursor_y = 0;
	//设置了光标的坐标之后,记得及时更新
	move_cursor();
}

//屏幕滚动,其实也很简单,就是将所有行向上移动,让原来的第一行被覆盖,最后一行清空
static void scroll()
{
	uint16_t  blank = 0x20 | bblack_fwhite ;
	//当cursor_y的值大于屏幕所能够显示的高度时,就该滚动屏幕了
	if(cursor_y >= HIGH)
	{
		int i;

		//逐个重新赋值
		for(i = 0; i < WIDTH * HIGH; i++)
		{
			vga_buffer[i] = vga_buffer[i + 80];
		}

		//最后一行填充空格就行了
		for(i = (HIGH - 1) * WIDTH; i < HIGH * WIDTH; i++)
		{
			vga_buffer[i] = blank;
		}

		//都向上移动了一行,所以目前光标的y坐标应该是倒数第二行
		cursor_y = HIGH - 1;
	}
}

//显示字符串
void console_putc_color(char c, real_color_t back, real_color_t fore)
{
	uint8_t back_color = (uint8_t)back;
	uint8_t fore_color = (uint8_t)fore;

	uint16_t color = ((back_color << 4) | (fore_color & 0x0F)) << 8;

	//这里需要特殊处理退格(0x08)和tab(0x09)以及回车和换行
	//回车和换行涉及到平台的关系
	//windows:回车`\r`是将光标移到该行开始的位置,换行`\n`是将光标平移移向下一行
	//linux:换行`\n`完成了windows下`\r\n`组合的功能,即移到下一行开头的位置,`\r`还是将光标移到该行开始的位置的功能
	if(cursor_x && c == 0x08)
	{
		cursor_x --;
	}
	else if(c == 0x09)
	{
		cursor_x = (cursor_x + 8) & ~(8-1);
	}
	else if(c == '\r')
	{
		cursor_x = 0;
	}
	else if(c == '\n')
	{
		cursor_x = 0;
		cursor_y++;
	}
	else if(c >= ' ')
	{
		vga_buffer[cursor_y * WIDTH + cursor_x ] = c | color;
		cursor_x ++;
	}

	//如果该行已满
	if(cursor_x >= WIDTH)
	{
		cursor_x = 0;
		cursor_y ++;
	}

	//检测下是否需要滚屏,如果要,则滚动屏幕
	scroll();

	//更新光标位置
	move_cursor();
}

void console_write(char *cstr)
{
	while (*cstr) 
	{
		console_putc_color(*cstr++, rc_black, rc_white);
	}
}

void console_write_color(char *cstr, real_color_t back, real_color_t fore)
{
	while (*cstr) 
	{
		console_putc_color(*cstr++, back, fore);
	}
}
