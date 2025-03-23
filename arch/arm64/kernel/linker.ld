
PAGE_SIZE = 4096;

SECTIONS
{
	. = 0x80000;
	.text : {
        _text_boot_start = .;
		KEEP(*(.text.boot))
        _text_boot_end = .;

        _text_start = .;
		*(.text .text.* .gnu.linkonce.t*)

		. = ALIGN(PAGE_SIZE);
		readonly_page = .;
		. += PAGE_SIZE;

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
		. = ALIGN(PAGE_SIZE);
        *(.data.init_task)
	}
	. = ALIGN(PAGE_SIZE);
	idmap_pgd_dir = .;
	. += PAGE_SIZE;
	_data_end = .;

	.bss : {
		. = ALIGN(0x16);
    	bss_begin = .;
		*(.bss*)
		*(COMMON*)
    	bss_end = .;
	}

	/* page table */
	. = ALIGN(PAGE_SIZE);
	//init_pg_dir = .;
	. += PAGE_SIZE;
}
