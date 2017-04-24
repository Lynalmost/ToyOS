#ifndef INCLUDE_MULTIBOOT_H_
#define INCLUDE_MULTIBOOT_H_

#include "types.h"

//进入到操作系统后,EBX寄存器包含了multiboot信息数据结构的物理地址,通过它就可以传递信息。
//这些都是由载入器载入的,操作系统可以使用或者忽略这些信息
//所以看到这个结构体那么复杂,其实不用心虚,根据flags的不同的设置,用到的结构体成员也不同,所以这些成员是不会全部用到的
//如果有兴趣可以网上查阅一下multiboot规范方面的资料


typedef struct multiboot_t 
{
	uint32_t flags;			//它的作用是显示在multiboot信息结构体中其它域存在及有效性。

	
	 
	//设置了flags[0]会使用mem_lower和mem_upper,作用是获取可用内存
	//mem_lower和mem_upper分别指出了低端和高端内存的大小，单位是K。
	uint32_t mem_lower;
	uint32_t mem_upper;

	uint32_t boot_device;		//设置了flags[1]时 指出引导程序从哪个BIOS磁盘设备载入的OS映像
	
	
	uint32_t cmdline;			// 内核命令行
	uint32_t mods_count;		// boot 模块列表
	uint32_t mods_addr;

	//这些域表示何处来自ELF内核的段头表,每个项的个数,大小,及用名字作为索引的字符串表
	uint32_t num;
	uint32_t size;
	uint32_t addr;
	uint32_t shndx;

	
	//以下两项指出保存由BIOS提供的内存分布的缓冲区的地址和长度
	//mmap_addr是缓冲区的地址，mmap_length是缓冲区的总大小
	//缓冲区由一个或者多个下面的大小/结构对 mmap_entry_t（size实际上是用来跳过下一个对的）组成
	uint32_t mmap_length;		
	uint32_t mmap_addr;
	
	uint32_t drives_length; 	// 指出第一个驱动器结构的大小
	uint32_t drives_addr; 		// 指出第一个驱动器这个结构的地址
	uint32_t config_table; 		// ROM 配置表
	uint32_t boot_loader_name; 	// boot loader 的名字
	uint32_t apm_table; 	    
	uint32_t vbe_control_info;
	uint32_t vbe_mode_info;
	uint32_t vbe_mode;
	uint32_t vbe_interface_seg;
	uint32_t vbe_interface_off;
	uint32_t vbe_interface_len;
}__attribute__((packed)) multiboot_t; //__attribute__((packed))作用是修改了编译器对结构体成员的布局,尽可能压缩存储空间

/**
 * size是相关结构的大小，单位是字节，它可能大于最小值20
 * base_addr_low是启动地址的低32位，base_addr_high是高32位，启动地址总共有64位
 * length_low是内存区域大小的低32位，length_high是内存区域大小的高32位，总共是64位
 * type是相应地址区间的类型，1代表可用RAM，所有其它的值代表保留区域
 */
typedef struct mmap_entry_t 
{
	uint32_t size; 		
	uint32_t base_addr_low;	
	uint32_t base_addr_high;
	uint32_t length_low;
	uint32_t length_high;
	uint32_t type;
} __attribute__((packed)) mmap_entry_t;

// 临时的multiboot_t * 指针
extern multiboot_t *mboot_ptr_tmp;
// 声明全局的 multiboot_t * 指针
extern multiboot_t *glb_mboot_ptr;

#endif 	
