;我们将使用grub作为引导,而不是自己去写引导程序
;实现一个简单的引导程序并不难,但是由于后面还要进行修改,并且一些历史遗留下来的问题很容易把我们搞晕,建议可以跳过这个坑,有兴趣可以自己实现。(我的情况是看了《Orange's一个操作系统的实现》前面的引导程序就有点热情削减了,特别是看到了大量汇编时。。。)
;首先简单介绍下grub是什么。
;GNU GRUB（GRand Unified Bootloader简称“GRUB”）是一个来自GNU项目的多操作系统启动程序。简单的说,它是一个多重操作系统启动管理器,用来引导不同系统。
;引导过程:
;1.硬盘启动时,BIOS会初始化计算机,计算机开始读磁盘,磁盘读取一个扇区(512字节)
;2.当读到某个磁盘0磁道1扇区的最后的结束地址是0xaa55,bios就会认为它是一个引导扇区
;3.512字节的内容装载到内存地址0000:7c00处，然后跳转到0000:7c00处将控制权交给这段引导代码，这样BIOS的任务就结束了，操作系统也成功被引导起来了。
;这是基本的引导过程,grub则是作为基本引导程序装载的第二引导装载程序来装载操作系统
;(我觉得了解这些就够了,再根据grub的multiboot规范来完成我们的入口部分boot.s
;可以在网上查找GRUB的Multiboot标准的资料,有助于理解下面的代码

;multiboot_header必须包含4字节对齐的3个域(其它域不必须,所以不作讨论):
;下面三个域的类型是32位的无符号值, u32

;魔数域(magic)(偏移量:0):必须等于0x1BADB002
;标志域(flag)(偏移量:4):是否需要引导程序支持某些特性
;检验域(checksum)(偏移量:8):校验等式是否成立(magic+flags+checksum=0)


;multiboot魔数域
MBOOT_HEADER_MAGIC		EQU		0x1BADB002	

;表示所有引导模块将按4KB边界对齐,1 << 0意思是将第0位置1(true)
MBOOT_PAGE_ALIGN		EQU		1 << 0
;表示grub会把内存空间的信息存在multiboot结构中
MBOOT_MEM_INFO			EQU		1 << 1

;使用我们设置的上两个标记
MBOOT_HEADER_FLAGS		EQU		MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO

;checksum和magic和flags相加时,要求magic + flags + checksum = 0
MBOOT_CHECKSUM			EQU		- (MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

[BITS 32]					;所有代码以32-bit的方式编译
section .text				;声明代码段的开始位置

;在代码段的起始位置设置符合multiboot规范的标记

dd MBOOT_HEADER_MAGIC		;GRUB会通过这个魔数判断该映像是否支持
dd MBOOT_HEADER_FLAGS		;GRUB的一些加载时选项
dd MBOOT_CHECKSUM			;检测数值

[GLOBAL start]				;向外部声明内核代码入口
[GLOBAL glb_mboot_ptr]		;向外部声明struct multiboot * 变量
[EXTERN kern_entry]			;声明内核C代码的入口函数

start:
	cli						;关闭中断(因为目前还没设置好保护模式的中断处理)
	mov esp, STACK_TOP		;设置内核栈地址
	mov ebp, 0				;栈指针修改为0
	and esp, 0FFFFFFF0H		;栈地址按照16字节对齐
	mov [glb_mboot_ptr], ebx;将ebx中存储的指针存入全局变量
	call kern_entry			;调用内核入口函数

stop:
	hlt
	jmp stop
	
section .bss				;未初始化的数据段从这里开始
stack:
    resb 32768				;这里作为内核栈
glb_mboot_ptr:          ;全局的 multiboot 结构体指针
    resb 4

STACK_TOP equ $-stack-1 ;内核栈顶，$ 符指代是当前地址








