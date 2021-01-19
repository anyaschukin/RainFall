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

First ```main()``` reads the first argument ```nb = atoi(argv[1])```, and exits the program if ```nb``` not less than 10. So any number less than 10, easy!

Next ```memcpy()``` copies ```argv[2]``` to a buffer on the stack with the potential to overflow ```nb```.

Finally, if now ```nb``` == 0x574f4c46 a shell is opened.

So we want to use ```memcpy()``` to overflow ```nb``` with 0x574f4c46. The problem is ```memcpy()``` copies ```nb``` * 4 bytes, max 9 * 4 = 36, not long enough to reach and overflow ```nb``` at 40 bytes.
