/*
**    Copyright 2021 Ilja Slepnev
**
**    This program is free software: you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation, either version 3 of the License, or
**    (at your option) any later version.
**
**    This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdbool.h>
#include <stdint.h>

#include "debug_helpers.h"
#include "stm32f769i_discovery_sdram.h"

#define BUFFER_SIZE            ((uint32_t)0x0100)

// Buffer location should aligned to cache line size (32 bytes)
static ALIGN_32BYTES (uint32_t wbuf[BUFFER_SIZE]);
static ALIGN_32BYTES (uint32_t rbuf[BUFFER_SIZE]);

#define SDRAM_START_ADDR ((uint32_t)0xC0000000)
#define SDRAM_END_ADDR   ((uint32_t)0xC07FFFFF)
//#define SDRAM_END_ADDR   ((uint32_t)0xDFFFFFFF)
#define SDRAM_SIZE   ((uint32_t)(SDRAM_END_ADDR-SDRAM_START_ADDR+1))

#define log_line(str, ...) debug_printf(str "\n", ##__VA_ARGS__)

static void Fill_Buffer(uint32_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset)
{
    uint32_t tmpIndex = 0;
    // Put in global buffer different values
    for (tmpIndex = 0; tmpIndex < uwBufferLenght; tmpIndex++) {
        pBuffer[tmpIndex] = tmpIndex + uwOffset;
    }
}

static uint8_t Buffercmp(uint32_t* pBuffer1, uint32_t* pBuffer2, uint16_t BufferLength)
{
    while (BufferLength--) {
        if (*pBuffer1 != *pBuffer2) {
            return 1;
        }
        pBuffer1++;
        pBuffer2++;
    }
    return 0;
}

/*
// cache invalidation is required for DMA transfers
static uint8_t BSP_SDRAM_WriteData_nocache(uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize)
{
    // Clean Data Cache to update the content of the SRAM
    SCB_CleanDCache_by_Addr((uint32_t*)pData, uwDataSize * 4);
    return BSP_SDRAM_WriteData(uwStartAddress, pData, uwDataSize);
}

uint8_t BSP_SDRAM_ReadData_nocache(uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize)
{
    uint8_t ret = BSP_SDRAM_ReadData(uwStartAddress, pData, uwDataSize);
    // Invalidate Data Cache to get the updated content of the SRAM
    SCB_InvalidateDCache_by_Addr((uint32_t *)pData, uwDataSize*4);
    return ret;
}
*/

bool write_block(uint32_t start, uint32_t pattern)
{
    for (int i=0; i<BUFFER_SIZE; i++)
        wbuf[i] = pattern;

    if (BSP_SDRAM_WriteData(start, wbuf, BUFFER_SIZE) != SDRAM_OK) {
        log_line("SDRAM WRITE : FAILED");
        return false;
    }
    return true;
}

const int log_max_errors = 3;

int compare_block(uint32_t start, uint32_t pattern)
{
    if (BSP_SDRAM_ReadData(start, rbuf, BUFFER_SIZE) != SDRAM_OK) {
        log_line("SDRAM READ : FAILED");
        return 1;
    }

    if (Buffercmp(wbuf, rbuf, BUFFER_SIZE) > 0) {
        // log_line("SDRAM COMPARE : FAILED");
        int errorCount = 0;
        for (int i=0; i<BUFFER_SIZE; i++)
            if (wbuf[i] != rbuf[i]) {
                if (errorCount < log_max_errors)
                    log_line("[%08X]: w %08X, r %08X, xor %08X",
                             start - SDRAM_START_ADDR + i * 4,
                             wbuf[i],
                             rbuf[i],
                             wbuf[i] ^ rbuf[i]);
                else if (errorCount == log_max_errors)
                    log_line("...");
                errorCount++;
            }
        return errorCount;
    }
    return 0;
}

int counter_test_block(uint32_t start, uint32_t offset)
{
    Fill_Buffer(wbuf, BUFFER_SIZE, offset);

    if (BSP_SDRAM_WriteData(start, wbuf, BUFFER_SIZE) != SDRAM_OK) {
        log_line("SDRAM WRITE : FAILED");
        return 1;
    }

    if (BSP_SDRAM_ReadData(start, rbuf, BUFFER_SIZE) != SDRAM_OK) {
        log_line("SDRAM READ : FAILED");
        return 1;
    }

    if (Buffercmp(wbuf, rbuf, BUFFER_SIZE) > 0) {
        int errorCount = 0;
        for (int i=0; i<BUFFER_SIZE; i++)
            if (wbuf[i] != rbuf[i]) {
                if (errorCount < log_max_errors)
                    log_line("[%08X]: w %08X, r %08X, xor %08X",
                               start + i * 4,
                               wbuf[i],
                               rbuf[i],
                               wbuf[i] ^ rbuf[i]);
                else if (errorCount == log_max_errors)
                    log_line("...");
                errorCount++;
            }
        return errorCount;
    }
    return 0;
}

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

uint32_t patterns[] = {
    0x00000000,
    0xFFFFFFFF,
};

bool SDRAM_quick_test(void)
{
    log_line("SDRAM Test [%08X-%08X] %d MB", SDRAM_START_ADDR, SDRAM_END_ADDR, SDRAM_SIZE/1024/1024);

    int errorCount = 0;
    for (int np = 0; np < ARRAY_SIZE(patterns); np++) {
        uint32_t pattern = patterns[np];
        for (uint32_t addr = SDRAM_START_ADDR; addr + BUFFER_SIZE * 4 - 1 <= SDRAM_END_ADDR; addr += BUFFER_SIZE * 4) {
            if (! write_block(addr, pattern))
                goto err;
        }
        for (uint32_t addr = SDRAM_START_ADDR; addr + BUFFER_SIZE * 4 - 1 <= SDRAM_END_ADDR; addr += BUFFER_SIZE * 4) {
            errorCount += compare_block(addr, pattern) ? 1 : 0;
            if (errorCount > 3)
                goto err;
        }
        log_line("SDRAM pattern %08X %s", pattern, (errorCount==0) ? "Ok" : "FAILED");
    }

    for (uint32_t addr = SDRAM_START_ADDR; addr + BUFFER_SIZE * 4 - 1 <= SDRAM_END_ADDR; addr += BUFFER_SIZE * 4) {
        errorCount += counter_test_block(addr, 0) ? 1 : 0;
    }
    log_line("SDRAM counter test %s", (errorCount==0) ? "Ok" : "FAILED");

    if (errorCount)
        goto err;
    log_line("SDRAM Test Ok");
    return true;
err:
    log_line("SDRAM Test FAILED");
    return false;
}
