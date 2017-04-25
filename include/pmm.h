/*************************************************************************
    > File Name: pmm.h
    > Author: Ukey
    > Mail: gsl110809@gmail.com
    > Created Time: 2017年04月18日 星期二 12时32分52秒
 ************************************************************************/

#ifndef INCLUDE_PMM_H_
#define INCLUDE_PMM_H_

#include "types.h"
#include "multiboot.h"
#include "list.h"
#include "console.h"
#include "debug.h"
//内核起始位置为1M
#define PMM_KERN_START		0x100000

//物理页大小 4KB
#define PMM_PAGE_SIZE		0x1000	

//按4KB对齐
#define PMM_PAGE_MASK		0xFFFFF000

//栈的大小
#define STACK_SIZE			0x1000


//最大内存页块大小,即2^10=1024
//范围是2^0~2^10
#define MAX_ORDER			10


extern uint8_t kern_start[];
extern uint8_t kern_end[];

//bios获取获取内存信息其实是通过int 0x15 ax = 0xe820中断实现的
//这个结构体集合了mmap_entry_t数组和数组的个数,便于管理,并没有新增什么东西,关于mmap_entry_t,是在multiboot.h文件中定义的

#define MAP_MAX		20
typedef struct map_t
{
	//记录个数
	uint32_t count;
	struct
	{
		uint32_t addr_low;
		uint32_t addr_high;
		uint32_t length_low;
		uint32_t length_high;
		uint32_t type;
	}__attribute__((packed)) map[MAP_MAX];
}map_t;

//物理的页由该结构体来描述
typedef struct page_t
{
	uint32_t ref;		//物理页引用次数	
	uint32_t flag;		//页的状态,为0代表不可用
	uint32_t order;		//当前页的order值
	struct list_head list; 
}page_t;

typedef struct buddy_mm_t 
{
	//free_area
	struct list_head free_list[MAX_ORDER + 1];		//空闲内存块链
	uint32_t nr_free[MAX_ORDER + 1];				//对应的链上的内存块个数

	page_t *pages_base;								//物理页管理结构的起始地址
	uint32_t mm_addr_start;							//物理内存页起始地址
	uint32_t mm_addr_end;							//物理内存页结束地址
	uint32_t phy_page_count;						//物理内存页的总数量
	uint32_t phy_page_now_count;					//物理内存页当前数量
}buddy_mm_t;
//内存分为3个区域,ZONE_DMA代表在物理内存的低端,ZONE_NORMAL表示中端,ZONE_HIGHMEM表示高端。
//ZONE_HIGHMEM高端以上的地址会保留不用
typedef enum zone_t
{
	ZONE_DMA = 0,
	ZONE_NORMAL = 1,
	ZONE_HIGHMEM = 2
}zone_t;

//ZONE_DMA 0~16MB
//ZONE_NORMAL 16MB~8MB
//ZONE_HIGHMEM 896MB~结束
#define ZONE_NORMAL_ADDR		0x1000000   //16MB
#define ZONE_HIGHMEM_ADDR		0x38000000	//896MB

//物理内存初始化
void pmm_init();

//输出物理内存布局
void show_memory_map();

//根据物理页管理结构得到地址
uint32_t page2addr(page_t *page);

//根据地址得到物理管理结构
page_t *addr2page(uint32_t addr);

//伙伴系统算法的各个函数
void buddy_page_init(page_t *pages, uint32_t n);
uint32_t buddy_alloc_pages(uint32_t n);
void buddy_free_pages(uint32_t addr, uint32_t n);

#endif

