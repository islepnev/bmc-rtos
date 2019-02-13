# -----------------------------------------------------------------------------
# CMake project wrapper Makefile ----------------------------------------------
# -----------------------------------------------------------------------------

SHELL := /bin/bash
RM    := rm -rf
MKDIR := mkdir -p
ifneq ($(TOOLCHAIN_PREFIX),)
    CMAKE_ARGS := -DTOOLCHAIN_PREFIX=$(TOOLCHAIN_PREFIX)
endif

all: ./build/Makefile
	@ $(MAKE) -C build

./build/Makefile:
	@  ($(MKDIR) build > /dev/null)
	@  (cd build > /dev/null 2>&1 && cmake $(CMAKE_ARGS) ..)

distclean:
	@  ($(MKDIR) build > /dev/null)
	@  (cd build > /dev/null 2>&1 && cmake $(CMAKE_ARGS) .. > /dev/null 2>&1)
	@- $(MAKE) --silent -C build clean || true
	@- $(RM) ./build/Makefile
	@- $(RM) ./build/src
	@- $(RM) ./build/test
	@- $(RM) ./build/CMake*
	@- $(RM) ./build/cmake.*
	@- $(RM) ./build/*.cmake
	@- $(RM) ./build/*.txt
	@- $(RM) ./build/*.ld

ifeq ($(findstring distclean,$(MAKECMDGOALS)),)
    $(MAKECMDGOALS): ./build/Makefile
	@ $(MAKE) -C build $(MAKECMDGOALS)
endif

flash:
	openocd -f interface/stlink-v2-1.cfg -f target/stm32f7x.cfg -c "program build/freertos-stm32f769i_discovery.elf verify reset exit"
