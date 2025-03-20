SECTIONS
{
        . = 0x80000;
	.text : {
		KEEP(*(.text.boot))
		*(.text .text.* .gnu.linkonce.t*)
	}

    	.rodata : { 
		*(.rodata) 
	}

    	.data : { 
		*(.data) 
	}
    	
    	.bss : { 
		. = ALIGN(0x16);
    		bss_begin = .;
		*(.bss*)
		*(COMMON*)
    		bss_end = .;
	}
}
