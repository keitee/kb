
host_asan_uaf:     file format elf64-x86-64


Disassembly of section .init:

00000000004006d8 <_init>:
_init():
  4006d8:	48 83 ec 08          	sub    rsp,0x8
  4006dc:	48 8b 05 4d 0b 20 00 	mov    rax,QWORD PTR [rip+0x200b4d]        # 601230 <_DYNAMIC+0x200>
  4006e3:	48 85 c0             	test   rax,rax
  4006e6:	74 05                	je     4006ed <_init+0x15>
  4006e8:	e8 43 00 00 00       	call   400730 <__gmon_start__@plt>
  4006ed:	48 83 c4 08          	add    rsp,0x8
  4006f1:	c3                   	ret    

Disassembly of section .plt:

0000000000400700 <printf@plt-0x10>:
  400700:	ff 35 3a 0b 20 00    	push   QWORD PTR [rip+0x200b3a]        # 601240 <_GLOBAL_OFFSET_TABLE_+0x8>
  400706:	ff 25 3c 0b 20 00    	jmp    QWORD PTR [rip+0x200b3c]        # 601248 <_GLOBAL_OFFSET_TABLE_+0x10>
  40070c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]

0000000000400710 <printf@plt>:
  400710:	ff 25 3a 0b 20 00    	jmp    QWORD PTR [rip+0x200b3a]        # 601250 <_GLOBAL_OFFSET_TABLE_+0x18>
  400716:	68 00 00 00 00       	push   0x0
  40071b:	e9 e0 ff ff ff       	jmp    400700 <_init+0x28>

0000000000400720 <__asan_init_v3@plt>:
  400720:	ff 25 32 0b 20 00    	jmp    QWORD PTR [rip+0x200b32]        # 601258 <_GLOBAL_OFFSET_TABLE_+0x20>
  400726:	68 01 00 00 00       	push   0x1
  40072b:	e9 d0 ff ff ff       	jmp    400700 <_init+0x28>

0000000000400730 <__gmon_start__@plt>:
  400730:	ff 25 2a 0b 20 00    	jmp    QWORD PTR [rip+0x200b2a]        # 601260 <_GLOBAL_OFFSET_TABLE_+0x28>
  400736:	68 02 00 00 00       	push   0x2
  40073b:	e9 c0 ff ff ff       	jmp    400700 <_init+0x28>

0000000000400740 <puts@plt>:
  400740:	ff 25 22 0b 20 00    	jmp    QWORD PTR [rip+0x200b22]        # 601268 <_GLOBAL_OFFSET_TABLE_+0x30>
  400746:	68 03 00 00 00       	push   0x3
  40074b:	e9 b0 ff ff ff       	jmp    400700 <_init+0x28>

0000000000400750 <__libc_start_main@plt>:
  400750:	ff 25 1a 0b 20 00    	jmp    QWORD PTR [rip+0x200b1a]        # 601270 <_GLOBAL_OFFSET_TABLE_+0x38>
  400756:	68 04 00 00 00       	push   0x4
  40075b:	e9 a0 ff ff ff       	jmp    400700 <_init+0x28>

0000000000400760 <malloc@plt>:
  400760:	ff 25 12 0b 20 00    	jmp    QWORD PTR [rip+0x200b12]        # 601278 <_GLOBAL_OFFSET_TABLE_+0x40>
  400766:	68 05 00 00 00       	push   0x5
  40076b:	e9 90 ff ff ff       	jmp    400700 <_init+0x28>

0000000000400770 <__asan_report_store4@plt>:
  400770:	ff 25 0a 0b 20 00    	jmp    QWORD PTR [rip+0x200b0a]        # 601280 <_GLOBAL_OFFSET_TABLE_+0x48>
  400776:	68 06 00 00 00       	push   0x6
  40077b:	e9 80 ff ff ff       	jmp    400700 <_init+0x28>

0000000000400780 <__asan_unregister_globals@plt>:
  400780:	ff 25 02 0b 20 00    	jmp    QWORD PTR [rip+0x200b02]        # 601288 <_GLOBAL_OFFSET_TABLE_+0x50>
  400786:	68 07 00 00 00       	push   0x7
  40078b:	e9 70 ff ff ff       	jmp    400700 <_init+0x28>

0000000000400790 <__asan_register_globals@plt>:
  400790:	ff 25 fa 0a 20 00    	jmp    QWORD PTR [rip+0x200afa]        # 601290 <_GLOBAL_OFFSET_TABLE_+0x58>
  400796:	68 08 00 00 00       	push   0x8
  40079b:	e9 60 ff ff ff       	jmp    400700 <_init+0x28>

00000000004007a0 <free@plt>:
  4007a0:	ff 25 f2 0a 20 00    	jmp    QWORD PTR [rip+0x200af2]        # 601298 <_GLOBAL_OFFSET_TABLE_+0x60>
  4007a6:	68 09 00 00 00       	push   0x9
  4007ab:	e9 50 ff ff ff       	jmp    400700 <_init+0x28>

Disassembly of section .text:

00000000004007b0 <_start>:
_start():
  4007b0:	31 ed                	xor    ebp,ebp
  4007b2:	49 89 d1             	mov    r9,rdx
  4007b5:	5e                   	pop    rsi
  4007b6:	48 89 e2             	mov    rdx,rsp
  4007b9:	48 83 e4 f0          	and    rsp,0xfffffffffffffff0
  4007bd:	50                   	push   rax
  4007be:	54                   	push   rsp
  4007bf:	49 c7 c0 40 0a 40 00 	mov    r8,0x400a40
  4007c6:	48 c7 c1 d0 09 40 00 	mov    rcx,0x4009d0
  4007cd:	48 c7 c7 35 09 40 00 	mov    rdi,0x400935
  4007d4:	e8 77 ff ff ff       	call   400750 <__libc_start_main@plt>
  4007d9:	f4                   	hlt    
  4007da:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]

00000000004007e0 <deregister_tm_clones>:
deregister_tm_clones():
  4007e0:	b8 f7 13 60 00       	mov    eax,0x6013f7
  4007e5:	55                   	push   rbp
  4007e6:	48 2d f0 13 60 00    	sub    rax,0x6013f0
  4007ec:	48 83 f8 0e          	cmp    rax,0xe
  4007f0:	48 89 e5             	mov    rbp,rsp
  4007f3:	76 1b                	jbe    400810 <deregister_tm_clones+0x30>
  4007f5:	b8 00 00 00 00       	mov    eax,0x0
  4007fa:	48 85 c0             	test   rax,rax
  4007fd:	74 11                	je     400810 <deregister_tm_clones+0x30>
  4007ff:	5d                   	pop    rbp
  400800:	bf f0 13 60 00       	mov    edi,0x6013f0
  400805:	ff e0                	jmp    rax
  400807:	66 0f 1f 84 00 00 00 	nop    WORD PTR [rax+rax*1+0x0]
  40080e:	00 00 
  400810:	5d                   	pop    rbp
  400811:	c3                   	ret    
  400812:	66 66 66 66 66 2e 0f 	data16 data16 data16 data16 nop WORD PTR cs:[rax+rax*1+0x0]
  400819:	1f 84 00 00 00 00 00 

0000000000400820 <register_tm_clones>:
register_tm_clones():
  400820:	be f0 13 60 00       	mov    esi,0x6013f0
  400825:	55                   	push   rbp
  400826:	48 81 ee f0 13 60 00 	sub    rsi,0x6013f0
  40082d:	48 c1 fe 03          	sar    rsi,0x3
  400831:	48 89 e5             	mov    rbp,rsp
  400834:	48 89 f0             	mov    rax,rsi
  400837:	48 c1 e8 3f          	shr    rax,0x3f
  40083b:	48 01 c6             	add    rsi,rax
  40083e:	48 d1 fe             	sar    rsi,1
  400841:	74 15                	je     400858 <register_tm_clones+0x38>
  400843:	b8 00 00 00 00       	mov    eax,0x0
  400848:	48 85 c0             	test   rax,rax
  40084b:	74 0b                	je     400858 <register_tm_clones+0x38>
  40084d:	5d                   	pop    rbp
  40084e:	bf f0 13 60 00       	mov    edi,0x6013f0
  400853:	ff e0                	jmp    rax
  400855:	0f 1f 00             	nop    DWORD PTR [rax]
  400858:	5d                   	pop    rbp
  400859:	c3                   	ret    
  40085a:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]

0000000000400860 <__do_global_dtors_aux>:
__do_global_dtors_aux():
  400860:	80 3d 89 0b 20 00 00 	cmp    BYTE PTR [rip+0x200b89],0x0        # 6013f0 <__TMC_END__>
  400867:	75 11                	jne    40087a <__do_global_dtors_aux+0x1a>
  400869:	55                   	push   rbp
  40086a:	48 89 e5             	mov    rbp,rsp
  40086d:	e8 6e ff ff ff       	call   4007e0 <deregister_tm_clones>
  400872:	5d                   	pop    rbp
  400873:	c6 05 76 0b 20 00 01 	mov    BYTE PTR [rip+0x200b76],0x1        # 6013f0 <__TMC_END__>
  40087a:	f3 c3                	repz ret 
  40087c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]

0000000000400880 <frame_dummy>:
frame_dummy():
  400880:	bf 28 10 60 00       	mov    edi,0x601028
  400885:	48 83 3f 00          	cmp    QWORD PTR [rdi],0x0
  400889:	75 05                	jne    400890 <frame_dummy+0x10>
  40088b:	eb 93                	jmp    400820 <register_tm_clones>
  40088d:	0f 1f 00             	nop    DWORD PTR [rax]
  400890:	b8 00 00 00 00       	mov    eax,0x0
  400895:	48 85 c0             	test   rax,rax
  400898:	74 f1                	je     40088b <frame_dummy+0xb>
  40089a:	55                   	push   rbp
  40089b:	48 89 e5             	mov    rbp,rsp
  40089e:	ff d0                	call   rax
  4008a0:	5d                   	pop    rbp
  4008a1:	e9 7a ff ff ff       	jmp    400820 <register_tm_clones>

00000000004008a6 <call_add>:
call_add():
/home/kyoupark/git/kb/asan/uaf_mod.c:7
  4008a6:	55                   	push   rbp
  4008a7:	48 89 e5             	mov    rbp,rsp
  4008aa:	89 7d ec             	mov    DWORD PTR [rbp-0x14],edi
  4008ad:	89 75 e8             	mov    DWORD PTR [rbp-0x18],esi
/home/kyoupark/git/kb/asan/uaf_mod.c:8
  4008b0:	8b 55 ec             	mov    edx,DWORD PTR [rbp-0x14]
  4008b3:	89 d0                	mov    eax,edx
  4008b5:	c1 e0 02             	shl    eax,0x2
  4008b8:	01 d0                	add    eax,edx
  4008ba:	01 c0                	add    eax,eax
  4008bc:	89 c2                	mov    edx,eax
  4008be:	8b 45 e8             	mov    eax,DWORD PTR [rbp-0x18]
  4008c1:	01 d0                	add    eax,edx
  4008c3:	89 45 fc             	mov    DWORD PTR [rbp-0x4],eax
/home/kyoupark/git/kb/asan/uaf_mod.c:9
  4008c6:	8b 45 fc             	mov    eax,DWORD PTR [rbp-0x4]
/home/kyoupark/git/kb/asan/uaf_mod.c:10
  4008c9:	5d                   	pop    rbp
  4008ca:	c3                   	ret    

00000000004008cb <call_uaf>:
call_uaf():
/home/kyoupark/git/kb/asan/uaf_mod.c:13
  4008cb:	55                   	push   rbp
  4008cc:	48 89 e5             	mov    rbp,rsp
  4008cf:	48 83 ec 10          	sub    rsp,0x10
/home/kyoupark/git/kb/asan/uaf_mod.c:14
  4008d3:	bf 28 00 00 00       	mov    edi,0x28
  4008d8:	e8 83 fe ff ff       	call   400760 <malloc@plt>
  4008dd:	48 89 45 f8          	mov    QWORD PTR [rbp-0x8],rax
/home/kyoupark/git/kb/asan/uaf_mod.c:15
  4008e1:	48 8b 45 f8          	mov    rax,QWORD PTR [rbp-0x8]
  4008e5:	48 89 c7             	mov    rdi,rax
  4008e8:	e8 b3 fe ff ff       	call   4007a0 <free@plt>
/home/kyoupark/git/kb/asan/uaf_mod.c:16
  4008ed:	48 8b 45 f8          	mov    rax,QWORD PTR [rbp-0x8]
  4008f1:	48 8d 48 14          	lea    rcx,[rax+0x14]
  4008f5:	48 89 c8             	mov    rax,rcx
  4008f8:	48 89 c2             	mov    rdx,rax
  4008fb:	48 c1 ea 03          	shr    rdx,0x3
  4008ff:	48 81 c2 00 80 ff 7f 	add    rdx,0x7fff8000
  400906:	0f b6 12             	movzx  edx,BYTE PTR [rdx]
  400909:	84 d2                	test   dl,dl
  40090b:	40 0f 95 c6          	setne  sil
  40090f:	48 89 c7             	mov    rdi,rax
  400912:	83 e7 07             	and    edi,0x7
  400915:	83 c7 03             	add    edi,0x3
  400918:	40 38 d7             	cmp    dil,dl
  40091b:	0f 9d c2             	setge  dl
  40091e:	21 f2                	and    edx,esi
  400920:	84 d2                	test   dl,dl
  400922:	74 08                	je     40092c <call_uaf+0x61>
  400924:	48 89 c7             	mov    rdi,rax
  400927:	e8 44 fe ff ff       	call   400770 <__asan_report_store4@plt>
  40092c:	c7 01 0a 00 00 00    	mov    DWORD PTR [rcx],0xa
/home/kyoupark/git/kb/asan/uaf_mod.c:18
  400932:	90                   	nop
/home/kyoupark/git/kb/asan/uaf_mod.c:19
  400933:	c9                   	leave  
  400934:	c3                   	ret    

0000000000400935 <main>:
main():
/home/kyoupark/git/kb/asan/uaf_mod.c:22
  400935:	55                   	push   rbp
  400936:	48 89 e5             	mov    rbp,rsp
  400939:	48 83 ec 10          	sub    rsp,0x10
  40093d:	89 7d fc             	mov    DWORD PTR [rbp-0x4],edi
  400940:	48 89 75 f0          	mov    QWORD PTR [rbp-0x10],rsi
/home/kyoupark/git/kb/asan/uaf_mod.c:23
  400944:	bf 80 0a 40 00       	mov    edi,0x400a80
  400949:	e8 f2 fd ff ff       	call   400740 <puts@plt>
/home/kyoupark/git/kb/asan/uaf_mod.c:24
  40094e:	bf c0 0a 40 00       	mov    edi,0x400ac0
  400953:	e8 e8 fd ff ff       	call   400740 <puts@plt>
/home/kyoupark/git/kb/asan/uaf_mod.c:25
  400958:	bf 20 0b 40 00       	mov    edi,0x400b20
  40095d:	e8 de fd ff ff       	call   400740 <puts@plt>
/home/kyoupark/git/kb/asan/uaf_mod.c:27
  400962:	b8 00 00 00 00       	mov    eax,0x0
  400967:	e8 5f ff ff ff       	call   4008cb <call_uaf>
/home/kyoupark/git/kb/asan/uaf_mod.c:29
  40096c:	be 14 00 00 00       	mov    esi,0x14
  400971:	bf 0a 00 00 00       	mov    edi,0xa
  400976:	e8 2b ff ff ff       	call   4008a6 <call_add>
  40097b:	89 c6                	mov    esi,eax
  40097d:	bf 60 0b 40 00       	mov    edi,0x400b60
  400982:	b8 00 00 00 00       	mov    eax,0x0
  400987:	e8 84 fd ff ff       	call   400710 <printf@plt>
/home/kyoupark/git/kb/asan/uaf_mod.c:30
  40098c:	bf c0 0b 40 00       	mov    edi,0x400bc0
  400991:	e8 aa fd ff ff       	call   400740 <puts@plt>
/home/kyoupark/git/kb/asan/uaf_mod.c:32
  400996:	b8 00 00 00 00       	mov    eax,0x0
/home/kyoupark/git/kb/asan/uaf_mod.c:33
  40099b:	c9                   	leave  
  40099c:	c3                   	ret    

000000000040099d <_GLOBAL__sub_D_00099_0_call_add>:
_GLOBAL__sub_D_00099_0_call_add():
  40099d:	55                   	push   rbp
  40099e:	48 89 e5             	mov    rbp,rsp
  4009a1:	be 05 00 00 00       	mov    esi,0x5
  4009a6:	bf 00 13 60 00       	mov    edi,0x601300
  4009ab:	e8 d0 fd ff ff       	call   400780 <__asan_unregister_globals@plt>
  4009b0:	5d                   	pop    rbp
  4009b1:	c3                   	ret    

00000000004009b2 <_GLOBAL__sub_I_00099_1_call_add>:
_GLOBAL__sub_I_00099_1_call_add():
  4009b2:	55                   	push   rbp
  4009b3:	48 89 e5             	mov    rbp,rsp
  4009b6:	e8 65 fd ff ff       	call   400720 <__asan_init_v3@plt>
  4009bb:	be 05 00 00 00       	mov    esi,0x5
  4009c0:	bf 00 13 60 00       	mov    edi,0x601300
  4009c5:	e8 c6 fd ff ff       	call   400790 <__asan_register_globals@plt>
  4009ca:	5d                   	pop    rbp
  4009cb:	c3                   	ret    
  4009cc:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]

00000000004009d0 <__libc_csu_init>:
__libc_csu_init():
  4009d0:	41 57                	push   r15
  4009d2:	41 89 ff             	mov    r15d,edi
  4009d5:	41 56                	push   r14
  4009d7:	49 89 f6             	mov    r14,rsi
  4009da:	41 55                	push   r13
  4009dc:	49 89 d5             	mov    r13,rdx
  4009df:	41 54                	push   r12
  4009e1:	4c 8d 25 20 06 20 00 	lea    r12,[rip+0x200620]        # 601008 <__init_array_start>
  4009e8:	55                   	push   rbp
  4009e9:	48 8d 2d 28 06 20 00 	lea    rbp,[rip+0x200628]        # 601018 <__init_array_end>
  4009f0:	53                   	push   rbx
  4009f1:	4c 29 e5             	sub    rbp,r12
  4009f4:	31 db                	xor    ebx,ebx
  4009f6:	48 c1 fd 03          	sar    rbp,0x3
  4009fa:	48 83 ec 08          	sub    rsp,0x8
  4009fe:	e8 d5 fc ff ff       	call   4006d8 <_init>
  400a03:	48 85 ed             	test   rbp,rbp
  400a06:	74 1e                	je     400a26 <__libc_csu_init+0x56>
  400a08:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
  400a0f:	00 
  400a10:	4c 89 ea             	mov    rdx,r13
  400a13:	4c 89 f6             	mov    rsi,r14
  400a16:	44 89 ff             	mov    edi,r15d
  400a19:	41 ff 14 dc          	call   QWORD PTR [r12+rbx*8]
  400a1d:	48 83 c3 01          	add    rbx,0x1
  400a21:	48 39 eb             	cmp    rbx,rbp
  400a24:	75 ea                	jne    400a10 <__libc_csu_init+0x40>
  400a26:	48 83 c4 08          	add    rsp,0x8
  400a2a:	5b                   	pop    rbx
  400a2b:	5d                   	pop    rbp
  400a2c:	41 5c                	pop    r12
  400a2e:	41 5d                	pop    r13
  400a30:	41 5e                	pop    r14
  400a32:	41 5f                	pop    r15
  400a34:	c3                   	ret    
  400a35:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
  400a3c:	00 00 00 00 

0000000000400a40 <__libc_csu_fini>:
__libc_csu_fini():
  400a40:	f3 c3                	repz ret 

Disassembly of section .fini:

0000000000400a44 <_fini>:
_fini():
  400a44:	48 83 ec 08          	sub    rsp,0x8
  400a48:	48 83 c4 08          	add    rsp,0x8
  400a4c:	c3                   	ret    
