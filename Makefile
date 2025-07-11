# ESP-IDF installation path
IDF_PATH ?= ./esp-idf
SHELL := /bin/bash

# Script to export the ESP-IDF environment
EXPORT_SCRIPT := $(IDF_PATH)/export.sh

# Serial port used for flashing/monitoring (change to match your setup)
PORT ?= /dev/tty.usbmodem141401

# ESP target (esp32, esp32s3, esp32c3, ...)
TARGET ?= esp32s3

# ELF file generated by the build process
ELF_FILE := build/$(notdir $(CURDIR)).elf

# Command wrapper to run ESP-IDF commands with environment sourced
define IDF_CMD
. $(EXPORT_SCRIPT) && idf.py -B build -DIDF_TARGET=$(TARGET) $(1)
endef

.PHONY: build flash monitor menuconfig clean fullclean debug build-emulator test

help: # Show this helper
	@grep -E '^[a-zA-Z0-9 -]+:.*#'  Makefile | sort | while read -r l; do printf "\033[1;32m$$(echo $$l | cut -f 1 -d':')\033[00m:$$(echo $$l | cut -f 2- -d'#')\n"; done

prepare: # Prepare ESP-IDF environment: install required tools and Python packages
	@echo "🔧 Installing ESP-IDF tools and Python requirements..."
	@cd $(IDF_PATH) && ./install.sh $(TARGET)

build: # Compile the firmware
	@echo "🔧 Building firmware..."
	@$(call IDF_CMD,build)


flash: # Flash firmware to the board
	@echo "⚡ Flashing firmware to device on $(PORT)..."
	@$(call IDF_CMD,flash -p $(PORT))


monitor: # Open serial monitor
	@echo "📟 Opening serial monitor on $(PORT)..."
	@$(call IDF_CMD,monitor -p $(PORT))


menuconfig: # Launch menuconfig UI
	@echo "🛠️ Launching menuconfig..."
	@$(call IDF_CMD,menuconfig)


clean: # Clean the build directory
	@echo "🧹 Cleaning build artifacts..."
	@$(call IDF_CMD,clean)
	@rm -rf build*


fullclean: # Remove entire build folder including CMake cache
	@echo "🔥 Performing full clean..."
	@$(call IDF_CMD,fullclean)
	@rm -rf build*


build-emulator: # Build the emulator version of the firmware (native Unix build)
	@echo "📦 Building emulator in ./build-emulator..."
	@mkdir -p build-emulator
	@cd build-emulator && cmake ../emulator
	@cd build-emulator && make

test: # Build and run tests (native Unix build)
	@echo "🧪 Building tests in ./build-test..."
	@mkdir -p build-test
	@cd build-test && cmake ../test
	@cd build-test && make
	@cd build-test && LSAN_OPTIONS=detect_leaks=0 ctest --output-on-failure