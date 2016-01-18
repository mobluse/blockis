# Blockis
Blockis is similar to Tetris. C99- &amp; J2ME-versions included.

The C99-versions of Blockis runs in a terminal (Linux console) or 
on an 8x8 LED matrix. I made this mainly to educate in C, but also as
a preparation for an Arduino-program.

A video of the running nCurses program: https://youtu.be/RkcHImd9MX0  
A video of the running 8x8 LED matrix program: https://youtu.be/O5Xhr-NHvAw

How to connect the 8x8 LED matrix to the GPIO-pins of a Raspberry Pi:

| Board Pin | Name | Remarks     | RPi Pin | RPi Function      |
|----------:|:-----|:------------|--------:|-------------------|
| 1         | VCC  | +5V Power   | 2       | 5V0               |
| 2         | GND  | Ground      | 6       | GND               |
| 3         | DIN  | Data In     | 19      | GPIO 10 (MOSI)    |
| 4         | CS   | Chip Select | 24      | GPIO 8 (SPI CE0)  |
| 5         | CLK  | Clock       | 23      | GPIO 11 (SPI CLK) |
(Table from https://github.com/rm-hull/max7219.) 

Discuss this resource in Swedish (på svenska): https://groups.google.com/forum/#!topic/coderdojo-lund/5CGq0gMiQac  
A video of the 8x8 LED matrix program is also in Swedish: https://youtu.be/A5NT93l-vrI
