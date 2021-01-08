# Level 2

## Vulnerability

Buffer overflow

## Context

We find a binary with owner ```level3``` and suid set.
```
level2@RainFall:~$ ls -l
-rwsr-s---+ 1 level3 users 5403 Mar  6  2016 level2
```

## Solution

Running the binary, it seems to read from stdin, and print what is given.
```
level2@RainFall:~$ ./level2
oh hi
oh hi
```
We get some weird output with longer input, then after a certain length it segfaults.
```
level2@RainFall:~$ ./level2
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaJaaaaaaa
level2@RainFall:~$ ./level2
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaJaaaaaaaa
Segmentation fault (core dumped)
```
When we inspect with gdb we find main calls function ```p```. Function ```p``` includes a call to ```gets()``` which is vulnerable to buffer overflow.
```
level2@RainFall:~$ gdb -q level2
...
(gdb) disas main
...
   0x08048545 <+6>:	call   0x80484d4 <p>
...
(gdb) disas p
...
   0x080484ed <+25>:	call   0x80483c0 <gets@plt>
...
```
Function ```p``` checks the return address starts with ```b```
```
(gdb) disas p
...
   0x080484fb <+39>:	and    $0xb0000000,%eax
   0x08048500 <+44>:	cmp    $0xb0000000,%eax
   0x08048505 <+49>:	jne    0x8048527 <p+83>
...
```
This prevents us from overwriting the return address with and address in the stack, or an environment variable, where we have written our malicious code.

However, we can use the heap.


```
level2@RainFall:~$ cat /usr/include/i386-linux-gnu/asm/unistd_32.h | grep "execve"
#define __NR_execve		 11
```

