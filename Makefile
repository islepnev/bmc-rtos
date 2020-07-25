# -----------------------------------------------------------------------------
# CMake project wrapper Makefile ----------------------------------------------
# -----------------------------------------------------------------------------

SHELL := /bin/bash
RM    := rm -rf
MKDIR := mkdir -p
ifneq ($(TOOLCHAIN_PREFIX),)
    CMAKE_ARGS := -DTOOLCHAIN_PREFIX=$(TOOLCHAIN_PREFIX)
endif

TOPTARGETS := all clean

all: cmake
	cmake --build build

clean:
	cmake --build build --target clean

.PHONY: cmake

cmake:
	cmake $(CMAKE_ARGS) -B build

distclean:
	@  ($(RM) build/.* build/*)

#flash:
#	openocd -f interface/stlink-v2.cfg -f target/stm32f7x.cfg -c "program build/tdc72vxs4_rtos.elf verify reset exit"

format:
	@find cubemx -iname '*.h' -o -iname '*.c' -o -iname '*.s' | while read f; do \
	    dos2unix -q $$f; \
	    sed -i -e 's/[ \t]*$$//' $$f; \
	done
