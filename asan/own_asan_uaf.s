
own_asan_uaf:     file format elf32-i386


Disassembly of section .init:

08048468 <_init>:
_init():
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crti.S:63
 8048468:	53                   	push   ebx
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crti.S:65
 8048469:	83 ec 08             	sub    esp,0x8
 804846c:	e8 0f 01 00 00       	call   8048580 <__x86.get_pc_thunk.bx>
 8048471:	81 c3 8f 1b 00 00    	add    ebx,0x1b8f
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crti.S:68
 8048477:	8b 83 fc ff ff ff    	mov    eax,DWORD PTR [ebx-0x4]
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crti.S:69
 804847d:	85 c0                	test   eax,eax
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crti.S:70
 804847f:	74 05                	je     8048486 <_init+0x1e>
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crti.S:71
 8048481:	e8 ba 00 00 00       	call   8048540 <.plt.got>
 8048486:	e8 d5 01 00 00       	call   8048660 <frame_dummy>
 804848b:	e8 c0 03 00 00       	call   8048850 <__do_global_ctors_aux>
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crtn.S:40
 8048490:	83 c4 08             	add    esp,0x8
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crtn.S:41
 8048493:	5b                   	pop    ebx
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crtn.S:42
 8048494:	c3                   	ret    

Disassembly of section .plt:

080484a0 <.plt>:
 80484a0:	ff 35 04 a0 04 08    	push   DWORD PTR ds:0x804a004
 80484a6:	ff 25 08 a0 04 08    	jmp    DWORD PTR ds:0x804a008
 80484ac:	00 00                	add    BYTE PTR [eax],al
	...

080484b0 <__asan_init_v3@plt>:
 80484b0:	ff 25 0c a0 04 08    	jmp    DWORD PTR ds:0x804a00c
 80484b6:	68 00 00 00 00       	push   0x0
 80484bb:	e9 e0 ff ff ff       	jmp    80484a0 <.plt>

080484c0 <malloc@plt>:
 80484c0:	ff 25 10 a0 04 08    	jmp    DWORD PTR ds:0x804a010
 80484c6:	68 08 00 00 00       	push   0x8
 80484cb:	e9 d0 ff ff ff       	jmp    80484a0 <.plt>

080484d0 <__libc_start_main@plt>:
 80484d0:	ff 25 14 a0 04 08    	jmp    DWORD PTR ds:0x804a014
 80484d6:	68 10 00 00 00       	push   0x10
 80484db:	e9 c0 ff ff ff       	jmp    80484a0 <.plt>

080484e0 <printf@plt>:
 80484e0:	ff 25 18 a0 04 08    	jmp    DWORD PTR ds:0x804a018
 80484e6:	68 18 00 00 00       	push   0x18
 80484eb:	e9 b0 ff ff ff       	jmp    80484a0 <.plt>

080484f0 <__asan_report_store4@plt>:
 80484f0:	ff 25 1c a0 04 08    	jmp    DWORD PTR ds:0x804a01c
 80484f6:	68 20 00 00 00       	push   0x20
 80484fb:	e9 a0 ff ff ff       	jmp    80484a0 <.plt>

08048500 <puts@plt>:
 8048500:	ff 25 20 a0 04 08    	jmp    DWORD PTR ds:0x804a020
 8048506:	68 28 00 00 00       	push   0x28
 804850b:	e9 90 ff ff ff       	jmp    80484a0 <.plt>

08048510 <__asan_unregister_globals@plt>:
 8048510:	ff 25 24 a0 04 08    	jmp    DWORD PTR ds:0x804a024
 8048516:	68 30 00 00 00       	push   0x30
 804851b:	e9 80 ff ff ff       	jmp    80484a0 <.plt>

08048520 <__asan_register_globals@plt>:
 8048520:	ff 25 28 a0 04 08    	jmp    DWORD PTR ds:0x804a028
 8048526:	68 38 00 00 00       	push   0x38
 804852b:	e9 70 ff ff ff       	jmp    80484a0 <.plt>

08048530 <free@plt>:
 8048530:	ff 25 2c a0 04 08    	jmp    DWORD PTR ds:0x804a02c
 8048536:	68 40 00 00 00       	push   0x40
 804853b:	e9 60 ff ff ff       	jmp    80484a0 <.plt>

Disassembly of section .plt.got:

08048540 <.plt.got>:
 8048540:	ff 25 fc 9f 04 08    	jmp    DWORD PTR ds:0x8049ffc
 8048546:	66 90                	xchg   ax,ax

Disassembly of section .text:

08048550 <_start>:
_start():
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/start.S:61
 8048550:	31 ed                	xor    ebp,ebp
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/start.S:66
 8048552:	5e                   	pop    esi
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/start.S:67
 8048553:	89 e1                	mov    ecx,esp
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/start.S:73
 8048555:	83 e4 f0             	and    esp,0xfffffff0
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/start.S:74
 8048558:	50                   	push   eax
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/start.S:79
 8048559:	54                   	push   esp
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/start.S:81
 804855a:	52                   	push   edx
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/start.S:105
 804855b:	68 40 88 04 08       	push   0x8048840
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/start.S:106
 8048560:	68 e0 87 04 08       	push   0x80487e0
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/start.S:108
 8048565:	51                   	push   ecx
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/start.S:109
 8048566:	56                   	push   esi
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/start.S:111
 8048567:	68 1d 87 04 08       	push   0x804871d
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/start.S:115
 804856c:	e8 5f ff ff ff       	call   80484d0 <__libc_start_main@plt>
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/start.S:118
 8048571:	f4                   	hlt    
 8048572:	66 90                	xchg   ax,ax
 8048574:	66 90                	xchg   ax,ax
 8048576:	66 90                	xchg   ax,ax
 8048578:	66 90                	xchg   ax,ax
 804857a:	66 90                	xchg   ax,ax
 804857c:	66 90                	xchg   ax,ax
 804857e:	66 90                	xchg   ax,ax

08048580 <__x86.get_pc_thunk.bx>:
__x86.get_pc_thunk.bx():
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crti.S:66
 8048580:	8b 1c 24             	mov    ebx,DWORD PTR [esp]
 8048583:	c3                   	ret    
 8048584:	66 90                	xchg   ax,ax
 8048586:	66 90                	xchg   ax,ax
 8048588:	66 90                	xchg   ax,ax
 804858a:	66 90                	xchg   ax,ax
 804858c:	66 90                	xchg   ax,ax
 804858e:	66 90                	xchg   ax,ax

08048590 <deregister_tm_clones>:
deregister_tm_clones():
 8048590:	b8 fb a0 04 08       	mov    eax,0x804a0fb
 8048595:	2d f8 a0 04 08       	sub    eax,0x804a0f8
 804859a:	83 f8 06             	cmp    eax,0x6
 804859d:	76 1a                	jbe    80485b9 <deregister_tm_clones+0x29>
 804859f:	b8 00 00 00 00       	mov    eax,0x0
 80485a4:	85 c0                	test   eax,eax
 80485a6:	74 11                	je     80485b9 <deregister_tm_clones+0x29>
 80485a8:	55                   	push   ebp
 80485a9:	89 e5                	mov    ebp,esp
 80485ab:	83 ec 14             	sub    esp,0x14
 80485ae:	68 f8 a0 04 08       	push   0x804a0f8
 80485b3:	ff d0                	call   eax
 80485b5:	83 c4 10             	add    esp,0x10
 80485b8:	c9                   	leave  
 80485b9:	f3 c3                	repz ret 
 80485bb:	90                   	nop
 80485bc:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

080485c0 <register_tm_clones>:
register_tm_clones():
 80485c0:	b8 f8 a0 04 08       	mov    eax,0x804a0f8
 80485c5:	2d f8 a0 04 08       	sub    eax,0x804a0f8
 80485ca:	c1 f8 02             	sar    eax,0x2
 80485cd:	89 c2                	mov    edx,eax
 80485cf:	c1 ea 1f             	shr    edx,0x1f
 80485d2:	01 d0                	add    eax,edx
 80485d4:	d1 f8                	sar    eax,1
 80485d6:	74 1b                	je     80485f3 <register_tm_clones+0x33>
 80485d8:	ba 00 00 00 00       	mov    edx,0x0
 80485dd:	85 d2                	test   edx,edx
 80485df:	74 12                	je     80485f3 <register_tm_clones+0x33>
 80485e1:	55                   	push   ebp
 80485e2:	89 e5                	mov    ebp,esp
 80485e4:	83 ec 10             	sub    esp,0x10
 80485e7:	50                   	push   eax
 80485e8:	68 f8 a0 04 08       	push   0x804a0f8
 80485ed:	ff d2                	call   edx
 80485ef:	83 c4 10             	add    esp,0x10
 80485f2:	c9                   	leave  
 80485f3:	f3 c3                	repz ret 
 80485f5:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
 80485f9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

08048600 <__do_global_dtors_aux>:
__do_global_dtors_aux():
 8048600:	80 3d f8 a0 04 08 00 	cmp    BYTE PTR ds:0x804a0f8,0x0
 8048607:	75 50                	jne    8048659 <__do_global_dtors_aux+0x59>
 8048609:	55                   	push   ebp
 804860a:	89 e5                	mov    ebp,esp
 804860c:	53                   	push   ebx
 804860d:	bb f4 9e 04 08       	mov    ebx,0x8049ef4
 8048612:	81 eb f0 9e 04 08    	sub    ebx,0x8049ef0
 8048618:	83 ec 04             	sub    esp,0x4
 804861b:	a1 fc a0 04 08       	mov    eax,ds:0x804a0fc
 8048620:	c1 fb 02             	sar    ebx,0x2
 8048623:	83 eb 01             	sub    ebx,0x1
 8048626:	39 d8                	cmp    eax,ebx
 8048628:	73 1e                	jae    8048648 <__do_global_dtors_aux+0x48>
 804862a:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
 8048630:	83 c0 01             	add    eax,0x1
 8048633:	a3 fc a0 04 08       	mov    ds:0x804a0fc,eax
 8048638:	ff 14 85 f0 9e 04 08 	call   DWORD PTR [eax*4+0x8049ef0]
 804863f:	a1 fc a0 04 08       	mov    eax,ds:0x804a0fc
 8048644:	39 d8                	cmp    eax,ebx
 8048646:	72 e8                	jb     8048630 <__do_global_dtors_aux+0x30>
 8048648:	e8 43 ff ff ff       	call   8048590 <deregister_tm_clones>
 804864d:	c6 05 f8 a0 04 08 01 	mov    BYTE PTR ds:0x804a0f8,0x1
 8048654:	83 c4 04             	add    esp,0x4
 8048657:	5b                   	pop    ebx
 8048658:	5d                   	pop    ebp
 8048659:	f3 c3                	repz ret 
 804865b:	90                   	nop
 804865c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

08048660 <frame_dummy>:
frame_dummy():
 8048660:	b8 f8 9e 04 08       	mov    eax,0x8049ef8
 8048665:	8b 10                	mov    edx,DWORD PTR [eax]
 8048667:	85 d2                	test   edx,edx
 8048669:	75 05                	jne    8048670 <frame_dummy+0x10>
 804866b:	e9 50 ff ff ff       	jmp    80485c0 <register_tm_clones>
 8048670:	ba 00 00 00 00       	mov    edx,0x0
 8048675:	85 d2                	test   edx,edx
 8048677:	74 f2                	je     804866b <frame_dummy+0xb>
 8048679:	55                   	push   ebp
 804867a:	89 e5                	mov    ebp,esp
 804867c:	83 ec 14             	sub    esp,0x14
 804867f:	50                   	push   eax
 8048680:	ff d2                	call   edx
 8048682:	83 c4 10             	add    esp,0x10
 8048685:	c9                   	leave  
 8048686:	e9 35 ff ff ff       	jmp    80485c0 <register_tm_clones>

0804868b <call_add>:
call_add():
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:7
 804868b:	55                   	push   ebp
 804868c:	89 e5                	mov    ebp,esp
 804868e:	83 ec 10             	sub    esp,0x10
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:8
 8048691:	8b 55 08             	mov    edx,DWORD PTR [ebp+0x8]
 8048694:	89 d0                	mov    eax,edx
 8048696:	c1 e0 02             	shl    eax,0x2
 8048699:	01 d0                	add    eax,edx
 804869b:	01 c0                	add    eax,eax
 804869d:	89 c2                	mov    edx,eax
 804869f:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 80486a2:	01 d0                	add    eax,edx
 80486a4:	89 45 fc             	mov    DWORD PTR [ebp-0x4],eax
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:9
 80486a7:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:10
 80486aa:	c9                   	leave  
 80486ab:	c3                   	ret    

080486ac <call_uaf>:
call_uaf():
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:13
 80486ac:	55                   	push   ebp
 80486ad:	89 e5                	mov    ebp,esp
 80486af:	57                   	push   edi
 80486b0:	56                   	push   esi
 80486b1:	53                   	push   ebx
 80486b2:	83 ec 1c             	sub    esp,0x1c
; /home/kyoupark/si_logs/flash0-asan/uaf_mod.c:14
;  80486b5:	83 ec 0c             	sub    esp,0xc
;  80486b8:	6a 28                	push   0x28
;  80486ba:	e8 01 fe ff ff       	call   80484c0 <malloc@plt>
;  80486bf:	83 c4 10             	add    esp,0x10
;  80486c2:	89 45 e4             	mov    DWORD PTR [ebp-0x1c],eax
; /home/kyoupark/si_logs/flash0-asan/uaf_mod.c:15
;  80486c5:	83 ec 0c             	sub    esp,0xc
;  80486c8:	ff 75 e4             	push   DWORD PTR [ebp-0x1c]
;  80486cb:	e8 60 fe ff ff       	call   8048530 <free@plt>
;  80486d0:	83 c4 10             	add    esp,0x10
; /home/kyoupark/si_logs/flash0-asan/uaf_mod.c:16
;  80486d3:	8b 45 e4             	mov    eax,DWORD PTR [ebp-0x1c]   ; x
 80486d6:	8d 48 14             	lea    ecx,[eax+0x14]               ; [x+20]
 80486d9:	89 c8                	mov    eax,ecx
 80486db:	89 c2                	mov    edx,eax
 80486dd:	c1 ea 03             	shr    edx,0x3                      ; shift >> 3
 80486e0:	81 c2 00 00 00 20    	add    edx,0x20000000
 80486e6:	0f b6 12             	movzx  edx,BYTE PTR [edx]
 80486e9:	84 d2                	test   dl,dl
 80486eb:	0f 95 c3             	setne  bl
 80486ee:	89 df                	mov    edi,ebx
 80486f0:	89 c6                	mov    esi,eax
 80486f2:	83 e6 07             	and    esi,0x7
 80486f5:	83 c6 03             	add    esi,0x3
 80486f8:	89 f3                	mov    ebx,esi
 80486fa:	38 d3                	cmp    bl,dl
 80486fc:	0f 9d c2             	setge  dl
 80486ff:	21 fa                	and    edx,edi
 8048701:	84 d2                	test   dl,dl

; >>> hex(0x080486ac+0x62)
; '0x804870e'

 8048703:	74 09                	je     804870e <call_uaf+0x62>      ; jump if equal
 8048705:	83 ec 0c             	sub    esp,0xc
 8048708:	50                   	push   eax
 8048709:	e8 e2 fd ff ff       	call   80484f0 <__asan_report_store4@plt>

 `804870e`:	c7 01 0a 00 00 00    	mov    DWORD PTR [ecx],0xa        ; x[5] = 10
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:18
 8048714:	90                   	nop
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:19
 8048715:	8d 65 f4             	lea    esp,[ebp-0xc]
 8048718:	5b                   	pop    ebx
 8048719:	5e                   	pop    esi
 804871a:	5f                   	pop    edi
 804871b:	5d                   	pop    ebp
 804871c:	c3                   	ret    

0804871d <main>:
main():
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:22
 804871d:	8d 4c 24 04          	lea    ecx,[esp+0x4]
 8048721:	83 e4 f0             	and    esp,0xfffffff0
 8048724:	ff 71 fc             	push   DWORD PTR [ecx-0x4]
 8048727:	55                   	push   ebp
 8048728:	89 e5                	mov    ebp,esp
 804872a:	51                   	push   ecx
 804872b:	83 ec 04             	sub    esp,0x4
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:23
 804872e:	83 ec 0c             	sub    esp,0xc
 8048731:	68 c0 88 04 08       	push   0x80488c0
 8048736:	e8 c5 fd ff ff       	call   8048500 <puts@plt>
 804873b:	83 c4 10             	add    esp,0x10
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:24
 804873e:	83 ec 0c             	sub    esp,0xc
 8048741:	68 00 89 04 08       	push   0x8048900
 8048746:	e8 b5 fd ff ff       	call   8048500 <puts@plt>
 804874b:	83 c4 10             	add    esp,0x10
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:25
 804874e:	83 ec 0c             	sub    esp,0xc
 8048751:	68 60 89 04 08       	push   0x8048960
 8048756:	e8 a5 fd ff ff       	call   8048500 <puts@plt>
 804875b:	83 c4 10             	add    esp,0x10
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:27
 804875e:	e8 49 ff ff ff       	call   80486ac <call_uaf>
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:29
 8048763:	83 ec 08             	sub    esp,0x8
 8048766:	6a 14                	push   0x14
 8048768:	6a 0a                	push   0xa
 804876a:	e8 1c ff ff ff       	call   804868b <call_add>
 804876f:	83 c4 10             	add    esp,0x10
 8048772:	83 ec 08             	sub    esp,0x8
 8048775:	50                   	push   eax
 8048776:	68 a0 89 04 08       	push   0x80489a0
 804877b:	e8 60 fd ff ff       	call   80484e0 <printf@plt>
 8048780:	83 c4 10             	add    esp,0x10
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:30
 8048783:	83 ec 0c             	sub    esp,0xc
 8048786:	68 00 8a 04 08       	push   0x8048a00
 804878b:	e8 70 fd ff ff       	call   8048500 <puts@plt>
 8048790:	83 c4 10             	add    esp,0x10
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:32
 8048793:	b8 00 00 00 00       	mov    eax,0x0
/home/kyoupark/si_logs/flash0-asan/uaf_mod.c:33
 8048798:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
 804879b:	c9                   	leave  
 804879c:	8d 61 fc             	lea    esp,[ecx-0x4]
 804879f:	c3                   	ret    

080487a0 <_GLOBAL__sub_D_00099_0_call_add>:
_GLOBAL__sub_D_00099_0_call_add():
 80487a0:	55                   	push   ebp
 80487a1:	89 e5                	mov    ebp,esp
 80487a3:	83 ec 08             	sub    esp,0x8
 80487a6:	83 ec 08             	sub    esp,0x8
 80487a9:	6a 05                	push   0x5
 80487ab:	68 80 a0 04 08       	push   0x804a080
 80487b0:	e8 5b fd ff ff       	call   8048510 <__asan_unregister_globals@plt>
 80487b5:	83 c4 10             	add    esp,0x10
 80487b8:	c9                   	leave  
 80487b9:	c3                   	ret    

080487ba <_GLOBAL__sub_I_00099_1_call_add>:
_GLOBAL__sub_I_00099_1_call_add():
 80487ba:	55                   	push   ebp
 80487bb:	89 e5                	mov    ebp,esp
 80487bd:	83 ec 08             	sub    esp,0x8
 80487c0:	e8 eb fc ff ff       	call   80484b0 <__asan_init_v3@plt>
 80487c5:	83 ec 08             	sub    esp,0x8
 80487c8:	6a 05                	push   0x5
 80487ca:	68 80 a0 04 08       	push   0x804a080
 80487cf:	e8 4c fd ff ff       	call   8048520 <__asan_register_globals@plt>
 80487d4:	83 c4 10             	add    esp,0x10
 80487d7:	c9                   	leave  
 80487d8:	c3                   	ret    
 80487d9:	66 90                	xchg   ax,ax
 80487db:	66 90                	xchg   ax,ax
 80487dd:	66 90                	xchg   ax,ax
 80487df:	90                   	nop

080487e0 <__libc_csu_init>:
__libc_csu_init():
/home/kyoupark/asan/gcc/glibc-2.25/csu/elf-init.c:68
 80487e0:	55                   	push   ebp
 80487e1:	57                   	push   edi
/home/kyoupark/asan/gcc/glibc-2.25/csu/elf-init.c:87
 80487e2:	31 ff                	xor    edi,edi
/home/kyoupark/asan/gcc/glibc-2.25/csu/elf-init.c:68
 80487e4:	56                   	push   esi
 80487e5:	53                   	push   ebx
 80487e6:	e8 95 fd ff ff       	call   8048580 <__x86.get_pc_thunk.bx>
 80487eb:	81 c3 15 18 00 00    	add    ebx,0x1815
 80487f1:	83 ec 0c             	sub    esp,0xc
 80487f4:	8b 6c 24 20          	mov    ebp,DWORD PTR [esp+0x20]
/home/kyoupark/asan/gcc/glibc-2.25/csu/elf-init.c:86
 80487f8:	8d b3 e4 fe ff ff    	lea    esi,[ebx-0x11c]
/home/kyoupark/asan/gcc/glibc-2.25/csu/elf-init.c:83
 80487fe:	e8 65 fc ff ff       	call   8048468 <_init>
/home/kyoupark/asan/gcc/glibc-2.25/csu/elf-init.c:86
 8048803:	8d 83 e0 fe ff ff    	lea    eax,[ebx-0x120]
 8048809:	29 c6                	sub    esi,eax
 804880b:	c1 fe 02             	sar    esi,0x2
/home/kyoupark/asan/gcc/glibc-2.25/csu/elf-init.c:87
 804880e:	85 f6                	test   esi,esi
 8048810:	74 23                	je     8048835 <__libc_csu_init+0x55>
 8048812:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
/home/kyoupark/asan/gcc/glibc-2.25/csu/elf-init.c:88 (discriminator 3)
 8048818:	83 ec 04             	sub    esp,0x4
 804881b:	ff 74 24 2c          	push   DWORD PTR [esp+0x2c]
 804881f:	ff 74 24 2c          	push   DWORD PTR [esp+0x2c]
 8048823:	55                   	push   ebp
 8048824:	ff 94 bb e0 fe ff ff 	call   DWORD PTR [ebx+edi*4-0x120]
/home/kyoupark/asan/gcc/glibc-2.25/csu/elf-init.c:87 (discriminator 3)
 804882b:	83 c7 01             	add    edi,0x1
 804882e:	83 c4 10             	add    esp,0x10
 8048831:	39 f7                	cmp    edi,esi
 8048833:	75 e3                	jne    8048818 <__libc_csu_init+0x38>
/home/kyoupark/asan/gcc/glibc-2.25/csu/elf-init.c:89
 8048835:	83 c4 0c             	add    esp,0xc
 8048838:	5b                   	pop    ebx
 8048839:	5e                   	pop    esi
 804883a:	5f                   	pop    edi
 804883b:	5d                   	pop    ebp
 804883c:	c3                   	ret    
 804883d:	8d 76 00             	lea    esi,[esi+0x0]

08048840 <__libc_csu_fini>:
__libc_csu_fini():
/home/kyoupark/asan/gcc/glibc-2.25/csu/elf-init.c:96
 8048840:	f3 c3                	repz ret 
 8048842:	66 90                	xchg   ax,ax
 8048844:	66 90                	xchg   ax,ax
 8048846:	66 90                	xchg   ax,ax
 8048848:	66 90                	xchg   ax,ax
 804884a:	66 90                	xchg   ax,ax
 804884c:	66 90                	xchg   ax,ax
 804884e:	66 90                	xchg   ax,ax

08048850 <__do_global_ctors_aux>:
__do_global_ctors_aux():
 8048850:	55                   	push   ebp
 8048851:	89 e5                	mov    ebp,esp
 8048853:	53                   	push   ebx
 8048854:	bb e8 9e 04 08       	mov    ebx,0x8049ee8
 8048859:	83 ec 04             	sub    esp,0x4
 804885c:	a1 e8 9e 04 08       	mov    eax,ds:0x8049ee8
 8048861:	83 f8 ff             	cmp    eax,0xffffffff
 8048864:	74 16                	je     804887c <__do_global_ctors_aux+0x2c>
 8048866:	8d 76 00             	lea    esi,[esi+0x0]
 8048869:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
 8048870:	83 eb 04             	sub    ebx,0x4
 8048873:	ff d0                	call   eax
 8048875:	8b 03                	mov    eax,DWORD PTR [ebx]
 8048877:	83 f8 ff             	cmp    eax,0xffffffff
 804887a:	75 f4                	jne    8048870 <__do_global_ctors_aux+0x20>
 804887c:	83 c4 04             	add    esp,0x4
 804887f:	5b                   	pop    ebx
 8048880:	5d                   	pop    ebp
 8048881:	c3                   	ret    

Disassembly of section .fini:

08048884 <_fini>:
_fini():
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crti.S:82
 8048884:	53                   	push   ebx
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crti.S:83
 8048885:	83 ec 08             	sub    esp,0x8
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crti.S:84
 8048888:	e8 f3 fc ff ff       	call   8048580 <__x86.get_pc_thunk.bx>
 804888d:	81 c3 73 17 00 00    	add    ebx,0x1773
 8048893:	e8 68 fd ff ff       	call   8048600 <__do_global_dtors_aux>
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crtn.S:45
 8048898:	83 c4 08             	add    esp,0x8
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crtn.S:46
 804889b:	5b                   	pop    ebx
/home/kyoupark/asan/gcc/glibc-2.25/csu/../sysdeps/i386/crtn.S:47
 804889c:	c3                   	ret    
