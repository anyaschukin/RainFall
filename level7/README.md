# Level 7

## Vulnerability



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


