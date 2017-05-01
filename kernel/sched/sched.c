/*************************************************************************
    > File Name: ../kernel/sched/sched.c
    > Author: Ukey
    > Mail: gsl110809@gmail.com
    > Created Time: 2017年05月01日 星期一 13时38分33秒
 ************************************************************************/

#include "debug.h"
#include "sched.h"

extern uint32_t kern_stack_top;
void sched_init()
{
	current = (struct task_struct *)(kern_stack_top - STACK_SIZE);

	current->state = TASK_RUNNING;
	current->pid = now_pid++;
	current->stack = current;
	current->mm = NULL;
	current->next = current;
	
	running_proc_head = current;
}


void schedule()
{
	if(current)
	{
		if(current != current->next)
		{
			struct task_struct *temp = current;
			current = current->next;
			switch_to(&(temp->context), &(current->context));
		}
	}
}
	
