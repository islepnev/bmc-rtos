#ifndef AFISDBID_H
#define AFISDBID_H

enum AFI_SDB_ID {
    AFI_SDB_UNKNOWN             = 0x00000000,

    AFI_SDB_AD5328_x2_SPI       = 0x3524236a,
    AFI_SDB_ADC_Core            = 0xfd4368fa,
    AFI_SDB_Clock_Control       = 0x7bc301a9,
    AFI_SDB_DDR3_Stream_FIFO    = 0xf6a4e20c,
    AFI_SDB_Deserializers_Contr = 0x3efb27ee,
    AFI_SDB_ETH_PORT_V1         = 0xe050724f,
    AFI_SDB_FE_LINK_CSR         = 0x371a0421,
    AFI_SDB_Flash               = 0x6e0f7007,
    AFI_SDB_HPTDC_RegIO_JTAG    = 0x1492d6c0,
    AFI_SDB_HWIP_ERR_CNTS       = 0x2a2a5a63,
    AFI_SDB_Input_Hit_Counters  = 0xb695b181,
    AFI_SDB_LTM9011_x2_SPI      = 0xde2766bc,
    AFI_SDB_MCU                 = 0x47567e3c, // from TQDC v2.0
    AFI_SDB_MCU1                = 0x8df3221b, // from TQDC v1
    AFI_SDB_MLINK_CSR           = 0x41a37c29,
    AFI_SDB_MLINK_TADC_CSR      = 0x50b1d9c7,
    AFI_SDB_MSC_CYCLE_COUNTERS  = 0x77268a46,
    AFI_SDB_MSC_HISTOGRAMS      = 0x8da907fb,
    AFI_SDB_MSC_INPUT           = 0xd26e4053,
    AFI_SDB_MSC_STREAM_READOUT  = 0xfd880c84,
    AFI_SDB_SDB                 = 0x9d754a61,
    AFI_SDB_STAT_READOUT_CTRL   = 0xfb0bcd7f,
    AFI_SDB_TDC_RUN_STAT_CNTS   = 0x5d28c101,
    AFI_SDB_TDC_Trigger_Control = 0x76b80b88,
    AFI_SDB_TRC_ERC             = 0x891f9f31,
    AFI_SDB_TRC_SYSTEM          = 0xe347158d,
    AFI_SDB_TTL_IO_CTRL         = 0x8857c5c8,
    AFI_SDB_WR_ENDPOINT_STATE   = 0x6dbf32d6,
    AFI_SDB_WR_STATUS           = 0xe5ea34bf,
    AFI_SDB_WR_TIME_EMULATOR    = 0x60120c01,
};

#endif // AFISDBID_H
