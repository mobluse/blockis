# Blockis
Blockis is similar to Tetris. C99- &amp; J2ME-versions included.

The C99-versions of Blockis runs in a terminal (Linux console) or 
on an 8x8 LED matrix. I made this mainly to educate in C, but also as
a preparation for an Arduino-program.

Keys to play: W=Up, A=Left, S=Down=Ins, D=Right, E=PgUp=Del, Q=PgDn=End=Home.

Download using Git (you may need to install Git first): `git clone https://github.com/mobluse/blockis`  
Install in Ubuntu/Raspbian/Debian: `cd blockis; ./install.sh` and run `./blockis` and it runs w/o known bugs.
It also runs in [Bash on Ubuntu on Windows](https://github.com/Microsoft/BashOnWindows) for Windows 10, but with 
some [bugs](https://github.com/Microsoft/BashOnWindows/issues/301):
Home and End keys don't always work to pause or quit. If you press and hold S, Down or Ins and then release, the game often stops,
but can be resumed by pressing the same key again. When you then resume, the block is rotated, because
it seems as if the key isn't consumed in that case.
It also works in _Bash on Ubuntu on Windows_ using SSH to a machine with Ubuntu with Blockis and then there is just one bug: 
Home and End keys never work.

It is also possible to install in Cygwin, but you have to install GCC and some libraries manually using setup-x86_64.exe, then 
you can run `./install.sh`.
In Cygwin Blockis has one bug: If you press and hold S, Down or Ins and then release, the block continues to fall fast for a while,
and even after a new block.
It also works in Cygwin using SSH to a machine with Ubuntu with Blockis and then it's bug-free, just as in real Ubuntu or Raspbian.

The version of Blockis using an SPI-LED matrix will only work on a Raspberry Pi, and it will not even compile in Cygwin,
due to missing header files.

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
A video of the 8x8 LED matrix program also exists in Swedish: https://youtu.be/A5NT93l-vrI
