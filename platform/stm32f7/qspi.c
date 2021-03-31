/**
  ******************************************************************************
  * @file    BSP/Src/qspi.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the QSPI Driver
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

#include <stdint.h>
#include "stm32746g_discovery_qspi.h"
#include <log/log.h>

#define BUFFER_SIZE         ((uint32_t)0x0200)
#define WRITE_READ_ADDR     ((uint32_t)0x0050)

uint8_t qspi_aTxBuffer[BUFFER_SIZE];
uint8_t qspi_aRxBuffer[BUFFER_SIZE];

static void     Fill_Buffer (uint8_t *pBuffer, uint32_t uwBufferLength, uint32_t uwOffset);
static uint8_t  Buffercmp   (uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength);

/**
* @brief  QSPI Demo
* @param  None
* @retval None
*/
void QSPI_demo (void)
{
    uint8_t status;

    /*##-1- Configure the QSPI device ##########################################*/
    /* QSPI device configuration */
    status = BSP_QSPI_Init();

    if (status == QSPI_NOT_SUPPORTED) {
        log_printf(LOG_ERR, "QSPI not supported");
        return;
    } else if (status == QSPI_ERROR) {
        log_printf(LOG_ERR, "QSPI init failed");
        return;
    }


    /*##-3- Erase QSPI memory ################################################*/
    if (BSP_QSPI_Erase_Block(WRITE_READ_ADDR) != QSPI_OK)
    {
        log_printf(LOG_ERR, "QSPI ERASE : FAILED");
        return;
    }

    /*##-4- QSPI memory read/write access  #################################*/
    /* Fill the buffer to write */
    Fill_Buffer(qspi_aTxBuffer, BUFFER_SIZE, 0xD20F);

    /* Write data to the QSPI memory */
    if (BSP_QSPI_Write(qspi_aTxBuffer, WRITE_READ_ADDR, BUFFER_SIZE) != QSPI_OK)
    {
        log_printf(LOG_ERR, "QSPI WRITE : FAILED");
        return;
    }

    /* Read back data from the QSPI memory */
    if (BSP_QSPI_Read(qspi_aRxBuffer, WRITE_READ_ADDR, BUFFER_SIZE) != QSPI_OK)
    {
        log_printf(LOG_ERR, "QSPI READ : FAILED");
        return;
    }

    /*##-5- Checking data integrity ############################################*/
    if (Buffercmp(qspi_aRxBuffer, qspi_aTxBuffer, BUFFER_SIZE) > 0)
    {
        log_printf(LOG_ERR, "QSPI COMPARE : FAILED");
        return;
    }

    log_printf(LOG_INFO, "QSPI Test Ok");
    status = BSP_QSPI_DeInit();
}

/**
* @brief  Fills buffer with user predefined data.
* @param  pBuffer: pointer on the buffer to fill
* @param  uwBufferLenght: size of the buffer to fill
* @param  uwOffset: first value to fill on the buffer
* @retval None
*/
static void Fill_Buffer(uint8_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset)
{
    uint32_t tmpIndex = 0;

    /* Put in global buffer different values */
    for (tmpIndex = 0; tmpIndex < uwBufferLenght; tmpIndex++ )
    {
        pBuffer[tmpIndex] = tmpIndex + uwOffset;
    }
}

/**
* @brief  Compares two buffers.
* @param  pBuffer1, pBuffer2: buffers to be compared.
* @param  BufferLength: buffer's length
* @retval 1: pBuffer identical to pBuffer1
*         0: pBuffer differs from pBuffer1
*/
static uint8_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength)
{
    while (BufferLength--)
    {
        if (*pBuffer1 != *pBuffer2)
        {
            return 1;
        }

        pBuffer1++;
        pBuffer2++;
    }

    return 0;
}
