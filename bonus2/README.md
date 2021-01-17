# Level bonus 2

## Vulnerability

strcat() heap overflow, return-to-libc

## Context

We find a binary with owner ```bonus3``` and SUID.
```
bonus2@RainFall:~$ ls -l
-rwsr-s---+ 1 bonus3 users 5664 Mar  6  2016 bonus2
```
When run with 0 or 1 arguments, it does nothing. Run with 2 arguments it prints "Hello " + first argument.
```
bonus2@RainFall:~$ ./bonus2
bonus2@RainFall:~$ ./bonus2 wait
bonus2@RainFall:~$ ./bonus2 wait what?
Hello wait
```

## Solution

