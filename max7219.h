/////////////////////////////////////////////////////////////////////
//
// SPI to MAX7219 8x8 LED matrix program in C for Linux.
// Copyright (C) 2016 by Mikael O. Bonnier, Lund, Sweden.
// License: GNU GPL v3 or later, http://www.gnu.org/licenses/gpl-3.0.txt
// Donations are welcome to PayPal mikael.bonnier@gmail.com.
// The source code is at <https://github.com/mobluse/blockis/>.
//
// Revision history:
// 2016-Jan-16:     v.0.0.1   C
/* indent -nbad -bap -nbc -bbo -hnl -br -brs -c33 -cd33 -ncdb -ce -ci4 -cli0 -d0 -di1 -nfc1 -i4 -ip0 -l80 -lp -npcs -nprs -npsl -sai -saf -saw -ncs -nsc -sob -nfca -cp33 -ss -ts8 -il1 --no-tabs max7219.c  */

// SPI
#include <curses.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <stdint.h>
int init_max7219();
int init_spi(int fd);
void clear_led_matrix(int fd);
void col_write(int fd, uint8_t address, uint8_t data);
