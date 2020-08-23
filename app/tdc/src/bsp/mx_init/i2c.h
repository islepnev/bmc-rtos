/**
  ******************************************************************************
  * File Name          : I2C.h
  * Description        : This file provides code for the configuration
  *                      of the I2C instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#ifndef __i2c_H
#define __i2c_H

#ifdef __cplusplus
 extern "C" {
#endif

void MX_I2C1_Init(void);
void MX_I2C2_Init(void);
void MX_I2C3_SMBUS_Init(void);
void MX_I2C4_Init(void);

#ifdef __cplusplus
}
#endif
#endif /*__ i2c_H */
