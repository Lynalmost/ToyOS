/*************************************************************************
    > File Name: sched.h
    > Author: Ukey
    > Mail: gsl110809@gmail.com
    > Created Time: 2017年05月01日 星期一 12时57分41秒
 ************************************************************************/

#ifndef INCLUDE_SCHED_H_
#define INCLUDE_SCHED_H_

#include "types.h"
#include "pmm.h"
#include "vmm.h"



//进程的五大状态
#define TASK_RUNNING			0
#define TASK_INTERRUPTIBLE		1
#define TASK_UNITERRUPTIBLE		2
#define TASK_ZOMBIE				3
#define TASK_STOPPED			4

//进程的内存地址的结构
struct mm_struct
{
	pgd_t *pgd_dir;	//进程页表
};
//需要保存的上下文的寄存器信息 linux的实现中称为tss(taks state segment)
struct context
{
	uint32_t esp;
	uint32_t ebp;
	uint32_t ebx;
	uint32_t esi;
	uint32_t edi;
	uint32_t eflags;
};

struct task_struct
{
	uint32_t state;
	pid_t	 pid;
	void *stack;
	struct mm_struct *mm;
	struct context context;	//作用类似于tss
	struct task_struct *next;
};

//可调度的进程的链表
struct task_struct *running_proc_head;

//等待的进程链表(缺少某种资源(非cpu))
struct task_struct *wait_proc_head;	

//当前正在运行的任务
struct task_struct *current;


extern pid_t now_pid;

void sched_init();

void schedule();

void switch_to(struct context *prev, struct context *next);

int32_t kernel_thread(int (*func)(void *), void *arg);


#endif
