/*************************************************************************
  > File Name: kalloc.c
  > Author: Ukey
  > Mail: gsl110809@gmail.com
  > Created Time: 2017年04月26日 星期三 17时04分53秒
 ************************************************************************/

#include "kalloc.h"


static LIST_HEAD(block_head);

typedef struct alloc_block
{
	uint32_t alloc_state : 1;		
	uint32_t length : 31;	
	struct list_head list;
}alloc_block_t;

#define BLOCK_USED	1
#define BLOCK_FREE	0

//最少分配单位,在正式的SLOB算法中,是通过粒度unit和对齐字节大小来减少内存碎片的。这里简化使用最少分配单位。
#define ALLOC_MIN_PER   0x10
//先分配512个页来使用,在SLOB算法中,是根据所需来分配,比如大于PAGE_SIZE就直接使用伙伴系统算法了
//并且如果没有剩余的空间units了,就会创建新的slob
#define ALLOC_PAGE_COUNT	512


void alloc_init()
{
	
	uint32_t alloc_addr = buddy_alloc_pages(ALLOC_PAGE_COUNT);
	assert(alloc_addr != NULL,
			"alloc :buddy_alloc_pages error\n");

	alloc_block_t *block = (alloc_block_t *)(alloc_addr + KERN_BASE);
	block->alloc_state = BLOCK_FREE;
	block->length = ALLOC_PAGE_COUNT * PAGE_SIZE - sizeof(alloc_block_t);
	list_add(&block->list, &block_head);

}

//分割块
static void split_chunk(alloc_block_t *chunk_block, uint32_t len)
{
	if(chunk_block->length - len > sizeof(alloc_block_t) + ALLOC_MIN_PER)
	{
		alloc_block_t *new_chunk = (alloc_block_t *)((uint32_t)chunk_block + len + sizeof(alloc_block_t));
		new_chunk->alloc_state = BLOCK_FREE;
		new_chunk->length = chunk_block->length - len - sizeof(alloc_block_t);

		list_add(&new_chunk->list, &chunk_block->list);

		chunk_block->length = len;
		chunk_block->alloc_state = BLOCK_USED;
	}
}

//合并块
static void glue_chunk(alloc_block_t *chunk_block)
{
	struct list_head *p = &chunk_block->list;

	//如果不是头结点的下一个结点
	if(p->prev != &block_head)
	{
		alloc_block_t *prev_block = list_entry(p->prev, alloc_block_t, list);
		if(prev_block->alloc_state == BLOCK_FREE)
		{
			prev_block->length += (chunk_block->length + sizeof(alloc_block_t));
			list_del(&chunk_block->list);
			chunk_block = prev_block;
		}
	}

	//如果不是尾结点
	if(p->next != &block_head)
	{
		alloc_block_t *next_block = list_entry(p->next, alloc_block_t, list);
		if(next_block->alloc_state == BLOCK_FREE)
		{
			chunk_block->length += (next_block->length + sizeof(alloc_block_t));
			list_del(&next_block->list);
		}
	}
}


void *kmalloc(uint32_t size)
{
	//保持最小分配的大于ALLOC_MIN_PER
	uint32_t len = (size > ALLOC_MIN_PER) ? size : ALLOC_MIN_PER;

	len += sizeof(alloc_block_t);

	if (!list_empty(&block_head)) 
	{
		struct list_head *p = NULL;
		//遍历分配链表
		list_for_each(p, &block_head)
		{
			alloc_block_t *block = list_entry(p, alloc_block_t, list);
			//如果当前的块符合要求,则分配
			if (block->alloc_state == BLOCK_FREE && block->length > len) 
			{
				split_chunk(block, len);
				return (void *)((uint32_t)block + sizeof(alloc_block_t));
			} 
		}
	}

	return NULL;
}

void kfree(void *addr)
{
	alloc_block_t *block = (alloc_block_t *)((uint32_t)addr - sizeof(alloc_block_t));

	//当前块未被使用
	if(block->alloc_state != BLOCK_USED)
	{
		return;
	}
	//如果使用了,设置alloc_state状态,并合并
	block->alloc_state = BLOCK_FREE;

	glue_chunk(block);
}

