/**
  ******************************************************************************
  * File Name          : ADC.h
  * Description        : This file provides code for the configuration
  *                      of the ADC instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#ifndef __adc_H
#define __adc_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32_hal.h"

extern ADC_HandleTypeDef hadc1;

void MX_ADC1_Init(void);

#ifdef __cplusplus
}
#endif
#endif /*__ adc_H */
