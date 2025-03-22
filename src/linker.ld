SECTIONS
{
        . = 0x80000;
	.text : {
                _text_boot_start = .;
		KEEP(*(.text.boot))
                _text_boot_end = .;

                _text_start = .;
		*(.text .text.* .gnu.linkonce.t*)
                _text_end = .;
	}
        
        _rodata_start = .;
    	.rodata : { 
		*(.rodata) 
	}
        _rodata_end = .;

        _data_start = .;
    	.data : { 
		*(.data) 
	}
		. = ALIGN(4096);
    	idmap_pgd_dir = .;
    	. += 4096;
        _data_end = .;
    	
    	.bss : { 
		. = ALIGN(0x16);
    		bss_begin = .;
		*(.bss*)
		*(COMMON*)
    		bss_end = .;
	}

        /* page table */
        . = ALIGN(4096);
        //init_pg_dir = .;
        . += 4096;
}
