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
    BusInterface bus = d->dev.bus;
    bus.address = SFP_MAIN_I2C_ADDRESS;
    uint16_t reg = SFF_8436_TX_DISABLE_REG;
    uint8_t val = 1;
    if (! sfpiic_mem_read(&bus, reg, &val, 1))
        return false;
    if ((val&0xF)==0)
        return true;

    ++d->priv.status.sfp[ch].tx_en_cnt;
    val &= ~0xF;
    if (! sfpiic_mem_write(&bus, reg, &val, 1))
        return false;
    val=1;
    if (! sfpiic_mem_read(&bus, reg, &val, 1))
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

bool dev_sfpiic_parse_id(sfpiic_ch_status_t *status)
{
    const unsigned char *id = &status->idprom[0];
    bool is_sfp = id[0] == SFF8024_ID_SFP;
    bool is_qsfp = id[0] == SFF8024_ID_QSFP ||
                   id[0] == SFF8024_ID_QSFP28 ||
                   id[0] == SFF8024_ID_QSFP_PLUS;
    bool is_cxp = id[0] == SFF8024_ID_CXP;
    bool supported = (is_sfp || is_qsfp || is_cxp);
    status->supported = supported;
    if (!supported)
        return false;
    status->is_cxp = is_cxp;
    status->is_sfp = is_sfp;
    status->is_qsfp = is_qsfp;
    return true;
}

bool dev_sfpiic_parse_sff_8636(sfpiic_ch_status_t *status)
{
    const unsigned char *id = &status->idprom[0];
    status->transceiver = id[128];
    status->connector = id[130];
    status->compliance_ethernet_10_40_100 = id[131];
    status->compliance_ethernet_1g = id[134];
    status->compliance_extended = id[192];
    copy_printable(status->vendor_name, &id[148], 16);
    copy_printable(status->vendor_pn, &id[168], 16);
    copy_printable(status->vendor_serial, &id[196], 16);
    // status->dom_supported = (id[220] != 0);
    status->temp = (int16_t)(id[22] << 8 | id[23]) / 256.0;
    status->voltage = (uint16_t)(id[26] << 8 | id[27]) / 10000.0;
    for (int i=0; i<4; i++) {
        double rx_power = (uint16_t)(id[34+2*i] << 8 | id[35+2*i]) / 10000.0;
        status->rx_power_mw[i] = rx_power;
        double bias_cur = (uint16_t)(id[42+2*i] << 8 | id[43+2*i]) / 10000.0;
        status->bias_cur[i] = bias_cur;
        double tx_power = (uint16_t)(id[50+2*i] << 8 | id[51+2*i]) / 10000.0;
        status->tx_power_mw[i] = tx_power;
    }
    status->dom_supported = true;
    status->dom_updated_timetick = osKernelSysTick();
    return true;
}

bool dev_sfpiic_parse_idprom(sfpiic_ch_status_t *status)
{
    const unsigned char *id = &status->idprom[0];

    if (!status->supported)
        return false;
    if (id[0] == SFF8024_ID_SFP) {
        status->transceiver = id[0];
        status->connector = id[2];
        status->compliance_ethernet_10_40_100 = id[3];
        status->compliance_ethernet_1g = id[6];
        status->compliance_extended = id[192];
        copy_printable(status->vendor_name, &id[20], 16);
        copy_printable(status->vendor_pn, &id[40], 16);
        copy_printable(status->vendor_serial, &id[68], 16);
    }
    if (id[0] == SFF8024_ID_QSFP_PLUS || id[0] == SFF8024_ID_QSFP28) {
        if (id[0] != id[128]) {
            return false; // SFF-8636 6.2.1
        }
        if (id[1] >= 0x03) {
            if (! dev_sfpiic_parse_sff_8636(status))
                return false;
        } else {
            //            if (! dev_sfpiic_parse_sff_8436(status))
            //                return false;
            if (! dev_sfpiic_parse_sff_8636(status))
                return false;
        }
    }
    return true;
}

bool dev_sfpiic_ch_read_idprom(Dev_sfpiic *d, uint8_t ch)
{
    BusInterface bus = d->dev.bus;
    bus.address = SFP_MAIN_I2C_ADDRESS;
    sfpiic_ch_status_t *status = &d->priv.status.sfp[ch];
    const unsigned char *id = &status->idprom[0];
    // read lower page 0-127
    if (! sfpiic_mem_read(&bus, 0, (uint8_t *)id, 128))
        return false;

    // read higher page 128-255
    if (id[0] == SFF8024_ID_QSFP ||
        id[0] == SFF8024_ID_QSFP_PLUS ||
        id[0] == SFF8024_ID_QSFP28) {
        uint8_t page_select = 0;
        if (! sfpiic_mem_write(&bus, 127, (uint8_t *)&page_select, 1))
            return false;
        if (! sfpiic_mem_read(&bus, 128, (uint8_t *)&id[128], 128))
            return false;
    }

    //    if (! sfpiic_mem_read(SFP_MAIN_I2C_ADDRESS, 0, (uint8_t *)id, 256))
//        return false;
    // TODO: validate checksum
    if (! dev_sfpiic_parse_idprom(status)) {
        return false;
    }
    return true;
}

bool dev_sfpiic_ch_update(Dev_sfpiic *d, uint8_t ch)
{
    BusInterface bus = d->dev.bus;
    bus.address = SFP_MAIN_I2C_ADDRESS;
    BusInterface bus2 = d->dev.bus;
    bus2.address = SFP_MAIN_I2C_ADDRESS + 1;
    if (! sfpiic_get_ch_i2c_status(&bus))
        return false;

    const uint32_t now = osKernelSysTick();
    sfpiic_ch_status_t *status = &d->priv.status.sfp[ch];
    bool old_present = status->present;
    enum {SFP_ID_SIZE = 3};
    const unsigned char *id = &status->idprom[0];
    // one retry
    bool present = sfpiic_mem_read(&bus, 0, (uint8_t *)id, SFP_ID_SIZE) ||
                   sfpiic_mem_read(&bus, 0, (uint8_t *)id, SFP_ID_SIZE);
    dev_sfpiic_update_ch_state(d, ch, present);
    status->present = present;
    status->id_updated_timetick = now;
    if (!present) {
        return false;
    }
    bool id_supported = dev_sfpiic_parse_id(status);
    if (!id_supported)
        return false;

    if (! dev_sfpiic_ch_read_idprom(d, ch)) {
        present = false;
    }

    if (status->is_qsfp) {
        // reset TX_DISABLE
        uint8_t data = 0;
        if (! sfpiic_mem_read(&bus, SFF_8436_TX_DISABLE_REG, (uint8_t *)&data, 1))
            return false;
        if (data) {
            log_printf(LOG_INFO, "%s: clearing TX_DISABLE %02X", d->priv.portName[ch], data);
            if (! sfpiic_mem_write(&bus, SFF_8436_TX_DISABLE_REG, (uint8_t *)&data, 1))
                return false;
        }
    }
    if (status->is_sfp) {
        bool a2_present = sfpiic_get_ch_i2c_status(&bus2);
        if (!a2_present) {
            status->dom_supported = false;
            return true;
        }
        uint32_t elapsed = now - status->dom_updated_timetick;
        // run every second
        if (elapsed > 1000) {
            // detect DOM
            unsigned char *diag = &d->priv.status.sfp[ch].diag[0];
            if (! sfpiic_mem_read(&bus2, 0, (uint8_t *)diag, 256)) {
                status->dom_supported = false;
                return false;
            }

            struct sff_diags sd;
            sff8472_parse_eeprom(status->idprom, &sd);
            if (sd.supports_dom) {
                status->temp = sd.sfp_temp[MCURR] / 256.0;
                status->voltage = sd.sfp_voltage[MCURR] / 10000.0;
                status->rx_power_mw[0] = sd.rx_power[MCURR] / 10000.0;
                status->tx_power_mw[0] = sd.tx_power[MCURR] / 10000.0;
                status->bias_cur[0] = sd.bias_cur[MCURR] / 10000.0;
                status->dom_updated_timetick = now;
                status->dom_supported = true;
            } else {
                status->dom_supported = false;
            }
        }
    }
    if (status->is_qsfp) {
        if (! dev_sfpiic_ch_enable_tx(d, ch))
            return false;
    }
    return true;
}
