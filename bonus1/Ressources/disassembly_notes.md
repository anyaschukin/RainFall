# Disassembly

```
bonus1@RainFall:~$ gdb -q bonus1
...
(gdb) disas main
Dump of assembler code for function main:
   0x08048424 <+0>:	push   %ebp
   0x08048425 <+1>:	mov    %esp,%ebp
   0x08048427 <+3>:	and    $0xfffffff0,%esp
   
   0x0804842a <+6>:	sub    $0x40,%esp             ; allocate 64 bytes for local variables
   
   0x0804842d <+9>:	mov    0xc(%ebp),%eax         ; Store all argv in eax
   0x08048430 <+12>:	add    $0x4,%eax              ; increment to argv[1]
   0x08048433 <+15>:	mov    (%eax),%eax            ; pointer to argv[1]
   
   0x08048435 <+17>:	mov    %eax,(%esp)            ; load pointer to argv[1] as argument to atoi
   0x08048438 <+20>:	call   0x8048360 <atoi@plt>   ; atoi(argv[1])
   0x0804843d <+25>:	mov    %eax,0x3c(%esp)        ; store atoi return
   0x08048441 <+29>:	cmpl   $0x9,0x3c(%esp)        ; compare atoi return with 9
   0x08048446 <+34>:	jle    0x804844f <main+43>    ; jump past return(1) if atoi(argv[1]) <= 9
   
   0x08048448 <+36>:	mov    $0x1,%eax              : load (1) for return(1)
   0x0804844d <+41>:	jmp    0x80484a3 <main+127>   ; jump to end
   
   0x0804844f <+43>:	mov    0x3c(%esp),%eax
   0x08048453 <+47>:	lea    0x0(,%eax,4),%ecx      ; store atoi(argv[1]) * 4
   0x0804845a <+54>:	mov    0xc(%ebp),%eax         ; retreive argv
   0x0804845d <+57>:	add    $0x8,%eax              ; increment to argv[2]
   0x08048460 <+60>:	mov    (%eax),%eax            ; pointer to argv[2]
   0x08048462 <+62>:	mov    %eax,%edx
   0x08048464 <+64>:	lea    0x14(%esp),%eax
   0x08048468 <+68>:	mov    %ecx,0x8(%esp)         ; load argument - atoi(argv[1]) * 4
   0x0804846c <+72>:	mov    %edx,0x4(%esp)         ; load argument - argv[2]
   0x08048470 <+76>:	mov    %eax,(%esp)            ; load argument - str
   0x08048473 <+79>:	call   0x8048320 <memcpy@plt> ; memcpy(str, argv[2], atoi(argv[1]) * 4)
   
   0x08048478 <+84>:	cmpl   $0x574f4c46,0x3c(%esp) ; if not atoi(argv[1]) == 0x574f4c46, return(0)
   0x08048480 <+92>:	jne    0x804849e <main+122>   ; jump to return(0)
   
   0x08048482 <+94>:	movl   $0x0,0x8(%esp)         ; load argument - 0
   0x0804848a <+102>:	movl   $0x8048580,0x4(%esp)   ; load argument - "sh"
   0x08048492 <+110>:	movl   $0x8048583,(%esp)      ; load argument - "/bin/sh"
   0x08048499 <+117>:	call   0x8048350 <execl@plt>  ; execl("/bin/sh", "sh", 0)
   
   0x0804849e <+122>:	mov    $0x0,%eax              ; load (0) for return(1)
   0x080484a3 <+127>:	leave
   0x080484a4 <+128>:	ret
End of assembler dump.
```
