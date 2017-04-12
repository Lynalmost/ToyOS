/*************************************************************************
    > File Name: vargs.h
    > Author: Ukey
    > Mail: gsl110809@gmail.com
    > Created Time: 2017年04月12日 星期三 19时16分59秒
 ************************************************************************/

#ifndef INCLUDE_VARGS_H_
#define INCLUDE_VARGS_H_

// __builtin_va_list这些是gcc内置的变量和函数,是为了解决变参问题的一组宏(即可变参数)
// 用法:
// va_start宏初始化定义的va_list变量
// va_arg返回可变参数,type是要返回的参数的类型(如果有函数多个可变参数,依次调用VA_ARG获取各个参数)
// va_end结束可变参数的获取



// 可变参数是由宏实现的,由于平台和编译器的不同,定义的也就不同,以vc的x86平台的定义来了解一下可变参数怎么实现的：
// typedef char* va_list;
// #define _INTSIZEOF(n) ((sizeof(n) + sizeof(int) -1 ) & ~(sizeof(int) - 1))	//为了满足内存对齐
// #define va_start(ap, v)	(ap = (va_list) & v + _INTSIZEOF(v))	//ap指向第一个变参的位置
// #define va_arg(ap, t)	(*(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t))) //获取变参的具体内容,t为变量的类型,如果有多个参数,则通过移动ap的指针来获取变参的地址,从而获取内容
// #define va_end(ap)	(ap = (va_list)0)	//清空va_list,结束变参的获取
//C语言的函数形参是从右向左压入堆栈的,保证了栈顶是第一个参数

typedef __builtin_va_list va_list;

#define va_start(ap, last)		(__builtin_va_start(ap, last))
#define va_arg(ap, type)		(__builtin_va_arg(ap, type))
#define va_end(ap)				

#endif
