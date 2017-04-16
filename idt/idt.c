#include "io.h"
#include "string.h"
#include "debug.h"
#include "idt.h"

#define INTERRUPT_MAX 256
// 中断描述符表
static idt_entry_t idt_entries[INTERRUPT_MAX];

// IDTR
static idt_ptr_t idt_ptr;

// 中断处理函数的指针数组
static interrupt_handler_t interrupt_handlers[INTERRUPT_MAX];

//处理函数的指针类型
typedef void (*deal_func_t)();
static deal_func_t deal_func[INTERRUPT_MAX] = {&isr0 , &isr1 , &isr2 , &isr3 , &isr4 , &isr5 , 
												 &isr6 , &isr7 , &isr8 , &isr9 , &isr10, &isr11,
												 &isr12, &isr13, &isr14, &isr15, &isr16, &isr17,
												 &isr19, &isr20, &isr21, &isr22, &isr23, &isr24,
												 &isr25, &isr26, &isr27, &isr28, &isr29, &isr30,
												 &isr31, &irq0 , &irq1 , &irq2 , &irq3 , &irq4 ,
												 &irq5 , &irq6 , &irq7 , &irq8 , &irq9 , &irq10, 
												 &irq11, &irq12, &irq13, &irq14, &irq15};

// 设置中断描述符的函数
static void idt_set_gate(uint8_t num, uint32_t offset, uint16_t selector, uint8_t flags);

//初始化设置8259A芯片
static void init_chip();

// 初始化中断描述符表
void init_idt()
{	
	//初始化设置8259A芯片
	init_chip();

	bzero((uint8_t *)&interrupt_handlers, sizeof(interrupt_handler_t) * INTERRUPT_MAX);
	
	idt_ptr.limit = sizeof(idt_entry_t) * INTERRUPT_MAX - 1;
	idt_ptr.base  = (uint32_t)&idt_entries;
	
	bzero((uint8_t *)&idt_entries, sizeof(idt_entry_t) * INTERRUPT_MAX);

	// isr0-32:  用于 CPU 的中断处理 irq0-15: 中断请求
	uint32_t i;
	for(i = 0; i < 48; i++)
	{
		idt_set_gate(i, (uint32_t)deal_func[i], 0x08, 0x8E);
	}
	// 128 将来用于实现系统调用
	idt_set_gate(128, (uint32_t)isr128, 0x08, 0x8E);

	// 更新设置中断描述符表
	idt_flush((uint32_t)&idt_ptr);
}

// 设置中断描述符
static void idt_set_gate(uint8_t num, uint32_t offset, uint16_t selector, uint8_t flags)
{
	idt_entries[num].offset_low = offset & 0xFFFF;
	idt_entries[num].offset_high = (offset >> 16) & 0xFFFF;

	idt_entries[num].selector = selector;
	idt_entries[num].always0 = 0;

	// 先留下 0x60 这个魔数，以后实现用户态时候
	// 这个与运算可以设置中断门的特权级别为 3
	idt_entries[num].flags = flags;  // | 0x60
}

// 中断处理函数
void isr_handler(pt_regs_t *regs)
{
	if (interrupt_handlers[regs->int_num]) 
	{
	      interrupt_handlers[regs->int_num](regs);
	}
	else 
	{
		printk_color(rc_black, rc_blue, "Unhandled interrupt: %d\n", regs->int_num);
	}
}

// IRQ处理函数
void irq_handler(pt_regs_t *regs)
{
	//如果中断号大于40,则应该由从片处理
	if(regs->int_num >= 40)
	{
		outb(0xA0, 0x20);
	}
	outb(0x20, 0x20);
	if(interrupt_handlers[regs->int_num])
	{
		interrupt_handlers[regs->int_num](regs);
	}
}
// 注册一个中断处理函数
void register_interrupt_handler(uint8_t n, interrupt_handler_t h)
{
	interrupt_handlers[n] = h;
	printk_color(rc_black, rc_red, "test1\n");
}

static void init_chip()
{
	//重新映射IRQ表
	//芯片分为主片和从片
	//主片端口0x20 0x21
	//从片端口0xA0 0xA1
	//
	//初始化主片、从片
	outb(0x20, 0x11);
	outb(0xA0, 0x11);

	//设置主片从32号中断开始
	outb(0x21, 0x20);

	//设置从片从40号中断开始
	outb(0xA1, 0x28);

	outb(0x21, 0x04);

	outb(0xA1, 0x02);

	outb(0x21, 0x01);
	outb(0xA1, 0x01);

	outb(0x21, 0x0);
	outb(0xA1, 0x0);
}



