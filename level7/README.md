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
```
level7@RainFall:~$ gdb -q level7
(gdb) disas main
   0x080485a0 <+127>:	call   0x80483e0 <strcpy@plt>
   0x080485bd <+156>:	call   0x80483e0 <strcpy@plt>
   0x080485d3 <+178>:	call   0x8048430 <fopen@plt>
   0x080485eb <+202>:	call   0x80483c0 <fgets@plt>
   0x080485f7 <+214>:	call   0x8048400 <puts@plt>
```
Let's take a deeper look at the fopen parameters.
It looks like it opens the file with the password we're looking for!
```
(gdb) x/s 0x80486e9
0x80486e9:	 "r"
(gdb) x/s 0x80486eb
0x80486eb:	 "/home/user/level8/.pass"
```






