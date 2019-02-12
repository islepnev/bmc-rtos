
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f7xx_hal.h"
#include "gpio.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"

#include "fpga_spi_hal.h"
#include "adt7301_spi_hal.h"
#include "ftoa.h"
#include "ansi_escape_codes.h"
#include <string.h>

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/*
// convert float to string one decimal digit at a time
// assumes float is < 65536 and ARRAYSIZE is big enough
// problem: it truncates numbers at size without rounding
// str is a char array to hold the result, float is the number to convert
// size is the number of decimal digits you want
void FloatToStringNew(char *str, float f, char size)
{
       char pos;  // position in string
       char len;  // length of decimal part of result
       char* curr;  // temp holder for next digit
       int value;  // decimal digit(s) to convert
       pos = 0;  // initialize pos, just to be sure

       value = (int)f;  // truncate the floating point number
       itoa(value,str);  // this is kinda dangerous depending on the length of str
       // now str array has the digits before the decimal

       if (f < 0 )  // handle negative numbers
       {
               f *= -1;
               value *= -1;
       }

    len = strlen(str);  // find out how big the integer part was
       pos = len;  // position the pointer to the end of the integer part
       str[pos++] = '.';  // add decimal point to string

       while(pos < (size + len + 1) )  // process remaining digits
       {
               f = f - (float)value;  // hack off the whole part of the number
               f *= 10;  // move next digit over
               value = (int)f;  // get next digit
               itoa(value, curr); // convert digit to string
               str[pos++] = *curr; // add digit to result string and increment pointer
       }
}
*/
void print_therm(uint16_t rawTemp)
{
//   if (rawTemp != TEMP_RAW_ERROR) {
      int16_t temp32 = adt7301_convert_temp_adt7301_scale32(rawTemp);
      int16_t tempInt = temp32 / 32;
      int16_t tempDec = (temp32 - tempInt * 32) * 10 / 32;
      printf("%d.%01d", tempInt, tempDec);
//   }
}

void print_therm_n(int16_t temp[], int n, int fractdigits)
{
    printf("temp=");
    for (int i=0; i<n; i++) {
        int16_t temp32 = adt7301_convert_temp_adt7301_scale32(temp[i]);
        float tempf = (float)temp32 / 32;
        char str[10];
        ftoa(tempf, str, fractdigits);
        printf("%s ", str);
//        print_therm(temp[i]); printf(" ");
    }
}

const char *STR_ON = ANSI_GREEN "ON" ANSI_CLEAR;
const char *STR_OFF = ANSI_RED "OFF" ANSI_CLEAR;

const char *STR_NORMAL = ANSI_GREEN "NORMAL" ANSI_CLEAR;
const char *STR_FAIL = ANSI_RED "FAIL" ANSI_CLEAR;

HAL_StatusTypeDef muxRead(uint8_t *data)
{
    const uint16_t deviceAddr = 0x74;
    HAL_StatusTypeDef ret;
    uint8_t pData;
    ret = HAL_I2C_Master_Receive(&hi2c4, deviceAddr << 1, &pData, 1, 100);
    if (ret != HAL_OK) {
        printf("I2C error: %d\n", ret);
    } else {
        if (data) {
            *data = pData;
        }
    }
    return ret;
}

void muxDetect()
{
    HAL_GPIO_WritePin(MON_SMB_SW_RST_B_GPIO_Port,  MON_SMB_SW_RST_B_Pin,  GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MON_SMB_SW_RST_B_GPIO_Port,  MON_SMB_SW_RST_B_Pin,  GPIO_PIN_SET);
    uint8_t data = 0;
    int muxPresent = (HAL_OK == muxRead(&data));
    printf("I2C mux: %02X %s\n", data, muxPresent ? STR_NORMAL : STR_FAIL);
}

int switch_5v = 1;
int switch_3v3 = 0;
int switch_1v5 = 1;
int switch_1v0 = 0;

enum {MANUFACTURER_ID_TI = 0x5449};
enum {DEVICE_ID_INA226 = 0x2260};
enum {
    INA226_REG_CONFIG = 0x00,
    INA226_REG_SHUNT_VOLT = 0x01,
    INA226_REG_BUS_VOLT = 0x02,
    INA226_REG_POWER = 0x03,
    INA226_REG_CURRENT = 0x04,
    INA226_REG_CAL = 0x05,
    INA226_REG_MASK = 0x06,
    INA226_REG_ALERT = 0x07,
    INA226_REG_MANUFACTURER_ID = 0xFE,
    INA226_REG_DEVICE_ID = 0xFF
};
enum {monSize = 13};
int monAddr[monSize] = {
    0x43, 0x45, 0x42, 0x44,
    0x40, 0x46, 0x47, 0x48,
    0x4A, 0x4B, 0x4C, 0x4D,
    0x4E};
float monShuntVal(int deviceAddr)
{
    switch(deviceAddr) {
    case 0x40: return 0.002;
    case 0x42: return 0;
    case 0x43: return 0.010;
    case 0x44: return 0;
    case 0x45: return 0.004;
    case 0x46: return 0.002;
    case 0x47: return 0.002;
    case 0x48: return 0.002;
    case 0x4A: return 0.002;
    case 0x4B: return 0.002;
    case 0x4C: return 0.002;
    case 0x4D: return 0.002;
    case 0x4E: return 0.002;
    default: return 0;
    }
}

float monBusNom(int deviceAddr)
{
    switch(deviceAddr) {
    case 0x40: return 1.5;
    case 0x42: return 5.0;
    case 0x43: return 5.0;
    case 0x44: return 3.3;
    case 0x45: return 3.3;
    case 0x46: return 1.0;
    case 0x47: return 1.0;
    case 0x48: return 1.2;
    case 0x4A: return 1.8;
    case 0x4B: return 2.5;
    case 0x4C: return 2.5;
    case 0x4D: return 2.5;
    case 0x4E: return 2.5;
    default: return 0;
    }
}

const char *monLabel(int deviceAddr)
{
    switch(deviceAddr) {
    case 0x40: return "  int 1.5V"; // U1, ? mOhm
    case 0x42: return "        5V"; // U3
    case 0x43: return "    VME 5V"; // U5, 10 mOhm
    case 0x44: return "      3.3V"; // U7
    case 0x45: return "  VME 3.3V"; // U9, 4 mOhm
    case 0x46: return "  FPGA 1.0"; // U11, 2 mOhm
    case 0x47: return "   MGT 1.0"; // U13, 2 mOhm
    case 0x48: return "   MGT 1.2"; // U2, 2 mOhm
    case 0x4A: return "  FPGA 1.8"; // U, 2 mOhm
    case 0x4B: return " TDC-A 2.5"; // U, 2 mOhm
    case 0x4C: return " TDC-B 2.5"; // U, 2 mOhm
    case 0x4D: return " TDC-C 2.5"; // U, 2 mOhm
    case 0x4E: return " Clock 2.5"; // U, 2 mOhm
    default: return "???";
    }
}

const float TOLERANCE_1 = 0.05;
const float TOLERANCE_CRIT = 0.1;

float monValuesBus[monSize];
float monValuesShunt[monSize];

HAL_StatusTypeDef monReadRegister(uint16_t deviceAddr, uint16_t reg, uint16_t *data)
{
    HAL_StatusTypeDef ret;
    int Size = 2;
    uint8_t pData[Size];
    ret = HAL_I2C_Mem_Read(&hi2c4, deviceAddr << 1, reg, I2C_MEMADD_SIZE_8BIT, pData, 2, 100);
    if (ret != HAL_OK) {
        printf("I2C error: %d\n", ret);
    } else {
        if (data) {
            *data = ((uint16_t)pData[0] << 8) | pData[1];
        }
    }
    return ret;
}

int monIsOn(int deviceAddr)
{
    switch(deviceAddr) {
    case 0x40: return switch_1v5;
    case 0x42: return switch_5v;
    case 0x43: return 1;
    case 0x44: return switch_3v3;
    case 0x45: return 1;
    case 0x46: return switch_1v0;
    case 0x47: return switch_1v0;
    case 0x48: return switch_1v0;
    case 0x4A: return switch_3v3;
    case 0x4B: return switch_3v3;
    case 0x4C: return switch_3v3;
    case 0x4D: return switch_3v3;
    case 0x4E: return switch_3v3;
    default: return 0;
    }

}

int monBusValid(int deviceAddr)
{
    int n = -1;
    for (int i=0; i<monSize; i++)
        if (monAddr[i] == deviceAddr) {
            n = i;
            break;
        }
    if (n == -1)
        return 0;
    float bus = monValuesBus[n];
//    float marginLo1 = monBusNom(deviceAddr) * (1-TOLERANCE_1);
//    float marginHi1 = monBusNom(deviceAddr) * (1+TOLERANCE_1);
    float marginLo2 = monBusNom(deviceAddr) * (1-TOLERANCE_CRIT);
    float marginHi2 = monBusNom(deviceAddr) * (1+TOLERANCE_CRIT);
//    int inRange1 = (bus > marginLo1) && (bus < marginHi1);
    int inRange2 = (bus > marginLo2) && (bus < marginHi2);
    int isOn = monIsOn(deviceAddr);
    return isOn ? inRange2 : 1;
}

void printMonValue(int deviceAddr, float bus, float shunt, float resistance)
{
    const int fractdigits = 3;
    char str1[10], str2[10], str3[10];
    ftoa(bus, str1, fractdigits);
    float marginLo1 = monBusNom(deviceAddr) * (1-TOLERANCE_1);
    float marginHi1 = monBusNom(deviceAddr) * (1+TOLERANCE_1);
    float marginLo2 = monBusNom(deviceAddr) * (1-TOLERANCE_CRIT);
    float marginHi2 = monBusNom(deviceAddr) * (1+TOLERANCE_CRIT);
    int inRange1 = (bus > marginLo1) && (bus < marginHi1);
    int inRange2 = (bus > marginLo2) && (bus < marginHi2);
    int isOn = monIsOn(deviceAddr);
    printf("%6s: %s%8s%s", monLabel(deviceAddr), inRange1 ? ANSI_GREEN : inRange2 ? ANSI_YELLOW : isOn ? ANSI_RED : ANSI_GRAY, str1, ANSI_CLEAR);
    if (resistance > 0) {
//        ftoa(shunt * 1e3, str2, fractdigits); // mV
        ftoa(shunt / resistance, str3, fractdigits);
        printf(" %8s", str3);
    } else {
        printf("         ");
    }
    printf(" %s\n", monBusValid(deviceAddr) ? (ANSI_GREEN "VALID" ANSI_CLEAR) : (ANSI_RED "FAIL" ANSI_CLEAR));
}

enum {
    MON_STATE_INIT = 0,
    MON_STATE_DETECT = 1,
    MON_STATE_READ = 2,
    MON_STATE_ERROR = 3
};

int monState = MON_STATE_INIT;
int monCycle = 0;

int monDetect()
{
    int err = 0;
    for (int i=0; i<monSize; i++) {
        if (err > 2)
            break;
        uint16_t deviceAddr = monAddr[i];
        uint16_t data;
        if (HAL_OK != monReadRegister(deviceAddr, INA226_REG_MANUFACTURER_ID, &data)) {
            err++;
            continue;
        }
        if (data != MANUFACTURER_ID_TI) {
            err++;
        }
        if (HAL_OK != monReadRegister(deviceAddr, INA226_REG_DEVICE_ID, &data)) {
            err++;
            continue;
        }
        if (data != DEVICE_ID_INA226) {
            err++;
        }
    }
    return err;
}

int monReadValues()
{
    int err = 0;
    for (int i=0; i<monSize; i++) {
        uint16_t deviceAddr = monAddr[i];
        uint16_t data;
        if (HAL_OK == monReadRegister(deviceAddr, INA226_REG_BUS_VOLT, &data)) {
            monValuesBus[i] = (int16_t)data * 1.25e-3;
        } else {
            err++;
            monValuesBus[i] = 0;
        }
        if (HAL_OK == monReadRegister(deviceAddr, INA226_REG_SHUNT_VOLT, &data)) {
            monValuesShunt[i] = (int16_t)data * 2.5e-6;
        } else {
            err++;
            monValuesShunt[i] = 0;
        }
//        printMonValue(deviceAddr, busVolt * 1.25e-3, shuntVolt * 2.5e-6, monShuntVal[i]);
    }
    return err;
}

const char *monStateStr(int monState)
{
    switch(monState) {
    case MON_STATE_INIT: return "INIT";
    case MON_STATE_DETECT: return "DETECT";
    case MON_STATE_READ: return "READ";
    case MON_STATE_ERROR: return "ERROR";
    default: return "?";
    }
}

void monPrintValues()
{
    printf("Mon state: %s %d\n", monStateStr(monState), monCycle);
    if (monState == MON_STATE_READ) {
        for (int i=0; i<monSize; i++) {
            uint16_t deviceAddr = monAddr[i];
            printMonValue(deviceAddr, monValuesBus[i], monValuesShunt[i], monShuntVal(deviceAddr));
        }
    }
}

void runMon()
{
    monCycle++;
//    if (!switch_5v) {
//        monState = MON_STATE_INIT;
//        return;
//    }
    switch(monState) {
    case MON_STATE_INIT:
        for (int i=0; i<monSize; i++) {
            monValuesBus[i] = 0;
            monValuesShunt[i] = 0;
        }
        monState = MON_STATE_DETECT;
        break;
    case MON_STATE_DETECT:
        if (monDetect() == 0)
            monState = MON_STATE_READ;
        else
            monState = MON_STATE_ERROR;
        break;
    case MON_STATE_READ:
        if (monReadValues() == 0)
            monState = MON_STATE_READ;
        else
            monState = MON_STATE_ERROR;
        break;
    case MON_STATE_ERROR:
        monState = MON_STATE_INIT;
        break;
    default:
        break;
    }
}

int fpga_core_pgood = 0;
int ltm_pgood = 0;

void update_power_switches()
{
    switch_5v  = 1; // monBusValid(0x43); // && monBusValid(0x45); // VME 5V and 3.3V
    switch_1v5 = 1; // monBusValid(0x42); // 5V
    switch_1v0 = ltm_pgood; // && monBusValid(0x40); // 1.5V
    switch_3v3 = fpga_core_pgood && switch_1v0 && switch_1v5; // && monBusValid(0x45);
    if (!switch_5v) {
        switch_3v3 = 0;
        switch_1v5 = 0;
        switch_1v0 = 0;
    }
    if (!switch_1v5) {
        switch_1v0 = 0;
        switch_3v3 = 0;
    }
    if (!switch_1v0) {
        switch_3v3 = 0;
    }
    HAL_GPIO_WritePin(GPIOC, ON_1_0V_1_2V_Pin, switch_1v0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOJ, ON_1_5V_Pin,      switch_1v5 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOJ, ON_3_3V_Pin,      switch_3v3 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOJ, ON_5V_Pin,        switch_5v  ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
//  // Power OFF
//  HAL_GPIO_WritePin(GPIOJ, ON_5V_Pin,        GPIO_PIN_RESET);
//  HAL_GPIO_WritePin(GPIOJ, ON_3_3V_Pin,      GPIO_PIN_RESET);
//  HAL_GPIO_WritePin(GPIOJ, ON_1_5V_Pin,      GPIO_PIN_RESET);
//  HAL_GPIO_WritePin(GPIOC, ON_1_0V_1_2V_Pin, GPIO_PIN_RESET);

int readPowerGoodFpga()
{
    return (GPIO_PIN_SET == HAL_GPIO_ReadPin(FPGA_CORE_PGOOD_B_GPIO_Port, FPGA_CORE_PGOOD_B_Pin));
}
int readPowerGood1v5()
{
    return (GPIO_PIN_SET == HAL_GPIO_ReadPin(LTM_PGOOD_B_GPIO_Port, LTM_PGOOD_B_Pin));
}

int getMonSystemState()
{
    for (int i=0; i < monSize; i++)
        if (!monBusValid(monAddr[i]))
            return 0;
    return 1;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
//  MX_I2C1_Init();
//  MX_I2C2_Init();
//  MX_I2C3_SMBUS_Init();
  MX_I2C4_Init();
  MX_SPI1_Init();
  MX_SPI4_Init();
  /* USER CODE BEGIN 2 */

//  printf("\n%lu, %d, %lu\n", HAL_RCC_GetHCLKFreq(), HAL_GetTickFreq(), HAL_GetTick());

  switch_5v = 1;
  switch_1v5 = 1;
  switch_1v0 = 1;
  switch_3v3 = 1;
//    update_power_switches();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
      printf(ANSI_CLEARTERM ANSI_GOHOME ANSI_CLEAR);
/*
      for (int i=0; i<100000; i++) {
//          HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_SET);
          HAL_GPIO_WritePin(LED_RED_B_GPIO_Port,    LED_RED_B_Pin,    GPIO_PIN_SET);
          HAL_GPIO_WritePin(LED_YELLOW_B_GPIO_Port, LED_YELLOW_B_Pin, GPIO_PIN_SET);
          HAL_GPIO_WritePin(LED_GREEN_B_GPIO_Port,  LED_GREEN_B_Pin,  GPIO_PIN_SET);
      }
      for (int i=0; i<100000; i++) {
//          HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_RESET);
//          HAL_GPIO_WritePin(LED_RED_B_GPIO_Port,    LED_RED_B_Pin,    GPIO_PIN_RESET);
//          HAL_GPIO_WritePin(LED_YELLOW_B_GPIO_Port, LED_YELLOW_B_Pin, GPIO_PIN_RESET);
          HAL_GPIO_WritePin(LED_GREEN_B_GPIO_Port,  LED_GREEN_B_Pin,  GPIO_PIN_RESET);
      }
      */
      printf("Tick: %8ld \n", HAL_GetTick());
      printf("Switch 5V   %s\n", switch_5v ? STR_ON : STR_OFF);
      printf("Switch 3.3V %s\n", switch_3v3 ? STR_ON : STR_OFF);
      printf("Switch 1.5V %s\n", switch_1v5 ? STR_ON : STR_OFF);
      printf("Switch 1.0V %s\n", switch_1v0 ? STR_ON : STR_OFF);

      for (int i=0; i<10; i++) {
          update_power_switches();
          fpga_core_pgood = readPowerGoodFpga();
          ltm_pgood = readPowerGood1v5();
      }

      printf("Intermediate 1.5V: %s\n", ltm_pgood ? STR_NORMAL : switch_1v5 ? STR_FAIL : STR_OFF);
      printf("FPGA Core 1.0V:    %s\n", fpga_core_pgood ? STR_NORMAL : switch_1v0 ? STR_FAIL : STR_OFF);

      muxDetect();

      int16_t temp[4] = {0,0,0,0};
      for(int i=0; i<4; i++) temp[i] = adt7301_convert_temp_adt7301_scale32(adt7301_read_temp(i));
      print_therm_n(temp, 4, 1);
      printf("\n");
      uint16_t data[8] = {0,0,0,0,0,0,0,0};
      for (int i=0; i<8; i++) fpga_spi_hal_read_reg(i, &data[i]);
      printf("fpga=%4X %4X %4X %4X %4X %4X %4X %4X\n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);

      runMon();
      int systemState = getMonSystemState();
      printf("System state: %s\n", systemState ? STR_NORMAL : STR_FAIL);
      HAL_GPIO_WritePin(LED_RED_B_GPIO_Port,    LED_RED_B_Pin,    systemState ? GPIO_PIN_SET : GPIO_PIN_RESET);

      monPrintValues();
//      for (int i=0; i<10; i++) printf("\n");
      fflush(stdout);
      for (int i=0; i<200000; i++) {
          HAL_GPIO_WritePin(LED_GREEN_B_GPIO_Port,  LED_GREEN_B_Pin,  GPIO_PIN_RESET);
      }
      for (int i=0; i<200000; i++) {
          HAL_GPIO_WritePin(LED_GREEN_B_GPIO_Port,  LED_GREEN_B_Pin,  GPIO_PIN_SET);
      }
  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    /**Configure the main internal regulator output voltage
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 12;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Activate the Over-Drive mode
    */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_USART3
                              |RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_I2C2
                              |RCC_PERIPHCLK_I2C3|RCC_PERIPHCLK_I2C4;
  PeriphClkInitStruct.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c3ClockSelection = RCC_I2C3CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c4ClockSelection = RCC_I2C4CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
    printf("HAL ERROR %s %d\n", file, line);
  while(1)
  {
      HAL_GPIO_WritePin(LED_GREEN_B_GPIO_Port, LED_GREEN_B_Pin, GPIO_PIN_RESET);
      int k = 100000;
      for (int i=0; i<k; i++) {
          HAL_GPIO_WritePin(LED_RED_B_GPIO_Port, LED_RED_B_Pin, GPIO_PIN_SET);
          HAL_GPIO_WritePin(LED_YELLOW_B_GPIO_Port, LED_YELLOW_B_Pin, GPIO_PIN_RESET);
      }
      for (int i=0; i<k; i++) {
          HAL_GPIO_WritePin(LED_YELLOW_B_GPIO_Port, LED_YELLOW_B_Pin, GPIO_PIN_SET);
          HAL_GPIO_WritePin(LED_RED_B_GPIO_Port, LED_RED_B_Pin, GPIO_PIN_RESET);
      }
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
