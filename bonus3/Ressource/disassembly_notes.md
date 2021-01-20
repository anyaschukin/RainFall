# Disassembly

```
(gdb) disas main
Dump of assembler code for function main:
   0x080484f4 <+0>:	push   %ebp
   0x080484f5 <+1>:	mov    %esp,%ebp
   0x080484f7 <+3>:	push   %edi
   0x080484f8 <+4>:	push   %ebx
   0x080484f9 <+5>:	and    $0xfffffff0,%esp

   0x080484fc <+8>:	sub    $0xa0,%esp               ; allocate 160 bytes for local variables

   0x08048502 <+14>:	mov    $0x80486f0,%edx          ; "r"
   0x08048507 <+19>:	mov    $0x80486f2,%eax          ; "/home/user/end/.pass"
   0x0804850c <+24>:	mov    %edx,0x4(%esp)           ; load argument 2 - "r"
   0x08048510 <+28>:	mov    %eax,(%esp)              ; load argument 1 - "/home/user/end/.pass"
   0x08048513 <+31>:	call   0x8048410 <fopen@plt>    ; fopen("/home/user/end/.pass", "r");
   0x08048518 <+36>:	mov    %eax,0x9c(%esp)          ; store fopen() return, pointer to password file

   0x0804851f <+43>:	lea    0x18(%esp),%ebx          ; buffer address
   0x08048523 <+47>:	mov    $0x0,%eax                ; load value (0), initialize buffer to 0
   0x08048528 <+52>:	mov    $0x21,%edx               ; 0x21 * 4 = 132 bytes len to write
   0x0804852d <+57>:	mov    %ebx,%edi
   0x0804852f <+59>:	mov    %edx,%ecx
   0x08048531 <+61>:	rep stos %eax,%es:(%edi)        ; memset(buffer, 0, 132);

   0x08048533 <+63>:	cmpl   $0x0,0x9c(%esp)          ; is fopen() return NULL? failed to read file
   0x0804853b <+71>:	je     0x8048543 <main+79>      ; jump to return(-1)
   
   0x0804853d <+73>:	cmpl   $0x2,0x8(%ebp)           ; 2 arguments?
   0x08048541 <+77>:	je     0x804854d <main+89>      ; jump past return(-1)
   
   0x08048543 <+79>:	mov    $0xffffffff,%eax         ; load value (-1) for return(-1)
   0x08048548 <+84>:	jmp    0x8048615 <main+289>     ; return(-1)
   
   0x0804854d <+89>:	lea    0x18(%esp),%eax          ; load buffer address
   0x08048551 <+93>:	mov    0x9c(%esp),%edx          ; load password file
   0x08048558 <+100>:	mov    %edx,0xc(%esp)           ; load argument 4 - password file
   0x0804855c <+104>:	movl   $0x42,0x8(%esp)          ; load argument 3 - read 66 bytes
   0x08048564 <+112>:	movl   $0x1,0x4(%esp)           ; load argument 2 - read once
   0x0804856c <+120>:	mov    %eax,(%esp)              ; load argument 1 - read to buffer
   0x0804856f <+123>:	call   0x80483d0 <fread@plt>    ; fread(buffer, 1, 66, f);
   
   0x08048574 <+128>:	movb   $0x0,0x59(%esp)          ; buffer[65] = 0;
   
   0x08048579 <+133>:	mov    0xc(%ebp),%eax           ; load argv
   0x0804857c <+136>:	add    $0x4,%eax                ; increment 1 to argv[1]
   0x0804857f <+139>:	mov    (%eax),%eax              ; pointer to argv[1]
   0x08048581 <+141>:	mov    %eax,(%esp)              ; load argument 1 - pointer to argv[1]
   0x08048584 <+144>:	call   0x8048430 <atoi@plt>     ; atoi(argv[1])
   
   0x08048589 <+149>:	movb   $0x0,0x18(%esp,%eax,1)   ; buffer[atoi(argv[1])] = 0;
   
   0x0804858e <+154>:	lea    0x18(%esp),%eax          ; load buffer address
   0x08048592 <+158>:	lea    0x42(%eax),%edx          ; increment to buffer[66] address
   0x08048595 <+161>:	mov    0x9c(%esp),%eax          ; load password file
   0x0804859c <+168>:	mov    %eax,0xc(%esp)           ; load argument 4 - password file
   0x080485a0 <+172>:	movl   $0x41,0x8(%esp)          ; load argument 3 - read 66 bytes
   0x080485a8 <+180>:	movl   $0x1,0x4(%esp)           ; load argument 2 - read once
   0x080485b0 <+188>:	mov    %edx,(%esp)              ; load argument 1 - buffer[66] address
   0x080485b3 <+191>:	call   0x80483d0 <fread@plt>    ; fread(&buffer[66], 1, 65, f);
   
   0x080485b8 <+196>:	mov    0x9c(%esp),%eax          ; load password file
   0x080485bf <+203>:	mov    %eax,(%esp)              ; load argument 1 - pointer to file
   0x080485c2 <+206>:	call   0x80483c0 <fclose@plt>   ; fclose(f);
   
   0x080485c7 <+211>:	mov    0xc(%ebp),%eax           ; load argv
   0x080485ca <+214>:	add    $0x4,%eax                ; increment 1 to argv[1]
   0x080485cd <+217>:	mov    (%eax),%eax              ; pointer to argv[1]
   0x080485cf <+219>:	mov    %eax,0x4(%esp)           ; load argument 2 - pointer to argv[1]
   0x080485d3 <+223>:	lea    0x18(%esp),%eax          ; load buffer address
   0x080485d7 <+227>:	mov    %eax,(%esp)              ; load argument 2 - buffer address
   0x080485da <+230>:	call   0x80483b0 <strcmp@plt>   ; strcmp(buffer, argv[1])               ####
   0x080485df <+235>:	test   %eax,%eax                ; strcmp() == 0?
   0x080485e1 <+237>:	jne    0x8048601 <main+269>     ; if !0 jump past execl() opening shell
   
   0x080485e3 <+239>:	movl   $0x0,0x8(%esp)           ; load argument 3 - NULL
   0x080485eb <+247>:	movl   $0x8048707,0x4(%esp)     ; load argument 2 - "sh"
   0x080485f3 <+255>:	movl   $0x804870a,(%esp)        ; load argument 1 - "/bin/sh"
   0x080485fa <+262>:	call   0x8048420 <execl@plt>    ; execl("/bin/sh", "sh", NULL)          ####
   0x080485ff <+267>:	jmp    0x8048610 <main+284>     ; jump to return(0)

   0x08048601 <+269>:	lea    0x18(%esp),%eax          ; load buffer address
   0x08048605 <+273>:	add    $0x42,%eax               ; increment to bugger[66]
   0x08048608 <+276>:	mov    %eax,(%esp)              ; load argument 1 - &buffer[66]
   0x0804860b <+279>:	call   0x80483e0 <puts@plt>     ; puts(&buffer[66]);
   
   0x08048610 <+284>:	mov    $0x0,%eax                ; return(0)
   0x08048615 <+289>:	lea    -0x8(%ebp),%esp
   0x08048618 <+292>:	pop    %ebx
   0x08048619 <+293>:	pop    %edi
   0x0804861a <+294>:	pop    %ebp
   0x0804861b <+295>:	ret
End of assembler dump.
```
