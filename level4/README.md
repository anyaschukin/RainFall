# Level 4

## Vulnerability

Printf format string exploit

## Context

We find a binary with owner level5 and SUID.
```
level4@RainFall:~$ ls -l
-rwsr-s---+ 1 level5 users 5252 Mar  6  2016 level4
```
The binary seems to do the same thing as in previous levels, read from stdin, and print what is given.
```
level4@RainFall:~$ ./level4
oh hi!
oh hi!
```

## Solution

Investigating with gdb, we find fgets(), unfortunately protected against buffer overflow attack. But printf() may provide our vulnerability, similar to the last level.
```
level4@RainFall:~$ gdb -q level4
...
(gdb) disas main
...
   0x080484ad <+6>:	call   0x8048457 <n>
...
(gdb) disas n
...
...
   0x0804847a <+35>:	call   0x8048350 <fgets@plt>
   0x08048488 <+49>:	call   0x8048444 <p>
...
(gdb) disas p
...
   0x08048450 <+12>:	call   0x8048340 <printf@plt>
```
