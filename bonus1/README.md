# Level bonus 1

## Vulnerability

memcpy() buffer overflow

## Context

We find a binary with owner ```bonus2``` and SUID.
```
bonus1@RainFall:~$ ls -l
-rwsr-s---+ 1 bonus2 users 5043 Mar  6  2016 bonus1
```
When run with no arguments it segfaults. with any number of random arguments it does nothing.
```
bonus1@RainFall:~$ ./bonus1
Segmentation fault (core dumped)
bonus1@RainFall:~$ ./bonus1 hi
bonus1@RainFall:~$
```

## Solution

Investigating with gdb we find only ```main()```, no other functions. ```main()``` calls ```memcpy()``` which is vulnerable to buffer overflow.

First ```main()``` calls ```atoi(argv[1])```, and exits the program if not less than 10. 
