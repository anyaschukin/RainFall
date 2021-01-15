# Level bonus 0

## Vulnerability



## Context

We find a binary with owner ```bonus1``` and SUID.
```
bonus0@RainFall:~$ ls -l
total 8
-rwsr-s---+ 1 bonus1 users 5566 Mar  6  2016 bonus0
```
When run it seems to accept 2 seperate inputs on stdin, and print them with a space between.
```
bonus0@RainFall:~$ ./bonus0
 -
wait...
 -
what?
wait... what?
```

## Solution


