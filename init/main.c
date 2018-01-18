/*************************************************************************
  > File Name: main.c
  > Author: Ukey
  > Mail: gsl110809@gmail.com
  > Created Time: 2017年04月11日 星期二 21时42分47秒
 ************************************************************************/
#include "gdt.h"
#include "console.h"
#include "debug.h"
#include "idt.h"
#include "timer.h"
#include "pmm.h"
#include "vmm.h"
#include "kalloc.h"
#include "sched.h"
//开启分页机制之后的multiboot指针
multiboot_t *glb_mboot_ptr;

//开启分页机制后的内核栈
int8_t kern_stack[STACK_SIZE]; 

uint32_t kern_stack_top;
//内核栈的栈顶

//__attribute__((section("")))的作用是指定变量或函数的存储区段

//临时页表和页目录
__attribute__((section(".init.data"))) pgd_t *pgd_tmp  = (pgd_t *)0x1000;
__attribute__((section(".init.data"))) pgd_t *pte_low  = (pgd_t *)0x2000;
__attribute__((section(".init.data"))) pgd_t *pte_high = (pgd_t *)0x3000;


static int kern_init();
//入口函数
__attribute__((section(".init.text"))) void kern_entry()
{
	uint32_t cr0;
	pgd_tmp[0] = (uint32_t)pte_low | PAGE_PRESENT | PAGE_WRITE;
	int i;

	for( i = 0; i < 4; i++) 
	{
		uint32_t pgd_idx = PGD_INDEX(PAGE_OFFSET + PAGE_MAP_SIZE * i);
		pgd_tmp[pgd_idx] = ((uint32_t)pte_high + PAGE_SIZE * i) | PAGE_PRESENT | PAGE_WRITE;
	}
	//将内核虚 地址4MB映射到物理地址的前4MB中
	//因为.init.data的代码肯定不会超过物理地址的前4MB
	for(i = 0; i < 1024; i++)
	{
		pte_low[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
	}

	//映射0x00000000-0xC0000000的物理地址到虚拟地址0xC0000000-0xC1000000
	for(i = 0; i < 1024 * 4; i++)
	{
		pte_high[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
	}

	//设置临时页表
	asm volatile("mov %0, %%cr3" : : "r" (pgd_tmp));

	//启用分页
	asm volatile("mov %%cr0, %0" : "=r" (cr0));
	//cr0寄存器的最高位置1,代表分页开启
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0" : : "r" (cr0));
	kern_stack_top = ((uint32_t)kern_stack + STACK_SIZE) & 0xFFFFFFF0; 
	//切换临时栈到新栈
	asm volatile("mov %0, %%esp\n\t"
			"xor %%ebp, %%ebp" : : "r" (kern_stack_top));

	//更新全局multiboot_t指针指向
	glb_mboot_ptr = mboot_ptr_tmp + PAGE_OFFSET;

	//调用内核初始化函数
	kern_init();
	return ;
}

//线程测试函数
int thread(void *arg)
{
	while (1) {
		printk_color(rc_black, rc_blue, "this is a new thread\n");
		int count = 100000000;
		while(count--);
	}
}


int kern_init()
{

	init_debug();
	init_gdt();
	init_idt();

	console_clear();

	init_timer(200);
	printk_color(rc_black, rc_red, "hello os\n");


	printk("kernel in memory start: 0x%08X\n", kern_start);
	printk("kernel in memory end:   0x%08X\n", kern_end);
	printk("kernel in memory used:   %d KB\n\n", (kern_end - kern_start + 1023) / 1024);

	show_memory_map();
	pmm_init();
	init_vmm();
	alloc_init();
	sched_init();
	
	//新建一个线程
	kernel_thread(thread, NULL);

	//开启中断
	asm volatile ("sti");

	while (1) {
		printk_color(rc_black, rc_red, "this is main thread\n");
		int count = 100000000;
		while(count--);
	}
	return 0;
}
