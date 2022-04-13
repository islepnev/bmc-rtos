/*
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

#include "dev_fpga.h"

#include <string.h>

#include "../ad9545/dev_ad9545.h"
#include "../ad9548/dev_ad9548.h"
#include "app_tasks.h"
#include "app_shared_data.h"
#include "bsp_fpga.h"
#include "bswap.h"
#include "dev_fpga_types.h"
#include "dev_fpga_board_func.h"
#include "devicelist.h"
#include "display.h"
#include "fpga_mcu_regs_v2_0.h"
#include "fpga_io.h"
#include "fpga_spi_hal.h"
#include "log/log.h"
#include "mac_address.h"
#include "powermon/dev_powermon_types.h"
#include "sdb_util.h"
#include "system_status.h"
#include "system_status_common.h"
#include "thset/dev_thset_types.h"
#include "version.h"

static uint16_t live_magic = 0x55AA;
static const bool ENABLE_V2_PROTOCOL = true;

static const int REG_CSR_DEVICE_ID = 0x42;

enum {
    FPGA_SPI_ADDR_0 = FPGA_REG_BASE_MCU + 0,
    FPGA_SPI_ADDR_1 = FPGA_REG_BASE_MCU + 1,
    FPGA_SPI_ADDR_2 = FPGA_REG_BASE_MCU + 2,
    FPGA_SPI_ADDR_3 = FPGA_REG_BASE_MCU + 3,
    FPGA_SPI_ADDR_4 = FPGA_REG_BASE_MCU + 4,
    FPGA_SPI_ADDR_5 = FPGA_REG_BASE_MCU + 5,
    FPGA_SPI_ADDR_6 = FPGA_REG_BASE_MCU + 6,
    FPGA_SPI_ADDR_7 = FPGA_REG_BASE_MCU + 7,
    FPGA_SPI_ADDR_8 = FPGA_REG_BASE_MCU + 8,
    FPGA_SPI_ADDR_9 = FPGA_REG_BASE_MCU + 9,
    FPGA_SPI_ADDR_A = FPGA_REG_BASE_MCU + 0xA,
    FPGA_SPI_ADDR_B = FPGA_REG_BASE_MCU + 0xB,
    FPGA_SPI_ADDR_C = FPGA_REG_BASE_MCU + 0xC,
    FPGA_SPI_ADDR_D = FPGA_REG_BASE_MCU + 0xD,
    FPGA_SPI_ADDR_E = FPGA_REG_BASE_MCU + 0xE,
    FPGA_SPI_ADDR_F = FPGA_REG_BASE_MCU + 0xF,
    FPGA_SPI_ADDR_SDB_BASE = 0x7C00,
};

#if 0
static bool fpga_test_reg(DeviceBase *dev, uint16_t addr, uint16_t wdata, uint16_t *rdata)
{
    BusInterface *bus = &dev->bus;
    return fpga_w16(bus, addr, wdata) &&
           fpga_r16(bus, addr, rdata) &&
           rdata &&
           (wdata == *rdata);
}
#endif

static bool fpga_write_live_magic(struct Dev_fpga *dev)
{
    uint16_t addr1 = 0x000E;
    uint16_t addr2 = 0x000F;
    live_magic++;
    uint16_t wdata1 = live_magic;
    uint16_t wdata2 = ~live_magic;
    return fpga_w16(dev, addr1, wdata1) &&
           fpga_w16(dev, addr2, wdata2);
}

bool fpga_check_live_magic(struct Dev_fpga *dev)
{
    uint16_t addr1 = 0x000E;
    uint16_t addr2 = 0x000F;
    uint16_t rdata1 = 0, rdata2 = 0;
    if (!fpga_r16(dev, addr1, &rdata1))
        return false;
    if (!fpga_r16(dev, addr2, &rdata2))
        return false;
    uint16_t test1 = live_magic;
    uint16_t test2 = ~live_magic;
    if ((rdata1 != test1) || (rdata2 != test2)) {
        log_put(LOG_ERR, "FPGA register contents unexpectedly changed");
        return false;
    }
    return fpga_write_live_magic(dev);
}

bool fpga_test_v2(struct Dev_fpga *dev)
{
    uint16_t addr1 = 0x000E;
    uint16_t addr2 = 0x000F;
    uint16_t wdata1 = 0x3210;
    uint16_t wdata2 = 0xDCBA;
    uint16_t rdata1 = 0, rdata2 = 0;
    if (! fpga_w16(dev, addr1, wdata1))
        goto err;
    if (! fpga_w16(dev, addr2, wdata2))
        goto err;
    if (! fpga_r16(dev, addr1, &rdata1))
        goto err;
    if (! fpga_r16(dev, addr2, &rdata2))
        goto err;
    if (rdata1 == wdata1 && rdata2 == wdata2) {
        // log_printf(LOG_DEBUG, "FPGA register test Ok: addr1 %04X, wdata1 %04X, rdata1 %04X", addr1, wdata1, rdata1);
        // log_printf(LOG_DEBUG, "FPGA register test Ok: addr2 %04X, wdata2 %04X, rdata2 %04X", addr2, wdata2, rdata2);
        fpga_write_live_magic(dev);
        return true;
    }
    log_printf(LOG_ERR, "FPGA register test failed: addr1 %04X, wdata1 %04X, rdata1 %04X", addr1, wdata1, rdata1);
    log_printf(LOG_ERR, "FPGA register test failed: addr2 %04X, wdata2 %04X, rdata2 %04X", addr2, wdata2, rdata2);
    return false;
err:
    log_printf(LOG_ERR, "FPGA register test failed: SPI error");
    return false;
}

bool fpga_read_base_csr(struct Dev_fpga *dev)
{
    // read Firmware Version
    {
        uint16_t data[2] = {0};
        if (
                fpga_r16(dev, 0x4C, &data[0]) &&
                fpga_r16(dev, 0x4D, &data[1])
                ) {
            dev->priv.fpga.fw_ver = data[0] & 0xFFFF;
            dev->priv.fpga.fw_rev = data[1] & 0xFFFF;
        } else {
            return false;
        }
    }
    // read 1Wire ID
    {
        uint64_t data;
        if (!fpga_r64(dev, 0x50, &data))
            return false;
        dev->priv.fpga.ow_id = data;
    }
    return true;
}

bool fpga_read_info(struct Dev_fpga *dev)
{
    // read Temp
    {
        uint16_t data = {0};
        if (fpga_r16(dev, 0x4B, &data)) {
            dev->priv.fpga.temp = data & 0xFFFF;
        } else {
            return false;
        }
    }
//    uint64_t data = 0;
//    for (int addr=0x40; addr<0x60; addr++) {
//        if (! fpga_spi_v3_hal_read_reg(bus, addr, &data)) {
//            log_printf(LOG_ERR, "%s failed", __func__);
//            return false;
//        }
//        log_printf(LOG_INFO, "FPGA Reg %X = %8llX", addr, data);
//    }

//    if (! fpga_spi_v3_hal_read_reg(bus, 0x50, &data)) {
//        log_printf(LOG_ERR, "%s failed", __func__);
//        return false;
//    }
//    dev->priv.serial = data;
    return true;
}

void fpga_print_sdb(struct Dev_fpga *dev)
{
    const Dev_fpga_sdb *sdb = &dev->priv.fpga.sdb;
    {
        const struct sdb_synthesis *syn = &sdb->syn;
        char date_str[16] = {0};
        snprint_sdb_date(date_str, sizeof(date_str), syn->date);
        enum { commit_id_len = sizeof(syn->commit_id) };
        char commit_id[commit_id_len+1] = {0};
        sdb_copy_printable(commit_id, syn->commit_id, commit_id_len, '\0');
        enum { user_name_len = sizeof(syn->user_name) };
        char user_name[user_name_len+1] = {0};
        sdb_copy_printable(user_name, syn->user_name, user_name_len, '\0');
        if ((strlen(commit_id) > 0) || (strlen(user_name) > 0))
            log_printf(LOG_INFO, "SDB: build %s by %s on %s", commit_id, user_name, date_str);
    }
    const struct sdb_interconnect *ic = &sdb->ic;
    {
        const struct sdb_product *product = &ic->sdb_component.product;
        char version_str[16] = {0};
        snprint_sdb_version(version_str, sizeof(version_str), product->version);
        char date_str[16] = {0};
        snprint_sdb_date(date_str, sizeof(date_str), product->date);
        char name[20] = {0};
        sdb_copy_printable(name, product->name, sizeof(product->name), '\0');

        log_printf(LOG_INFO, "SDB: device %02X %s v%s, %s",
                   product->device_id,
                   name,
                   version_str,
                   date_str);
    }
    for (int i=0; i<ic->sdb_records - 1; i++) {
        if (i >= SDB_MAX_RECORDS) {
            log_printf(LOG_WARNING, "  <too many devices>");
            break;
        }
        struct sdb_device *d = &dev->priv.fpga.sdb.devices[i];
        if (d->sdb_component.product.record_type == sdb_type_empty)
            continue;
        if (!sdb_dev_validate(d)) {
            log_printf(LOG_WARNING, "  <invalid record>");
        }
        char name[20];
        sdb_copy_printable(name, d->sdb_component.product.name, sizeof(d->sdb_component.product.name), ' ');
        name[19] = 0;
        log_printf(LOG_INFO, "  %06x-%06x: %08X v%d.%d %s",
                   (uint16_t)d->sdb_component.addr_first / REGIO_WORD_SIZE,
                   (uint16_t)d->sdb_component.addr_last / REGIO_WORD_SIZE,
                   (uint32_t)d->sdb_component.product.device_id,
                   d->abi_ver_major,
                   d->abi_ver_minor,
                   name);
    }
}

static bool SDB_DUMP = false;

bool fpga_read_sdb(struct Dev_fpga *dev)
{
    uint32_t sdb_magic;
    if (! fpga_r32(dev, FPGA_SPI_ADDR_SDB_BASE, &sdb_magic))
        return false;
    sdb_magic = ntohl(sdb_magic);
    if (sdb_magic != SDB_MAGIC) {
        log_printf(LOG_INFO, "SDB not found at %04X: read magic %08X, expected %08X",
                   FPGA_SPI_ADDR_SDB_BASE, sdb_magic, SDB_MAGIC);
        return true;
    }
    /*
    uint32_t csr[0x10];
    if (! fpga_read(dev, 0x40, &csr, sizeof (csr)))
        return false;
    log_printf(LOG_DEBUG, "CSR dump:");
    hexdump(&csr, sizeof(csr));
    */
    Dev_fpga_sdb *sdb = &dev->priv.fpga.sdb;
    if (! fpga_read(dev, FPGA_SPI_ADDR_SDB_BASE, sdb, sizeof (*sdb)))
        return false;
    struct sdb_interconnect *ic = &sdb->ic;
    if (SDB_DUMP) {
        log_printf(LOG_DEBUG, "SDB interconnect dump:");
        hexdump(ic, sizeof(*ic));
    }
    sdb_interconnect_fix_endian(ic);
    if (!sdb_ic_validate(ic)) {
        log_printf(LOG_WARNING, "SDB interconnect structure invalid");
        return false;
    }
    struct sdb_synthesis *syn = &sdb->syn;
    sdb_synthesis_fix_endian(syn);
    bool ok = true;
    for (int i=0; i<ic->sdb_records - 1; i++) {
        struct sdb_device *d = &sdb->devices[i];

        if (SDB_DUMP) {
            log_printf(LOG_DEBUG, "SDB device(%d) dump:", i);
            hexdump(d, sizeof(*d));
        }
        sdb_device_fix_endian(d);
        if (d->sdb_component.product.record_type == sdb_type_empty)
            continue;
        if (d->sdb_component.product.record_type == sdb_type_device) {
            if (!sdb_dev_validate(d))
                ok = false;
        }
    }
    fpga_print_sdb(dev);
    return ok;
}

bool fpga_detect_v2(Dev_fpga *dev)
{
    // read v1/v2 DeviceID
    uint16_t data = 0;
    if (! fpga_r16(dev, 0, &data)) {
        return false;
    }
    if (data == 0 || data == 0xFFFF)
        return false;
    uint8_t data_hi = (data >> 8) & 0xFF;
    uint8_t data_lo = data & 0xFF;
    if (data_hi == data_lo) {
        log_printf(LOG_INFO, "FPGA SPI v1 detected, device_id %02X", data_lo);
        dev->priv.fpga.proto_version = 1;
        dev->priv.fpga.id = data & 0xFF;
        dev->priv.fpga.id_read = 1;
        return true;
    }
    if (data_hi == (~data_lo & 0xFF) && data_lo != 0 && data_lo != 0xFF) {
        log_printf(LOG_INFO, "FPGA SPI v2 detected, device_id %02X", data_lo);
        dev->priv.fpga.proto_version = 2;
        dev->priv.fpga.id = data & 0xFF;
        dev->priv.fpga.id_read = 1;
        return true;
    }
    return false;
}

bool fpga_detect_v3(Dev_fpga *dev)
{
//    // FIXME
//    while (true) {
//        uint64_t data = 0;
//        for (int i=0; i<1000; i++) {
//            if (!fpga_spi_v3_hal_read_reg(bus, 0x0042, &data))
//                break;
//        }
//        fpga_spi_v3_hal_read_status(bus);
//        osDelay(100);
//    }
    uint64_t data = 0;
    if (!fpga_spi_v3_hal_read_reg(&dev->dev.bus, REG_CSR_DEVICE_ID, &data)) {
        return false;
    }
    // read Device ID
    {
        uint8_t id = (data >> 8) & 0xFF;
        if (id == 0 || id == 0xFF) {
            return false;
        }
        log_printf(LOG_INFO, "FPGA SPI v3 detected, device_id %02X", id);
        dev->priv.fpga.proto_version = 3;
        dev->priv.fpga.id = id;
        dev->priv.fpga.id_read = 1;
    }
    if (!fpga_read_base_csr(dev))
        return false;
    log_printf(LOG_INFO, "FPGA firmware %d.%d.%d",
               (dev->priv.fpga.fw_ver >> 8) & 0xFF,
               (dev->priv.fpga.fw_ver) & 0xFF,
               dev->priv.fpga.fw_rev);
    log_printf(LOG_INFO, "Board serial %04X-%04X-%04X",
               (uint16_t)((dev->priv.fpga.ow_id >> 40) & 0xFFFF),
               (uint16_t)((dev->priv.fpga.ow_id >> 24) & 0xFFFF),
               (uint16_t)((dev->priv.fpga.ow_id >> 8) & 0xFFFF));
    if (!fpga_read_info(dev))
        return false;
    if (!fpga_read_sdb(dev))
        return true; // not a problem
    return true;
}

bool fpgaDetect(Dev_fpga *dev)
{
    int err = 0;

    dev->priv.fpga.proto_version = 0;

    if (ENABLE_V2_PROTOCOL && fpga_detect_v2(dev)) {
        for (int i=0; i<FPGA_REG_COUNT; i++) {
            if (! fpga_r16(dev, i, &dev->priv.fpga.regs[i])) {
                err++;
                return false;
            }
        }
        dev->dev.device_status = DEVICE_NORMAL;
        return true;
    }
    if (fpga_detect_v3(dev)) {
        dev->dev.device_status = DEVICE_NORMAL;
        return true;
    }

    dev->dev.device_status = DEVICE_FAIL;
    return false;
}

bool fpga_test_v3(Dev_fpga *dev)
{
    uint32_t addr1 = 0x0048;
    uint32_t addr2 = 0x0100;
    uint16_t wdata1 = 0x3210;
    uint16_t wdata2 = 0xDCBA;
    uint16_t rdata1 = 0, rdata2 = 0;
    if (! fpga_w16(dev, addr1, wdata1))
        goto err;
    if (! fpga_w16(dev, addr2, wdata2))
        goto err;
    if (! fpga_r16(dev, addr1, &rdata1))
        goto err;
    if (! fpga_r16(dev, addr2, &rdata2))
        goto err;
    if (rdata1 == wdata1 && rdata2 == wdata2) {
        // log_printf(LOG_DEBUG, "FPGA register test Ok: addr1 %04X, wdata1 %04X, rdata1 %04X", addr1, wdata1, rdata1);
        // log_printf(LOG_DEBUG, "FPGA register test Ok: addr2 %04X, wdata2 %04X, rdata2 %04X", addr2, wdata2, rdata2);
//        fpga_write_live_magic(d);
        return true;
    }
    log_printf(LOG_ERR, "FPGA register test failed: addr1 %04X, wdata1 %04X, rdata1 %04X", addr1, wdata1, rdata1);
    log_printf(LOG_ERR, "FPGA register test failed: addr2 %04X, wdata2 %04X, rdata2 %04X", addr2, wdata2, rdata2);
    return false;
err:
    log_printf(LOG_ERR, "FPGA register test failed: SPI error");
    return false;
}

static bool fpga_v1_WriteBmcVersion(Dev_fpga *dev)
{
    bmc_version_t bmc_version;
    bmc_version.b.major = VERSION_MAJOR_NUM;
    bmc_version.b.minor = VERSION_MINOR_NUM;
    uint16_t bmc_revision = VERSION_PATCH_NUM;

    if (! fpga_w16(dev, FPGA_SPI_ADDR_0, bmc_version.raw))
        return false;
    if (! fpga_w16(dev, FPGA_SPI_ADDR_7, bmc_revision))
        return false;
    return true;
}

bool fpgaWriteBmcVersion(Dev_fpga *dev)
{
    bmc_version_t bmc_version;
    bmc_version.b.major = VERSION_MAJOR_NUM;
    bmc_version.b.minor = VERSION_MINOR_NUM;
    uint16_t bmc_revision = VERSION_PATCH_NUM;

    if (! fpga_w16(dev, FPGA_SPI_ADDR_8, bmc_version.raw))
        return false;
    if (! fpga_w16(dev, FPGA_SPI_ADDR_9, bmc_revision))
        return false;
    return true;
}

bool fpgaWriteBmcTemperature(Dev_fpga *dev)
{
    const Dev_thset_priv *p = get_thset_priv_const();
    if (!p)
        return true;

    for (int i=0; i<4; i++) {
        int16_t v = (i < p->count && p->sensors[i].hdr.b.state == DEVICE_NORMAL)
                        ? (p->sensors[i].value * 32)
                        : 0x8000;
        if (! fpga_w16(dev, FPGA_SPI_ADDR_3 + i, v))
            return false;
    }
    return true;
}

bool fpgaWriteBmcNetworkInfo(Dev_fpga *dev)
{
#if defined(ENABLE_ETHERNET)
    if (tcpipThreadId) {
        uint8_t macaddress[6];
        get_mac_address(macaddress);
        uint16_t mac_low = ((uint16_t)(macaddress[4]) << 8) | macaddress[5];
        uint16_t mac_mid = ((uint16_t)(macaddress[2]) << 8) | macaddress[3];
        uint32_t ipv4_le = ntohl(app_ipv4);
        uint16_t ip_lo = (ipv4_le & 0xFFFF);
        uint16_t ip_hi = ((ipv4_le >> 16) & 0xFFFF);
        // TODO: check eth_link_up
        if (! fpga_w16(dev, FPGA_REG_BASE_MCU + 0x1C, mac_low))
            return false;
        if (! fpga_w16(dev, FPGA_REG_BASE_MCU + 0x1D, mac_mid))
            return false;
        if (! fpga_w16(dev, FPGA_REG_BASE_MCU + 0x1E, ip_lo))
            return false;
        if (! fpga_w16(dev, FPGA_REG_BASE_MCU + 0x1F, ip_hi))
            return false;
    }
#endif
    return true;
}

bool fpgaWritePllStatus(Dev_fpga *dev)
{
    fpga_mcu_reg_pll_t pll = {0};
    uint16_t unlock_count = 0;
#if ENABLE_AD9545
    const DeviceBase *d = find_device_const(DEV_CLASS_AD9545);
    if (!d || !d->priv)
        return false;
    const Dev_ad9545_priv *priv = (Dev_ad9545_priv *)device_priv_const(d);

    pll.b.locked = (SENSOR_NORMAL == d->sensor) &&
                 priv->status.sysclk.b.pll0_locked;
    pll.b.ref_a_valid = priv->status.ref[0].b.valid;
    pll.b.ref_b_valid = priv->status.ref[2].b.valid;
#endif
#if ENABLE_AD9548
    const DeviceBase *d = find_device_const(DEV_CLASS_AD9548);
    if (!d || !d->priv)
        return true;
    const Dev_ad9548_priv *priv = (Dev_ad9548_priv *)device_priv_const(d);

    unlock_count = priv->status.pll_unlock_cntr <= 65535
                       ? priv->status.pll_unlock_cntr : 65535;
    bool pll_locked = priv->status.DpllStat.b.dpll_freq_lock && priv->status.DpllStat.b.dpll_phase_lock;
    pll.b.locked = (pll_locked && (SENSOR_NORMAL == d->sensor));
    pll.b.active_ref = priv->status.DpllStat2.b.active_ref >> 1;
    pll.b.ref_a_valid = priv->status.refStatus[0].b.valid;
    pll.b.ref_b_valid = priv->status.refStatus[2].b.valid;
    pll.b.ref_c_valid = priv->status.refStatus[4].b.valid;
    pll.b.ref_d_valid = priv->status.refStatus[6].b.valid;
#endif
    if (! fpga_w16(dev, FPGA_SPI_ADDR_1, pll.raw))
        return false;
    if (! fpga_w16(dev, FPGA_SPI_ADDR_2, unlock_count))
        return false;
    return true;
}

bool fpgaWriteSystemStatus(Dev_fpga *dev)
{
    uint16_t data = 0;
    data = getSystemStatus();
    if (! fpga_w16(dev, FPGA_SPI_ADDR_A, data))
        return false;
#ifdef ENABLE_POWERMON
    data = getPowermonStatus();
#else
    data = 0;
#endif
    if (! fpga_w16(dev, FPGA_SPI_ADDR_B, data))
        return false;
    data = getPllStatus();
    if (! fpga_w16(dev, FPGA_SPI_ADDR_C, data))
        return false;
    return true;
}

#ifdef ENABLE_SENSORS
static bool fpgaWriteSensorsByIndex(Dev_fpga *dev, int *indices, int count)
{
    const Dev_powermon_priv *p = get_powermon_priv_const();
    if (!p)
        return false;
    const pm_sensors_arr *sensors = &p->sensors;
    uint16_t address = FPGA_SPI_ADDR_0 + 0x10;

    for (int i=0; i<count; i++) {
        int index = indices[i];
        const pm_sensor_priv *p = &sensors->arr[index].priv;
        if (!fpga_w16(dev, address++,
                                    (int16_t)(p->busVoltage * 1000)) ||
            !fpga_w16(dev, address++,
                                   (int16_t)(p->current * 1000)))
            return false;
    }
    return true;
}

bool fpgaWriteSensors(struct Dev_fpga *dev)
{
    return fpgaWriteSensorsByIndex(
        dev, fpga_sensor_map.indices, fpga_sensor_map.count);
}
#else
bool fpgaWriteSensors(struct Dev_fpga *dev)
{
    return true;
}
#endif

bool fpga_periodic_task_v1(struct Dev_fpga *dev)
{
    return
//        fpga_check_live_magic(dev) &&
        fpga_v1_WriteBmcVersion(dev) &&
        fpgaWriteBmcTemperature(dev) &&
        fpgaWritePllStatus(dev) &&
//        fpgaWriteSystemStatus(dev) &&
//        fpgaWriteSensors(dev)
        true;
}

bool fpga_periodic_task_v2(struct Dev_fpga *dev)
{
    return
        fpga_test_v2(dev) &&
        fpga_check_live_magic(dev) &&
        fpgaWriteBmcVersion(dev) &&
        fpgaWriteBmcTemperature(dev) &&
        fpgaWritePllStatus(dev) &&
        fpgaWriteSystemStatus(dev) &&
        fpgaWriteSensors(dev);
}

bool fpga_periodic_task_v3(struct Dev_fpga *dev)
{
// destructive test
//    int nloops = 3;
//    for (int i=0; i<nloops; i++) {
//        if (!fpga_test_v3(dev))
//            return false;
//    }
    bool ok =
            fpga_read_info(dev) &&
            fpgaWriteBmcVersion(dev) &&
            fpgaWriteBmcTemperature(dev) &&
            fpgaWriteBmcNetworkInfo(dev) &&
            fpgaWritePllStatus(dev) &&
            fpgaWriteSystemStatus(dev) &&
            fpgaWriteSensors(dev) &&
            fpgaBoardSpecificPoll(dev);
    return ok;
}

bool fpga_periodic_task(struct Dev_fpga *dev)
{
    if (dev->priv.fpga.proto_version == 1) {
        return fpga_periodic_task_v1(dev);
    }
    if (dev->priv.fpga.proto_version == 2) {
        return fpga_periodic_task_v2(dev);
    }
    if (dev->priv.fpga.proto_version == 3) {
        return fpga_periodic_task_v3(dev);
    }
    return false;
}
