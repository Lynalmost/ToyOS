/*************************************************************************
    > File Name: vmm.h
    > Author: Ukey
    > Mail: gsl110809@gmail.com
    > Created Time: 2017年04月24日 星期一 14时14分28秒
 ************************************************************************/

#ifndef INCLUDE_VMM_H_
#define INCLUDE_VMM_H_

#include "types.h"
#include "idt.h"

//页目录数据类型
typedef uint32_t pgd_t;

//页表数据类型
typedef uint32_t pte_t;

//虚拟分页大小 4KB
#define PAGE_SIZE		0x1000

//内核起始虚拟地址
#define KERN_BASE		0xC0000000

//内核偏移地址
#define PAGE_OFFSET		0xC0000000

//按照4KB对齐
#define PAGE_MASK		0xFFFFF000

//内核页表大小
#define KVPAGE_SIZE		0x400000

//每个页表可以映射的内存数
#define PAGE_MAP_SIZE	0x400000
/*
 * P位 代表虚拟页是否存在于物理内存中 
 * 0代表不存在 1代表存在
 */
#define PAGE_PRESENT	0x1

/*
 * R/W位 代表读写权限
 * 1代表页面可读、写或执行 0代表页面只读或执行
 */

#define PAGE_WRITE		0x2

/*
 * U/S位 用户/超级用户标志
 * 1代表任何特权级的程序都可以访问该页面 
 * 0代表页面只能被运行在超级用户权限上的程序访问
 */
#define PAGE_USER		0x4


//获取地址的页目录项
#define PGD_INDEX(x)	(((x) >> 22) & 0x3FF)

//获取地址的页表项
#define PTE_INDEX(x)	(((x) >> 12) & 0x3FF)

//获取地址的页内偏移
#define OFFSET_INDEX(x)	((x) & 0xFFF)

//页目录成员数
#define PGD_SIZE		(PAGE_SIZE/sizeof(pgd_t))

//页表成员数
#define PTE_SIZE		(PAGE_SIZE/sizeof(pte_t))

//映射512MB内存需要的内存数
#define PTE_COUNT		128

// 内核页目录区
extern pgd_t pgd_kern[PGD_SIZE];

// 初始化虚拟内存管理
void init_vmm();

// 更换当前的页目录
void switch_pgd(uint32_t addr);

// 物理地址映射到虚拟地址
void map(pgd_t *pgd_now, uint32_t va, uint32_t pa, uint32_t flags);

// 取消虚拟地址的物理映射
void unmap(pgd_t *pgd_now, uint32_t va);

// 如果虚拟地址映射到物理地址则返回 1 如果 pa 不是空指针则把物理地址写入 pa 参数
uint32_t get_mapping(pgd_t *pgd_now, uint32_t va, uint32_t *pa);

// 页错误中断的函数处理
void page_fault(pt_regs_t *regs);

#endif
