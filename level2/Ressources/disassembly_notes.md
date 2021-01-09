# Disassembly

```
level2@RainFall:~$ gdb -q level2
...
(gdb) disas main
Dump of assembler code for function main:
   0x0804853f <+0>:	push   %ebp                     ; save the beginning of the previous function's stackframe
   0x08048540 <+1>:	mov    %esp,%ebp                ; store contents of %esp(address of top of stack) in %ebp
   0x08048542 <+3>:	and    $0xfffffff0,%esp         ; align Stack on 16 bytes, unnecessary as there are no SIMD instructions
   0x08048545 <+6>:	call   0x80484d4 <p>            ; call function p()
   0x0804854a <+11>:	leave
   0x0804854b <+12>:	ret
End of assembler dump.

(gdb) disas p
Dump of assembler code for function p:
   0x080484d4 <+0>:	push   %ebp
   0x080484d5 <+1>:	mov    %esp,%ebp
   0x080484d7 <+3>:	sub    $0x68,%esp               ; allocate 104 bytes for local variables
   0x080484da <+6>:	mov    0x8049860,%eax           ; move value from data segment to %eax (stdout)
   0x080484df <+11>:	mov    %eax,(%esp)            ; set argument for fflush() function
   0x080484e2 <+14>:	call   0x80483b0 <fflush@plt> ; fflush(stdout)
   
   0x080484e7 <+19>:	lea    -0x4c(%ebp),%eax       ; load address ebp-0x4c in eax (gets() takes a pointer as argument)
   0x080484ea <+22>:	mov    %eax,(%esp)            ; set argument for fflush() function
   0x080484ed <+25>:	call   0x80483c0 <gets@plt>   ; call gets() (vulnerable to buffer overflow)
   
   0x080484f2 <+30>:	mov    0x4(%ebp),%eax
   0x080484f5 <+33>:	mov    %eax,-0xc(%ebp)
   0x080484f8 <+36>:	mov    -0xc(%ebp),%eax
   0x080484fb <+39>:	and    $0xb0000000,%eax       ; logical and on %eax
   0x08048500 <+44>:	cmp    $0xb0000000,%eax       ; check if return address is in stack
   0x08048505 <+49>:	jne    0x8048527 <p+83>       ; jump to puts
   
   0x08048507 <+51>:	mov    $0x8048620,%eax        ; store value at 0x8048620 in eax (eax = "%p\n").
   0x0804850c <+56>:	mov    -0xc(%ebp),%edx        ; store return_address (edx)
   0x0804850f <+59>:	mov    %edx,0x4(%esp)         ; set argument for printf(), print 4 chars
   0x08048513 <+63>:	mov    %eax,(%esp)            ; set argument for printf()
   0x08048516 <+66>:	call   0x80483a0 <printf@plt> ; call printf()
   
   0x0804851b <+71>:	movl   $0x1,(%esp)            ; set argument for exit(1)
   0x08048522 <+78>:	call   0x80483d0 <_exit@plt>  ; call exit(1)
   
   0x08048527 <+83>:	lea    -0x4c(%ebp),%eax       ; load address bp-0x4c in eax (puts() takes a pointer as argument)
   0x0804852a <+86>:	mov    %eax,(%esp)            ; set argument for puts()
   0x0804852d <+89>:	call   0x80483f0 <puts@plt>   ; call puts()
   
   0x08048532 <+94>:	lea    -0x4c(%ebp),%eax
   0x08048535 <+97>:	mov    %eax,(%esp)
   0x08048538 <+100>:	call   0x80483e0 <strdup@plt>
   0x0804853d <+105>:	leave
   0x0804853e <+106>:	ret
End of assembler dump.
```
