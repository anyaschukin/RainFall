# Disassembly

```
push   %ebp
mov    %esp,%ebp
```
Establishes a new stack frame within the callee, while preserving the stack frame of the caller. A stack frame allows consistent access to passed parameters and local variables using fixed offsets relative to EBP anywhere in the function, while ESP is free to continue being modified as needed while the function is running. 

Let's take a deeper look at ```main()```.
```
level7@RainFall:~$ gdb -q level7
...
(gdb) disas main
Dump of assembler code for function main:

   0x08048521 <+0>:	push   %ebp
   0x08048522 <+1>:	mov    %esp,%ebp
   
   0x08048524 <+3>:	and    $0xfffffff0,%esp
   0x08048527 <+6>:	sub    $0x20,%esp               ; allocates 32 bytes for main() and its local variables

   0x0804852a <+9>:	movl   $0x8,(%esp)
   0x08048531 <+16>:	call   0x80483f0 <malloc@plt>   ; malloc(8)
   0x08048536 <+21>:	mov    %eax,0x1c(%esp)          ; store the return (eax) value in esp+0x1c
   0x0804853a <+25>:	mov    0x1c(%esp),%eax          ; a = malloc(8)
   0x0804853e <+29>:	movl   $0x1,(%eax)              ; a[0] = 1
   
   0x08048544 <+35>:	movl   $0x8,(%esp)
   0x0804854b <+42>:	call   0x80483f0 <malloc@plt>   ; malloc(8)
   0x08048550 <+47>:	mov    %eax,%edx                ; store the return (eax) value in edx
   0x08048552 <+49>:	mov    0x1c(%esp),%eax          ; a = malloc(8)
   0x08048556 <+53>:	mov    %edx,0x4(%eax)           ; a[1] = (int)malloc(8)
   
   0x08048559 <+56>:	movl   $0x8,(%esp)
   0x08048560 <+63>:	call   0x80483f0 <malloc@plt>   ; malloc(8)
   0x08048565 <+68>:	mov    %eax,0x18(%esp)          ; store the return (eax) value in esp+0x18
   0x08048569 <+72>:	mov    0x18(%esp),%eax          ; b = malloc(8)
   0x0804856d <+76>:	movl   $0x2,(%eax)              ; b[0] = 2
   
   0x08048573 <+82>:	movl   $0x8,(%esp)
   0x0804857a <+89>:	call   0x80483f0 <malloc@plt>   ; malloc(8)
   0x0804857f <+94>:	mov    %eax,%edx                ; store the return (eax) value in edx
   0x08048581 <+96>:	mov    0x18(%esp),%eax          ; b = malloc(8)
   0x08048585 <+100>:	mov    %edx,0x4(%eax)           ; b[1] = (int)malloc(8)
   
   0x08048588 <+103>:	mov    0xc(%ebp),%eax           ; store the value of argv[1] in eax... eax now points to argv[1]
   0x0804858b <+106>:	add    $0x4,%eax                ; add value of 4 to eax (argv[1] + 4)
   0x0804858e <+109>:	mov    (%eax),%eax              ; create a pointer to eax
   0x08048590 <+111>:	mov    %eax,%edx                ; save the value of eax in edx
   
   0x08048592 <+113>:	mov    0x1c(%esp),%eax
   0x08048596 <+117>:	mov    0x4(%eax),%eax           ; eax = a[1]
   
   0x08048599 <+120>:	mov    %edx,0x4(%esp)           ; set argument 2
   0x0804859d <+124>:	mov    %eax,(%esp)              ; set argument 1
   0x080485a0 <+127>:	call   0x80483e0 <strcpy@plt>   ; strcpy(esp, esp+4) ... strcpy((char *)a[1], av[1])
   
   0x080485a5 <+132>:	mov    0xc(%ebp),%eax           ; store the value of argv[2] in eax... eax now points to argv[2]
   0x080485a8 <+135>:	add    $0x8,%eax
   0x080485ab <+138>:	mov    (%eax),%eax
   0x080485ad <+140>:	mov    %eax,%edx                ; save the value of eax in edx
   
   0x080485af <+142>:	mov    0x18(%esp),%eax          
   0x080485b3 <+146>:	mov    0x4(%eax),%eax           ; eax = b[1]
   
   0x080485b6 <+149>:	mov    %edx,0x4(%esp)           ; set argument 2
   0x080485ba <+153>:	mov    %eax,(%esp)              ; set argument 1
   0x080485bd <+156>:	call   0x80483e0 <strcpy@plt>   ; strcpy(esp, esp+4) ... strcpy((char *)b[1], av[2])
   
   0x080485c2 <+161>:	mov    $0x80486e9,%edx          ; set argument 2
   0x080485c7 <+166>:	mov    $0x80486eb,%eax          ; set argument 1
   0x080485cc <+171>:	mov    %edx,0x4(%esp)           ; move edx into esp+4 and set up esp+4 as argument
   0x080485d0 <+175>:	mov    %eax,(%esp)              ; move eax into esp and set up esp as argument
   0x080485d3 <+178>:	call   0x8048430 <fopen@plt>    ; fopen(esp, esp+4) ... fopen("/home/user/level8/.pass", "r"))
   
   0x080485d8 <+183>:	mov    %eax,0x8(%esp)           ; set argument 3
   0x080485dc <+187>:	movl   $0x44,0x4(%esp)          ; set argument 2 (load value of 64 in fgets argument 2)
   0x080485e4 <+195>:	movl   $0x8049960,(%esp)        ; set argument 1
   0x080485eb <+202>:	call   0x80483c0 <fgets@plt>    ; fgets(esp, esp+4, esp+8) ... fgets(c, 68, fopen("/home/user/level8/.pass", "r"))
   
   0x080485f0 <+207>:	movl   $0x8048703,(%esp)        ; set arguments
   0x080485f7 <+214>:	call   0x8048400 <puts@plt>     ; puts("~~")
   
   0x080485fc <+219>:	mov    $0x0,%eax                ; return(0)
   0x08048601 <+224>:	leave
   0x08048602 <+225>:	ret
```

Let's take a closer look at ```m()```.
```
(gdb) disas m
Dump of assembler code for function m:

   0x080484f4 <+0>:	push   %ebp
   0x080484f5 <+1>:	mov    %esp,%ebp                    ; create a new stack frame
   
   0x080484f7 <+3>:	sub    $0x18,%esp                   ; 24 bytes are allocated for the function and its local variables
   
   0x080484fa <+6>:	movl   $0x0,(%esp)                  ; set arguments
   0x08048501 <+13>:	call   0x80483d0 <time@plt>         ; time(0)
   
   0x08048506 <+18>:	mov    $0x80486e0,%edx              ; edx = "%s - %d\n"
   0x0804850b <+23>:	mov    %eax,0x8(%esp)               ; set argument 3
   0x0804850f <+27>:	movl   $0x8049960,0x4(%esp)         ; set argument 2 (store the address of global variable c in esp+4)
   0x08048517 <+35>:	mov    %edx,(%esp)                  ; set argument 1
   0x0804851a <+38>:	call   0x80483b0 <printf@plt>       ; printf(esp, esp+4, esp+8) ... printf("%s - %d\n", global_c, time(0))
   
   0x0804851f <+43>:	leave
   0x08048520 <+44>:	ret
   
End of assembler dump.
```

