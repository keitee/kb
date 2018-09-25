
host_no_asan_uaf:     file format elf64-x86-64


Disassembly of section .init:

0000000000400450 <_init>:
_init():
  400450:	48 83 ec 08          	sub    rsp,0x8
  400454:	48 8b 05 a5 06 20 00 	mov    rax,QWORD PTR [rip+0x2006a5]        # 600b00 <_DYNAMIC+0x1d0>
  40045b:	48 85 c0             	test   rax,rax
  40045e:	74 05                	je     400465 <_init+0x15>
  400460:	e8 5b 00 00 00       	call   4004c0 <__gmon_start__@plt>
  400465:	48 83 c4 08          	add    rsp,0x8
  400469:	c3                   	ret    

Disassembly of section .plt:

0000000000400470 <free@plt-0x10>:
  400470:	ff 35 9a 06 20 00    	push   QWORD PTR [rip+0x20069a]        # 600b10 <_GLOBAL_OFFSET_TABLE_+0x8>
  400476:	ff 25 9c 06 20 00    	jmp    QWORD PTR [rip+0x20069c]        # 600b18 <_GLOBAL_OFFSET_TABLE_+0x10>
  40047c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]

0000000000400480 <free@plt>:
  400480:	ff 25 9a 06 20 00    	jmp    QWORD PTR [rip+0x20069a]        # 600b20 <_GLOBAL_OFFSET_TABLE_+0x18>
  400486:	68 00 00 00 00       	push   0x0
  40048b:	e9 e0 ff ff ff       	jmp    400470 <_init+0x20>

0000000000400490 <puts@plt>:
  400490:	ff 25 92 06 20 00    	jmp    QWORD PTR [rip+0x200692]        # 600b28 <_GLOBAL_OFFSET_TABLE_+0x20>
  400496:	68 01 00 00 00       	push   0x1
  40049b:	e9 d0 ff ff ff       	jmp    400470 <_init+0x20>

00000000004004a0 <printf@plt>:
  4004a0:	ff 25 8a 06 20 00    	jmp    QWORD PTR [rip+0x20068a]        # 600b30 <_GLOBAL_OFFSET_TABLE_+0x28>
  4004a6:	68 02 00 00 00       	push   0x2
  4004ab:	e9 c0 ff ff ff       	jmp    400470 <_init+0x20>

00000000004004b0 <__libc_start_main@plt>:
  4004b0:	ff 25 82 06 20 00    	jmp    QWORD PTR [rip+0x200682]        # 600b38 <_GLOBAL_OFFSET_TABLE_+0x30>
  4004b6:	68 03 00 00 00       	push   0x3
  4004bb:	e9 b0 ff ff ff       	jmp    400470 <_init+0x20>

00000000004004c0 <__gmon_start__@plt>:
  4004c0:	ff 25 7a 06 20 00    	jmp    QWORD PTR [rip+0x20067a]        # 600b40 <_GLOBAL_OFFSET_TABLE_+0x38>
  4004c6:	68 04 00 00 00       	push   0x4
  4004cb:	e9 a0 ff ff ff       	jmp    400470 <_init+0x20>

00000000004004d0 <malloc@plt>:
  4004d0:	ff 25 72 06 20 00    	jmp    QWORD PTR [rip+0x200672]        # 600b48 <_GLOBAL_OFFSET_TABLE_+0x40>
  4004d6:	68 05 00 00 00       	push   0x5
  4004db:	e9 90 ff ff ff       	jmp    400470 <_init+0x20>

Disassembly of section .text:

00000000004004e0 <_start>:
_start():
  4004e0:	31 ed                	xor    ebp,ebp
  4004e2:	49 89 d1             	mov    r9,rdx
  4004e5:	5e                   	pop    rsi
  4004e6:	48 89 e2             	mov    rdx,rsp
  4004e9:	48 83 e4 f0          	and    rsp,0xfffffffffffffff0
  4004ed:	50                   	push   rax
  4004ee:	54                   	push   rsp
  4004ef:	49 c7 c0 10 07 40 00 	mov    r8,0x400710
  4004f6:	48 c7 c1 a0 06 40 00 	mov    rcx,0x4006a0
  4004fd:	48 c7 c7 2e 06 40 00 	mov    rdi,0x40062e
  400504:	e8 a7 ff ff ff       	call   4004b0 <__libc_start_main@plt>
  400509:	f4                   	hlt    
  40050a:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]

0000000000400510 <deregister_tm_clones>:
deregister_tm_clones():
  400510:	b8 67 0b 60 00       	mov    eax,0x600b67
  400515:	55                   	push   rbp
  400516:	48 2d 60 0b 60 00    	sub    rax,0x600b60
  40051c:	48 83 f8 0e          	cmp    rax,0xe
  400520:	48 89 e5             	mov    rbp,rsp
  400523:	76 1b                	jbe    400540 <deregister_tm_clones+0x30>
  400525:	b8 00 00 00 00       	mov    eax,0x0
  40052a:	48 85 c0             	test   rax,rax
  40052d:	74 11                	je     400540 <deregister_tm_clones+0x30>
  40052f:	5d                   	pop    rbp
  400530:	bf 60 0b 60 00       	mov    edi,0x600b60
  400535:	ff e0                	jmp    rax
  400537:	66 0f 1f 84 00 00 00 	nop    WORD PTR [rax+rax*1+0x0]
  40053e:	00 00 
  400540:	5d                   	pop    rbp
  400541:	c3                   	ret    
  400542:	66 66 66 66 66 2e 0f 	data16 data16 data16 data16 nop WORD PTR cs:[rax+rax*1+0x0]
  400549:	1f 84 00 00 00 00 00 

0000000000400550 <register_tm_clones>:
register_tm_clones():
  400550:	be 60 0b 60 00       	mov    esi,0x600b60
  400555:	55                   	push   rbp
  400556:	48 81 ee 60 0b 60 00 	sub    rsi,0x600b60
  40055d:	48 c1 fe 03          	sar    rsi,0x3
  400561:	48 89 e5             	mov    rbp,rsp
  400564:	48 89 f0             	mov    rax,rsi
  400567:	48 c1 e8 3f          	shr    rax,0x3f
  40056b:	48 01 c6             	add    rsi,rax
  40056e:	48 d1 fe             	sar    rsi,1
  400571:	74 15                	je     400588 <register_tm_clones+0x38>
  400573:	b8 00 00 00 00       	mov    eax,0x0
  400578:	48 85 c0             	test   rax,rax
  40057b:	74 0b                	je     400588 <register_tm_clones+0x38>
  40057d:	5d                   	pop    rbp
  40057e:	bf 60 0b 60 00       	mov    edi,0x600b60
  400583:	ff e0                	jmp    rax
  400585:	0f 1f 00             	nop    DWORD PTR [rax]
  400588:	5d                   	pop    rbp
  400589:	c3                   	ret    
  40058a:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]

0000000000400590 <__do_global_dtors_aux>:
__do_global_dtors_aux():
  400590:	80 3d c9 05 20 00 00 	cmp    BYTE PTR [rip+0x2005c9],0x0        # 600b60 <__TMC_END__>
  400597:	75 11                	jne    4005aa <__do_global_dtors_aux+0x1a>
  400599:	55                   	push   rbp
  40059a:	48 89 e5             	mov    rbp,rsp
  40059d:	e8 6e ff ff ff       	call   400510 <deregister_tm_clones>
  4005a2:	5d                   	pop    rbp
  4005a3:	c6 05 b6 05 20 00 01 	mov    BYTE PTR [rip+0x2005b6],0x1        # 600b60 <__TMC_END__>
  4005aa:	f3 c3                	repz ret 
  4005ac:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]

00000000004005b0 <frame_dummy>:
frame_dummy():
  4005b0:	bf 28 09 60 00       	mov    edi,0x600928
  4005b5:	48 83 3f 00          	cmp    QWORD PTR [rdi],0x0
  4005b9:	75 05                	jne    4005c0 <frame_dummy+0x10>
  4005bb:	eb 93                	jmp    400550 <register_tm_clones>
  4005bd:	0f 1f 00             	nop    DWORD PTR [rax]
  4005c0:	b8 00 00 00 00       	mov    eax,0x0
  4005c5:	48 85 c0             	test   rax,rax
  4005c8:	74 f1                	je     4005bb <frame_dummy+0xb>
  4005ca:	55                   	push   rbp
  4005cb:	48 89 e5             	mov    rbp,rsp
  4005ce:	ff d0                	call   rax
  4005d0:	5d                   	pop    rbp
  4005d1:	e9 7a ff ff ff       	jmp    400550 <register_tm_clones>

00000000004005d6 <call_add>:
call_add():
/home/kyoupark/git/kb/asan/uaf_mod.c:7
  4005d6:	55                   	push   rbp
  4005d7:	48 89 e5             	mov    rbp,rsp
  4005da:	89 7d ec             	mov    DWORD PTR [rbp-0x14],edi
  4005dd:	89 75 e8             	mov    DWORD PTR [rbp-0x18],esi
/home/kyoupark/git/kb/asan/uaf_mod.c:8
  4005e0:	8b 55 ec             	mov    edx,DWORD PTR [rbp-0x14]
  4005e3:	89 d0                	mov    eax,edx
  4005e5:	c1 e0 02             	shl    eax,0x2
  4005e8:	01 d0                	add    eax,edx
  4005ea:	01 c0                	add    eax,eax
  4005ec:	89 c2                	mov    edx,eax
  4005ee:	8b 45 e8             	mov    eax,DWORD PTR [rbp-0x18]
  4005f1:	01 d0                	add    eax,edx
  4005f3:	89 45 fc             	mov    DWORD PTR [rbp-0x4],eax
/home/kyoupark/git/kb/asan/uaf_mod.c:9
  4005f6:	8b 45 fc             	mov    eax,DWORD PTR [rbp-0x4]
/home/kyoupark/git/kb/asan/uaf_mod.c:10
  4005f9:	5d                   	pop    rbp
  4005fa:	c3                   	ret    

00000000004005fb <call_uaf>:
call_uaf():
/home/kyoupark/git/kb/asan/uaf_mod.c:13
  4005fb:	55                   	push   rbp
  4005fc:	48 89 e5             	mov    rbp,rsp
  4005ff:	48 83 ec 10          	sub    rsp,0x10
/home/kyoupark/git/kb/asan/uaf_mod.c:14
  400603:	bf 28 00 00 00       	mov    edi,0x28                   ; 40 bytes
  400608:	e8 c3 fe ff ff       	call   4004d0 <malloc@plt>
  40060d:	48 89 45 f8          	mov    QWORD PTR [rbp-0x8],rax    ; get return
/home/kyoupark/git/kb/asan/uaf_mod.c:15
  400611:	48 8b 45 f8          	mov    rax,QWORD PTR [rbp-0x8]    ; get x
  400615:	48 89 c7             	mov    rdi,rax                    ; rdi = x
  400618:	e8 63 fe ff ff       	call   400480 <free@plt>
/home/kyoupark/git/kb/asan/uaf_mod.c:16
  40061d:	48 8b 45 f8          	mov    rax,QWORD PTR [rbp-0x8]    ; get x
  400621:	48 83 c0 14          	add    rax,0x14                   ; x+20
  400625:	c7 00 0a 00 00 00    	mov    DWORD PTR [rax],0xa        ; x[5] = 10
/home/kyoupark/git/kb/asan/uaf_mod.c:18
  40062b:	90                   	nop
/home/kyoupark/git/kb/asan/uaf_mod.c:19
  40062c:	c9                   	leave  
  40062d:	c3                   	ret    

000000000040062e <main>:
main():
/home/kyoupark/git/kb/asan/uaf_mod.c:22
  40062e:	55                   	push   rbp
  40062f:	48 89 e5             	mov    rbp,rsp
  400632:	48 83 ec 10          	sub    rsp,0x10
  400636:	89 7d fc             	mov    DWORD PTR [rbp-0x4],edi
  400639:	48 89 75 f0          	mov    QWORD PTR [rbp-0x10],rsi
/home/kyoupark/git/kb/asan/uaf_mod.c:23
  40063d:	bf 28 07 40 00       	mov    edi,0x400728
  400642:	e8 49 fe ff ff       	call   400490 <puts@plt>
/home/kyoupark/git/kb/asan/uaf_mod.c:24
  400647:	bf 38 07 40 00       	mov    edi,0x400738
  40064c:	e8 3f fe ff ff       	call   400490 <puts@plt>
/home/kyoupark/git/kb/asan/uaf_mod.c:25
  400651:	bf 5c 07 40 00       	mov    edi,0x40075c
  400656:	e8 35 fe ff ff       	call   400490 <puts@plt>
/home/kyoupark/git/kb/asan/uaf_mod.c:27
  40065b:	b8 00 00 00 00       	mov    eax,0x0
  400660:	e8 96 ff ff ff       	call   4005fb <call_uaf>
/home/kyoupark/git/kb/asan/uaf_mod.c:29
  400665:	be 14 00 00 00       	mov    esi,0x14
  40066a:	bf 0a 00 00 00       	mov    edi,0xa
  40066f:	e8 62 ff ff ff       	call   4005d6 <call_add>
  400674:	89 c6                	mov    esi,eax
  400676:	bf 68 07 40 00       	mov    edi,0x400768
  40067b:	b8 00 00 00 00       	mov    eax,0x0
  400680:	e8 1b fe ff ff       	call   4004a0 <printf@plt>
/home/kyoupark/git/kb/asan/uaf_mod.c:30
  400685:	bf 8d 07 40 00       	mov    edi,0x40078d
  40068a:	e8 01 fe ff ff       	call   400490 <puts@plt>
/home/kyoupark/git/kb/asan/uaf_mod.c:32
  40068f:	b8 00 00 00 00       	mov    eax,0x0
/home/kyoupark/git/kb/asan/uaf_mod.c:33
  400694:	c9                   	leave  
  400695:	c3                   	ret    
  400696:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
  40069d:	00 00 00 

00000000004006a0 <__libc_csu_init>:
__libc_csu_init():
  4006a0:	41 57                	push   r15
  4006a2:	41 89 ff             	mov    r15d,edi
  4006a5:	41 56                	push   r14
  4006a7:	49 89 f6             	mov    r14,rsi
  4006aa:	41 55                	push   r13
  4006ac:	49 89 d5             	mov    r13,rdx
  4006af:	41 54                	push   r12
  4006b1:	4c 8d 25 60 02 20 00 	lea    r12,[rip+0x200260]        # 600918 <__frame_dummy_init_array_entry>
  4006b8:	55                   	push   rbp
  4006b9:	48 8d 2d 60 02 20 00 	lea    rbp,[rip+0x200260]        # 600920 <__init_array_end>
  4006c0:	53                   	push   rbx
  4006c1:	4c 29 e5             	sub    rbp,r12
  4006c4:	31 db                	xor    ebx,ebx
  4006c6:	48 c1 fd 03          	sar    rbp,0x3
  4006ca:	48 83 ec 08          	sub    rsp,0x8
  4006ce:	e8 7d fd ff ff       	call   400450 <_init>
  4006d3:	48 85 ed             	test   rbp,rbp
  4006d6:	74 1e                	je     4006f6 <__libc_csu_init+0x56>
  4006d8:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
  4006df:	00 
  4006e0:	4c 89 ea             	mov    rdx,r13
  4006e3:	4c 89 f6             	mov    rsi,r14
  4006e6:	44 89 ff             	mov    edi,r15d
  4006e9:	41 ff 14 dc          	call   QWORD PTR [r12+rbx*8]
  4006ed:	48 83 c3 01          	add    rbx,0x1
  4006f1:	48 39 eb             	cmp    rbx,rbp
  4006f4:	75 ea                	jne    4006e0 <__libc_csu_init+0x40>
  4006f6:	48 83 c4 08          	add    rsp,0x8
  4006fa:	5b                   	pop    rbx
  4006fb:	5d                   	pop    rbp
  4006fc:	41 5c                	pop    r12
  4006fe:	41 5d                	pop    r13
  400700:	41 5e                	pop    r14
  400702:	41 5f                	pop    r15
  400704:	c3                   	ret    
  400705:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
  40070c:	00 00 00 00 

0000000000400710 <__libc_csu_fini>:
__libc_csu_fini():
  400710:	f3 c3                	repz ret 

Disassembly of section .fini:

0000000000400714 <_fini>:
_fini():
  400714:	48 83 ec 08          	sub    rsp,0x8
  400718:	48 83 c4 08          	add    rsp,0x8
  40071c:	c3                   	ret    
