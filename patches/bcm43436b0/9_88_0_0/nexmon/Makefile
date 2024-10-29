SHELL = /bin/bash

# Use a more robust method for getting the Git version
GIT_VERSION := $(shell git describe --tags --always --dirty="-modified" 2>/dev/null || echo "unknown")

include ../version.mk
include $(FW_PATH)/definitions.mk

LOCAL_SRCS := $(wildcard src/*.c) src/ucode_compressed.c src/templateram.c
COMMON_SRCS := $(wildcard $(NEXMON_ROOT)/patches/common/*.c)
FW_SRCS := $(wildcard $(FW_PATH)/*.c)

OBJS := $(addprefix obj/,$(notdir $(LOCAL_SRCS:.c=.o)) $(notdir $(COMMON_SRCS:.c=.o)) $(notdir $(FW_SRCS:.c=.o)))

# Use more specific compiler flags for better optimization and debugging
CFLAGS := -fplugin=$(CCPLUGIN) \
	-fplugin-arg-nexmon-objfile=$@ \
	-fplugin-arg-nexmon-prefile=gen/nexmon.pre \
	-fplugin-arg-nexmon-chipver=$(NEXMON_CHIP_NUM) \
	-fplugin-arg-nexmon-fwver=$(NEXMON_FW_VERSION_NUM) \
	$(DEFINES) \
	-Wall -Wextra -Werror -Os -nostdlib -nostartfiles -ffreestanding -mthumb -march=$(NEXMON_ARCH) \
	-ffunction-sections -fdata-sections \
	-I$(NEXMON_ROOT)/patches/include -Iinclude -I$(FW_PATH)

# Add debug flags if DEBUG is defined
ifdef DEBUG
CFLAGS += -g -O0
endif

all: $(RAM_FILE)

init:
	@mkdir -p obj gen log
	@touch BUILD_NUMBER # Don't increment automatically, manage separately
	@make -s -f $(NEXMON_ROOT)/patches/common/header.mk

# Simplified and more generic compile rules
obj/%.o: %.c
	@printf "\033[0;31m  COMPILING\033[0m $< => $@ (details: log/compiler.log)\n"
	@$(CC)gcc $(CFLAGS) -c $< -o $@ 2>>log/compiler.log

# ... (rest of the Makefile)

# Improved install-firmware target with error checking
install-firmware: $(RAM_FILE)
	@printf "\033[0;31m  REMOUNTING /vendor\033[0m\n"
	@adb shell 'su -c "mount -o rw,remount /vendor"' || (echo "Remounting /vendor failed!"; exit 1)
	@printf "\033[0;31m  COPYING TO PHONE\033[0m $< => /sdcard/$<\n"
	@adb push $< /sdcard/ || (echo "adb push failed!"; exit 1)
	@printf "\033[0;31m  COPYING\033[0m /sdcard/$(RAM_FILE) => /vendor/firmware/$(RAM_FILE)\n"
	@adb shell 'su -c "cp /sdcard/$(RAM_FILE) /vendor/firmware/$(RAM_FILE)"' || (echo "Copying firmware on device failed!"; exit 1)
	@printf "\033[0;31m  RELOADING FIRMWARE\033[0m\n"
	@adb shell 'su -c "svc wifi disable && svc wifi enable"' || (echo "Reloading firmware failed!"; exit 1) # More reliable way to restart Wi-Fi

clean-firmware:
	@printf "\033[0;31m  CLEANING\033[0m\n"
	$(RM) $(RAM_FILE) obj gen log src/ucode_compressed.c src/templateram.c ucode_compressed.bin

clean: clean-firmware
	$(RM) BUILD_NUMBER

.PHONY: all init clean clean-firmware FORCE install-firmware backup-firmware check-nexmon-setup-env
