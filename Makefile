
## base variables
HOME_ROOT_DIR ?= ./
board ?= rpi4b

## Toolchain
CC_PREFIX := aarch64-linux-gnu

CFLAGS += -g -Wall -nostdlib -nostdinc
CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS	+= --gc-sections

OPTIMIZE ?= -O0

DEFINE += -U__unix__ -U__linux__
ifeq ($(board), rpi5)
DEFINE += -DRPI5_BCM2712
endif

ARCH += -DAARCH=64 -mcpu=cortex-a76 -mlittle-endian
INCLUDE +=  -I $(HOME_ROOT_DIR)/include

ASMFLAGS += $(ARCH) $(DEFINE) $(INCLUDE) $(OPTIMIZE)
CFLAGS += $(ARCH) $(DEFINE) $(INCLUDE) $(OPTIMIZE)

LDFLAGS	+= --section-start=.init=$(LOADADDR)

SRC_DIR := src
BUILD_DIR := build

LOADADDR = 0x80000

all : arm64os.bin

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
		mkdir -p $(@D)
		$(CC_PREFIX)-gcc $(CFLAGS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
		mkdir -p $(@D)
		$(CC_PREFIX)-gcc $(ASMFLAGS) -MMD -c $< -o $@


#OBJ_FILES := $(wildcard $(BUILD_DIR)/*.o)
C_FILES := $(wildcard $(SRC_DIR)/*.c)
ASM_FILES := $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES := $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

DEP_FILES := %(OBJ_FILES:%.o:%.d)
-include $(DEP_FILES)

arm64os.bin: $(SRC_DIR)/linker.ld $(OBJ_FILES)
		$(CC_PREFIX)-ld -Map arm64os.map $(LDFLAGS) -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/arm64os.elf $(OBJ_FILES) --start-group $(LIBS) $(EXTRALIBS) --end-group
		$(CC_PREFIX)-objcopy -O binary $(BUILD_DIR)/arm64os.elf arm64os.bin

clean :
		rm -rf $(BUILD_DIR) *.bin *.map

qemu:
		./qemu-system-aarch64 -M raspi4b2g -nographic -kernel arm64os.bin