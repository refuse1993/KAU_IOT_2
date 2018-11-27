/*
 * sh1106.c
 *
 *  Created on: 2018. 9. 29.
 *      Author: ������
 */

#include <string.h>
#include <fcntl.h>
#include <tinyara/gpio.h>
#include <apps/netutils/mqtt_api.h>
#include <apps/netutils/dhcpc.h>
#include <tinyara/analog/ioctl.h>
#include "iotlib.h"

static Sh0116Manager sh1106_manager;

const uint8_t ASCII[][5] = {
	 { 0x00, 0x00, 0x00, 0x00, 0x00 } // 20   (space)
	,{ 0x00, 0x00, 0x5f, 0x00, 0x00 } // 21 !
	,{ 0x00, 0x07, 0x00, 0x07, 0x00 } // 22 "
	,{ 0x14, 0x7f, 0x14, 0x7f, 0x14 } // 23 #
	,{ 0x24, 0x2a, 0x7f, 0x2a, 0x12 } // 24 $
	,{ 0x23, 0x13, 0x08, 0x64, 0x62 } // 25 %
	,{ 0x36, 0x49, 0x55, 0x22, 0x50 } // 26 &
	,{ 0x00, 0x05, 0x03, 0x00, 0x00 } // 27 '
	,{ 0x00, 0x1c, 0x22, 0x41, 0x00 } // 28 (
	,{ 0x00, 0x41, 0x22, 0x1c, 0x00 } // 29 )
	,{ 0x14, 0x08, 0x3e, 0x08, 0x14 } // 2a *
	,{ 0x08, 0x08, 0x3e, 0x08, 0x08 } // 2b +
	,{ 0x00, 0x50, 0x30, 0x00, 0x00 } // 2c ,
	,{ 0x08, 0x08, 0x08, 0x08, 0x08 } // 2d -
	,{ 0x00, 0x60, 0x60, 0x00, 0x00 } // 2e .
	,{ 0x20, 0x10, 0x08, 0x04, 0x02 } // 2f /
	,{ 0x3e, 0x51, 0x49, 0x45, 0x3e } // 30 0
	,{ 0x00, 0x42, 0x7f, 0x40, 0x00 } // 31 1
	,{ 0x42, 0x61, 0x51, 0x49, 0x46 } // 32 2
	,{ 0x21, 0x41, 0x45, 0x4b, 0x31 } // 33 3
	,{ 0x18, 0x14, 0x12, 0x7f, 0x10 } // 34 4
	,{ 0x27, 0x45, 0x45, 0x45, 0x39 } // 35 5
	,{ 0x3c, 0x4a, 0x49, 0x49, 0x30 } // 36 6
	,{ 0x01, 0x71, 0x09, 0x05, 0x03 } // 37 7
	,{ 0x36, 0x49, 0x49, 0x49, 0x36 } // 38 8
	,{ 0x06, 0x49, 0x49, 0x29, 0x1e } // 39 9
	,{ 0x00, 0x36, 0x36, 0x00, 0x00 } // 3a :
	,{ 0x00, 0x56, 0x36, 0x00, 0x00 } // 3b ;
	,{ 0x08, 0x14, 0x22, 0x41, 0x00 } // 3c <
	,{ 0x14, 0x14, 0x14, 0x14, 0x14 } // 3d =
	,{ 0x00, 0x41, 0x22, 0x14, 0x08 } // 3e >
	,{ 0x02, 0x01, 0x51, 0x09, 0x06 } // 3f ?
	,{ 0x32, 0x49, 0x79, 0x41, 0x3e } // 40 @
	,{ 0x7e, 0x11, 0x11, 0x11, 0x7e } // 41 A
	,{ 0x7f, 0x49, 0x49, 0x49, 0x36 } // 42 B
	,{ 0x3e, 0x41, 0x41, 0x41, 0x22 } // 43 C
	,{ 0x7f, 0x41, 0x41, 0x22, 0x1c } // 44 D
	,{ 0x7f, 0x49, 0x49, 0x49, 0x41 } // 45 E
	,{ 0x7f, 0x09, 0x09, 0x09, 0x01 } // 46 F
	,{ 0x3e, 0x41, 0x49, 0x49, 0x7a } // 47 G
	,{ 0x7f, 0x08, 0x08, 0x08, 0x7f } // 48 H
	,{ 0x00, 0x41, 0x7f, 0x41, 0x00 } // 49 I
	,{ 0x20, 0x40, 0x41, 0x3f, 0x01 } // 4a J
	,{ 0x7f, 0x08, 0x14, 0x22, 0x41 } // 4b K
	,{ 0x7f, 0x40, 0x40, 0x40, 0x40 } // 4c L
	,{ 0x7f, 0x02, 0x0c, 0x02, 0x7f } // 4d M
	,{ 0x7f, 0x04, 0x08, 0x10, 0x7f } // 4e N
	,{ 0x3e, 0x41, 0x41, 0x41, 0x3e } // 4f O
	,{ 0x7f, 0x09, 0x09, 0x09, 0x06 } // 50 P
	,{ 0x3e, 0x41, 0x51, 0x21, 0x5e } // 51 Q
	,{ 0x7f, 0x09, 0x19, 0x29, 0x46 } // 52 R
	,{ 0x46, 0x49, 0x49, 0x49, 0x31 } // 53 S
	,{ 0x01, 0x01, 0x7f, 0x01, 0x01 } // 54 T
	,{ 0x3f, 0x40, 0x40, 0x40, 0x3f } // 55 U
	,{ 0x1f, 0x20, 0x40, 0x20, 0x1f } // 56 V
	,{ 0x3f, 0x40, 0x38, 0x40, 0x3f } // 57 W
	,{ 0x63, 0x14, 0x08, 0x14, 0x63 } // 58 X
	,{ 0x07, 0x08, 0x70, 0x08, 0x07 } // 59 Y
	,{ 0x61, 0x51, 0x49, 0x45, 0x43 } // 5a Z
	,{ 0x00, 0x7f, 0x41, 0x41, 0x00 } // 5b [
	,{ 0x02, 0x04, 0x08, 0x10, 0x20 } // 5c backslash
	,{ 0x00, 0x41, 0x41, 0x7f, 0x00 } // 5d ]
	,{ 0x04, 0x02, 0x01, 0x02, 0x04 } // 5e ^
	,{ 0x40, 0x40, 0x40, 0x40, 0x40 } // 5f _
	,{ 0x00, 0x01, 0x02, 0x04, 0x00 } // 60 `
	,{ 0x20, 0x54, 0x54, 0x54, 0x78 } // 61 a
	,{ 0x7f, 0x48, 0x44, 0x44, 0x38 } // 62 b
	,{ 0x38, 0x44, 0x44, 0x44, 0x20 } // 63 c
	,{ 0x38, 0x44, 0x44, 0x48, 0x7f } // 64 d
	,{ 0x38, 0x54, 0x54, 0x54, 0x18 } // 65 e
	,{ 0x08, 0x7e, 0x09, 0x01, 0x02 } // 66 f
	,{ 0x0c, 0x52, 0x52, 0x52, 0x3e } // 67 g
	,{ 0x7f, 0x08, 0x04, 0x04, 0x78 } // 68 h
	,{ 0x00, 0x44, 0x7d, 0x40, 0x00 } // 69 i
	,{ 0x20, 0x40, 0x44, 0x3d, 0x00 } // 6a j
	,{ 0x7f, 0x10, 0x28, 0x44, 0x00 } // 6b k
	,{ 0x00, 0x41, 0x7f, 0x40, 0x00 } // 6c l
	,{ 0x7c, 0x04, 0x18, 0x04, 0x78 } // 6d m
	,{ 0x7c, 0x08, 0x04, 0x04, 0x78 } // 6e n
	,{ 0x38, 0x44, 0x44, 0x44, 0x38 } // 6f o
	,{ 0x7c, 0x14, 0x14, 0x14, 0x08 } // 70 p
	,{ 0x08, 0x14, 0x14, 0x18, 0x7c } // 71 q
	,{ 0x7c, 0x08, 0x04, 0x04, 0x08 } // 72 r
	,{ 0x48, 0x54, 0x54, 0x54, 0x20 } // 73 s
	,{ 0x04, 0x3f, 0x44, 0x40, 0x20 } // 74 t
	,{ 0x3c, 0x40, 0x40, 0x20, 0x7c } // 75 u
	,{ 0x1c, 0x20, 0x40, 0x20, 0x1c } // 76 v
	,{ 0x3c, 0x40, 0x30, 0x40, 0x3c } // 77 w
	,{ 0x44, 0x28, 0x10, 0x28, 0x44 } // 78 x
	,{ 0x0c, 0x50, 0x50, 0x50, 0x3c } // 79 y
	,{ 0x44, 0x64, 0x54, 0x4c, 0x44 } // 7a z
	,{ 0x00, 0x08, 0x36, 0x41, 0x00 } // 7b {
	,{ 0x00, 0x00, 0x7f, 0x00, 0x00 } // 7c |
	,{ 0x00, 0x41, 0x36, 0x08, 0x00 } // 7d }
	,{ 0x10, 0x08, 0x08, 0x10, 0x08 } // 7e ~
	,{ 0x78, 0x46, 0x41, 0x46, 0x78 } // 7f DEL
};

uint8_t back1[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,6,6,6,254,254,6,6,6,6,126,252,206,134,134,134,134,134,134,4,0,252,254,134,134,134,134,4,4,6,6,14,254,254,6,6,4,0,0,0,0,0,0,0,0,0,0,0};
uint8_t back2[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,63,0,0,0,0,0,31,63,49,49,49,49,49,49,49,0,33,49,49,49,49,63,63,0,0,0,0,63,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint8_t back3[]={0,0,0,0,0,0,0,0,0,252,252,252,252,252,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,252,252,252,252,252,252,252,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,252,252,252,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,252,252,252,252,252,0,0,0,0,0,0,0,0,0};
uint8_t back4[]={0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,192,224,112,60,30,6,134,134,198,198,198,198,231,99,99,99,102,230,236,236,236,236,248,248,248,184,248,248,140,12,12,14,6,3,3,1,1,1,1,1,1,255,255,0,0,128,192,224,240,248,24,24,24,24,152,152,152,252,254,28,28,252,254,63,123,241,225,99,195,199,198,142,140,140,28,120,224,192,0,0,0,0,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0};
uint8_t back5[]={0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,96,240,240,248,252,188,31,15,7,3,3,3,3,3,1,1,0,240,248,240,240,126,127,195,193,193,192,192,224,96,112,49,57,29,15,7,3,7,6,6,14,12,12,12,12,204,252,127,15,237,252,63,15,15,7,7,15,62,120,254,223,255,255,127,255,251,246,102,255,255,124,60,63,55,240,224,0,1,1,1,3,3,7,254,255,255,0,0,0,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0};
uint8_t back6[]={0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,248,255,15,3,3,3,3,6,6,14,12,12,12,12,12,15,63,127,255,220,156,152,24,24,24,24,24,24,24,152,152,152,216,216,216,248,120,56,56,24,28,254,255,243,179,248,255,239,193,128,128,0,0,0,0,0,0,0,3,7,15,12,12,31,31,62,127,127,252,248,248,240,255,255,240,240,240,240,240,252,222,255,127,15,0,0,0,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0};
uint8_t back7[]={0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,3,3,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,3,3,3,3,3,3,3,3,1,1,1,0,0,0,0,0,0,0,0,0,0,1,3,7,7,15,15,15,31,31,28,24,24,24,24,24,24,24,24,24,16,24,24,24,28,12,12,15,15,14,7,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,0,0,0,0,0,0,0,0,0};
uint8_t back8[]={0,0,0,0,0,0,0,0,0,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,0,0,0,0,0,0,0,0,0};

static void gpio_write(int port, int value)
{
	char str[4];
	static char devpath[16];
	snprintf(devpath, 16, "/dev/gpio%d", port);
	int fd = open(devpath, O_RDWR);
	if (fd < 0) {
		printf("fd open fail\n");
		return;
	}

	ioctl(fd, GPIOIOC_SET_DIRECTION, GPIO_DIRECTION_OUT);
	if (write(fd, str, snprintf(str, 4, "%d", value != 0) + 1) < 0) {
		printf("write error\n");
		return;
	}

	close(fd);
}

static void lcd_write(DC_STATUS dc, const uint8_t *w_data, uint32_t size)
{
	SPI_LOCK(sh1106_manager.spi, true);

	SPI_SELECT(sh1106_manager.spi, sh1106_manager.port, true);
	gpio_write(sh1106_manager.dc_pin, dc);
	SPI_SNDBLOCK(sh1106_manager.spi, w_data, size);
	SPI_SELECT(sh1106_manager.spi, sh1106_manager.port, false);

	SPI_LOCK(sh1106_manager.spi, false);
}

static SH1106_RET sh1106_set_point(uint8_t x, uint8_t y)
{
	uint8_t data[3];
	uint32_t i = 0;

	if (x >= SH1106_X_PIXELS || y >= SH1106_ROWS)
	{
		return SH1106_POINT_ERROR;
	}

	sh1106_manager.column = x;
	sh1106_manager.line   = y;

	x += 2;

	data[0] = 0xB0 + y;
	data[1] = x & 0x0F;
	data[2] = 0x10 | (x >> 4);

	for(; i < 3; i++)
	{
		lcd_write(COMMAND, &data[i], 1);
	}

	return SH1106_OK;
}

SH1106_RET sh1106_write_image(uint8_t x, uint8_t y, const uint8_t* string)
{

	SH1106_RET ret = SH1106_OK;
	uint32_t i = 0;
	if(!sh1106_manager.is_init)
	{
		ret = SH1106_NOT_INITIALIZED;

		return ret;
	}

	ret = sh1106_set_point(x, y);

	if(ret)
	{
		return ret;
	}

	lcd_write(WRITE, string, 128);
	return ret;
}



static uint8_t get_eight_multiple(uint8_t num)
{
	uint8_t divi = num & 7;

	divi = 8 - divi;

	return num + divi;
}

static uint8_t bit_reverse(uint8_t *data)
{
	uint8_t tmp = 0;

	tmp |= (*data & 0x80) >> 7;
	tmp |= (*data & 0x40) >> 5;
	tmp |= (*data & 0x20) >> 3;
	tmp |= (*data & 0x10) >> 1;
	tmp |= (*data & 0x08) << 1;
	tmp |= (*data & 0x04) << 3;
	tmp |= (*data & 0x02) << 5;
	tmp |= (*data & 0x01) << 7;

	return tmp;
}

static void layer_draw(uint8_t *layer, const uint8_t *btm, uint8_t l_width, uint8_t f_width, uint8_t f_height)
{
	uint32_t i = 0, j = 0;
    uint32_t l_idx = 0;
    uint32_t f_idx = 0;

	for(i = 0; i < f_height; i++)
	{
		l_idx = i * l_width;

		for(j = 0; j < f_width; j++)
		{
			layer[l_idx >> 3] |= ((btm[f_idx >> 3] << (f_idx & 0x7)) & 0x80) >> (l_idx & 0x7);
            l_idx++;
            f_idx++;
		}
	}

}

static void advanceXY(uint32_t colums)
{
	sh1106_manager.column += colums;

	if(sh1106_manager.column >= SH1106_X_PIXELS)
	{
		sh1106_manager.column %= SH1106_X_PIXELS;
		sh1106_manager.line++;
		sh1106_manager.line %= SH1106_ROWS;
		sh1106_set_point(sh1106_manager.column, sh1106_manager.line);
	}
}

uint8_t get_width(void)
{
	return SH1106_X_PIXELS;
}

uint8_t get_height(void)
{
	return SH1106_Y_PIXELS;
}

SH1106_RET sh1106_clear(void)
{
	uint8_t i = 0, j = 0;
	uint8_t data = 0;

	if(!sh1106_manager.is_init)
	{
		return SH1106_NOT_INITIALIZED;
	}

	for(i = SH1106_ROWS; i > 0; i--)
	{
		sh1106_set_point(0, i - 1);

		for(j = SH1106_X_PIXELS; j > 0; j--)
			lcd_write(WRITE, &data, 1);
	}

	sh1106_set_point(0, 0);

	return SH1106_OK;
}

SH1106_RET sh1106_inverse(void)
{
	static bool is_black = true;
	uint8_t data = 0;

	if(!sh1106_manager.is_init)
	{
		return SH1106_NOT_INITIALIZED;
	}

	if(is_black)
	{
		data = 0xA7;
	}
	else
	{
		data = 0xA6;
	}

	lcd_write(COMMAND, &data, 1);

	is_black = ~is_black;

	return SH1106_OK;
}

SH1106_RET sh1106_write_string(uint8_t x, uint8_t y, uint8_t *string)
{
	SH1106_RET ret = SH1106_OK;
	uint32_t i = 0;
	uint8_t len = strlen(string);


	if(!sh1106_manager.is_init)
	{
		ret = SH1106_NOT_INITIALIZED;

		return ret;
	}

	ret = sh1106_set_point(x, y);

	if(ret)
	{
		return ret;
	}

	for(; i < len; i++)
	{
		uint8_t tmp[6] = { 0, };

		memcpy(tmp, ASCII[string[i] - 0x20], 5);

		lcd_write(WRITE, tmp, 6);
	}

	return ret;
}



SH1106_RET sh1106_init(uint8_t port, uint8_t dc_pin, uint8_t reset_pin)
{
	Sh0116Manager *self = &sh1106_manager;
	uint32_t i = 0;
	 uint8_t lcd_init[] =
	{
			0xAE, 0x02, 0x10, 0x40, 0xB0, 0x81, 0x80, 0xA1, 0xA6,
			0xA8, 0x3F, 0xAd, 0x8B, 0x30, 0xC8, 0xD3, 0x00, 0xD5,
			0x80, 0xD9, 0x1F, 0xDA, 0x12, 0xDB, 0x40, 0xAF,
	};

	const int freq = 1000000;
	const int bits = 8;
	const int conf = 0;

	if(self->is_init)
	{
		return SH1106_OK;
	}

	if(port > SPI_MAX)
	{
		return SH1106_PORT_NUMBER_ERROR;
	}

	self->port = port;
	self->dc_pin = dc_pin;
	self->reset_pin = reset_pin;

	gpio_write(self->reset_pin, 0);

	self->spi = up_spiinitialize(self->port);

	gpio_write(self->reset_pin, 1);

	SPI_SETFREQUENCY(self->spi, freq);
	SPI_SETBITS(self->spi, bits);
	SPI_SETMODE(self->spi, conf);

	for(i = 0; i < sizeof(lcd_init); i++)
	{
		lcd_write(COMMAND, &lcd_init[i], 1);
	}

	self->is_init = true;

	sh1106_clear();

	return SH1106_OK;
}
//++

void lcd_background(void){
	sh1106_init(0, 51, 52);
	sh1106_write_image(0,0,back1);
	sh1106_write_image(0,1,back2);
	sh1106_write_image(0,2,back3);
	sh1106_write_image(0,3,back4);
	sh1106_write_image(0,4,back5);
	sh1106_write_image(0,5,back6);
	sh1106_write_image(0,6,back7);
	sh1106_write_image(0,7,back8);
}

