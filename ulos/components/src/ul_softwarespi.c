/*
 * Copyright (c) 2025 ulOS Community
 *
 * SPDX-License-Identifier: GPL-2.0-or-late
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-8-12     zhuqinsheng   the first version，only verify spi mode0
 */
#include "ul_softwarespi.h"

/**
 * 软件SPI初始化函数
 *
 * @param self          软件SPI对象
 * @param mode          SPI模式[1 -- 4]
 *
 * @return 错误代码
 */
ul_ecode ul_swspi_init(ul_swspi_t *self, uint8_t spi_mode)
{
    if (self == NULL)
    {
        return UL_ENULL;
    }
    
    self->mode = spi_mode;
    
    return UL_EOK;
}

/**
 * 软件SPI读写函数
 *
 * @param self          软件SPI对象
 * @param wbyte         要写的数据[字节]
 *
 * @return 读出的数据[字节]
 */
ul_uint8_t ul_swspi_rwbyte(ul_swspi_t *self, ul_uint8_t wbyte)
{
    uint8_t rbyte = 0;
    uint8_t readen = 1;
    if (self == NULL)
    {
        return UL_ENULL;
    }
    if (NULL == self->miso_read)
    {
        readen = 0;
    }
    
    switch (self->mode)
    {
        case 0:
            for (uint8_t i = 0; i < 8; i++)
            {
                if ((wbyte & 0x80))
                {
                    self->mosi_write(1);
                }
                else
                {  
                    self->mosi_write(0);
                }
                self->sck_write(1);// 时钟拉高，采样数据（CPHA = 0）
                wbyte <<= 1;
                
                rbyte <<= 1;
                if (readen == 1)
                rbyte |= self->miso_read();
                
                self->sck_write(0);// 时钟拉低（CPOL = 0）
            }
            break;
        case 1:
            for (uint8_t i = 0; i < 8; i++) 
            {
                if (wbyte & 0x80) {
                    self->mosi_write(1);
                } else {
                    self->mosi_write(0);
                }
                self->sck_write(0); // 时钟拉低（CPOL = 0）
                self->sck_write(1); // 时钟拉高，采样数据（CPHA = 1）
                wbyte <<= 1;

                rbyte <<= 1;
                if (readen == 1)
                rbyte |= self->miso_read();
            }
            break;
        case 2:
            for (uint8_t i = 0; i < 8; i++) 
            {
                self->sck_write(1); // 时钟拉高（CPOL = 1）
                if (wbyte & 0x80) {
                    self->mosi_write(1);
                } else {
                    self->mosi_write(0);
                }
                self->sck_write(0); // 时钟拉低，采样数据（CPHA = 0）
                wbyte <<= 1;

                rbyte <<= 1;
                if (readen == 1)
                rbyte |= self->miso_read();
            }
            break;
        case 3:
            for (uint8_t i = 0; i < 8; i++) 
            {
                if (wbyte & 0x80) {
                    self->mosi_write(1);
                } else {
                    self->mosi_write(0);
                }
                self->sck_write(1); // 时钟拉高（CPOL = 1）
                self->sck_write(0); // 时钟拉低，采样数据（CPHA = 1）
                wbyte <<= 1;

                rbyte <<= 1;
                if (readen == 1)
                rbyte |= self->miso_read();
            }
            break;
        default:
            return 0;
    }
	
	return rbyte;
}
