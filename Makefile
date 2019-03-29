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

flash:
	openocd -f interface/stlink-v2.cfg -f target/stm32f7x.cfg -c "program build/ttvxs_rtos.elf verify reset exit"

SRC_DIRS := app common drivers FreeRTOS Inc Src STM32Cube cubemx

import-cubemx:
	rm -f Inc/*.h Src/*.c
	cp -fp cubemx/Inc/*.h Inc/
	cp -fp cubemx/Src/*.c Src/
	cp -fp cubemx/Src/system_stm32f7xx.c STM32Cube/Drivers/CMSIS/Device/ST/STM32F7xx/Source/Templates/
	rm -f  Src/system_stm32f7xx.c Src/syscalls.c
	sed -i -e '/RCC_OscInitStruct.PLL.PLLQ/a\' -e '  RCC_OscInitStruct.PLL.PLLR = 2;' Src/main.c
	rm -rf FreeRTOS
	cp -rp cubemx/Middlewares/Third_Party/FreeRTOS ./

export-cubemx:
	cp -fp Inc/*.h cubemx/Inc/
	cp -fp Src/*.c cubemx/Src/

format:
	@find $(SRC_DIRS) -iname '*.h' -o -iname '*.c' -o -iname '*.s' | while read f; do \
	    dos2unix -q $$f; \
	    sed -i -e 's/[ \t]*$$//' $$f; \
	done

#ifeq ($(findstring distclean,$(MAKECMDGOALS)),)
#    $(MAKECMDGOALS): ./build/Makefile
#	@ $(MAKE) -C build $(MAKECMDGOALS)
#endif
