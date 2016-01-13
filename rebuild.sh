#!/bin/sh
# sudo apt-get install libncurses5-dev
p=blockis; gcc $p.c -o $p -lncurses -std=c99
