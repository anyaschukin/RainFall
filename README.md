# Rainfall

Discover gdb disassembly and binary exploitation through 14 little challenges

For each level:

* ```flag``` password for next level

* ```README.md``` how to find password for each level

* ```source.c``` the reverse engineered binary

See the [subject](https://github.com/anyashuka/Rainfall/blob/master/subject.pdf) for more details.


## Levels Overview

* Level 0 - gdb disassembly reveals argument password

* Level 1 - gets() stack overflow

* Level 2 - gets() stack overflow, return address in heap

* Level 3 - printf() format string attack

* Level 4 - printf() format string attack, with %d

* Level 5 - printf() format string attack, with %d

* Level 6 - strcpy() buffer overflow

* Level 7 - strcpy() buffer overflow

* Level 8 - idiocy

* Level 9 - memcpy() buffer overflow

* bonus 0 - strcpy() buffer overflow

* bonus 1 - memcpy() buffer overflow

* bonus 2 - strcat() buffer overflow, exploit in env

* bonus 3 - strcmp() with empty argument

## Virtual Machine setup

On Mac OSX, install VirtualBox and create a VM

> New VM - Type : Linux - Version : (Oracle 64-bit)

> New VM > settings > Network > Adapter 1 > Attached to: Bridged Adapter

Mount the SnowCrash.iso to the VM:

> New VM > settings > Storage > Under IDE Controller there is a CD icon with a + sign on it...

> click on the CD then select RainFall.iso
