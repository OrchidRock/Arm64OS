#include "uapi/syscall.h"

.align 3
.global syscall
syscall:
	mov     w8, w0
	mov     x0, x1
	mov     x1, x2
	mov     x2, x3
	mov     x3, x4
	mov     x4, x5
	mov     x5, x6
	mov     x6, x7
	svc     0x0
	ret


.global __clone
__clone:
    mov     x10, x0
    mov     x11, x2
    mov     x12, x3
