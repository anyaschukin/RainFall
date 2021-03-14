# Rainfall

Discover gdb disassembly and binary exploitation through 14 little challenges.

In the folder for each level you will find:

* *flag* - password for next level

* *README.md* - how to find the password

* *source.c* - the reverse engineered binary

See the [subject](https://github.com/anyashuka/Rainfall/blob/master/subject.pdf) for more details.

#### Final Score 125/100


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

Still in settings click Storage > Right of "Controller: IDE" there is a CD icon with a + sign (add optical drive).
Click Add Disk Image, and select *RainFall.iso*.

Click start to start the VM, once runnning it should show the VM IP address and prompt user to login.

### SSH connect

Log in from a separate shell as user *level0* with password *level0*.

```ssh level0@{VM_IP} -p 4242```

<img src="https://github.com/anyashuka/Rainfall/blob/master/img/ssh.png" width="500">

### Level Up

As user *level0* the goal is to find the password (flag) for user *flag0*, see [level00/](https://github.com/anyashuka/Rainfall/tree/master/level0) for how.

Log in as user *flag00*.

```su flag00```

Run *getflag* to find the password for user *level01*.

```getflag```

Log in as user *level01*.

```su level01```

<img src="https://github.com/anyashuka/Rainfall/blob/master/img/su.png" width="500">

Repeat these steps for each level.



## Levels Overview

* Level 0 - gdb disassembly reveals hardcoded password

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
