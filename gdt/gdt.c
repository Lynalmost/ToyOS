/*************************************************************************
  > File Name: gdt.c
  > Author: Ukey
  > Mail: gsl110809@gmail.com
  > Created Time: 2017年04月14日 星期五 11时53分41秒
 ************************************************************************/

#include "gdt.h"
#include "string.h"

//全局描述符表长度
#define GDT_LENGTH 5

//全局描述符表定义
gdt_entry_t gdt_entries[GDT_LENGTH];

//GDTR
gdt_ptr_t gdt_ptr;

//内核栈地址
extern uint32_t stack;

//设置相应的描述符的信息
static void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
	gdt_entries[num].base_low     = (base & 0xFFFF);
	gdt_entries[num].base_middle  = (base >> 16) & 0xFF;
	gdt_entries[num].base_high    = (base >> 24) & 0xFF;

	gdt_entries[num].limit_low    = (limit & 0xFFFF);
	gdt_entries[num].granularity  = (limit >> 16) & 0x0F;

	gdt_entries[num].granularity |= gran & 0xF0;
	gdt_entries[num].access       = access;
}

//初始化gdt并将gdtr寄存器的值设置好
void init_gdt()
{
	//设置gdtr寄存器的值,因为是从0开始,所以需要减1
	gdt_ptr.limit = sizeof(gdt_entry_t) * GDT_LENGTH - 1;
	gdt_ptr.base = (uint32_t)gdt_entries;

	//使用平坦模式(意思就是在段式内存管理的基础上，如果每个段寄存器都指向同一个段描述子，
	//而此段描述子中把段的基地址设为0，长度设为最大（4G），这样就形成了一个覆盖整个地址空间的巨大段。
	//此时逻辑地址就和物理地址相同。形象的看，这样的地址就没有层次结构（段：偏移）了，所以叫做平坦模式)

	//由于我们使用的是平坦模式,所以起始地址和终止地址以及gran都是相同的,但是权限不同,所以需要自己根据intel文档去计算
	gdt_set_gate(0, 0, 0, 0, 0);	//intel文档要求第一个描述符必须为全0
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);		//指令段
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);		//数据段
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);		//用户模式代码段
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);		//用户模式数据段

	//加载到gdtr寄存器中
	gdt_flush((uint32_t)&gdt_ptr);
}


