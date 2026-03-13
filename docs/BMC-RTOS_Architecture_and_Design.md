---
doc_id: BMC-RTOS-ARCH
title: BMC-RTOS Architecture and Design
status: draft
system: bmc
component: rtos
platform: stm32f769
authors:
  - Ilia Slepnev
created: 2026-03-17
sources:
  - software
  - firmware
  - codebase
confidence_policy:
  confirmed: direct code and configuration evidence
  inferred: derived from architecture and integration analysis
---
# BMC-RTOS Architecture and Design

## Table of Contents

1. [Overview](#overview)
2. [System Architecture](#system-architecture)
3. [Core Components](#core-components)
4. [Data Flow](#data-flow)
5. [Configuration](#configuration)
6. [Networking](#networking)
7. [Performance](#performance)
8. [Error Handling](#error-handling)
9. [Code Organization](#code-organization)
10. [Design Patterns](#design-patterns)
11. [VXS Switched Serial I2C Framework](#vxs-switched-serial-i2c-framework)

## Overview

BMC-RTOS is a lightweight, deterministic real-time firmware for STM32-based Board Management Controllers (BMCs). The system supervises board-level infrastructure in instrumentation electronics, including clock distribution, power monitoring, thermal sensing, EEPROM/configuration storage, FPGA service interfaces, and backplane communication.

The system manages hardware components across multiple communication buses (I2C, SPI) and network protocols (SNMP, LLDP) using FreeRTOS as the underlying kernel.

The architecture is designed for multi-board support with variants including CRU-16, TDC series, TQDC-16, and TTVXS platforms, enabling consistent board management across detector and readout systems.

## System Architecture

### Architectural Layers

The system is organized into five distinct layers:

#### Layer 1: Hardware Abstraction Layer (HAL)

Location: `platform/`, `drivers/bus/`

The HAL layer provides hardware abstraction for STM32 microcontrollers:

- STM32 HAL initialization and configuration
- GPIO, I2C, SPI, RTC, and peripheral drivers
- Bus interface abstraction for I2C and SPI protocols
- Clock management and system configuration
- Real-time clock initialization

#### Layer 2: Device Drivers

Location: `drivers/`, `dev/`

Device drivers provide hardware-specific implementations for integrated circuits and sensors:

- Clock Management: AD9545, AD9548, AD9516 (Phase-Locked Loops)
- Sensor Drivers: ADT7301, MAX31725, TMP421 (Temperature sensors)
- Power Monitoring: INA226 (Current and voltage measurement)
- I/O Expanders: MCP23017 (General-purpose I/O expansion)
- Memory: AT24C series (EEPROM)
- Specialized: FPGA management, optical module I2C (SFP/CXP), VXSIIC/VXSIICS interfaces

#### Layer 3: Device Management Layer

Location: `dev/common/`, `dev/*/`

The device management layer provides device abstraction and hierarchical organization:

- DeviceBase structure for unified device representation
- Tree-based device hierarchy with parent-child relationships
- Device state management (DEVICE_NORMAL, DEVICE_FAIL, DEVICE_UNKNOWN)
- Sensor status aggregation (SENSOR_NORMAL, SENSOR_WARNING, SENSOR_CRITICAL)
- Bus interface configuration (I2C/SPI with address mapping)
- Finite State Machine support for device lifecycle

#### Layer 4: Task and Application Layer

Location: `src/rtos/`, `src/tasks/`, `src/app/`

The task layer implements FreeRTOS-based concurrent execution:

- Main control loop (10ms period)
- Power monitoring task (10ms period)
- PLL management task (50ms period)
- Display/UI task (variable period)
- Command-line interface task
- TCP/IP networking task (selected boards)
- FPGA management task
- Analog-to-digital conversion task
- VXSIIC master task (TTVXS only, 10ms period)
- VXSIIC slave task (payload boards, 10ms period)

#### Layer 5: Application Logic

Location: `src/app/`

The application layer handles high-level functionality:

- User command processing and validation
- Display rendering and output formatting
- System status aggregation and reporting
- Configuration management
- Inter-task communication via mail queues

### System Initialization

System boot follows this sequence:

```
1. main() entry point
   a. Enable instruction and data caches (STM32F7)
   b. HAL_Init() - Initialize STM32 HAL
   c. SystemClock_Config() - Configure system clock
   d. MX_GPIO_Init() - Initialize GPIO peripherals

2. app_task_init() - Application initialization
   a. Initialize command queues
   b. Configure task parameters
   c. Set up shared data structures

3. create_tasks() - Create RTOS tasks
   a. Display task
   b. CLI task
   c. Power monitoring task (board-dependent)
   d. Main control task
   e. PLL management task
   f. FPGA task
   g. TCP/IP task (if enabled)
   h. ADC task (if enabled)
   i. VXSIIC master/slave task (board-dependent)

4. osKernelStart() - Start FreeRTOS scheduler
   a. Begin periodic task execution
   b. System enters steady-state operation
```

## Core Components

### DeviceBase Structure

The DeviceBase structure is the fundamental abstraction for all hardware components:

```c
typedef struct DeviceBase {
    DeviceClass device_class;       /* Device type identifier */
    DeviceStatus device_status;     /* Current operational status */
    SensorStatus sensor;            /* Aggregate sensor status */
    BusInterface bus;               /* Communication interface */
    char name[DEVICE_NAME_LEN+1];   /* Device name (max 16 chars) */
    dev_fsm_t fsm;                  /* Device state machine */
    void *priv;                     /* Device-specific private data */
    struct DeviceBase *parent;      /* Parent device pointer */
    struct DeviceBase *children;    /* Child devices pointer */
    struct DeviceBase *next;        /* Sibling device pointer */
} DeviceBase;
```

Fields are used as follows:

- device_class: Identifies device type (AD9545, ADT7301, etc.)
- device_status: Tracks operational state transitions
- sensor: Aggregates device health status
- bus: Specifies communication protocol and parameters
- name: Descriptive device identifier
- fsm: Manages device lifecycle state transitions
- priv: Points to device-specific data structures
- parent: Enables hierarchical device organization
- children: Links to dependent devices
- next: Chains sibling devices

### Device Hierarchy

Devices are organized in a tree structure rooted at a virtual device:

```
Device Root (Virtual)
├── Power Monitor
│   ├── ADT7301[0] (PLL temperature)
│   └── ADT7301[1] (Analog temperature)
├── Main PLL
│   ├── Clock Multiplexer
│   └── Mezzanine EEPROM
├── FPGA
├── VXS Interface
└── VXSIIC/VXSIICS
```

This hierarchy reflects physical device dependencies and enables:

- Graceful device initialization in dependency order
- Status propagation from leaf to root
- Coordinated shutdown procedures
- Organized access to device states

### Communication Bus Architecture

#### Bus Types and Interfaces

The system supports multiple communication protocols:

```c
typedef struct BusInterface {
    BusType type;           /* BUS_IIC, BUS_SPI, or BUS_VIRTUAL */
    int bus_number;         /* Bus instance identifier */
    uint32_t address;       /* Device address (I2C) or chip select (SPI) */
} BusInterface;
```

#### I2C Bus Configuration

The system uses multiple I2C buses for different device groups:

| Bus | Devices | Addresses |
| --- | ------- | --------- |
| I2C1 | EEPROM, general peripherals | 0x50-0x57, 0x66, 0x68 |
| I2C2 | Main PLL (AD9545) | 0x4A |
| I2C3 | SMBus interface, diagnostic devices | 0x02 |
| I2C4 | Power monitors (INA226) | 0x40-0x4F |

#### SPI Bus Configuration

SPI buses are used for devices requiring high bandwidth or deterministic timing:

- ADT7301 temperature sensors (multiple chip selects)
- AD9548 auxiliary clock chip (if enabled)
- AD9516 clock synthesizer (if enabled)
- FPGA configuration and data transfer

#### Bus Driver Implementation

The `drivers/bus/` directory contains platform-independent bus drivers:

- `i2c_driver.c`: Implements I2C master mode with mutex protection
- `spi_driver.c`: Implements SPI master mode
- Bus-specific error handling and retry logic
- Per-bus mutex synchronization for concurrent access

### State Management

#### Device Status Enumeration

```c
typedef enum {
    DEVICE_NORMAL,      /* Device operational */
    DEVICE_FAIL,        /* Device failure detected */
    DEVICE_UNKNOWN      /* Status indeterminate */
} DeviceStatus;
```

#### Sensor Status Enumeration

```c
typedef enum {
    SENSOR_NORMAL,      /* All parameters within specification */
    SENSOR_WARNING,     /* Non-critical deviation detected */
    SENSOR_CRITICAL,    /* Critical fault condition */
    SENSOR_UNKNOWN      /* Status cannot be determined */
} SensorStatus;
```

#### Status Propagation

System status is computed as the aggregate of all device statuses:

1. Leaf devices update sensor status based on measurements
2. Device status transitions via FSM state machine
3. Parent devices aggregate child status (worst-case)
4. System status reflects root device status
5. LED indicators display system status

Status mapping:

- SENSOR_NORMAL: Green LED
- SENSOR_WARNING: Yellow LED
- SENSOR_CRITICAL: Red LED
- SENSOR_UNKNOWN: No LED indication

### Finite State Machines

#### Device FSM States

Each device implements a state machine for lifecycle management:

| State | Description | Transitions |
| ----- | ----------- | ----------- |
| INIT | Device discovery and initialization | SUCCESS -> READY, FAIL -> ERROR |
| READY | Device operational and monitoring | MEASUREMENT -> READY, ERROR -> ERROR |
| ERROR | Fault condition detected | RETRY -> INIT, CRITICAL -> SHUTDOWN |
| SHUTDOWN | Device deactivation sequence | COMPLETE -> IDLE |
| IDLE | Device inactive | ENABLE -> INIT |

#### FSM Execution

Device FSM execution occurs in the context of task execution:

```
Task wakeup (periodic or event-driven)
    ↓
For each device in task's device list:
    a. Execute current state handler
    b. Check state transition conditions
    c. Update device_status
    d. Update sensor value or error count
    ↓
Aggregate device statuses
    ↓
Update system-level shared variables
    ↓
Perform output actions (LED, display, network)
    ↓
Task sleep or wait
```

### Shared Data and Synchronization

#### Shared Data Structures

Global shared variables (marked volatile) are defined in `src/app/app_shared_data`:

```c
extern volatile bool enable_power;          /* Power enable control */
extern volatile bool system_power_present;  /* Power supply status */
extern volatile bool main_clock_ready;      /* Main clock validity */
extern volatile bool aux_clock_ready;       /* Auxiliary clock validity */
```

These variables are:

- Accessed by multiple tasks
- Updated by monitoring tasks
- Read by main control task
- Used for LED and status indication

#### Synchronization Mechanisms

##### Mail Queues

FreeRTOS mail queues enable task-to-task communication:

- `mq_cmd_digipots`: Command queue for digital potentiometer control
- `mq_cmd_menu`: Command queue for menu/UI commands

Queue size: 10 messages each
Message types: CommandDigipots, CommandMenu

##### Mutex Protection

I2C and SPI buses use per-bus mutexes:

```c
osOK = i2c_driver_wait_dev_mutex(bus_number, osWaitForever);
/* Critical section: I2C bus access */
i2c_driver_release_dev_mutex(bus_number);
```

Mutex protection ensures:

- Only one task accesses a bus at a time
- No concurrent I2C/SPI transactions
- Deterministic bus arbitration

##### RTOS Signals

Tasks can signal each other using RTOS signals:

```c
#define SIGNAL_REFRESH_DISPLAY  1000
#define SIGNAL_POWER_ON         1001
#define SIGNAL_POWER_OFF        1002
```

## Data Flow

### Initialization Sequence

```
Hardware Startup
    ↓
STM32 Reset Sequence
    ├── Clear RAM
    ├── Initialize stack pointer
    └── Call Reset_Handler()
    ↓
main()
    ├── Enable CPU caches
    ├── HAL_Init()
    └── SystemClock_Config()
    ↓
Peripheral Initialization
    ├── GPIO enable and configuration
    ├── I2C peripheral initialization
    ├── SPI peripheral initialization
    ├── RTC initialization
    └── UART initialization
    ↓
app_task_init()
    ├── Initialize command queues
    └── Initialize synchronization primitives
    ↓
create_tasks()
    ├── Initialize device tree root
    ├── Create and start each task
    └── Print heap usage statistics
    ↓
osKernelStart()
    └── Begin task scheduling
```

### Steady-State Execution

During steady-state operation, tasks execute periodically:

#### Main Task (10ms period)

```
Task Wakeup
    ↓
Execute task_main_run():
    a. Increment loop counter
    b. Read system status
    c. Compare with previous status
    d. Log status changes
    e. Check clock ready condition
    e. Update LED states based on status
    ↓
osDelay(10)
```

#### PLL Task (50ms period)

```
Task Wakeup
    ↓
Execute pll FSM:
    a. dev_eeprom_config_run()
    b. dev_tqdc_clkmux_run()
    c. Evaluate power state
    d. dev_ad9545_run()
    ↓
Update main_clock_ready flag
    ↓
Log excessive execution time
    ↓
osDelay(adjusted_period)
```

#### Power Monitor Task (10ms period)

```
Task Wakeup
    ↓
For each temperature sensor:
    a. Execute sensor FSM
    b. Read temperature value
    c. Update sensor status
    ↓
Aggregate sensor statuses
    ↓
Call sync_ipmi_sensors()
    ↓
osDelay(10)
```

#### VXSIIC Tasks (10ms period)

```
Task Wakeup (master or slave)
    ↓
Execute VXSIIC FSM (RESET → RUN → PAUSE/ERROR)
    ↓
Poll slots / respond to queries
    ↓
Update cached registers for SNMP/web
    ↓
osDelay(10)
```

## Configuration

### Board Support Variants

The system supports configuration for multiple board types:

| Board ID | Configuration | Purpose | VXSIIC Role |
| -------- | ------------- | ------- | ----------- |
| BOARD_CRU16 | CRU-16 v1.0 + MCB32F769 v1.1 | CRU detector system | VXSIIC slave |
| BOARD_TDC72 | TDC72VHL base configuration | Generic TDC72 variant | VXSIIC slave |
| BOARD_TDC72VHLV2 | TDC72VHL version 2 specific | Enhanced TDC72VHL | VXSIIC slave |
| BOARD_TQDC16VSV1 | TQDC-16 variant | TQDC detector system | VXSIIC slave |
| BOARD_TTVXS | TTVXS configuration | TTVXS detector system | VXSIIC master (centralized monitoring) |

Board selection is specified at compile time via compiler flags:

```
-DBOARD_CRU16
-DBOARD_TDC72
-DBOARD_TQDC16VSV1
```

### Feature Compilation Flags

Optional features are enabled/disabled via compilation flags:

#### Clock Synthesizer Chips

```
-DENABLE_AD9545    /* AD9545 primary PLL */
-DENABLE_AD9548    /* AD9548 auxiliary PLL */
-DENABLE_AD9516    /* AD9516 secondary synthesizer */
```

#### Detector Interfaces

```
-DENABLE_VXSIICM   /* VXS IIC-M interface */
-DENABLE_VXSIICS   /* VXS IIC-S interface */
-DENABLE_SFPIIC    /* SFP/CXP transceiver I2C interface */
```

#### Functional Subsystems

```
-DENABLE_POWERMON  /* Power monitoring subsystem */
-DENABLE_FPGA      /* FPGA management */
-DENABLE_TCPIP     /* TCP/IP networking */
```

### Platform-Specific Configuration

#### STM32F3 Series

Location: `platform/stm32f3/`

- Minimal stack configuration (configMINIMAL_STACK_SIZE + 72 words)
- RTC configuration with 750 kHz clock
- I2C and SPI timing parameters

#### STM32F7 Series

Location: `platform/stm32f7/`

- Extended stack configuration (512 words)
- Instruction and data cache enable sequence
- FPU (Floating-Point Unit) initialization
- Higher clock frequency support

### Board Support Package (BSP)

Each board variant has BSP configuration under `app/<board>/src/bsp/`:

- GPIO pin definitions and initialization
- I2C/SPI bus pinout configuration
- Peripheral clock distribution
- Device address mapping for board variant

## Networking

### TCP/IP Stack Integration

#### Stack Components

- **Framework**: LwIP (Lightweight IP stack)
- **Wrapper**: FreeRTOS-TCP adaptation layer
- **Ethernet Driver**: STM32 MAC interface
- **DHCP**: Dynamic IP address assignment
- **DNS**: Domain name resolution

#### Enabled Boards

TCP/IP is conditionally compiled for boards supporting Ethernet:

- BOARD_TTVXS
- BOARD_CRU16
- BOARD_TQDC
- BOARD_TDC64VLE

#### Network Task

The TCP/IP task manages:

- Stack initialization during boot
- Periodic stack processing (ARP, TCP retransmits)
- Event handling (packet reception, transmission)
- Connection state management

### SNMP Integration

#### MIB (Management Information Base)

SNMP MIB files are located in `mib/` directory:

- Device objects and their OIDs
- Sensor readings (temperature, voltage, current)
- System statistics (uptime, packet counts)
- Trap definitions for alarm conditions

#### SNMP Agent

The SNMP agent provides:

- GET operations for system status queries
- GETNEXT operations for table traversal
- SET operations for configuration changes
- TRAP generation for critical events

#### Remote Monitoring

SNMP enables remote monitoring of:

- Device operational status
- Real-time sensor readings
- Power supply metrics
- Clock synchronization state
- Network interface statistics

### LLDP (Link Layer Discovery Protocol)

LLDP provides network topology discovery:

- Advertise device capabilities and configuration
- Discover network switches and topology
- Enable network management integration
- Support automatic device location identification

## Performance

### Stack Memory Usage

Task stack sizes are calculated based on platform:

#### STM32F3 Series

```
threadStackSize = configMINIMAL_STACK_SIZE + 72  /* words */
```

#### STM32F7 Series

```
threadStackSize = 512  /* words */
```

Additional stack allocation for specific tasks:

| Task | Additional Allocation |
| ---- | --------------------- |
| Main | +70 words |
| PLL | +150 words |
| Power Monitor | +120 words |
| VXSIIC master/slave | +200 words (polling + multiplexer cache) |

#### Stack Overhead

- debug_printf: 100 bytes stack overhead
- I2C operations: 50 bytes
- FSM execution: 30 bytes per level
- Total safety margin: ~20% of allocated size

### Heap Memory Management

Heap usage is monitored during task creation:

```c
debug_heap_usage("task_name");  /* Log heap change */
```

Monitoring enables:

- Detection of memory leaks
- Verification of available heap
- Early warning of memory exhaustion
- Optimization target identification

### Execution Timing

Real-time constraints for periodic tasks:

| Task | Period | Tolerance | Overrun Action |
| ---- | ------ | --------- | -------------- |
| Main | 10ms | ±5% | Log warning, continue |
| PLL | 50ms | ±10% | Log notice, continue |
| Power Monitor | 10ms | ±5% | Log warning, continue |
| VXSIIC master/slave | 10ms | ±5% | Log warning, continue |

Timing violations are logged but do not halt execution.

## Error Handling

### Error Handler Pattern

```c
void Error_Handler(void)
{
    /* Implementation details */
}
```

Error handler is called for:

- HAL initialization failures
- Peripheral configuration errors
- Fatal runtime conditions
- Hardware fault conditions

### Device-Level Error Handling

Device-specific error handling occurs in FSM state machines:

1. Error detection (communication timeout, invalid measurement)
2. Error logging
3. Retry attempt (up to retry limit)
4. State transition to ERROR
5. Status update to indicate failure
6. Graceful degradation

### VXSIIC-Specific Error Handling

- Multiplexer (PCA9548) access failures
- Slot polling timeouts or module detection loss
- Synchronization errors during channel routing
- Automatic recovery: reset multiplexer, pause polling, retry on next cycle

### System-Level Error Recovery

System-level recovery mechanisms:

- Automatic device re-initialization after timeout
- Graceful mode degradation (partial functionality)
- Status reporting for remote monitoring
- User notification via LED and display

### Communication Timeout Handling

I2C/SPI communication implements timeout detection:

```c
i2c_driver_read(bus, data, size, timeout_ms);
```

Timeout handling:

1. Wait for completion up to timeout_ms
2. If timeout occurs, return failure
3. Mark device as communication error
4. Retry on next cycle
5. After N retries, mark as DEVICE_FAIL

### Logging and Diagnostics

The logging subsystem provides diagnostic output:

```c
log_put(LOG_INFO, "Application started");
log_printf(LOG_WARNING, "Device %s not responding", device_name);
```

Log levels:

- LOG_DEBUG: Detailed diagnostic information
- LOG_INFO: Informational messages
- LOG_NOTICE: Normal but significant condition
- LOG_WARNING: Warning conditions
- LOG_ERR: Error conditions
- LOG_CRIT: Critical conditions
- LOG_ALERT: Alert conditions
- LOG_EMERG: Emergency conditions

## Code Organization

### Directory Structure

| Directory | Purpose | Contents |
| --------- | ------- | -------- |
| `src/system/` | System initialization | main(), clock init, boot code |
| `src/rtos/` | Task definitions | Task creation, RTOS interface |
| `src/tasks/` | Task implementations | Individual task code |
| `src/app/` | Application logic | CLI, display, commands |
| `dev/` | Device abstraction | Device types, FSMs |
| `drivers/` | Hardware drivers | Bus drivers, chip drivers |
| `platform/` | Platform-specific | STM32F3, STM32F7 specific |
| `cpu/` | CPU utilities | CPU-specific code |
| `external/` | External libraries | Third-party code |
| `mib/` | SNMP MIB | Management information base |
| `app/<board>/` | Board-specific | Board variants, BSP |
| `dev/vxsiicm/` | VXSIIC master | Master polling, multiplexer control |
| `dev/vxsiics/` | VXSIIC slave | Slave responder logic |

### Device Directory Organization

Device implementations follow a consistent structure:

```
dev/<device_type>/
├── dev_<device>_types.h        /* Type definitions */
├── dev_<device>.h              /* Public interface */
├── dev_<device>.c              /* Implementation */
├── dev_<device>_fsm.h          /* FSM definitions */
└── dev_<device>_fsm.c          /* FSM implementation */
```

### Driver Directory Organization

Driver implementations:

```
drivers/<device_type>/
├── <chip>.h                    /* Chip interface */
└── <chip>.c                    /* Implementation */
```

Bus drivers:

```
drivers/bus/
├── bus_types.h                 /* Bus definitions */
├── i2c_driver.h
├── i2c_driver.c
├── spi_driver.h
└── spi_driver.c
```

## Design Patterns

### Device Abstraction Pattern

All hardware components are abstracted through DeviceBase:

- Common interface for diverse device types
- Device-specific data via opaque priv pointer
- Uniform lifecycle management via FSM
- Status aggregation at each level

Benefits:

- Code reuse across device types
- Simplified addition of new devices
- Consistent error handling
- Simplified testing and simulation

### Task Factory Pattern

Task creation follows a consistent factory pattern:

```c
void create_task_<subsystem>(DeviceBase *parent)
{
    local_init(parent);              /* Initialize devices */
    osThreadCreate(osThread(...));   /* Create RTOS thread */
}
```

Provides:

- Consistent interface for all tasks
- Automatic device tree integration
- Centralized task creation logic
- Simplified task lifecycle management

### Bus Driver Abstraction

Bus operations abstract platform differences:

```c
bool i2c_driver_read(BusInterface *bus, uint8_t *data, 
                     uint16_t size, uint32_t timeout);
```

Enables:

- Platform-independent device code
- Easy porting to new platforms
- Unified error handling
- Consistent timing and retries

### Finite State Machine Pattern

FSM pattern ensures robust state transitions:

```c
typedef struct {
    uint32_t current_state;
    void (*handler[NUM_STATES])(DeviceBase *);
} dev_fsm_t;
```

Advantages:

- Predictable behavior
- Clear state transitions
- Simplified fault handling
- Testable state sequences

### Observer Pattern

Status changes trigger notifications:

- Device status changes → Update system status
- System status changes → Update LED
- System status changes → Update display
- Network events → Trigger network task

Enables:

- Decoupled component updates
- Asynchronous event processing
- Simplified inter-task communication
- Modular system design

## VXS Switched Serial I2C Framework

### Architecture Overview

The VXSIIC (VXS Switched Serial I2C) framework implements a master-slave system for communication over the VXS backplane:

- TTVXS acts as the centralized board monitoring controller (master)
- Payload modules (CRU-16, TQDC, TDC64VLE) act as slave responders
- I2C traffic is routed over the VXS backplane using multiplexed channel switching

### VXSIIC Master (vxsiicm)

- 10ms task wakeup with sequential slot-by-slot polling
- FSM states: RESET, RUN, PAUSE, ERROR
- PCA9548 multiplexer routing to 18 payload slots
- Register access to EEPROM, GPIO expander, MCU at known addresses
- Cached data for SNMP and web interface access

### VXSIIC Slave (vxsiics)

- 10ms task with 100ms polling delay
- I2C slave interface responding to master queries
- Decoupled data model from front-panel Ethernet interfaces
- Environmental, power, and status register exposure

### Physical Implementation Details

- PCA9548 multiplexer addresses (0x71, 0x72, 0x73)
- Slot-to-channel mapping tables
- Error recovery procedures for module detection loss
- Multiplexer access synchronization patterns

### Configuration and Performance

- VXSIIC roles integrated into board variant table
- Dedicated stack allocation for master/slave tasks
- VXSIIC-specific error handling and recovery
- Code organization extended with `dev/vxsiicm/` and `dev/vxsiics/` directories

## Extensibility Guidelines

### Adding a New Device Type

To add support for a new device:

1. Define DeviceClass enum value
2. Create device type files in `dev/<device>/`
3. Implement device FSM and handlers
4. Create device driver in `drivers/`
5. Implement device-specific commands
6. Integrate into appropriate task
7. Add SNMP MIB objects if applicable

### Adding a New Task

To add a new concurrent task:

1. Define task priority and period
2. Implement task function
3. Create osThreadDef wrapper
4. Add `create_task_<name>()` factory function
5. Call factory from `create_tasks()`
6. Add corresponding osThreadId declaration
7. Implement task-specific FSM if needed

### Adding Board Support

To add a new board variant:

1. Create `app/<board>/` directory
2. Implement board-specific BSP in bsp/mx_init/
3. Define board-specific device tree initialization
4. Add `BOARD_<NAME>` compilation flag
5. Update CMakeLists.txt with board variant
6. Test initialization sequence
7. Validate device enumeration

### Platform Porting

To port to a new STM32 platform:

1. Create `platform/stm32<series>/` directory
2. Implement STM32 HAL wrapper functions
3. Configure clock, RTC, GPIO initialization
4. Define I2C and SPI pin mappings
5. Adjust stack sizes for platform
6. Update platform-specific device drivers
7. Validate memory layout and performance
