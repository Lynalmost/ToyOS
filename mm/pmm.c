/*************************************************************************
  > File Name: pmm.c
  > Author: Ukey
  > Mail: gsl110809@gmail.com
  > Created Time: 2017年04月18日 星期二 12时40分12秒
 ************************************************************************/

#include "multiboot.h"
#include "io.h"
#include "debug.h"
#include "pmm.h"
#include "vmm.h"
#include "string.h"

//物理页数组指针
static page_t *phy_pages = (page_t * )((uint32_t)kern_end + KERN_BASE + KVPAGE_SIZE);

//物理页数量
static uint32_t phy_pages_count;

//物理页的起始地址
static uint32_t pmm_addr_start;

//物理页的终止地址
static uint32_t pmm_addr_end;

//获取内存信息
static void get_mem_info(map_t *mmap);

//物理页初始化
static void phy_pages_init(map_t *mmap);



//总的初始化函数
void pmm_init()
{
	map_t mmap;
	bzero(&mmap, sizeof(mmap));

	get_mem_info(&mmap);

	phy_pages_init(&mmap);

	buddy_page_init(phy_pages, phy_pages_count);
}

//获取内存信息并存储在结构体中
static void get_mem_info(map_t *mmap)
{
	mmap_entry_t *map_array;
	for(map_array = (mmap_entry_t *)glb_mboot_ptr->mmap_addr; map_array < (mmap_entry_t *)glb_mboot_ptr->mmap_addr + glb_mboot_ptr->mmap_length  ; map_array ++)
	{
		//如果是可用区域并且起始地址是1M则满足要求,type=2的话表示当前内存块已被占用,type=3代表保留该内存块
		if(map_array->type == 1 && map_array->base_addr_low == 0x100000)
		{
			mmap->map[mmap->count].addr_low = map_array->base_addr_low;
			mmap->map[mmap->count].addr_high = map_array->base_addr_high;
			mmap->map[mmap->count].length_low = map_array->length_low;
			mmap->map[mmap->count].length_high = map_array->length_high;
			//type为1表示是可用区域
			mmap->map[mmap->count].type = 1;
			mmap->count++;
		}
	}
}

//物理页初始化
static void phy_pages_init(map_t *mmap)
{
	uint32_t phy_mem_length = 0;
	uint32_t i;
	for(i = 0; i < mmap->count; i++)
	{
		//如果是高端区,则直接break,因为高端区保留
		if(mmap->map[i].addr_low > ZONE_HIGHMEM_ADDR)
		{
			break;
		}
		//如果基址+长度涉及到了高端区,那就设置到高端区为止
		if(mmap->map[i].addr_low + mmap->map[i].length_low > ZONE_HIGHMEM_ADDR)
		{
			phy_mem_length = ZONE_HIGHMEM_ADDR;
			break;
		}
		phy_mem_length = mmap->map[i].length_low;
	}

	//物理页数组的总数
	bzero(phy_pages, sizeof(page_t) * (phy_mem_length / PMM_PAGE_SIZE));

	//物理内存页管理起始地址
	pmm_addr_start = ((uint32_t)phy_pages - KERN_BASE + (sizeof(page_t) * (phy_mem_length / PMM_PAGE_SIZE)) + PMM_PAGE_SIZE) & PMM_PAGE_MASK;
	for(i = 0; i < mmap->count; i++)
	{
		uint32_t start_addr = mmap->map[i].addr_low;
		uint32_t end_addr = mmap->map[i].addr_low + mmap->map[i].length_low;
		uint32_t addr;
		//找到起始和终止地址
		if(start_addr < pmm_addr_start)
		{
			start_addr = pmm_addr_start;
		}
		if(end_addr > ZONE_HIGHMEM_ADDR)
		{
			end_addr = ZONE_HIGHMEM_ADDR;
		}
		//统计物理页总数
		for(addr = start_addr; addr < end_addr; addr += PMM_PAGE_SIZE)
		{
			
			phy_pages_count++;
		}
		pmm_addr_end = end_addr;
	}
}
//输出内存信息
void show_memory_map()
{
	uint32_t mmap_addr = glb_mboot_ptr->mmap_addr;
	uint32_t mmap_length = glb_mboot_ptr->mmap_length;

	printk("memory map info:\n");
	
	mmap_entry_t *mmap = (mmap_entry_t *)mmap_addr;
	for (; (uint32_t)mmap < mmap_addr + mmap_length; mmap++) {
		printk("base_addr = 0x%X%08X, length = 0x%X%08X, type = 0x%X\n",
				(uint32_t)mmap->base_addr_high, (uint32_t)mmap->base_addr_low,
				(uint32_t)mmap->length_high, (uint32_t)mmap->length_low,
				(uint32_t)mmap->type);
	}

}

//通过页找到地址
uint32_t page2addr(page_t *page)
{
	assert(pmm_addr_start != 0, "memory not init");
	return pmm_addr_start + (uint32_t )(page - phy_pages) * PMM_PAGE_SIZE;
}

//通过地址找到页
page_t *addr2page(uint32_t addr)
{
	assert(pmm_addr_start != 0, "memory not init");
	return (phy_pages + ((addr & PMM_PAGE_MASK) - pmm_addr_start) / PMM_PAGE_SIZE);
}



