#ifndef BSP_PIN_DEFS_H
#define BSP_PIN_DEFS_H

#define ADT_CS_B0_GPIO_Port GPIOE
#define ADT_CS_B0_Pin GPIO_PIN_4
#define ADT_CS_B1_GPIO_Port GPIOE
#define ADT_CS_B1_Pin GPIO_PIN_3
#define ADT_CS_B2_GPIO_Port GPIOE
#define ADT_CS_B2_Pin GPIO_PIN_1
#define ADT_CS_B3_GPIO_Port GPIOE
#define ADT_CS_B3_Pin GPIO_PIN_0
#define ADT_DIN_GPIO_Port GPIOE
#define ADT_DIN_Pin GPIO_PIN_6
#define ADT_DOUT_GPIO_Port GPIOE
#define ADT_DOUT_Pin GPIO_PIN_5
#define ADT_SCLK_GPIO_Port GPIOE
#define ADT_SCLK_Pin GPIO_PIN_2

// CONFIG_IIC N/C, for compatibility with v4
#define CONFIG_IIC_SCL_GPIO_Port GPIOF
#define CONFIG_IIC_SCL_Pin GPIO_PIN_1
#define CONFIG_IIC_SDA_GPIO_Port GPIOF
#define CONFIG_IIC_SDA_Pin GPIO_PIN_0

#define FPGA_CORE_PGOOD_GPIO_Port GPIOF
#define FPGA_CORE_PGOOD_Pin GPIO_PIN_11
#define FPGA_NSS_GPIO_Port GPIOA
#define FPGA_NSS_Pin GPIO_PIN_4
#define FPGA_MISO_GPIO_Port GPIOA
#define FPGA_MISO_Pin GPIO_PIN_6
#define FPGA_MOSI_GPIO_Port GPIOA
#define FPGA_MOSI_Pin GPIO_PIN_7
#define FPGA_SCK_GPIO_Port GPIOA
#define FPGA_SCK_Pin GPIO_PIN_5

// FPGA_UART N/C, for compatibility with v4
#define FPGA_UART_CTS_GPIO_Port GPIOA
#define FPGA_UART_CTS_Pin GPIO_PIN_0
#define FPGA_UART_RTS_GPIO_Port GPIOA
#define FPGA_UART_RTS_Pin GPIO_PIN_1
#define FPGA_UART_RX_GPIO_Port GPIOA
#define FPGA_UART_RX_Pin GPIO_PIN_3
#define FPGA_UART_TX_GPIO_Port GPIOA
#define FPGA_UART_TX_Pin GPIO_PIN_2

#define LED_GREEN_B_GPIO_Port GPIOG
#define LED_GREEN_B_Pin GPIO_PIN_2
#define LED_RED_B_GPIO_Port GPIOG
#define LED_RED_B_Pin GPIO_PIN_6
#define LED_YELLOW_B_GPIO_Port GPIOG
#define LED_YELLOW_B_Pin GPIO_PIN_3

// N/C, for compatibility with v4
#define LED_ERROR_B_GPIO_Port GPIOH
#define LED_ERROR_B_Pin GPIO_PIN_14
#define LED_HEARTBEAT_B_GPIO_Port GPIOH
#define LED_HEARTBEAT_B_Pin GPIO_PIN_15

#define LTM_PGOOD_GPIO_Port GPIOE
#define LTM_PGOOD_Pin GPIO_PIN_11

// MON_SMB N/C, for compatibility with v4
#define MON_SMB_ALERT_B_GPIO_Port GPIOH
#define MON_SMB_ALERT_B_Pin GPIO_PIN_10
#define MON_SMB_CLK_GPIO_Port GPIOH
#define MON_SMB_CLK_Pin GPIO_PIN_11
#define MON_SMB_DAT_GPIO_Port GPIOH
#define MON_SMB_DAT_Pin GPIO_PIN_12
#define MON_SMB_SW_RST_B_GPIO_Port GPIOH
#define MON_SMB_SW_RST_B_Pin GPIO_PIN_8

#define ON_1V0_1V2_GPIO_Port GPIOE
#define ON_1V0_1V2_Pin GPIO_PIN_10
#define ON_1V5_GPIO_Port GPIOE
#define ON_1V5_Pin GPIO_PIN_9
#define ON_3V3_GPIO_Port GPIOE
#define ON_3V3_Pin GPIO_PIN_8
#define ON_5V_GPIO_Port GPIOE
#define ON_5V_Pin GPIO_PIN_7
#define SYSTEM_RDY_GPIO_Port GPIOF
#define SYSTEM_RDY_Pin GPIO_PIN_14
#define PLL_IRQ_B_EXTI_IRQn EXTI15_10_IRQn
#define PLL_IRQ_B_GPIO_Port GPIOF
#define PLL_IRQ_B_Pin GPIO_PIN_12
#define PLL_LED_EN_B_GPIO_Port GPIOE
#define PLL_LED_EN_B_Pin GPIO_PIN_12
#define PLL_RESET_GPIO_Port GPIOB
#define PLL_RESET_Pin GPIO_PIN_0
#define PLL_SPI_MISO_GPIO_Port GPIOF
#define PLL_SPI_MISO_Pin GPIO_PIN_8
#define PLL_SPI_MOSI_GPIO_Port GPIOF
#define PLL_SPI_MOSI_Pin GPIO_PIN_9
#define PLL_SPI_NSS_GPIO_Port GPIOF
#define PLL_SPI_NSS_Pin GPIO_PIN_6
#define PLL_SPI_SCLK_GPIO_Port GPIOF
#define PLL_SPI_SCLK_Pin GPIO_PIN_7

#define VME_DET_B_GPIO_Port GPIOE
#define VME_DET_B_Pin GPIO_PIN_15
#define PA_SCL_GPIO_Port GPIOA
#define PA_SCL_Pin GPIO_PIN_8
#define PA_SDA_GPIO_Port GPIOC
#define PA_SDA_Pin GPIO_PIN_9
#define PB_SCL_GPIO_Port GPIOB
#define PB_SCL_Pin GPIO_PIN_6
#define PB_SDA_GPIO_Port GPIOB
#define PB_SDA_Pin GPIO_PIN_9
#define PB_SMBA_GPIO_Port GPIOB
#define PB_SMBA_Pin GPIO_PIN_5

#endif // BSP_PIN_DEFS_H
