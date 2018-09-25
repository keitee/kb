
own_no_asan_uaf:     file format elf32-i386


Disassembly of section .init:

080482fc <_init>:
_init():
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crti.S:63
 80482fc:	53                   	push   ebx
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crti.S:65
 80482fd:	83 ec 08             	sub    esp,0x8
 8048300:	e8 cb 00 00 00       	call   80483d0 <__x86.get_pc_thunk.bx>
 8048305:	81 c3 fb 1c 00 00    	add    ebx,0x1cfb
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crti.S:68
 804830b:	8b 83 fc ff ff ff    	mov    eax,DWORD PTR [ebx-0x4]
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crti.S:69
 8048311:	85 c0                	test   eax,eax
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crti.S:70
 8048313:	74 05                	je     804831a <_init+0x1e>
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crti.S:71
 8048315:	e8 76 00 00 00       	call   8048390 <.plt.got>
 804831a:	e8 91 01 00 00       	call   80484b0 <frame_dummy>
 804831f:	e8 0c 03 00 00       	call   8048630 <__do_global_ctors_aux>
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crtn.S:40
 8048324:	83 c4 08             	add    esp,0x8
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crtn.S:41
 8048327:	5b                   	pop    ebx
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crtn.S:42
 8048328:	c3                   	ret    

Disassembly of section .plt:

08048330 <.plt>:
 8048330:	ff 35 04 a0 04 08    	push   DWORD PTR ds:0x804a004
 8048336:	ff 25 08 a0 04 08    	jmp    DWORD PTR ds:0x804a008
 804833c:	00 00                	add    BYTE PTR [eax],al
	...

08048340 <printf@plt>:
 8048340:	ff 25 0c a0 04 08    	jmp    DWORD PTR ds:0x804a00c
 8048346:	68 00 00 00 00       	push   0x0
 804834b:	e9 e0 ff ff ff       	jmp    8048330 <.plt>

08048350 <free@plt>:
 8048350:	ff 25 10 a0 04 08    	jmp    DWORD PTR ds:0x804a010
 8048356:	68 08 00 00 00       	push   0x8
 804835b:	e9 d0 ff ff ff       	jmp    8048330 <.plt>

08048360 <malloc@plt>:
 8048360:	ff 25 14 a0 04 08    	jmp    DWORD PTR ds:0x804a014
 8048366:	68 10 00 00 00       	push   0x10
 804836b:	e9 c0 ff ff ff       	jmp    8048330 <.plt>

08048370 <puts@plt>:
 8048370:	ff 25 18 a0 04 08    	jmp    DWORD PTR ds:0x804a018
 8048376:	68 18 00 00 00       	push   0x18
 804837b:	e9 b0 ff ff ff       	jmp    8048330 <.plt>

08048380 <__libc_start_main@plt>:
 8048380:	ff 25 1c a0 04 08    	jmp    DWORD PTR ds:0x804a01c
 8048386:	68 20 00 00 00       	push   0x20
 804838b:	e9 a0 ff ff ff       	jmp    8048330 <.plt>

Disassembly of section .plt.got:

08048390 <.plt.got>:
 8048390:	ff 25 fc 9f 04 08    	jmp    DWORD PTR ds:0x8049ffc
 8048396:	66 90                	xchg   ax,ax

Disassembly of section .text:

080483a0 <_start>:
_start():
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/start.S:61
 80483a0:	31 ed                	xor    ebp,ebp
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/start.S:66
 80483a2:	5e                   	pop    esi
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/start.S:67
 80483a3:	89 e1                	mov    ecx,esp
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/start.S:73
 80483a5:	83 e4 f0             	and    esp,0xfffffff0
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/start.S:74
 80483a8:	50                   	push   eax
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/start.S:79
 80483a9:	54                   	push   esp
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/start.S:81
 80483aa:	52                   	push   edx
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/start.S:105
 80483ab:	68 20 86 04 08       	push   0x8048620
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/start.S:106
 80483b0:	68 c0 85 04 08       	push   0x80485c0
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/start.S:108
 80483b5:	51                   	push   ecx
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/start.S:109
 80483b6:	56                   	push   esi
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/start.S:111
 80483b7:	68 2f 85 04 08       	push   0x804852f
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/start.S:115
 80483bc:	e8 bf ff ff ff       	call   8048380 <__libc_start_main@plt>
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/start.S:118
 80483c1:	f4                   	hlt    
 80483c2:	66 90                	xchg   ax,ax
 80483c4:	66 90                	xchg   ax,ax
 80483c6:	66 90                	xchg   ax,ax
 80483c8:	66 90                	xchg   ax,ax
 80483ca:	66 90                	xchg   ax,ax
 80483cc:	66 90                	xchg   ax,ax
 80483ce:	66 90                	xchg   ax,ax

080483d0 <__x86.get_pc_thunk.bx>:
__x86.get_pc_thunk.bx():
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crti.S:66
 80483d0:	8b 1c 24             	mov    ebx,DWORD PTR [esp]
 80483d3:	c3                   	ret    
 80483d4:	66 90                	xchg   ax,ax
 80483d6:	66 90                	xchg   ax,ax
 80483d8:	66 90                	xchg   ax,ax
 80483da:	66 90                	xchg   ax,ax
 80483dc:	66 90                	xchg   ax,ax
 80483de:	66 90                	xchg   ax,ax

080483e0 <deregister_tm_clones>:
deregister_tm_clones():
 80483e0:	b8 2b a0 04 08       	mov    eax,0x804a02b
 80483e5:	2d 28 a0 04 08       	sub    eax,0x804a028
 80483ea:	83 f8 06             	cmp    eax,0x6
 80483ed:	76 1a                	jbe    8048409 <deregister_tm_clones+0x29>
 80483ef:	b8 00 00 00 00       	mov    eax,0x0
 80483f4:	85 c0                	test   eax,eax
 80483f6:	74 11                	je     8048409 <deregister_tm_clones+0x29>
 80483f8:	55                   	push   ebp
 80483f9:	89 e5                	mov    ebp,esp
 80483fb:	83 ec 14             	sub    esp,0x14
 80483fe:	68 28 a0 04 08       	push   0x804a028
 8048403:	ff d0                	call   eax
 8048405:	83 c4 10             	add    esp,0x10
 8048408:	c9                   	leave  
 8048409:	f3 c3                	repz ret 
 804840b:	90                   	nop
 804840c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

08048410 <register_tm_clones>:
register_tm_clones():
 8048410:	b8 28 a0 04 08       	mov    eax,0x804a028
 8048415:	2d 28 a0 04 08       	sub    eax,0x804a028
 804841a:	c1 f8 02             	sar    eax,0x2
 804841d:	89 c2                	mov    edx,eax
 804841f:	c1 ea 1f             	shr    edx,0x1f
 8048422:	01 d0                	add    eax,edx
 8048424:	d1 f8                	sar    eax,1
 8048426:	74 1b                	je     8048443 <register_tm_clones+0x33>
 8048428:	ba 00 00 00 00       	mov    edx,0x0
 804842d:	85 d2                	test   edx,edx
 804842f:	74 12                	je     8048443 <register_tm_clones+0x33>
 8048431:	55                   	push   ebp
 8048432:	89 e5                	mov    ebp,esp
 8048434:	83 ec 10             	sub    esp,0x10
 8048437:	50                   	push   eax
 8048438:	68 28 a0 04 08       	push   0x804a028
 804843d:	ff d2                	call   edx
 804843f:	83 c4 10             	add    esp,0x10
 8048442:	c9                   	leave  
 8048443:	f3 c3                	repz ret 
 8048445:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
 8048449:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

08048450 <__do_global_dtors_aux>:
__do_global_dtors_aux():
 8048450:	80 3d 28 a0 04 08 00 	cmp    BYTE PTR ds:0x804a028,0x0
 8048457:	75 50                	jne    80484a9 <__do_global_dtors_aux+0x59>
 8048459:	55                   	push   ebp
 804845a:	89 e5                	mov    ebp,esp
 804845c:	53                   	push   ebx
 804845d:	bb 2c 9f 04 08       	mov    ebx,0x8049f2c
 8048462:	81 eb 28 9f 04 08    	sub    ebx,0x8049f28
 8048468:	83 ec 04             	sub    esp,0x4
 804846b:	a1 2c a0 04 08       	mov    eax,ds:0x804a02c
 8048470:	c1 fb 02             	sar    ebx,0x2
 8048473:	83 eb 01             	sub    ebx,0x1
 8048476:	39 d8                	cmp    eax,ebx
 8048478:	73 1e                	jae    8048498 <__do_global_dtors_aux+0x48>
 804847a:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
 8048480:	83 c0 01             	add    eax,0x1
 8048483:	a3 2c a0 04 08       	mov    ds:0x804a02c,eax
 8048488:	ff 14 85 28 9f 04 08 	call   DWORD PTR [eax*4+0x8049f28]
 804848f:	a1 2c a0 04 08       	mov    eax,ds:0x804a02c
 8048494:	39 d8                	cmp    eax,ebx
 8048496:	72 e8                	jb     8048480 <__do_global_dtors_aux+0x30>
 8048498:	e8 43 ff ff ff       	call   80483e0 <deregister_tm_clones>
 804849d:	c6 05 28 a0 04 08 01 	mov    BYTE PTR ds:0x804a028,0x1
 80484a4:	83 c4 04             	add    esp,0x4
 80484a7:	5b                   	pop    ebx
 80484a8:	5d                   	pop    ebp
 80484a9:	f3 c3                	repz ret 
 80484ab:	90                   	nop
 80484ac:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

080484b0 <frame_dummy>:
frame_dummy():
 80484b0:	b8 30 9f 04 08       	mov    eax,0x8049f30
 80484b5:	8b 10                	mov    edx,DWORD PTR [eax]
 80484b7:	85 d2                	test   edx,edx
 80484b9:	75 05                	jne    80484c0 <frame_dummy+0x10>
 80484bb:	e9 50 ff ff ff       	jmp    8048410 <register_tm_clones>
 80484c0:	ba 00 00 00 00       	mov    edx,0x0
 80484c5:	85 d2                	test   edx,edx
 80484c7:	74 f2                	je     80484bb <frame_dummy+0xb>
 80484c9:	55                   	push   ebp
 80484ca:	89 e5                	mov    ebp,esp
 80484cc:	83 ec 14             	sub    esp,0x14
 80484cf:	50                   	push   eax
 80484d0:	ff d2                	call   edx
 80484d2:	83 c4 10             	add    esp,0x10
 80484d5:	c9                   	leave  
 80484d6:	e9 35 ff ff ff       	jmp    8048410 <register_tm_clones>

080484db <call_add>:
call_add():
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:7
 80484db:	55                   	push   ebp
 80484dc:	89 e5                	mov    ebp,esp
 80484de:	83 ec 10             	sub    esp,0x10
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:8
 80484e1:	8b 55 08             	mov    edx,DWORD PTR [ebp+0x8]
 80484e4:	89 d0                	mov    eax,edx
 80484e6:	c1 e0 02             	shl    eax,0x2
 80484e9:	01 d0                	add    eax,edx
 80484eb:	01 c0                	add    eax,eax
 80484ed:	89 c2                	mov    edx,eax
 80484ef:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 80484f2:	01 d0                	add    eax,edx
 80484f4:	89 45 fc             	mov    DWORD PTR [ebp-0x4],eax
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:9
 80484f7:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:10
 80484fa:	c9                   	leave  
 80484fb:	c3                   	ret    

080484fc <call_uaf>:
call_uaf():
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:13
 80484fc:	55                   	push   ebp
 80484fd:	89 e5                	mov    ebp,esp
 80484ff:	83 ec 18             	sub    esp,0x18                 ; -24
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:14
 8048502:	83 ec 0c             	sub    esp,0xc                  ; -12
 8048505:	6a 28                	push   0x28                     ; 40 bytes
 8048507:	e8 54 fe ff ff       	call   8048360 <malloc@plt>     ; call malloc
 804850c:	83 c4 10             	add    esp,0x10                 ; move sp back, +16
 804850f:	89 45 f4             	mov    DWORD PTR [ebp-0xc],eax  ; int *x = malloc()
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:15
 8048512:	83 ec 0c             	sub    esp,0xc
 8048515:	ff 75 f4             	push   DWORD PTR [ebp-0xc]      ; x
 8048518:	e8 33 fe ff ff       	call   8048350 <free@plt>       ; call free
 804851d:	83 c4 10             	add    esp,0x10                 ; move sp back,
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:16
 8048520:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]  ; x
 8048523:	83 c0 14             	add    eax,0x14                 ; x+20
 8048526:	c7 00 0a 00 00 00    	mov    DWORD PTR [eax],0xa      ; x[5] = 10
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:18
 804852c:	90                   	nop
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:19
 804852d:	c9                   	leave  
 804852e:	c3                   	ret    

0804852f <main>:
main():
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:22
 804852f:	8d 4c 24 04          	lea    ecx,[esp+0x4]
 8048533:	83 e4 f0             	and    esp,0xfffffff0
 8048536:	ff 71 fc             	push   DWORD PTR [ecx-0x4]
 8048539:	55                   	push   ebp
 804853a:	89 e5                	mov    ebp,esp
 804853c:	51                   	push   ecx
 804853d:	83 ec 04             	sub    esp,0x4
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:23
 8048540:	83 ec 0c             	sub    esp,0xc
 8048543:	68 88 86 04 08       	push   0x8048688
 8048548:	e8 23 fe ff ff       	call   8048370 <puts@plt>
 804854d:	83 c4 10             	add    esp,0x10
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:24
 8048550:	83 ec 0c             	sub    esp,0xc
 8048553:	68 98 86 04 08       	push   0x8048698
 8048558:	e8 13 fe ff ff       	call   8048370 <puts@plt>
 804855d:	83 c4 10             	add    esp,0x10
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:25
 8048560:	83 ec 0c             	sub    esp,0xc
 8048563:	68 bc 86 04 08       	push   0x80486bc
 8048568:	e8 03 fe ff ff       	call   8048370 <puts@plt>
 804856d:	83 c4 10             	add    esp,0x10
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:27
 8048570:	e8 87 ff ff ff       	call   80484fc <call_uaf>
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:29
 8048575:	83 ec 08             	sub    esp,0x8
 8048578:	6a 14                	push   0x14
 804857a:	6a 0a                	push   0xa
 804857c:	e8 5a ff ff ff       	call   80484db <call_add>
 8048581:	83 c4 10             	add    esp,0x10
 8048584:	83 ec 08             	sub    esp,0x8
 8048587:	50                   	push   eax
 8048588:	68 c4 86 04 08       	push   0x80486c4
 804858d:	e8 ae fd ff ff       	call   8048340 <printf@plt>
 8048592:	83 c4 10             	add    esp,0x10
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:30
 8048595:	83 ec 0c             	sub    esp,0xc
 8048598:	68 e9 86 04 08       	push   0x80486e9
 804859d:	e8 ce fd ff ff       	call   8048370 <puts@plt>
 80485a2:	83 c4 10             	add    esp,0x10
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:32
 80485a5:	b8 00 00 00 00       	mov    eax,0x0
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:33
 80485aa:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
 80485ad:	c9                   	leave  
 80485ae:	8d 61 fc             	lea    esp,[ecx-0x4]
 80485b1:	c3                   	ret    
 80485b2:	66 90                	xchg   ax,ax
 80485b4:	66 90                	xchg   ax,ax
 80485b6:	66 90                	xchg   ax,ax
 80485b8:	66 90                	xchg   ax,ax
 80485ba:	66 90                	xchg   ax,ax
 80485bc:	66 90                	xchg   ax,ax
 80485be:	66 90                	xchg   ax,ax

080485c0 <__libc_csu_init>:
__libc_csu_init():
/home/kyoupark/asan/gcc/glibc-2.25/csu/elf-init.c:68
 80485c0:	55                   	push   ebp
 80485c1:	57                   	push   edi
/home/kyoupark/asan/gcc/glibc-2.25/csu/elf-init.c:87
 80485c2:	31 ff                	xor    edi,edi
/home/kyoupark/asan/gcc/glibc-2.25/csu/elf-init.c:68
 80485c4:	56                   	push   esi
 80485c5:	53                   	push   ebx
 80485c6:	e8 05 fe ff ff       	call   80483d0 <__x86.get_pc_thunk.bx>
 80485cb:	81 c3 35 1a 00 00    	add    ebx,0x1a35
 80485d1:	83 ec 0c             	sub    esp,0xc
 80485d4:	8b 6c 24 20          	mov    ebp,DWORD PTR [esp+0x20]
/home/kyoupark/asan/gcc/glibc-2.25/csu/elf-init.c:86
 80485d8:	8d b3 20 ff ff ff    	lea    esi,[ebx-0xe0]
/home/kyoupark/asan/gcc/glibc-2.25/csu/elf-init.c:83
 80485de:	e8 19 fd ff ff       	call   80482fc <_init>
/home/kyoupark/asan/gcc/glibc-2.25/csu/elf-init.c:86
 80485e3:	8d 83 20 ff ff ff    	lea    eax,[ebx-0xe0]
 80485e9:	29 c6                	sub    esi,eax
 80485eb:	c1 fe 02             	sar    esi,0x2
/home/kyoupark/asan/gcc/glibc-2.25/csu/elf-init.c:87
 80485ee:	85 f6                	test   esi,esi
 80485f0:	74 23                	je     8048615 <__libc_csu_init+0x55>
 80485f2:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
/home/kyoupark/asan/gcc/glibc-2.25/csu/elf-init.c:88 (discriminator 3)
 80485f8:	83 ec 04             	sub    esp,0x4
 80485fb:	ff 74 24 2c          	push   DWORD PTR [esp+0x2c]
 80485ff:	ff 74 24 2c          	push   DWORD PTR [esp+0x2c]
 8048603:	55                   	push   ebp
 8048604:	ff 94 bb 20 ff ff ff 	call   DWORD PTR [ebx+edi*4-0xe0]
/home/kyoupark/asan/gcc/glibc-2.25/csu/elf-init.c:87 (discriminator 3)
 804860b:	83 c7 01             	add    edi,0x1
 804860e:	83 c4 10             	add    esp,0x10
 8048611:	39 f7                	cmp    edi,esi
 8048613:	75 e3                	jne    80485f8 <__libc_csu_init+0x38>
/home/kyoupark/asan/gcc/glibc-2.25/csu/elf-init.c:89
 8048615:	83 c4 0c             	add    esp,0xc
 8048618:	5b                   	pop    ebx
 8048619:	5e                   	pop    esi
 804861a:	5f                   	pop    edi
 804861b:	5d                   	pop    ebp
 804861c:	c3                   	ret    
 804861d:	8d 76 00             	lea    esi,[esi+0x0]

08048620 <__libc_csu_fini>:
__libc_csu_fini():
/home/kyoupark/asan/gcc/glibc-2.25/csu/elf-init.c:96
 8048620:	f3 c3                	repz ret 
 8048622:	66 90                	xchg   ax,ax
 8048624:	66 90                	xchg   ax,ax
 8048626:	66 90                	xchg   ax,ax
 8048628:	66 90                	xchg   ax,ax
 804862a:	66 90                	xchg   ax,ax
 804862c:	66 90                	xchg   ax,ax
 804862e:	66 90                	xchg   ax,ax

08048630 <__do_global_ctors_aux>:
__do_global_ctors_aux():
 8048630:	55                   	push   ebp
 8048631:	89 e5                	mov    ebp,esp
 8048633:	53                   	push   ebx
 8048634:	bb 20 9f 04 08       	mov    ebx,0x8049f20
 8048639:	83 ec 04             	sub    esp,0x4
 804863c:	a1 20 9f 04 08       	mov    eax,ds:0x8049f20
 8048641:	83 f8 ff             	cmp    eax,0xffffffff
 8048644:	74 16                	je     804865c <__do_global_ctors_aux+0x2c>
 8048646:	8d 76 00             	lea    esi,[esi+0x0]
 8048649:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
 8048650:	83 eb 04             	sub    ebx,0x4
 8048653:	ff d0                	call   eax
 8048655:	8b 03                	mov    eax,DWORD PTR [ebx]
 8048657:	83 f8 ff             	cmp    eax,0xffffffff
 804865a:	75 f4                	jne    8048650 <__do_global_ctors_aux+0x20>
 804865c:	83 c4 04             	add    esp,0x4
 804865f:	5b                   	pop    ebx
 8048660:	5d                   	pop    ebp
 8048661:	c3                   	ret    

Disassembly of section .fini:

08048664 <_fini>:
_fini():
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crti.S:82
 8048664:	53                   	push   ebx
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crti.S:83
 8048665:	83 ec 08             	sub    esp,0x8
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crti.S:84
 8048668:	e8 63 fd ff ff       	call   80483d0 <__x86.get_pc_thunk.bx>
 804866d:	81 c3 93 19 00 00    	add    ebx,0x1993
 8048673:	e8 d8 fd ff ff       	call   8048450 <__do_global_dtors_aux>
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crtn.S:45
 8048678:	83 c4 08             	add    esp,0x8
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crtn.S:46
 804867b:	5b                   	pop    ebx
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crtn.S:47
 804867c:	c3                   	ret    
