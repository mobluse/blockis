/////////////////////////////////////////////////////////////////////
//
// SPI to MAX7219 8x8 LED matrix program in C for Linux.
// Copyright (C) 2016 by Mikael O. Bonnier, Lund, Sweden.
// License: GNU GPL v3 or later, http://www.gnu.org/licenses/gpl-3.0.txt
// Donations are welcome to PayPal mikael.bonnier@gmail.com.
// The source code is at <https://github.com/mobluse/blockis/>.
//
// Revision history:
// 2016-Jan-15:     v.0.0.1   C
/* indent -nbad -bap -nbc -bbo -hnl -br -brs -c33 -cd33 -ncdb -ce -ci4 -cli0 -d0 -di1 -nfc1 -i4 -ip0 -l80 -lp -npcs -nprs -npsl -sai -saf -saw -ncs -nsc -sob -nfca -cp33 -ss -ts8 -il1 --no-tabs max7219.c  */

// SPI
//#include <sys/param.h>
#include "max7219.h"

const char *device = "/dev/spidev0.0";
uint8_t mode = 0;
uint8_t bits = 8;
uint32_t speed = 200000;

int init_max7219()
{
   int fd = open(device, O_RDWR);

   if (fd < 0)
      return fd;

   init_spi(fd);

   clear_led_matrix(fd);

   // Self test
   col_write(fd, 0x0F, 0x01);
   napms(500);
   col_write(fd, 0x0F, 0x00);
   napms(500);

   // Initialize LED matrix
   col_write(fd, 0x0B, 0x07); // Scan Limit (all digits)
   col_write(fd, 0x0A, 0x07); // Intensity
   col_write(fd, 0x09, 0x00); // Decode Mode (off)
   col_write(fd, 0x0C, 0x01); // Shutdown (normal operation)

   //clear_led_matrix(fd);

   return fd;
}

void col_write(int fd, uint8_t address, uint8_t data)
{
   uint8_t tx[] = {address, data};
   write(fd, tx, 2);
}

int init_spi(int fd)
{
// See "[SPI] LED Matrix using Maxim MAX7221" by Klaas
// https://www.raspberrypi.org/forums/viewtopic.php?t=41713
   int ret = 0;

   ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
   if (ret == -1)
      return 1;

   ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
   if (ret == -1)
      return 2;

   ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
   if (ret == -1)
      return 3;

   ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
   if (ret == -1)
      return 4;

   ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
   if (ret == -1)
      return 5;

   ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
   if (ret == -1)
      return 6;

   return 0;
}

void clear_led_matrix(int fd) {
   for (uint8_t c = 1; c <= 8; c++) {
       col_write(fd, c, 0x00);
   }
}
