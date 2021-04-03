# RainFall

Discover gdb disassembly, binary exploitation & reverse-engineering through 14 little challenges.

In the folder for each level you will find:

* **flag** - password for next level

* **README.md** - how to find the password

* **source.c** - the reverse engineered binary

See the [subject](https://github.com/anyashuka/Rainfall/blob/master/subject.pdf) for more details.

For more gdb & exploitation fun check out the next project [OverRide](https://github.com/anyaschukin/OverRide).

*Final Score 125/100*


## Getting Started

First download from 42 [*RainFall.iso*](https://projects.intra.42.fr/uploads/document/document/2087/RainFall.iso).

### Virtual Machine setup

On Mac OSX, install [VirtualBox](https://www.virtualbox.org/).

In VirtualBox create a new VM (click new).

* Name and operating system - Type: Linux, Version: (Oracle 64-bit)

Continue through all the next steps with the default settings:

* Memory size: 4MB
* Hard disk: Create a disk now
* Hard disk file type: VDI(VirtualBox Disk Image)
* Storage on physical hard disk: Dynamically allocated
* File size: 12,00GB

Next click Settings > Network > Adapter 1 > Attached to: Bridged Adapter.

Still in settings click Storage > Right of "Controller: IDE", there is a CD icon with a + sign (add optical drive).
Click Add Disk Image, and select *RainFall.iso*.

Click Start to start the VM, once runnning it should show the VM IP address and prompt user to login.

### SSH connect

Log in from a separate shell as user *level0* with password *level0*.

```ssh level0@{VM_IP} -p 4242```

<img src="https://github.com/anyashuka/Rainfall/blob/master/img/ssh.png" width="800">

### Level Up

As user *level0* the goal is to read the password for user *level1*, found at */home/user/level1/.pass*. However, user *level0* does not have permissions to read this file.

In the home folder for user *level0* is a binary *level0* with SUID set and owner *level1*.

<img src="https://github.com/anyashuka/Rainfall/blob/master/img/suid.png" width="400">

This means when we execute the binary *level0*, we do so with the permissions of user *level1*.

We must find a vulnerability in the binary *level0* with gdb. Then exploit the vulnerability to run *system("/bin/sh")*, opening a shell as user *level1* where we have permissions to read the password.

```cat /home/user/level1/.pass```

Then log in as user *level1*.

```su level1```

<img src="https://github.com/anyashuka/Rainfall/blob/master/img/su.png" width="330">

Repeat for each level.


## Reverse-engineered binary

For each level, we reverse engineered the original *source.c* by examining the gdb disassembly of the binary.

Each level's *README.md* details how to to compile *source.c*, set SUID, then run with the exploit, demonstrating the same vulnerability.


## Levels Overview

* [0](https://github.com/anyashuka/Rainfall/tree/master/level0) - hardcoded password

* [1](https://github.com/anyashuka/Rainfall/tree/master/level1) - gets() stack overflow

* [2](https://github.com/anyashuka/Rainfall/tree/master/level2) - gets() stack overflow, return address in heap

* [3](https://github.com/anyashuka/Rainfall/tree/master/level3) - printf() format string attack

* [4](https://github.com/anyashuka/Rainfall/tree/master/level4) - printf() format string attack, with %d

* [5](https://github.com/anyashuka/Rainfall/tree/master/level5) - printf() format string attack, with %d

* [6](https://github.com/anyashuka/Rainfall/tree/master/level6) - strcpy() buffer overflow

* [7](https://github.com/anyashuka/Rainfall/tree/master/level7) - strcpy() buffer overflow

* [8](https://github.com/anyashuka/Rainfall/tree/master/level8) - idiocy

* [9](https://github.com/anyashuka/Rainfall/tree/master/level9) - memcpy() buffer overflow

* [Bonus 0](https://github.com/anyashuka/Rainfall/tree/master/bonus0) - strcpy() buffer overflow

* [Bonus 1](https://github.com/anyashuka/Rainfall/tree/master/bonus1) - memcpy() buffer overflow

* [Bonus 2](https://github.com/anyashuka/Rainfall/tree/master/bonus2) - strcat() buffer overflow, exploit in env

* [Bonus 3](https://github.com/anyashuka/Rainfall/tree/master/bonus3) - strcmp() with empty argument

## Team

I wrote this project in a team with the awesome [@dfinnis](https://github.com/dfinnis).
