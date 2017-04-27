/*************************************************************************
  > File Name: list.h
  > Author: Ukey
  > Mail: gsl110809@gmail.com
  > Created Time: 2017年04月18日 星期二 20时16分14秒
 ************************************************************************/

#ifndef INCLUDE_LIST_H_
#define INCLUDE_LIST_H_

#include "types.h"

struct list_head
{
	struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name)	{ &(name), &(name) }

#define LIST_HEAD(name)	 struct list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

static inline void __list_add(struct list_head *new, struct list_head *prev, struct list_head *next)  
{  
	next->prev = new;  
	new->next = next;  
	new->prev = prev;  
	prev->next = new;  
}  

static inline void list_add(struct list_head *new, struct list_head *head)  
{  
	__list_add(new, head, head->next);  
}

static inline void list_add_tail(struct list_head *new, struct list_head *head)  
{  
	__list_add(new, head->prev, head);  
}  

static inline void __list_del(struct list_head * prev, struct list_head * next)  
{  
	next->prev = prev;  
	prev->next = next;  
}  
//linux源码中这里是加了条件编译防止其它头文件也声明该函数,编译时会出现重复编译的错。不过我们的其它头文件并没有定义该函数,所以就不用加了
static inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	INIT_LIST_HEAD(entry);
}

//替换元素
static inline void list_replace(struct list_head *old, struct list_head *new)
{
	new->next = old->next;
	new->next->prev = new;
	new->prev = old->prev;
	new->prev->next = new;
}	

static inline void list_replace_init(struct list_head *old, struct list_head *new)
{
	list_replace(old, new);
	INIT_LIST_HEAD(old);
}

//删除元素
static inline void list_del_init(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	INIT_LIST_HEAD(entry);
}

//从一个链表移动到另一个链表的头部
static inline void list_move(struct list_head *list, struct list_head *head)
{
	__list_del(list->prev, list->next);
	list_add(list, head);
}

//从一个链表移动到另一个链表的尾部
static inline void list_move_tail(struct list_head *list, struct list_head *head)
{
	__list_del(list->prev, list->next);
	list_add(list, head);
}

//判断是不是尾结点
static inline int list_is_last(const struct list_head *list, const struct list_head *head)
{
	return list->next == head;
}

//判断是否为空
static inline int list_empty(const struct list_head *head)
{
	return head->next == head;
}

static inline int list_empty_careful(const struct list_head *head)
{
	struct list_head *next = head->next;
	return (next == head) && (next == head->prev);
}

//测试是否链表只有一个元素
static inline int list_is_singular(const struct list_head *head)
{
	return !list_empty(head) && (head->next == head->prev);
}

//分割链表的分割操作
static inline void __list_cut_position(struct list_head *list, struct list_head *head, struct list_head *entry)
{
	struct list_head *new_first = entry->next;
	list->next = head->next;
	list->next->prev = list;
	list->prev = entry;
	entry->next = list;
	head->next = new_first;
	new_first->prev = head;
}

//根据entry来切割链表,切割下来的部分接到list中(切割的部分是head到entry(head除外))
static inline void list_cut_position(struct list_head *list, struct list_head *head, struct list_head *entry)
{
	if(list_empty(head))
		return;
	if(list_is_singular(head) && (head->next != entry && head != entry))
		return;
	if(entry == head)
		INIT_LIST_HEAD(list);
	else
		__list_cut_position(list, head, entry);
}

static inline void __list_splice(const struct list_head *list, struct list_head *prev, struct list_head *next)
{
	struct list_head *first = list->next;
	struct list_head *last = list->prev;

	first->prev = prev;
	prev->next = first;

	last->next = next;
	next->prev = last;
}

//头插法
static inline void list_splice(const struct list_head *list, struct list_head *head)
{
	if(!list_empty(list))
	{
		__list_splice(list, head, head->next);
	}
}

//尾插法
static inline void list_splice_tail(struct list_head *list, struct list_head *head)
{
	if(!list_empty(list))
	{
		__list_splice(list, head->prev, head);
	}
}

static inline void list_splice_init(struct list_head *list, struct list_head *head)
{
	if(!list_empty(list))
	{
		__list_splice(list, head, head->next);	
		INIT_LIST_HEAD(list);
	}
}

static inline void list_splice_tail_init(struct list_head *list, struct list_head *head)
{
	if(!list_empty(list))
	{
		__list_splice(list, head->prev, head);
		INIT_LIST_HEAD(list);
	}
}

#define list_entry(ptr, type, member) \
	(type *)((char *)ptr - __offsetof(type, member))

#define __offsetof(type, member) \
	((unsigned int) & ((type *)0)->member)

#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != head; pos = pos->next)

#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
			pos = n, n = pos->next)

#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

#define list_for_each_prev_safe(pos, n, head) \
	for (pos = (head)->prev, n = pos->prev;   \
			pos != (head);                    \
			pos = n, n = pos->prev)

#define list_for_each_entry(pos, head, member) \
	for (pos = list_entry((head)->next, typeof(*pos), member);  \
			&pos->member != (head);                             \
			pos = list_entry(pos->member.next, typeof(*pos), member))

#define list_for_each_entry_reverse(pos, head, member)  \
	for (pos = list_entry((head)->prev, typeof(*pos), member); \
			pos->member != (head);						       \
			pos = list_entry(pos->member.prev, typeof(*pos), member))

#define list_for_each_entry_safe(pos, n, head, member) \
    for (pos = list_entry((head)->next, typeof(*pos), member),  \
		    n = list_entry(pos->member.next, typeof(*pos), member); \
		    &pos->member != (head);                                 \
		    pos = n, n = list_entry(n->member.next, typeof(*n), member))

#endif








