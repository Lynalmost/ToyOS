/*************************************************************************
    > File Name: idt.h
    > Author: Ukey
    > Mail: gsl110809@gmail.com
    > Created Time: 2017年04月14日 星期五 15时06分51秒
 ************************************************************************/

/*
 * 中断机制想必大家都听说过,比如我们平时用键盘输入或者鼠标点击时,就发生了中断。中断分为两大类,硬件中断和软件中断。
 * 硬件中断是指通过中断控制芯片及电路完成的中断。
 * 软件中断是利用硬件中断的概念,用软件的方式去模拟中断
 * 硬中断是外部设备对CPU的中断而软中断通常是硬中断服务程序对内核的中断。
 * 当中断发生时,cpu会将当前正在执行的任务的相关信息保存下来,并根据中断的中断号找到处理程序去执行。之后再恢复原来的执行流程
 * 关于中断就介绍到这里,由于平台的不同,设计的中断号等信息其实也有不同
 * 而在x86环境下,中断控制芯片是8259A PIC。intel的中断号处理范围是0-255,也就是说有256个不同的中断。相关的更详细的资料请自行查阅。
 */
#ifndef INCLUDE_IDT_H_
#define INCLUDE_IDT_H_
#include "types.h"
/*
 * 这里引入了idt,类似于gdt的作用,也是一个描述符表,它的作用是将每一个中断向量和一个描述符对应起来。cpu根据中断号获取中断
 * 向量,即对应中断服务程序的入口地址。这样就可以把每一种中断和处理方式一一对应起来了。idt表的基地址也存在idtr寄存器中。
 * 发生一个中断的过程是这样的：
 * 首先cpu会把当前正在执行的任务的相关信息保存下来
 * 然后根据中断号获取中断向量查idt表找到目标代码段描述符选择子
 * 通过选择子查找GDT表,找到目标代码段
 * 接着通过idt对应的表项中的中断处理函数的地址来取得中断处理程序的入口
 * 调用了处理程序之后,再恢复原来被中断任务,继续正常的执行
 * 在我们条理理清了之后,需要做什么就比较明朗了。 
 */

//首先我们定义IDT结构体,成员有:中断处理程序的底16位偏移、选择子、属性等、中断处理程序的高16位偏移
typedef struct idt_entry_t
{
	uint16_t offset_low;		//低16位偏移
	uint16_t selector;			//目标代码段描述符选择子
	uint8_t  always0;			//置0位
	uint8_t  flags;				//有4位的type、1位的S、2位的DPL、1位的P
	uint16_t offset_high;		//高16位偏移
}__attribute__((packed)) idt_entry_t;

//声明idtr寄存器的内容
typedef struct idt_ptr_t
{
	uint16_t limit;				//低16位限长
	uint32_t base;				//高32位基地址
}__attribute__((packed)) idt_ptr_t;

//cpu在处理中断的时候需要保护当前任务的信息，需要保存：
//1.保存中断号,以及错误号(如果有的话) 
//2.通用寄存器 
//3.中断前数据段选择子(硬件会自动为我们保存代码段选择子(CS寄存器))


//这个结构体的成员是处理中断时需要保存的现场信息
typedef struct pt_regs_t
{
	uint32_t ds;		//数据段选择子
	//通用寄存器由pusha指令一次压入
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
	uint32_t int_num;	//中断号
	uint32_t err_code;	//错误号
	//以下由硬件压入
	uint32_t cs;
	uint32_t eflags;
	uint32_t user_esp;
	uint32_t ss;
	uint32_t eip;
}pt_regs_t;

//初始化idt
void init_idt();

//irq处理函数
void irq_handler(pt_regs_t *regs);

// 声明加载 IDTR 的函数
extern void idt_flush(uint32_t);

//定义中断处理函数指针
typedef void (*interrupt_handler_t)(pt_regs_t *);

//注册中断处理服务函数
void register_interrupt_handler(uint8_t n, interrupt_handler_t h);

//调用中断处理函数
void isr_handler(pt_regs_t *regs);

void isr0();        // 0 #DE 除 0 异常 
void isr1();        // 1 #DB 调试异常 
void isr2();        // 2 NMI 
void isr3();        // 3 BP 断点异常 
void isr4();        // 4 #OF 溢出 
void isr5();        // 5 #BR 对数组的引用超出边界 
void isr6();        // 6 #UD 无效或未定义的操作码 
void isr7();        // 7 #NM 设备不可用(无数学协处理器) 
void isr8();        // 8 #DF 双重故障(有错误代码) 
void isr9();        // 9 协处理器跨段操作 
void isr10();       // 10 #TS 无效TSS(有错误代码) 
void isr11();       // 11 #NP 段不存在(有错误代码) 
void isr12();       // 12 #SS 栈错误(有错误代码) 
void isr13();       // 13 #GP 常规保护(有错误代码) 
void isr14();       // 14 #PF 页故障(有错误代码) 
void isr15();       // 15 CPU 保留 
void isr16();       // 16 #MF 浮点处理单元错误 
void isr17();       // 17 #AC 对齐检查 
void isr18();       // 18 #MC 机器检查 
void isr19();		// 19 #XM SIMD浮点异常

//20-31 Intel保留
void isr20();
void isr21();
void isr22();
void isr23();
void isr24();
void isr25();
void isr26();
void isr27();
void isr28();
void isr29();
void isr30();
void isr31();

//32-255 用户自定义异常
void isr128();

//定义IRQ
#define  IRQ0     32    // 电脑系统计时器
#define  IRQ1     33    // 键盘
#define  IRQ2     34    // 与 IRQ9 相接，MPU-401 MD 使用
#define  IRQ3     35    // 串口设备
#define  IRQ4     36    // 串口设备
#define  IRQ5     37    // 建议声卡使用
#define  IRQ6     38    // 软驱传输控制使用
#define  IRQ7     39    // 打印机传输控制使用
#define  IRQ8     40    // 即时时钟
#define  IRQ9     41    // 与 IRQ2 相接，可设定给其他硬件
#define  IRQ10    42    // 建议网卡使用
#define  IRQ11    43    // 建议 AGP 显卡使用
#define  IRQ12    44    // 接 PS/2 鼠标，也可设定给其他硬件
#define  IRQ13    45    // 协处理器使用
#define  IRQ14    46    // IDE0 传输控制使用
#define  IRQ15    47    // IDE1 传输控制使用

//声明相应的IRQ函数
void irq0();        // 电脑系统计时器
void irq1();        // 键盘
void irq2();        // 与 IRQ9 相接，MPU-401 MD 使用
void irq3();        // 串口设备
void irq4();        // 串口设备
void irq5();        // 建议声卡使用
void irq6();        // 软驱传输控制使用
void irq7();        // 打印机传输控制使用
void irq8();        // 即时时钟
void irq9();        // 与 IRQ2 相接，可设定给其他硬件
void irq10();       // 建议网卡使用
void irq11();       // 建议 AGP 显卡使用
void irq12();       // 接 PS/2 鼠标，也可设定给其他硬件
void irq13();       // 协处理器使用
void irq14();       // IDE0 传输控制使用
void irq15();       // IDE1 传输控制使用
#endif



