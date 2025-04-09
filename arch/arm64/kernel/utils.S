.globl put32
put32:
	str w1,[x0]
	ret

.globl get32
get32:
	ldr w0,[x0]
	ret

.globl delay
delay:
	subs x0, x0, #1
	bne delay
	ret

.global mmu_at_test
mmu_at_test:
    at s1e1r, x0
    mrs x0, par_el1
    ret