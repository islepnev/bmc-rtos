# BMC-RTOS Brief

## Purpose

BMC-RTOS is firmware for STM32-based board management controllers used on instrumentation payload boards and the TTVXS switch board. It supervises power, clocking, thermal sensors, EEPROM/configuration devices, FPGA service functions, and network management interface.

## Operational scope

- power monitoring and sequencing
- PLL and clock-device control
- thermal monitoring and threshold handling
- configuration EEPROM access
- FPGA status and control interface
- optical-module I2C via muxes
- local CLI / terminal UI
- network services
- VXSIIC integration for switch/payload communication

## Architecture

Subsystems are implemented as RTOS tasks. Each task:

- creates its own device subtree
- registers devices in the global device list
- runs its control loop in task context

A target is composed from:

- STM32 platform and bus drivers
- reusable device modules
- device-local FSMs
- RTOS task bindings
- BSP/application code defining topology, addresses, and limits

The same codebase is reused across boards by varying BSP configuration and task composition.

## Control chain

1. Power supervision validates rails and system power state  
2. PLL tasks bring up and monitor the clock path  
3. FPGA management is enabled only when power and clocks are valid  

## Board variants

Differences between boards are defined by:

- BSP sensor maps and limits
- powermon / PLL task composition
- optional devices (clkmux, digipot, SFP mux, sensors)
- build-time feature flags

TTVXS acts as VXSIIC master. Other boards operate as payload nodes (slave side).

## Device model

- devices form a parent/child tree
- subsystem tasks own top-level device instances
- a global registry provides lookup and status access

This model defines system composition across all variants.
