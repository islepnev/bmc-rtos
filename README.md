# BMC-RTOS

**Board Management Controller Real-Time Operating System**

![MCB-32F769](doc/mcb-32f769-v1.1_320px.png)

BMC-RTOS is a lightweight, deterministic real-time firmware designed for STM32-based Board Management Controllers (BMCs) in high-energy physics detector systems. Built on **FreeRTOS**, it provides unified hardware monitoring, clock distribution management, power supervision, and remote network access across multiple board platforms.

The architecture follows a clean five-layer design (HAL → Drivers → Device Management with FSMs → Tasks → Application) and supports hierarchical device trees, status aggregation, and seamless multi-board operation—including the advanced VXSIIC (VXS Switched Serial I2C) framework for crate-level backplane monitoring.

### Supported Boards

- CRU-16 v1.0 + MCB32F769 v1.1
- TDC64VHLE v2.0
- TDC72VHL v4.2
- TTVXS v1.1 (VXSIIC master)
- TQDC-16 series (VXSIIC slave payloads)

### Key Features

- Real-time task scheduling with strict 10 ms / 50 ms periodicity
- Device abstraction layer with FSM-based lifecycle management
- Multi-bus support (I2C, SPI) with mutex-protected access
- TCP/IP, SNMP, and LLDP network stack (LwIP)
- VXSIIC master/slave framework for VXS backplane monitoring
- Comprehensive remote monitoring and configuration via SNMP
- Board-variant support via compile-time flags

### Build Instructions

1. Download and unpack the **GNU Arm Embedded Toolchain** (recommended: `gcc-arm-none-eabi-10.3-2021.10` or later).
2. Set the `TOOLCHAIN_PREFIX` environment variable (e.g. `/opt/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-`).
3. Run:
   ```bash
   make
   ```
   (or `make BOARD=CRU16` / `make BOARD=TTVXS` for specific variants).

### Documentation

- [Architecture & Design](https://github.com/islepnev/bmc-rtos/blob/docs/docs/Architecture_and_Design.md)
- [CRU-16 Board Control](https://afi-project.jinr.ru/projects/cru-16/wiki/CRU-16_v10_Board_Control)
- [TDC64VHLE v2.0 Board Control](https://afi-project.jinr.ru/projects/tdc64vhle/wiki/TDC64VHLE_v20_Board_Control)
- [TDC72VHL v4 Board Management](https://afi-project.jinr.ru/projects/tdc72vhl/wiki/TDC72VHL_V4_Board_Management)
- [TDC72VXS4 MCU Software](https://afi-project.jinr.ru/projects/tdc72vhl/wiki/TDC72VXS4_MCU_Software)
- [MCU Programming Guide](https://afi-project.jinr.ru/projects/tdc72vhl/wiki/MCU_Programming)

---

For questions, contributions, or board support requests, please open an issue or pull request.
