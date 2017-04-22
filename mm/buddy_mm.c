/*************************************************************************
  > File Name: mm/buddy_mm.c
  > Author: Ukey
  > Mail: gsl110809@gmail.com
  > Created Time: 2017年04月21日 星期五 19时55分34秒
 ************************************************************************/

#include "pmm.h"

buddy_mm_t zone_info;

static void printk_order_info();
static void	test();
//根据传入的size大小获取匹配的order值
static uint32_t get_order(uint32_t size);

//分配器
static page_t *buddy_alloc_pages_sub(uint32_t order);

//释放器
static void buddy_free_pages_sub(page_t *base, uint32_t order);

//根据页结构来找到是索引
static inline uint32_t page2idx(page_t *page)
{
	return page - zone_info.pages_base;
}

static inline page_t *idx2page(uint32_t idx)
{
	return zone_info.pages_base + idx;
}

static int page_is_buddy(page_t *page, uint32_t order)
{

	if(page2idx(page) < zone_info.phy_page_count)
	{
		return (page->flag && page->order == order);
	}
	return 0;
}

void buddy_page_init(page_t *pages, uint32_t n)
{
	//初始化该区域的信息
	zone_info.pages_base = pages;
	zone_info.phy_page_count = n;
	zone_info.phy_page_now_count = n;
	zone_info.mm_addr_start = page2addr(&pages[0]);
	zone_info.mm_addr_end = page2addr(&pages[n - 1]) + PMM_PAGE_SIZE;

	int i;
	for(i = 0; i <= MAX_ORDER; i++)
	{
		INIT_LIST_HEAD(&zone_info.free_list[i]);
		zone_info.nr_free[i] = 0;
	}

	//分配内存块
	int order = MAX_ORDER;
	int max_size = 1u << MAX_ORDER;
	page_t *p = pages;
	//根据最大的order来依次递减分配
	while(n != 0)
	{
		while(n >= max_size)
		{
			p->flag = 1;
			p->order = order;
			list_add(&p->list, &zone_info.free_list[order]);
			n -= max_size;
			p += max_size;
			zone_info.nr_free[order]++;
		}
		order--;
		max_size >>= 1;
	}
	//测试函数
	test();
}

uint32_t buddy_alloc_pages(uint32_t n)
{
	//n的值不合法
	if(n <= 0 || n > zone_info.phy_page_now_count)
	{
		return 0;
	}

	uint32_t order = get_order(n);
	uint32_t max_size = 1u << order;

	page_t *page = buddy_alloc_pages_sub(order);

	if(!page)
	{
		return 0;
	}
	//如果还有剩余的内存,则整合
	if(n != max_size)
	{
		buddy_free_pages(page2addr(page + n), max_size - n);
	}

	//更新还有多少物理页
	zone_info.phy_page_now_count -= n;

	//返回分配了的地址
	return page2addr(page);
}

//释放
void buddy_free_pages(uint32_t addr, uint32_t n)
{
	if(n <= 0)
	{
		return ;
	}

	page_t *base = addr2page(addr);

	uint32_t order = 0, max_size = 1;
	while(n >= max_size)
	{
		if((page2idx(base) & max_size) != 0)
		{
			buddy_free_pages_sub(base, order);
			base += max_size;
			n -= max_size;
		}
		order++;
		max_size <<= 1;
	}

	while(n != 0)
	{
		while(n < max_size)
		{
			order--;
			max_size >>= 1;
		}
		buddy_free_pages_sub(base, order);
		base += max_size;
		n -= max_size;
	}

	zone_info.phy_page_now_count += n;
}

//根据order分配
static page_t *buddy_alloc_pages_sub(uint32_t order)
{
	if(order > MAX_ORDER)
	{
		return NULL;
	}

	uint32_t current_order;
	for(current_order = order; current_order <= MAX_ORDER; current_order++)
	{
		//对应order的链表不为空
		if(!list_empty(&zone_info.free_list[current_order]))
		{

			struct list_head *next = (&zone_info.free_list[current_order])->next;
			//通过page_t结构体成员next获取到page_t的地址
			page_t * page = list_entry(next, page_t, list);
			zone_info.nr_free[current_order]--;
			list_del(next);
			uint32_t size = 1u << current_order;

			//将伙伴重新加到合适的order中
			while(current_order > order)
			{
				current_order--;
				size >>= 1;
				page_t *buddy_page = page + size;
				buddy_page->order = current_order;
				buddy_page->flag = 1;
				zone_info.nr_free[current_order]++;
				list_add(&(buddy_page->list), &zone_info.free_list[current_order]);
			}
			page->flag = 0;

			return page;
		}
	}
	return NULL;
}

static void buddy_free_pages_sub(page_t *base, uint32_t order)
{
	uint32_t buddy_idx, page_idx = page2idx(base);

	page_t *p = base;
	for (; p != base + (1u << order); p++) {
		p->ref = 0;
	}

	//将伙伴分配到低级的order中
	while(order < MAX_ORDER) {
		buddy_idx = page_idx ^ (1u << order);
		page_t *buddy_page = idx2page(buddy_idx);
		if (!page_is_buddy(buddy_page, order)) {
			break;
		}
		zone_info.nr_free[order]--;
		list_del(&buddy_page->list);
		buddy_page->flag = 0;
		page_idx &= buddy_idx;
		order++;
	}
	//更新page结构体的成员
	page_t *page = idx2page(page_idx);
	page->order = order;
	page->flag = 1;
	zone_info.nr_free[order]++;
	list_add(&page->list, &zone_info.free_list[order]);
}

static uint32_t get_order(uint32_t size)
{
	uint32_t order = 0, max_size = 1;

	while (order <= MAX_ORDER) {
		if (max_size >= size) {
			return order;
		}
		order++;
		max_size <<= 1;
	}
	panic("getorder failed.");
	return 0;
}
static void printk_order_info()
{
	uint32_t order;
	for (order = 0; order < MAX_ORDER + 1; ++order) 
	{
		printk("order%d:%4u     ", order, zone_info.nr_free[order]);
	}
	printk("\n");
}
static void test()
{
	uint32_t page[2];
	printk("before alloc:\n ");
	printk_order_info();
	
	page[0] = buddy_alloc_pages(1);
	buddy_free_pages(page[0], 1);
	printk("after alloc 1:\n");
	printk_order_info();
	
	page[1] = buddy_alloc_pages(680);
	buddy_free_pages(page[1], 680);
	printk("after alloc 680:\n");
	printk_order_info();
}



