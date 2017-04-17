/*************************************************************************
    > File Name: io.h
    > Author: Ukey
    > Mail: gsl110809@gmail.com
    > Created Time: 2017年04月12日 星期三 09时51分24秒
 ************************************************************************/
#ifndef INCLUDE_IO_H_
#define INCLUDE_IO_H_

#include "types.h"

//这里也可以用内联函数来实现,不过需要注意,内联函数的定义和实现都需要在同一个文件下,如果在.h文件下定义,在.c文件下实现,则没有任何效果
//使用define或者inline定义函数,当调用到这些函数时,直接展开代码就行了，就降低了调用函数的系统的消耗。不过大型函数就不应该使用这两种方法了,具体原因请查阅相关资料。


//下面都是使用内联汇编实现,因为涉及到对硬件的访问,而C语言又没有直接操作端口的方法。


//端口写一个字节
#define outb(port, value) \
__asm__ volatile("outb %%al, %%dx" : : "a"(value), "d"(port))
//端口读一个字节
#define inb(port)({\
uint8_t v;\
__asm__ volatile ("inb %%dx, %%al" : "=a"(v) : "d"(port));\
v;\
})

//端口读一个字
#define inw(port)({\
uint16_t v;\
__asm__ volatile ("inw %%dx, %%ax" : "=a"(v) : "d"(port));\
v;\
})

#endif

