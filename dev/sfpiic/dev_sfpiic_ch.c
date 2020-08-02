/*
**    Copyright 2019-2020 Ilja Slepnev
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

#include "cmsis_os.h"
#include "dev_sfpiic.h"
#include "dev_sfpiic_driver.h"
#include "dev_sfpiic_types.h"
#include "ipmi_sensor_types.h"
#include "log/log.h"
#include "sff_8436.h"
#include "sff/sff-internal.h"
#include "sff/sff-common.h"
#include "sff/sfpdiag.h"

static void dev_sfpiic_update_ch_state(Dev_sfpiic *d, int pp, bool state)
{
    sfpiic_ch_status_t *status = &d->priv.status.sfp[pp];
    if (state) {
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
    if (! sfpiic_mem_read(SFP_MAIN_I2C_ADDRESS, reg, &val, 1))
        return false;
    if ((val&0xF)==0)
        return true;

    ++d->priv.status.sfp[ch].tx_en_cnt;
    val &= ~0xF;
    if (! sfpiic_mem_write(SFP_MAIN_I2C_ADDRESS, reg, &val, 1))
        return false;
    val=1;
    if (! sfpiic_mem_read(SFP_MAIN_I2C_ADDRESS, reg, &val, 1))
        return false;
    if ((val&0xF) != 0) {
        log_printf(LOG_WARNING, "Failed to enable TX port at SFP #%d: reg[86]=0x%02X\n", ch, val);
        return false;
    }
    return true;
}

void copy_printable(char *dst, const unsigned char *src, int n)
{
    for (int i=0; i<n; i++) {
        bool is_printable = (src[i] >= 32) && (src[i] <= 126);
        dst[i] = is_printable ? src[i] : '_';
    }
    // trim trailing space
    for (int i=n-1; i>0; i--) {
        if (dst[i] == ' ')
            dst[i] = '\0';
    }
}

bool dev_sfpiic_ch_update(Dev_sfpiic *d, uint8_t ch)
{
    uint16_t addr = SFP_MAIN_I2C_ADDRESS;
    if (! sfpiic_get_ch_i2c_status(addr))
        return false;

    const uint32_t now = osKernelSysTick();
    sfpiic_ch_status_t *status = &d->priv.status.sfp[ch];
    bool old_present = status->present;
    unsigned char id_id;
    bool ret = sfpiic_mem_read(SFP_MAIN_I2C_ADDRESS, 0, (uint8_t *)&id_id, 1);
    bool is_sfp = id_id == SFF8024_ID_SFP;
    bool is_qsfp = id_id == SFF8024_ID_QSFP ||
                   id_id == SFF8024_ID_QSFP28 ||
                   id_id == SFF8024_ID_QSFP_PLUS;
    bool is_cxp = id_id == SFF8024_ID_CXP;
    bool valid = ret && (is_sfp || is_qsfp);
    status->present = valid;
    status->multilane = is_qsfp;
    status->is_cxp = is_cxp;
    status->is_sfp = is_sfp;
    status->is_qsfp = is_qsfp;
    dev_sfpiic_update_ch_state(d, ch, valid);
    if (!valid)
        return false;
    status->id_updated_timetick = now;
    if (!old_present && status->present) {
        unsigned char *idprom = &d->priv.status.sfp[ch].idprom[0];
        if (! sfpiic_mem_read(SFP_MAIN_I2C_ADDRESS, 0, (uint8_t *)idprom, 256))
            return false;
        copy_printable(status->vendor_name, &idprom[20], 16);
        copy_printable(status->vendor_pn, &idprom[40], 16);
        copy_printable(status->vendor_serial, &idprom[68], 16);
    }
    bool a2_present = sfpiic_get_ch_i2c_status(0x51);
    if (!a2_present) {
        status->dom_supported = false;
        return true;
    }
    uint32_t elapsed = now - status->dom_updated_timetick;
    // run every second
    if (elapsed > 1000) {
        // detect DOM
        unsigned char *diag = &d->priv.status.sfp[ch].diag[0];
        if (! sfpiic_mem_read(SFP_MAIN_I2C_ADDRESS+1, 0, (uint8_t *)diag, 256)) {
            status->dom_supported = false;
            return false;
        }

        struct sff_diags sd;
        sff8472_parse_eeprom(status->idprom, &sd);
        if (sd.supports_dom) {
            status->temp = sd.sfp_temp[MCURR] / 256.0;
            status->voltage = sd.sfp_voltage[MCURR] / 10000.0;
            status->rx_power_dBm[0] = convert_mw_to_dbm(sd.rx_power[MCURR] / 10000.0);
            status->tx_power_dBm[0] = convert_mw_to_dbm(sd.tx_power[MCURR] / 10000.0);
            status->dom_updated_timetick = now;
            status->dom_supported = true;
        } else {
            status->dom_supported = false;
        }
    }

    if (is_qsfp) {
        if (! dev_sfpiic_ch_enable_tx(d, ch))
            return false;
    }
    return true;
}
