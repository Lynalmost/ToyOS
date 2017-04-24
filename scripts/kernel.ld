ENTRY(start)
SECTIONS
{
		PROVIDE( kern_start = 0xC0100000 );
		. = 0x100000;	/*起始的链接地址*/
		.init.text : 
		{
			*(.init.text)	/* *是通配符,意思是把所有的被链接的object文件中的.init.text段都链接进.init.text段 */
			. = ALIGN(4096); /* ALIGN是声明对齐的大小,按4KB对齐 */
		}
		.init.data :
		{
			*(.init.data)
			. = ALIGN(4096);
		}
		
		. += 0xC0000000;	/* 起始的链接地址,下面是我们要映射到虚拟空间3G以上的,所以需要加0xC0000000 */
		.text : AT(ADDR(.text) - 0xC0000000) /* AT指定该section的加载地址(LMA),LMA是区段所载入内存的实际地址 所以需要减去0xC0000000*/
		{
				*(.text)
				. = ALIGN(4096);
		}
		.data : AT(ADDR(.data) - 0xC0000000)
		{
				*(.data)
				*(.rodata)
				. = ALIGN(4096);
		}
		.bss : AT(ADDR(.bss) - 0xC0000000)
		{
				*(.bss)
				. = ALIGN(4096);
		}
		.stab : AT(ADDR(.stab) - 0xC0000000)
		{
				*(.stab)
				. = ALIGN(4096);
		}
		.stabstr : AT(ADDR(.stabstr) - 0xC0000000)
		{
				*(.stabstr)
				. = ALIGN(4096);
		}

		PROVIDE( kern_end = . ); 
		
		/DISCARD/ : { *(.comment) *(.eh_frame) }																		
}



