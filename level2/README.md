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
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaJaaaaa
level2@RainFall:~$ ./level2
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
Segmentation fault (core dumped)
```
