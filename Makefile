
## base variables
HOME_ROOT_DIR ?= ./
board ?= rpi4b
enable_gic ?= 1

ARCH_DIR ?= arch/arm64

## Toolchain
CC_PREFIX := aarch64-linux-gnu


## set option 'mgeneral-regs-only' to suggent vargs.
CFLAGS += -g -Wall -mgeneral-regs-only  -nostdlib #-nostdinc
CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS	+= --gc-sections

OPTIMIZE ?= -O0

DEFINE += -U__unix__ -U__linux__
ifeq ($(board), rpi5)
DEFINE += -DRPI5_BCM2712
endif

ifeq ($(enable_gic), 1)
DEFINE += -DENABLE_GIC_V2
endif

ARCH += -DAARCH=64 -mcpu=cortex-a76 -mlittle-endian
INCLUDE +=  -I $(HOME_ROOT_DIR)/include -I $(HOME_ROOT_DIR)/$(ARCH_DIR)/include -I $(HOME_ROOT_DIR)/$(ARCH_DIR)/mach-rpi

ASMFLAGS += -g $(ARCH) $(DEFINE) $(INCLUDE) $(OPTIMIZE)
CFLAGS += $(ARCH) $(DEFINE) $(INCLUDE) $(OPTIMIZE)

LDFLAGS	+= --section-start=.init=$(LOADADDR) -p --no-undefined -X

#SRC_DIR := src
#BUILD_DIR := build

LOADADDR = 0x80000

all : arm64os.bin

%_c.o: %.c
		mkdir -p $(@D)
		$(CC_PREFIX)-gcc $(CFLAGS) -MMD -c $< -o $@

%_s.o: %.S
		mkdir -p $(@D)
		$(CC_PREFIX)-gcc $(ASMFLAGS) -MMD -c $< -o $@


#OBJ_FILES := $(wildcard $(BUILD_DIR)/*.o)
C_FILES := $(wildcard $(HOME_ROOT_DIR)/init/*.c)
ASM_FILES := $(wildcard $(HOME_ROOT_DIR)/init/*.S)
C_FILES += $(wildcard $(HOME_ROOT_DIR)/kernel/*.c)
ASM_FILES += $(wildcard $(HOME_ROOT_DIR)/kernel/*.S)
C_FILES += $(wildcard $(HOME_ROOT_DIR)/lib/*.c)
ASM_FILES += $(wildcard $(HOME_ROOT_DIR)/lib/*.S)
C_FILES += $(wildcard $(HOME_ROOT_DIR)/mm/*.c)
ASM_FILES += $(wildcard $(HOME_ROOT_DIR)/mm/*.S)

C_FILES += $(wildcard $(HOME_ROOT_DIR)/$(ARCH_DIR)/kernel/*.c)
ASM_FILES += $(wildcard $(HOME_ROOT_DIR)/$(ARCH_DIR)/kernel/*.S)
C_FILES += $(wildcard $(HOME_ROOT_DIR)/$(ARCH_DIR)/mach-rpi/*.c)
ASM_FILES += $(wildcard $(HOME_ROOT_DIR)/$(ARCH_DIR)/mach-rpi/*.S)

OBJ_FILES := $(C_FILES:%.c=%_c.o)
OBJ_FILES += $(ASM_FILES:%.S=%_s.o)

DEP_FILES := $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

arm64os.bin: arch/arm64/kernel/linker.ld $(OBJ_FILES)
		$(CC_PREFIX)-ld -Map arm64os.map $(LDFLAGS) -T $(HOME_ROOT_DIR)/$(ARCH_DIR)/kernel/linker.ld -o arm64os.elf $(OBJ_FILES) --start-group $(LIBS) $(EXTRALIBS) --end-group
		$(CC_PREFIX)-objcopy -O binary arm64os.elf arm64os.bin

clean :
		rm -rf $(OBJ_FILES) $(DEP_FILES) *.bin *.map

qemu:
		./qemu-system-aarch64 -M raspi4b2g -nographic -kernel arm64os.bin

