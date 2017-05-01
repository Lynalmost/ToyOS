/*************************************************************************
  > File Name: kernel_thread.c
  > Author: Ukey
  > Mail: gsl110809@gmail.com
  > Created Time: 2017年05月01日 星期一 14时29分35秒
 ************************************************************************/

#include "sched.h"
#include "gdt.h"
#include "kalloc.h"
#include "string.h"
#include "debug.h"

pid_t now_pid = 0;
void kthread_exit()
{
	register uint32_t val asm ("eax");

	printk("thread exited code :%d\n", val);

	while (1);
}

int32_t kernel_thread(int (*func)(void *), void *arg)
{
	//申请空间
	struct task_struct *new_task = (struct task_struct *)kmalloc(STACK_SIZE);
	assert(new_task != NULL, "kern_thread: kmalloc error");

	// 将栈低端结构信息初始化为 0 
	bzero(new_task, sizeof(struct task_struct));

	//设置task_struct结构体成员
	new_task->state = TASK_RUNNING;
	new_task->stack = current;
	new_task->pid = now_pid++;
	new_task->mm = NULL;

	//将需要的信息存入栈中
	uint32_t *stack_top = (uint32_t *)((uint32_t)new_task + STACK_SIZE);

	*(--stack_top) = (uint32_t)arg;
	*(--stack_top) = (uint32_t)kthread_exit;
	*(--stack_top) = (uint32_t)func;

	new_task->context.esp = (uint32_t)new_task + STACK_SIZE - sizeof(uint32_t) * 3;

	// 设置新任务的标志寄存器未屏蔽中
	new_task->context.eflags = 0x200;
	new_task->next = running_proc_head;

	// 找到当前进任务队列，插入到末尾
	struct task_struct *tail = running_proc_head;
	assert(tail != NULL, "you need to init sched!");

	while (tail->next != running_proc_head) {
		tail = tail->next;
	}
	tail->next = new_task;

	return new_task->pid;
}

