# Level 0

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
If we run the binary ```level0``` without arguments it segfaults
```
level0@RainFall:~$ ./level0
Segmentation fault (core dumped)
```
If we run it with random arguments it prints "No !"
```
level0@RainFall:~$ ./level0 hello
No !
```
Let's have a closer look in gdb.
```
level0@RainFall:~$ gdb level0
...
(gdb) disas main
...
   0x08048ed4 <+20>:	call   0x8049710 <atoi>
   0x08048ed9 <+25>:	cmp    $0x1a7,%eax
   0x08048ede <+30>:	jne    0x8048f58 <main+152>
...
```
It looks like we call atoi on argv[1], then compare it with 0x1a7, if not equal jumping to the end.

0x1a7 in decimal is 423, so lets try running the binary with argument 423.
```
level0@RainFall:~$ ./level0 423
$ whoami
level1
```
We are given a new shell in which the effective uid is level01.

Here we can cat the password. Then exit and log in to level1.
```
$ cat /home/user/level1/.pass
1fe8a524fa4bec01ca4ea2a869af2a02260d4a7d5fe7e7c24d8617e6dca12d3a
$ exit
level0@RainFall:~$ su level1
Password: 1fe8a524fa4bec01ca4ea2a869af2a02260d4a7d5fe7e7c24d8617e6dca12d3a
```

## Recreate the exploitable binary

As user level1:

Compile the program in /tmp
```
gcc source.c -o ft_level0
```
Edit permissions including suid
```
chmod u+s ft_level0
chmod +wx ft_level0
```
Move the program to home directory
```
mv ft_level0 ~
```
As user level0:
Run the program
```
./ft_level0 423
```
