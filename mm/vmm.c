/*************************************************************************
  > File Name: vmm.c
  > Author: Ukey
  > Mail: gsl110809@gmail.com
  > Created Time: 2017年04月24日 星期一 14时54分21秒
 ************************************************************************/

#include "string.h"
#include "debug.h"
#include "idt.h"
#include "pmm.h"
#include "vmm.h"

//内核页目录区域
pgd_t pgd_kern[PGD_SIZE] __attribute__ ((aligned(PAGE_SIZE)));

//内核页表区域
pte_t pte_kern[PTE_COUNT][PTE_SIZE]	__attribute__ ((aligned(PAGE_SIZE)));

void init_vmm()
{
	// 注册页错误中断的处理函数 ( 14 是页故障的中断号 )
	register_interrupt_handler(14, &page_fault);

	//获取到0xC0000000地址在页目录的索引,即内核区第一个索引
	uint32_t kern_pte_first_idx = PGD_INDEX(PAGE_OFFSET);

	uint32_t i, j;
	for(i = kern_pte_first_idx, j = 0; i < PTE_COUNT + kern_pte_first_idx; i++, j++)
	{
		//减去0xC0000000的原因是,mmu地址翻译时,需要得到页表的物理地址
		pgd_kern[i] = ((uint32_t)pte_kern[j] - PAGE_OFFSET) | PAGE_PRESENT | PAGE_WRITE;
	}

	uint32_t *pte = (uint32_t *)pte_kern;
	for(i = 0; i < PTE_COUNT * PTE_SIZE; i++)
	{
		pte[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
	}
	uint32_t pgd_kern_phy_addr = (uint32_t)pgd_kern - PAGE_OFFSET;

	//将第一个页目录条目的地址更新到cr3寄存器中
	switch_pgd(pgd_kern_phy_addr);
}

//更换当前页目录
void switch_pgd(uint32_t addr)
{
	asm volatile ("mov %0, %%cr3" : : "r" (addr));
}

void map(pgd_t *pgd_now, uint32_t va, uint32_t pa, uint32_t	flags)
{
	uint32_t pgd_idx = PGD_INDEX(va);
	uint32_t pte_idx = PTE_INDEX(va);

	pte_t *pte = (pte_t *)(pgd_now[pgd_idx] & PAGE_MASK);
	//如果没有映射到物理内存上
	if(!pte)
	{
		//申请页面
		pte = (pte_t *)buddy_alloc_pages(1);
		//存入页目录中,建立映射
		pgd_now[pgd_idx] = (uint32_t)pte | PAGE_PRESENT | PAGE_WRITE;

		// 转换到内核线性地址并清 0
		pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);
		bzero(pte, PAGE_SIZE);
	}
	else
	{
		pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);
	}

	pte[pte_idx] = (pa & PAGE_MASK) | flags;

	//更新页表
	asm volatile ("invlpg (%0)" : : "a" (va));
}

void unmap(pgd_t *pgd_now, uint32_t va)
{
	uint32_t pgd_idx = PGD_INDEX(va);
	uint32_t pte_idx = PTE_INDEX(va);

	pte_t *pte = (pte_t *)(pgd_now[pgd_idx] & PAGE_MASK);

	if (!pte) {
		return;
	}

	// 转换到内核线性地址
	pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);
	pte[pte_idx] = 0;
	// 更新页表
	asm volatile ("invlpg (%0)" : : "a" (va));
}

uint32_t get_mapping(pgd_t *pgd_now, uint32_t va, uint32_t *pa)
{
	uint32_t pgd_idx = PGD_INDEX(va);
	uint32_t pte_idx = PTE_INDEX(va);

	pte_t *pte = (pte_t *)(pgd_now[pgd_idx] & PAGE_MASK);
	if (!pte) 
	{
		return 0;
	}

	// 转换到内核线性地址
	pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);
	// 如果地址有效而且指针不为NULL，则返回地址
	if (pte[pte_idx] != 0 && pa) 
	{
		*pa = pte[pte_idx] & PAGE_MASK;
		return 1;
	}
	return 0;
}

void page_fault(pt_regs_t *regs)
{
	uint32_t cr2;
	asm volatile ("mov %%cr2, %0" : "=r" (cr2));

	printk("Page fault at 0x%x, virtual faulting address 0x%x\n", regs->eip, cr2);
	printk("Error code: %x\n", regs->err_code);

	if ( !(regs->err_code & 0x1)) 
	{
		printk_color(rc_black, rc_red, "Because the page wasn't present.\n");
	}
	if (regs->err_code & 0x2) 
	{
		printk_color(rc_black, rc_red, "Write error.\n");
	} else 
	{
		printk_color(rc_black, rc_red, "Read error.\n");
	}
	// bit 2 为 1 表示在用户模式打断的，为 0 是在内核模式打断的
	if (regs->err_code & 0x4) 
	{
		printk_color(rc_black, rc_red, "In user mode.\n");
	} else {
		printk_color(rc_black, rc_red, "In kernel mode.\n");
	}
	// bit 3 为 1 表示错误是由保留位覆盖造成的
	if (regs->err_code & 0x8) 
	{
		printk_color(rc_black, rc_red, "Reserved bits being overwritten.\n");
	}
	// bit 4 为 1 表示错误发生在取指令的时候
	if (regs->err_code & 0x10) 
	{
		printk_color(rc_black, rc_red, "The fault occurred during an instruction fetch.\n");
	}

	while (1);
}

