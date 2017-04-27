/*************************************************************************
    > File Name: kalloc.h
    > Author: Ukey
    > Mail: gsl110809@gmail.com
    > Created Time: 2017年04月26日 星期三 17时02分40秒
 ************************************************************************/
#ifndef INCLUDE_KALLOC_H_
#define INCLUDE_KALLOC_H_

#include "types.h"
#include "list.h"
#include "pmm.h"
#include "vmm.h"
#include "debug.h"
//分配器初始化
void alloc_init();


//内存分配函数
void *kmalloc(uint32_t size);

//内存释放函数
void kfree(void *addr);

#endif
