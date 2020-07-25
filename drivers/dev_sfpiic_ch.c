/*
**    VXS payload port IIC master functions
**
**    Copyright 2019 Ilja Slepnev
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
#include "dev_sfpiic_ch.h"

#include <string.h>
#include <stdint.h>
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_def.h"
#include "stm32f7xx_hal_i2c.h"
#include "dev_sfpiic.h"
#include "dev_sfpiic_driver.h"
#include "dev_sfpiic_types.h"
#include "logbuffer.h"
#include "debug_helpers.h"
#include "ipmi_sensor_types.h"
#include "cmsis_os.h"
#include "sff_8436.h"

static void dev_sfpiic_update_ch_state(Dev_sfpiic *d, int pp, HAL_StatusTypeDef ret)
{
    sfpiic_ch_status_t *status = &d->status.sfp[pp];
    if (HAL_OK == ret) {
        status->iic_master_stats.ops++;
        status->ch_state = SFPIIC_CH_STATE_READY;
    } else {
        if (status->ch_state == SFPIIC_CH_STATE_READY)
            status->iic_master_stats.errors++;
        status->ch_state = SFPIIC_CH_STATE_ERROR;
    }
}

static bool dev_sfpiic_ch_enable_tx(Dev_sfpiic *d, int ch)
{
    uint16_t reg = SFF_8436_TX_DISABLE_REG;
    uint8_t val = 1;
    HAL_StatusTypeDef ret = sfpiic_mem_read(SFP_MAIN_I2C_ADDRESS, reg, &val, 1);
    if (HAL_OK != ret)
        return false;
    if((val&0xF)==0)
        return true;

    ++d->status.sfp[ch].tx_en_cnt;
    val &= ~0xF;
    ret = sfpiic_mem_write(SFP_MAIN_I2C_ADDRESS, reg, &val, 1);
    if (HAL_OK != ret)
        return false;
    val=1;
    ret = sfpiic_mem_read(SFP_MAIN_I2C_ADDRESS, reg, &val, 1);
    if (HAL_OK == ret && (val&0xF) != 0) {
        debug_printf("Failed to enable TX port at SFP #%d: reg[86]=0x%02X\n", ch, val);
        return false;
    }
    return true;
}

static bool dev_sfpiic_ch_test(int ch)
{
    uint16_t reg=0;
    uint8_t id=(uint8_t)-1;
    HAL_StatusTypeDef ret = sfpiic_mem_read(SFP_MAIN_I2C_ADDRESS, reg, &id, 1);
    if (HAL_OK == ret)
        debug_printf("Test val at SFP #%d: 0x%02X\n", ch, id);
    return HAL_OK == ret;
}

static bool dev_sfpiic_ch_read_16(Dev_sfpiic *d, int ch, uint16_t reg, uint16_t *val)
{
    uint8_t data[2];
    HAL_StatusTypeDef ret = sfpiic_mem_read(SFP_MAIN_I2C_ADDRESS, reg, data, 2);
    dev_sfpiic_update_ch_state(d, ch, ret);
    if (HAL_OK == ret && val) {
        *val = (uint16_t)(data[0]<<8)| data[1];
//        debug_printf("Read 16-bit reg=%d at SFP #%d: %d\n", reg, ch, *val);
    }
    return HAL_OK == ret;
}

static bool dev_sfpiic_ch_read_temp(Dev_sfpiic *d, int ch)
{
    int16_t *temp = &d->status.sfp[ch].temp;
    if (! dev_sfpiic_ch_read_16(d, ch, SFF_8436_MON_TEMP_REG2, temp))
        return false;
    // debug_printf("Temp at SFP #%d: %4.1\n", ch, 1./256*(*temp));
    return true;
}

static bool dev_sfpiic_ch_read_voltage(Dev_sfpiic *d, int ch)
{
    uint16_t *volt = &d->status.sfp[ch].volt;
    if (! dev_sfpiic_ch_read_16(d, ch, SFF_8436_MON_VOLT_REG2, volt))
        return false;
    // debug_printf("Supply Volt. at SFP #%d: %4.2f %s\n", ch, 1e-4*(*volt));
    return true;
}

static bool dev_sfpiic_ch_read_rx_pow(Dev_sfpiic *d, int sfp)
{
    const uint16_t reg_base = SFF_8436_CH_1_RX_POW_REG2;

    for(int ch=0; ch<4; ++ch) {
        uint16_t *pow = &d->status.sfp[sfp].rx_pow[ch];
        if (! dev_sfpiic_ch_read_16(d, sfp, reg_base+2*ch, pow))
            return false;
        // debug_printf("Supply Volt. at SFP #%d: %4.2f %s\n", ch, 1e-4*(*volt));
    }
    return true;
}

static bool dev_sfpiic_ch_read_tx_pow(Dev_sfpiic *d, int sfp)
{
    const uint16_t reg_base = SFF_8436_CH_1_TX_POW_REG2;

    for(int ch=0; ch<4; ++ch) {
        uint16_t *pow = &d->status.sfp[sfp].tx_pow[ch];
        if (! dev_sfpiic_ch_read_16(d, sfp, reg_base+2*ch, pow))
            return false;
        // debug_printf("Supply Volt. at SFP #%d: %4.2f %s\n", ch, 1e-4*(*volt));
    }
    return true;
}

static bool dev_sfpiic_ch_read_vendor_serial(Dev_sfpiic *d, int ch)
{
    uint16_t reg;
    reg = SFF_8436_VENDOR_SN_REG16; // Vendor SN (16 bytes) Serial number provided by vendor (ASCII)
    const size_t size = 16;
    char buf[16];
    HAL_StatusTypeDef ret = sfpiic_mem_read(SFP_MAIN_I2C_ADDRESS, reg, (uint8_t *)&buf[0], size);
    dev_sfpiic_update_ch_state(d, ch, ret);
    if (HAL_OK == ret) {
        memcpy(d->status.sfp[ch].vendor_serial, buf, sizeof (buf));
//        debug_printf("Vendor serial at SFP #%d: %s\n", ch, buf);
    }
    return HAL_OK == ret;
}

static bool dev_sfpiic_ch_read_vendor_name(Dev_sfpiic *d, int ch)
{
    // Vendor name (16 bytes) QSFP/SFP vendor name (ASCII)
    const size_t size = 16;
    char buf[16];
    uint16_t addr = SFP_MAIN_I2C_ADDRESS;
    bool is_qsfp = ch >= 3;
    uint16_t reg = is_qsfp ? SFF_8436_VENDOR_NAME_REG16 : 20; // FIXME: 20???
    HAL_StatusTypeDef ret = sfpiic_mem_read(addr, reg, (uint8_t *)&buf[0], size);
    dev_sfpiic_update_ch_state(d, ch, ret);
    if (HAL_OK == ret) {
        memcpy(d->status.sfp[ch].vendor_name, buf, sizeof (buf));
//        debug_printf("Vendor name at SFP #%d: %s\n", ch, buf);
    }
    return HAL_OK == ret;
}

bool dev_sfpiic_ch_update(Dev_sfpiic *d, uint8_t ch)
{
    if (! sfpiic_get_ch_i2c_status(ch))
        return false;

//    if(sfpiic_device_detect(0x50)==HAL_OK){
//        debug_printf("Has device at 0x50\n");
//    }
//    if(sfpiic_device_detect(0x51)==HAL_OK){
//        debug_printf("Has device at 0x51\n");
//    }
//    if(sfpiic_device_detect(0x56)==HAL_OK){
//        debug_printf("Has device at 0x56\n");
//    }

    if(ch<3) {
        // SFP channals
        bool ret = dev_sfpiic_ch_read_vendor_name(d, ch);
        dev_sfpiic_update_ch_state(d, ch, ret);
        if (! ret)
            return false;
    } else {
        // QSFP channals
        //    ret = dev_sfpiic_test(ch);
        if (! dev_sfpiic_ch_enable_tx(d, ch))
            return false;
        if (! dev_sfpiic_ch_read_temp(d, ch))
            return false;
        if (! dev_sfpiic_ch_read_voltage(d, ch))
            return false;
        if (! dev_sfpiic_ch_read_rx_pow(d, ch))
            return false;
        if (! dev_sfpiic_ch_read_tx_pow(d, ch))
            return false;
        if (! dev_sfpiic_ch_read_vendor_name(d, ch))
            return false;
        if (! dev_sfpiic_ch_read_vendor_serial(d, ch))
            return false;
    }
    return true;
}
