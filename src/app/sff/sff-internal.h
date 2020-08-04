#ifndef INTERNAL_H
#define INTERNAL_H

#include <stdint.h>
#include <sys/types.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint32_t __u32;
typedef uint16_t u16;
typedef uint16_t __u16;
typedef uint8_t u8;
typedef uint8_t __u8;
typedef int32_t s32;
typedef int16_t __s16;

/* EEPROM Standards for plug in modules */
#define ETH_MODULE_SFF_8079		0x1
#define ETH_MODULE_SFF_8079_LEN		256
#define ETH_MODULE_SFF_8472		0x2
#define ETH_MODULE_SFF_8472_LEN		512
#define ETH_MODULE_SFF_8636		0x3
#define ETH_MODULE_SFF_8636_LEN		256
#define ETH_MODULE_SFF_8436		0x4
#define ETH_MODULE_SFF_8436_LEN		256

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Module EEPROM parsing code */
void sff8079_show_all(const __u8 *id);

/* Optics diagnostics */
void sff8472_show_all(const __u8 *id);

/* QSFP Optics diagnostics */
void sff8636_show_all(const __u8 *id, __u32 eeprom_len);

#ifdef __cplusplus
}
#endif

#endif /* INTERNAL_H */
