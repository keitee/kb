
gbo_host_no_asan_32:     file format elf64-x86-64


Disassembly of section .init:

00000000004004b0 <_init>:
_init():
  4004b0:	48 83 ec 08          	sub    rsp,0x8
  4004b4:	48 8b 05 b5 07 20 00 	mov    rax,QWORD PTR [rip+0x2007b5]        # 600c70 <_DYNAMIC+0x1d0>
  4004bb:	48 85 c0             	test   rax,rax
  4004be:	74 05                	je     4004c5 <_init+0x15>
  4004c0:	e8 6b 00 00 00       	call   400530 <__gmon_start__@plt>
  4004c5:	48 83 c4 08          	add    rsp,0x8
  4004c9:	c3                   	ret    

Disassembly of section .plt:

00000000004004d0 <dl_iterate_phdr@plt-0x10>:
  4004d0:	ff 35 aa 07 20 00    	push   QWORD PTR [rip+0x2007aa]        # 600c80 <_GLOBAL_OFFSET_TABLE_+0x8>
  4004d6:	ff 25 ac 07 20 00    	jmp    QWORD PTR [rip+0x2007ac]        # 600c88 <_GLOBAL_OFFSET_TABLE_+0x10>
  4004dc:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]

00000000004004e0 <dl_iterate_phdr@plt>:
  4004e0:	ff 25 aa 07 20 00    	jmp    QWORD PTR [rip+0x2007aa]        # 600c90 <_GLOBAL_OFFSET_TABLE_+0x18>
  4004e6:	68 00 00 00 00       	push   0x0
  4004eb:	e9 e0 ff ff ff       	jmp    4004d0 <_init+0x20>

00000000004004f0 <putchar@plt>:
  4004f0:	ff 25 a2 07 20 00    	jmp    QWORD PTR [rip+0x2007a2]        # 600c98 <_GLOBAL_OFFSET_TABLE_+0x20>
  4004f6:	68 01 00 00 00       	push   0x1
  4004fb:	e9 d0 ff ff ff       	jmp    4004d0 <_init+0x20>

0000000000400500 <puts@plt>:
  400500:	ff 25 9a 07 20 00    	jmp    QWORD PTR [rip+0x20079a]        # 600ca0 <_GLOBAL_OFFSET_TABLE_+0x28>
  400506:	68 02 00 00 00       	push   0x2
  40050b:	e9 c0 ff ff ff       	jmp    4004d0 <_init+0x20>

0000000000400510 <printf@plt>:
  400510:	ff 25 92 07 20 00    	jmp    QWORD PTR [rip+0x200792]        # 600ca8 <_GLOBAL_OFFSET_TABLE_+0x30>
  400516:	68 03 00 00 00       	push   0x3
  40051b:	e9 b0 ff ff ff       	jmp    4004d0 <_init+0x20>

0000000000400520 <__libc_start_main@plt>:
  400520:	ff 25 8a 07 20 00    	jmp    QWORD PTR [rip+0x20078a]        # 600cb0 <_GLOBAL_OFFSET_TABLE_+0x38>
  400526:	68 04 00 00 00       	push   0x4
  40052b:	e9 a0 ff ff ff       	jmp    4004d0 <_init+0x20>

0000000000400530 <__gmon_start__@plt>:
  400530:	ff 25 82 07 20 00    	jmp    QWORD PTR [rip+0x200782]        # 600cb8 <_GLOBAL_OFFSET_TABLE_+0x40>
  400536:	68 05 00 00 00       	push   0x5
  40053b:	e9 90 ff ff ff       	jmp    4004d0 <_init+0x20>

0000000000400540 <memcpy@plt>:
  400540:	ff 25 7a 07 20 00    	jmp    QWORD PTR [rip+0x20077a]        # 600cc0 <_GLOBAL_OFFSET_TABLE_+0x48>
  400546:	68 06 00 00 00       	push   0x6
  40054b:	e9 80 ff ff ff       	jmp    4004d0 <_init+0x20>

Disassembly of section .text:

0000000000400550 <_start>:
_start():
  400550:	31 ed                	xor    ebp,ebp
  400552:	49 89 d1             	mov    r9,rdx
  400555:	5e                   	pop    rsi
  400556:	48 89 e2             	mov    rdx,rsp
  400559:	48 83 e4 f0          	and    rsp,0xfffffffffffffff0
  40055d:	50                   	push   rax
  40055e:	54                   	push   rsp
  40055f:	49 c7 c0 50 08 40 00 	mov    r8,0x400850
  400566:	48 c7 c1 e0 07 40 00 	mov    rcx,0x4007e0
  40056d:	48 c7 c7 87 07 40 00 	mov    rdi,0x400787
  400574:	e8 a7 ff ff ff       	call   400520 <__libc_start_main@plt>
  400579:	f4                   	hlt    
  40057a:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]

0000000000400580 <deregister_tm_clones>:
deregister_tm_clones():
  400580:	b8 e7 0c 60 00       	mov    eax,0x600ce7
  400585:	55                   	push   rbp
  400586:	48 2d e0 0c 60 00    	sub    rax,0x600ce0
  40058c:	48 83 f8 0e          	cmp    rax,0xe
  400590:	48 89 e5             	mov    rbp,rsp
  400593:	76 1b                	jbe    4005b0 <deregister_tm_clones+0x30>
  400595:	b8 00 00 00 00       	mov    eax,0x0
  40059a:	48 85 c0             	test   rax,rax
  40059d:	74 11                	je     4005b0 <deregister_tm_clones+0x30>
  40059f:	5d                   	pop    rbp
  4005a0:	bf e0 0c 60 00       	mov    edi,0x600ce0
  4005a5:	ff e0                	jmp    rax
  4005a7:	66 0f 1f 84 00 00 00 	nop    WORD PTR [rax+rax*1+0x0]
  4005ae:	00 00 
  4005b0:	5d                   	pop    rbp
  4005b1:	c3                   	ret    
  4005b2:	66 66 66 66 66 2e 0f 	data16 data16 data16 data16 nop WORD PTR cs:[rax+rax*1+0x0]
  4005b9:	1f 84 00 00 00 00 00 

00000000004005c0 <register_tm_clones>:
register_tm_clones():
  4005c0:	be e0 0c 60 00       	mov    esi,0x600ce0
  4005c5:	55                   	push   rbp
  4005c6:	48 81 ee e0 0c 60 00 	sub    rsi,0x600ce0
  4005cd:	48 c1 fe 03          	sar    rsi,0x3
  4005d1:	48 89 e5             	mov    rbp,rsp
  4005d4:	48 89 f0             	mov    rax,rsi
  4005d7:	48 c1 e8 3f          	shr    rax,0x3f
  4005db:	48 01 c6             	add    rsi,rax
  4005de:	48 d1 fe             	sar    rsi,1
  4005e1:	74 15                	je     4005f8 <register_tm_clones+0x38>
  4005e3:	b8 00 00 00 00       	mov    eax,0x0
  4005e8:	48 85 c0             	test   rax,rax
  4005eb:	74 0b                	je     4005f8 <register_tm_clones+0x38>
  4005ed:	5d                   	pop    rbp
  4005ee:	bf e0 0c 60 00       	mov    edi,0x600ce0
  4005f3:	ff e0                	jmp    rax
  4005f5:	0f 1f 00             	nop    DWORD PTR [rax]
  4005f8:	5d                   	pop    rbp
  4005f9:	c3                   	ret    
  4005fa:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]

0000000000400600 <__do_global_dtors_aux>:
__do_global_dtors_aux():
  400600:	80 3d d7 06 20 00 00 	cmp    BYTE PTR [rip+0x2006d7],0x0        # 600cde <_edata>
  400607:	75 11                	jne    40061a <__do_global_dtors_aux+0x1a>
  400609:	55                   	push   rbp
  40060a:	48 89 e5             	mov    rbp,rsp
  40060d:	e8 6e ff ff ff       	call   400580 <deregister_tm_clones>
  400612:	5d                   	pop    rbp
  400613:	c6 05 c4 06 20 00 01 	mov    BYTE PTR [rip+0x2006c4],0x1        # 600cde <_edata>
  40061a:	f3 c3                	repz ret 
  40061c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]

0000000000400620 <frame_dummy>:
frame_dummy():
  400620:	bf 98 0a 60 00       	mov    edi,0x600a98
  400625:	48 83 3f 00          	cmp    QWORD PTR [rdi],0x0
  400629:	75 05                	jne    400630 <frame_dummy+0x10>
  40062b:	eb 93                	jmp    4005c0 <register_tm_clones>
  40062d:	0f 1f 00             	nop    DWORD PTR [rax]
  400630:	b8 00 00 00 00       	mov    eax,0x0
  400635:	48 85 c0             	test   rax,rax
  400638:	74 f1                	je     40062b <frame_dummy+0xb>
  40063a:	55                   	push   rbp
  40063b:	48 89 e5             	mov    rbp,rsp
  40063e:	ff d0                	call   rax
  400640:	5d                   	pop    rbp
  400641:	e9 7a ff ff ff       	jmp    4005c0 <register_tm_clones>

0000000000400646 <header_handler>:
header_handler():
/mnt/git/kb/asan/gbo.c:114
  400646:	55                   	push   rbp
  400647:	48 89 e5             	mov    rbp,rsp
  40064a:	48 83 ec 30          	sub    rsp,0x30
  40064e:	48 89 7d e8          	mov    QWORD PTR [rbp-0x18],rdi
  400652:	48 89 75 e0          	mov    QWORD PTR [rbp-0x20],rsi
  400656:	48 89 55 d8          	mov    QWORD PTR [rbp-0x28],rdx
/mnt/git/kb/asan/gbo.c:115
  40065a:	c7 45 fc 00 00 00 00 	mov    DWORD PTR [rbp-0x4],0x0
/mnt/git/kb/asan/gbo.c:117
  400661:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  400665:	48 8b 00             	mov    rax,QWORD PTR [rax]
/mnt/git/kb/asan/gbo.c:116
  400668:	48 89 c1             	mov    rcx,rax
/mnt/git/kb/asan/gbo.c:117
  40066b:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  40066f:	0f b7 40 18          	movzx  eax,WORD PTR [rax+0x18]
/mnt/git/kb/asan/gbo.c:116
  400673:	0f b7 d0             	movzx  edx,ax
  400676:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  40067a:	48 8b 40 08          	mov    rax,QWORD PTR [rax+0x8]
  40067e:	48 89 c6             	mov    rsi,rax
  400681:	bf 68 08 40 00       	mov    edi,0x400868
  400686:	b8 00 00 00 00       	mov    eax,0x0
  40068b:	e8 80 fe ff ff       	call   400510 <printf@plt>
/mnt/git/kb/asan/gbo.c:118
  400690:	c7 45 fc 00 00 00 00 	mov    DWORD PTR [rbp-0x4],0x0
  400697:	e9 a5 00 00 00       	jmp    400741 <header_handler+0xfb>
/mnt/git/kb/asan/gbo.c:120 (discriminator 3)
  40069c:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  4006a0:	48 8b 10             	mov    rdx,QWORD PTR [rax]
  4006a3:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  4006a7:	48 8b 48 10          	mov    rcx,QWORD PTR [rax+0x10]
  4006ab:	8b 45 fc             	mov    eax,DWORD PTR [rbp-0x4]
  4006ae:	48 98                	cdqe   
  4006b0:	48 c1 e0 03          	shl    rax,0x3
  4006b4:	48 8d 34 c5 00 00 00 	lea    rsi,[rax*8+0x0]
  4006bb:	00 
  4006bc:	48 29 c6             	sub    rsi,rax
  4006bf:	48 89 f0             	mov    rax,rsi
  4006c2:	48 01 c8             	add    rax,rcx
  4006c5:	48 8b 40 10          	mov    rax,QWORD PTR [rax+0x10]
  4006c9:	48 01 d0             	add    rax,rdx
/mnt/git/kb/asan/gbo.c:119 (discriminator 3)
  4006cc:	48 89 c2             	mov    rdx,rax
  4006cf:	8b 45 fc             	mov    eax,DWORD PTR [rbp-0x4]
  4006d2:	89 c6                	mov    esi,eax
  4006d4:	bf 8a 08 40 00       	mov    edi,0x40088a
  4006d9:	b8 00 00 00 00       	mov    eax,0x0
  4006de:	e8 2d fe ff ff       	call   400510 <printf@plt>
/mnt/git/kb/asan/gbo.c:122 (discriminator 3)
  4006e3:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  4006e7:	48 8b 50 10          	mov    rdx,QWORD PTR [rax+0x10]
  4006eb:	8b 45 fc             	mov    eax,DWORD PTR [rbp-0x4]
  4006ee:	48 98                	cdqe   
  4006f0:	48 c1 e0 03          	shl    rax,0x3
  4006f4:	48 8d 0c c5 00 00 00 	lea    rcx,[rax*8+0x0]
  4006fb:	00 
  4006fc:	48 29 c1             	sub    rcx,rax
  4006ff:	48 89 c8             	mov    rax,rcx
  400702:	48 01 d0             	add    rax,rdx
/mnt/git/kb/asan/gbo.c:121 (discriminator 3)
  400705:	8b 50 04             	mov    edx,DWORD PTR [rax+0x4]
/mnt/git/kb/asan/gbo.c:122 (discriminator 3)
  400708:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  40070c:	48 8b 48 10          	mov    rcx,QWORD PTR [rax+0x10]
  400710:	8b 45 fc             	mov    eax,DWORD PTR [rbp-0x4]
  400713:	48 98                	cdqe   
  400715:	48 c1 e0 03          	shl    rax,0x3
  400719:	48 8d 34 c5 00 00 00 	lea    rsi,[rax*8+0x0]
  400720:	00 
  400721:	48 29 c6             	sub    rsi,rax
  400724:	48 89 f0             	mov    rax,rsi
  400727:	48 01 c8             	add    rax,rcx
/mnt/git/kb/asan/gbo.c:121 (discriminator 3)
  40072a:	8b 00                	mov    eax,DWORD PTR [rax]
  40072c:	89 c6                	mov    esi,eax
  40072e:	bf a7 08 40 00       	mov    edi,0x4008a7
  400733:	b8 00 00 00 00       	mov    eax,0x0
  400738:	e8 d3 fd ff ff       	call   400510 <printf@plt>
/mnt/git/kb/asan/gbo.c:118 (discriminator 3)
  40073d:	83 45 fc 01          	add    DWORD PTR [rbp-0x4],0x1
/mnt/git/kb/asan/gbo.c:118 (discriminator 1)
  400741:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  400745:	0f b7 40 18          	movzx  eax,WORD PTR [rax+0x18]
  400749:	0f b7 c0             	movzx  eax,ax
  40074c:	3b 45 fc             	cmp    eax,DWORD PTR [rbp-0x4]
  40074f:	0f 8f 47 ff ff ff    	jg     40069c <header_handler+0x56>
/mnt/git/kb/asan/gbo.c:124
  400755:	bf 0a 00 00 00       	mov    edi,0xa
  40075a:	e8 91 fd ff ff       	call   4004f0 <putchar@plt>
/mnt/git/kb/asan/gbo.c:125
  40075f:	b8 00 00 00 00       	mov    eax,0x0
/mnt/git/kb/asan/gbo.c:126
  400764:	c9                   	leave  
  400765:	c3                   	ret    

0000000000400766 <call_gbo>:
call_gbo():
/mnt/git/kb/asan/gbo.c:131
  400766:	55                   	push   rbp
  400767:	48 89 e5             	mov    rbp,rsp
  40076a:	48 83 ec 10          	sub    rsp,0x10
/mnt/git/kb/asan/gbo.c:133
  40076e:	48 8d 45 f0          	lea    rax,[rbp-0x10]
  400772:	ba 08 00 00 00       	mov    edx,0x8
  400777:	be d8 0c 60 00       	mov    esi,0x600cd8
  40077c:	48 89 c7             	mov    rdi,rax
  40077f:	e8 bc fd ff ff       	call   400540 <memcpy@plt>
/mnt/git/kb/asan/gbo.c:134
  400784:	90                   	nop
/mnt/git/kb/asan/gbo.c:135
  400785:	c9                   	leave  
  400786:	c3                   	ret    

0000000000400787 <main>:
main():
/mnt/git/kb/asan/gbo.c:138
  400787:	55                   	push   rbp
  400788:	48 89 e5             	mov    rbp,rsp
  40078b:	48 83 ec 10          	sub    rsp,0x10
  40078f:	89 7d fc             	mov    DWORD PTR [rbp-0x4],edi
  400792:	48 89 75 f0          	mov    QWORD PTR [rbp-0x10],rsi
/mnt/git/kb/asan/gbo.c:139
  400796:	be 00 00 00 00       	mov    esi,0x0
  40079b:	bf 46 06 40 00       	mov    edi,0x400646
  4007a0:	e8 3b fd ff ff       	call   4004e0 <dl_iterate_phdr@plt>
/mnt/git/kb/asan/gbo.c:141
  4007a5:	bf c0 08 40 00       	mov    edi,0x4008c0
  4007aa:	e8 51 fd ff ff       	call   400500 <puts@plt>
/mnt/git/kb/asan/gbo.c:142
  4007af:	bf d0 08 40 00       	mov    edi,0x4008d0
  4007b4:	e8 47 fd ff ff       	call   400500 <puts@plt>
/mnt/git/kb/asan/gbo.c:143
  4007b9:	bf f4 08 40 00       	mov    edi,0x4008f4
  4007be:	e8 3d fd ff ff       	call   400500 <puts@plt>
/mnt/git/kb/asan/gbo.c:145
  4007c3:	b8 00 00 00 00       	mov    eax,0x0
  4007c8:	e8 99 ff ff ff       	call   400766 <call_gbo>
/mnt/git/kb/asan/gbo.c:147
  4007cd:	bf fb 08 40 00       	mov    edi,0x4008fb
  4007d2:	e8 29 fd ff ff       	call   400500 <puts@plt>
/mnt/git/kb/asan/gbo.c:149
  4007d7:	b8 00 00 00 00       	mov    eax,0x0
/mnt/git/kb/asan/gbo.c:150
  4007dc:	c9                   	leave  
  4007dd:	c3                   	ret    
  4007de:	66 90                	xchg   ax,ax

00000000004007e0 <__libc_csu_init>:
__libc_csu_init():
  4007e0:	41 57                	push   r15
  4007e2:	41 89 ff             	mov    r15d,edi
  4007e5:	41 56                	push   r14
  4007e7:	49 89 f6             	mov    r14,rsi
  4007ea:	41 55                	push   r13
  4007ec:	49 89 d5             	mov    r13,rdx
  4007ef:	41 54                	push   r12
  4007f1:	4c 8d 25 90 02 20 00 	lea    r12,[rip+0x200290]        # 600a88 <__frame_dummy_init_array_entry>
  4007f8:	55                   	push   rbp
  4007f9:	48 8d 2d 90 02 20 00 	lea    rbp,[rip+0x200290]        # 600a90 <__init_array_end>
  400800:	53                   	push   rbx
  400801:	4c 29 e5             	sub    rbp,r12
  400804:	31 db                	xor    ebx,ebx
  400806:	48 c1 fd 03          	sar    rbp,0x3
  40080a:	48 83 ec 08          	sub    rsp,0x8
  40080e:	e8 9d fc ff ff       	call   4004b0 <_init>
  400813:	48 85 ed             	test   rbp,rbp
  400816:	74 1e                	je     400836 <__libc_csu_init+0x56>
  400818:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
  40081f:	00 
  400820:	4c 89 ea             	mov    rdx,r13
  400823:	4c 89 f6             	mov    rsi,r14
  400826:	44 89 ff             	mov    edi,r15d
  400829:	41 ff 14 dc          	call   QWORD PTR [r12+rbx*8]
  40082d:	48 83 c3 01          	add    rbx,0x1
  400831:	48 39 eb             	cmp    rbx,rbp
  400834:	75 ea                	jne    400820 <__libc_csu_init+0x40>
  400836:	48 83 c4 08          	add    rsp,0x8
  40083a:	5b                   	pop    rbx
  40083b:	5d                   	pop    rbp
  40083c:	41 5c                	pop    r12
  40083e:	41 5d                	pop    r13
  400840:	41 5e                	pop    r14
  400842:	41 5f                	pop    r15
  400844:	c3                   	ret    
  400845:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
  40084c:	00 00 00 00 

0000000000400850 <__libc_csu_fini>:
__libc_csu_fini():
  400850:	f3 c3                	repz ret 

Disassembly of section .fini:

0000000000400854 <_fini>:
_fini():
  400854:	48 83 ec 08          	sub    rsp,0x8
  400858:	48 83 c4 08          	add    rsp,0x8
  40085c:	c3                   	ret    
