
CC_PREFIX := aarch64-linux-gnu

CFLAGS := -g -Wall -nostdlib -nostdinc -I./include
ASMFLAGS := -g -I./include

SRC_DIR := src
BUILD_DIR := build

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
		$(CC_PREFIX)-ld -Map arm64os.map -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/arm64os.elf $(OBJ_FILES)
		$(CC_PREFIX)-objcopy -O binary $(BUILD_DIR)/arm64os.elf arm64os.bin

clean :
		rm -rf $(BUILD_DIR) *.bin *.map

qemu:
		qemu-system-aarch64 -M raspi4b2g -nographic -kernel arm64os.bin