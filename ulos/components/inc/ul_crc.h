/*
 * Copyright (c) 2025 ulOS Community
 *
 * SPDX-License-Identifier: GPL-2.0-or-late
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-9-07     zhuqinsheng   the first version, modify from 
 *                             https://github.com/whik/crc-lib-c/blob/master/crcLib.c
 */
#ifndef _UL_CRC_H
#define _UL_CRC_H

#include "ul_config.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/*
 * crc interface
 */
ul_uint8_t  ul_crc4_itu         (ul_uint8_t *data, ul_uint16_t length);
ul_uint8_t  ul_crc5_epc         (ul_uint8_t *data, ul_uint16_t length);
ul_uint8_t  ul_crc5_itu         (ul_uint8_t *data, ul_uint16_t length);
ul_uint8_t  ul_crc5_usb         (ul_uint8_t *data, ul_uint16_t length);
ul_uint8_t  ul_crc6_itu         (ul_uint8_t *data, ul_uint16_t length);
ul_uint8_t  ul_crc7_mmc         (ul_uint8_t *data, ul_uint16_t length);
ul_uint8_t  ul_crc8             (ul_uint8_t *data, ul_uint16_t length);
ul_uint8_t  ul_crc8_itu         (ul_uint8_t *data, ul_uint16_t length);
ul_uint8_t  ul_crc8_rohc        (ul_uint8_t *data, ul_uint16_t length);
ul_uint8_t  ul_crc8_maxim       (ul_uint8_t *data, ul_uint16_t length);//DS18B20
ul_uint16_t ul_crc16_ibm        (ul_uint8_t *data, ul_uint16_t length);
ul_uint16_t ul_crc16_maxim      (ul_uint8_t *data, ul_uint16_t length);
ul_uint16_t ul_crc16_usb        (ul_uint8_t *data, ul_uint16_t length);
ul_uint16_t ul_crc16_modbus     (ul_uint8_t *data, ul_uint16_t length);
ul_uint16_t ul_crc16_ccitt      (ul_uint8_t *data, ul_uint16_t length);
ul_uint16_t ul_crc16_ccitt_false(ul_uint8_t *data, ul_uint16_t length);
ul_uint16_t ul_crc16_x25        (ul_uint8_t *data, ul_uint16_t length);
ul_uint16_t ul_crc16_xmodem     (ul_uint8_t *data, ul_uint16_t length);
ul_uint16_t ul_crc16_dnp        (ul_uint8_t *data, ul_uint16_t length);
ul_uint32_t ul_crc32            (ul_uint8_t *data, ul_uint16_t length);
ul_uint32_t ul_crc32_mpeg_2     (ul_uint8_t *data, ul_uint16_t length);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
