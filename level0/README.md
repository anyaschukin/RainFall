# Level 0

## Vulnerability



## Context

In our home directory we find a binary with suid set and owner ```level1```
```
level0@RainFall:~$ ls -l
-rwsr-x---+ 1 level1 users 747441 Mar  6  2016 level0
```

## Solution

Let's start by logging in from a separate shell.
```
>$ ssh level0@{VM_IP} -p 4242
level0@192.168.0.17's password: level0
```

