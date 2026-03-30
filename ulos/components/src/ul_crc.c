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
#include "ul_crc.h"

/******************************************************************************
 * Name:    CRC-4/ITU           x4+x+1
 * Poly:    0x03
 * Init:    0x00
 * Refin:   True
 * Refout:  True
 * Xorout:  0x00
 * Note:
 *****************************************************************************/
ul_uint8_t ul_crc4_itu(ul_uint8_t *data, ul_uint16_t length)
{
    ul_uint8_t i;
    ul_uint8_t crc = 0;                // Initial value
    while(length--)
    {
        crc ^= *data++;                 // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0x0C;// 0x0C = (reverse 0x03)>>(8-4)
            else
                crc = (crc >> 1);
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-5/EPC           x5+x3+1
 * Poly:    0x09
 * Init:    0x09
 * Refin:   False
 * Refout:  False
 * Xorout:  0x00
 * Note:
 *****************************************************************************/
ul_uint8_t ul_crc5_epc(ul_uint8_t *data, ul_uint16_t length)
{
    ul_uint8_t i;
    ul_uint8_t crc = 0x48;        // Initial value: 0x48 = 0x09<<(8-5)
    while(length--)
    {
        crc ^= *data++;        // crc ^= *data; data++;
        for ( i = 0; i < 8; i++ )
        {
            if ( crc & 0x80 )
                crc = (crc << 1) ^ 0x48;        // 0x48 = 0x09<<(8-5)
            else
                crc <<= 1;
        }
    }
    return crc >> 3;
}

/******************************************************************************
 * Name:    CRC-5/ITU           x5+x4+x2+1
 * Poly:    0x15
 * Init:    0x00
 * Refin:   True
 * Refout:  True
 * Xorout:  0x00
 * Note:
 *****************************************************************************/
ul_uint8_t ul_crc5_itu(ul_uint8_t *data, ul_uint16_t length)
{
    ul_uint8_t i;
    ul_uint8_t crc = 0;                // Initial value
    while(length--)
    {
        crc ^= *data++;                 // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0x15;// 0x15 = (reverse 0x15)>>(8-5)
            else
                crc = (crc >> 1);
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-5/USB           x5+x2+1
 * Poly:    0x05
 * Init:    0x1F
 * Refin:   True
 * Refout:  True
 * Xorout:  0x1F
 * Note:
 *****************************************************************************/
ul_uint8_t ul_crc5_usb(ul_uint8_t *data, ul_uint16_t length)
{
    ul_uint8_t i;
    ul_uint8_t crc = 0x1F;                // Initial value
    while(length--)
    {
        crc ^= *data++;                 // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0x14;// 0x14 = (reverse 0x05)>>(8-5)
            else
                crc = (crc >> 1);
        }
    }
    return crc ^ 0x1F;
}

/******************************************************************************
 * Name:    CRC-6/ITU           x6+x+1
 * Poly:    0x03
 * Init:    0x00
 * Refin:   True
 * Refout:  True
 * Xorout:  0x00
 * Note:
 *****************************************************************************/
ul_uint8_t ul_crc6_itu(ul_uint8_t *data, ul_uint16_t length)
{
    ul_uint8_t i;
    ul_uint8_t crc = 0;         // Initial value
    while(length--)
    {
        crc ^= *data++;        // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0x30;// 0x30 = (reverse 0x03)>>(8-6)
            else
                crc = (crc >> 1);
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-7/MMC           x7+x3+1
 * Poly:    0x09
 * Init:    0x00
 * Refin:   False
 * Refout:  False
 * Xorout:  0x00
 * Use:     MultiMediaCard,SD,ect.
 *****************************************************************************/
ul_uint8_t ul_crc7_mmc(ul_uint8_t *data, ul_uint16_t length)
{
    ul_uint8_t i;
    ul_uint8_t crc = 0;        // Initial value
    while(length--)
    {
        crc ^= *data++;        // crc ^= *data; data++;
        for ( i = 0; i < 8; i++ )
        {
            if ( crc & 0x80 )
                crc = (crc << 1) ^ 0x12;        // 0x12 = 0x09<<(8-7)
            else
                crc <<= 1;
        }
    }
    return crc >> 1;
}

/******************************************************************************
 * Name:    CRC-8               x8+x2+x+1
 * Poly:    0x07
 * Init:    0x00
 * Refin:   False
 * Refout:  False
 * Xorout:  0x00
 * Note:
 *****************************************************************************/
ul_uint8_t ul_crc8(ul_uint8_t *data, ul_uint16_t length)
{
    ul_uint8_t i;
    ul_uint8_t crc = 0;        // Initial value
    while(length--)
    {
        crc ^= *data++;        // crc ^= *data; data++;
        for ( i = 0; i < 8; i++ )
        {
            if ( crc & 0x80 )
                crc = (crc << 1) ^ 0x07;
            else
                crc <<= 1;
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-8/ITU           x8+x2+x+1
 * Poly:    0x07
 * Init:    0x00
 * Refin:   False
 * Refout:  False
 * Xorout:  0x55
 * Alias:   CRC-8/ATM
 *****************************************************************************/
ul_uint8_t ul_crc8_itu(ul_uint8_t *data, ul_uint16_t length)
{
    ul_uint8_t i;
    ul_uint8_t crc = 0;        // Initial value
    while(length--)
    {
        crc ^= *data++;        // crc ^= *data; data++;
        for ( i = 0; i < 8; i++ )
        {
            if ( crc & 0x80 )
                crc = (crc << 1) ^ 0x07;
            else
                crc <<= 1;
        }
    }
    return crc ^ 0x55;
}

/******************************************************************************
 * Name:    CRC-8/ROHC          x8+x2+x+1
 * Poly:    0x07
 * Init:    0xFF
 * Refin:   True
 * Refout:  True
 * Xorout:  0x00
 * Note:
 *****************************************************************************/
ul_uint8_t ul_crc8_rohc(ul_uint8_t *data, ul_uint16_t length)
{
    ul_uint8_t i;
    ul_uint8_t crc = 0xFF;         // Initial value
    while(length--)
    {
        crc ^= *data++;            // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xE0;        // 0xE0 = reverse 0x07
            else
                crc = (crc >> 1);
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-8/MAXIM         x8+x5+x4+1
 * Poly:    0x31
 * Init:    0x00
 * Refin:   True
 * Refout:  True
 * Xorout:  0x00
 * Alias:   DOW-CRC,CRC-8/IBUTTON
 * Use:     Maxim(Dallas)'s some devices,e.g. DS18B20
 *****************************************************************************/
ul_uint8_t ul_crc8_maxim(ul_uint8_t *data, ul_uint16_t length)
{
    ul_uint8_t i;
    ul_uint8_t crc = 0;         // Initial value
    while(length--)
    {
        crc ^= *data++;        // crc ^= *data; data++;
        for (i = 0; i < 8; i++)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0x8C;        // 0x8C = reverse 0x31
            else
                crc >>= 1;
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-16/IBM          x16+x15+x2+1
 * Poly:    0x8005
 * Init:    0x0000
 * Refin:   True
 * Refout:  True
 * Xorout:  0x0000
 * Alias:   CRC-16,CRC-16/ARC,CRC-16/LHA
 *****************************************************************************/
ul_uint16_t ul_crc16_ibm(ul_uint8_t *data, ul_uint16_t length)
{
    ul_uint8_t i;
    ul_uint16_t crc = 0;        // Initial value
    while(length--)
    {
        crc ^= *data++;        // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;        // 0xA001 = reverse 0x8005
            else
                crc = (crc >> 1);
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-16/MAXIM        x16+x15+x2+1
 * Poly:    0x8005
 * Init:    0x0000
 * Refin:   True
 * Refout:  True
 * Xorout:  0xFFFF
 * Note:
 *****************************************************************************/
ul_uint16_t ul_crc16_maxim(ul_uint8_t *data, ul_uint16_t length)
{
    ul_uint8_t i;
    ul_uint16_t crc = 0;        // Initial value
    while(length--)
    {
        crc ^= *data++;        // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;        // 0xA001 = reverse 0x8005
            else
                crc = (crc >> 1);
        }
    }
    return ~crc;    // crc^0xffff
}

/******************************************************************************
 * Name:    CRC-16/USB          x16+x15+x2+1
 * Poly:    0x8005
 * Init:    0xFFFF
 * Refin:   True
 * Refout:  True
 * Xorout:  0xFFFF
 * Note:
 *****************************************************************************/
ul_uint16_t ul_crc16_usb(ul_uint8_t *data, ul_uint16_t length)
{
    ul_uint8_t i;
    ul_uint16_t crc = 0xffff;        // Initial value
    while(length--)
    {
        crc ^= *data++;            // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;        // 0xA001 = reverse 0x8005
            else
                crc = (crc >> 1);
        }
    }
    return ~crc;    // crc^0xffff
}

/******************************************************************************
 * Name:    CRC-16/MODBUS       x16+x15+x2+1
 * Poly:    0x8005
 * Init:    0xFFFF
 * Refin:   True
 * Refout:  True
 * Xorout:  0x0000
 * Note:
 *****************************************************************************/
ul_uint16_t ul_crc16_modbus(ul_uint8_t *data, ul_uint16_t length)
{
    ul_uint8_t i;
    ul_uint16_t crc = 0xffff;        // Initial value
    while(length--)
    {
        crc ^= *data++;            // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;        // 0xA001 = reverse 0x8005
            else
                crc = (crc >> 1);
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-16/CCITT        x16+x12+x5+1
 * Poly:    0x1021
 * Init:    0x0000
 * Refin:   True
 * Refout:  True
 * Xorout:  0x0000
 * Alias:   CRC-CCITT,CRC-16/CCITT-TRUE,CRC-16/KERMIT
 *****************************************************************************/
ul_uint16_t ul_crc16_ccitt(ul_uint8_t *data, ul_uint16_t length)
{
    ul_uint8_t i;
    ul_uint16_t crc = 0;        // Initial value
    while(length--)
    {
        crc ^= *data++;        // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0x8408;        // 0x8408 = reverse 0x1021
            else
                crc = (crc >> 1);
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-16/CCITT-FALSE   x16+x12+x5+1
 * Poly:    0x1021
 * Init:    0xFFFF
 * Refin:   False
 * Refout:  False
 * Xorout:  0x0000
 * Note:
 *****************************************************************************/
ul_uint16_t ul_crc16_ccitt_false(ul_uint8_t *data, ul_uint16_t length)
{
    ul_uint8_t i;
    ul_uint16_t crc = 0xffff;        //Initial value
    while(length--)
    {
        crc ^= (ul_uint16_t)(*data++) << 8; // crc ^= (uint6_t)(*data)<<8; data++;
        for (i = 0; i < 8; ++i)
        {
            if ( crc & 0x8000 )
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-16/X25          x16+x12+x5+1
 * Poly:    0x1021
 * Init:    0xFFFF
 * Refin:   True
 * Refout:  True
 * Xorout:  0XFFFF
 * Note:
 *****************************************************************************/
ul_uint16_t ul_crc16_x25(ul_uint8_t *data, ul_uint16_t length)
{
    ul_uint8_t i;
    ul_uint16_t crc = 0xffff;        // Initial value
    while(length--)
    {
        crc ^= *data++;            // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0x8408;        // 0x8408 = reverse 0x1021
            else
                crc = (crc >> 1);
        }
    }
    return ~crc;                // crc^Xorout
}

/******************************************************************************
 * Name:    CRC-16/XMODEM       x16+x12+x5+1
 * Poly:    0x1021
 * Init:    0x0000
 * Refin:   False
 * Refout:  False
 * Xorout:  0x0000
 * Alias:   CRC-16/ZMODEM,CRC-16/ACORN
 *****************************************************************************/
ul_uint16_t ul_crc16_xmodem(ul_uint8_t *data, ul_uint16_t length)
{
    ul_uint8_t i;
    ul_uint16_t crc = 0;            // Initial value
    while(length--)
    {
        crc ^= (ul_uint16_t)(*data++) << 8; // crc ^= (ul_uint16_t)(*data)<<8; data++;
        for (i = 0; i < 8; ++i)
        {
            if ( crc & 0x8000 )
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-16/DNP          x16+x13+x12+x11+x10+x8+x6+x5+x2+1
 * Poly:    0x3D65
 * Init:    0x0000
 * Refin:   True
 * Refout:  True
 * Xorout:  0xFFFF
 * Use:     M-Bus,ect.
 *****************************************************************************/
ul_uint16_t ul_crc16_dnp(ul_uint8_t *data, ul_uint16_t length)
{
    ul_uint8_t i;
    ul_uint16_t crc = 0;            // Initial value
    while(length--)
    {
        crc ^= *data++;            // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA6BC;        // 0xA6BC = reverse 0x3D65
            else
                crc = (crc >> 1);
        }
    }
    return ~crc;                // crc^Xorout
}

/******************************************************************************
 * Name:    CRC-32  x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x+1
 * Poly:    0x4C11DB7
 * Init:    0xFFFFFFF
 * Refin:   True
 * Refout:  True
 * Xorout:  0xFFFFFFF
 * Alias:   CRC_32/ADCCP
 * Use:     WinRAR,ect.
 *****************************************************************************/
ul_uint32_t ul_crc32(ul_uint8_t *data, ul_uint16_t length)
{
    ul_uint8_t i;
    ul_uint32_t crc = 0xffffffff;        // Initial value
    while(length--)
    {
        crc ^= *data++;                // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;// 0xEDB88320= reverse 0x04C11DB7
            else
                crc = (crc >> 1);
        }
    }
    return ~crc;
}

/******************************************************************************
 * Name:    CRC-32/MPEG-2  x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x+1
 * Poly:    0x4C11DB7
 * Init:    0xFFFFFFF
 * Refin:   False
 * Refout:  False
 * Xorout:  0x0000000
 * Note:
 *****************************************************************************/
ul_uint32_t ul_crc32_mpeg_2(ul_uint8_t *data, ul_uint16_t length)
{
    ul_uint8_t i;
    ul_uint32_t crc = 0xffffffff;  // Initial value
    while(length--)
    {
        crc ^= (ul_uint32_t)(*data++) << 24;// crc ^=(ul_uint32_t)(*data)<<24; data++;
        for (i = 0; i < 8; ++i)
        {
            if ( crc & 0x80000000 )
                crc = (crc << 1) ^ 0x04C11DB7;
            else
                crc <<= 1;
        }
    }
    return crc;
}
