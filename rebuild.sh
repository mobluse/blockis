#!/bin/sh
# sudo apt-get install libncurses5-dev
p=blockis; gcc $p.c -o $p -lncurses -std=c99
# Enable SPI using raspi-config or rc-gui.
p=blockis_max7219; gcc $p.c max7219.c -o $p -lncurses -std=c99
