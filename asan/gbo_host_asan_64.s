
gbo_host_asan:     file format elf64-x86-64


Disassembly of section .init:

0000000000400758 <_init>:
_init():
  400758:	48 83 ec 08          	sub    rsp,0x8
  40075c:	48 8b 05 cd 0a 20 00 	mov    rax,QWORD PTR [rip+0x200acd]        # 601230 <_DYNAMIC+0x200>
  400763:	48 85 c0             	test   rax,rax
  400766:	74 05                	je     40076d <_init+0x15>
  400768:	e8 43 00 00 00       	call   4007b0 <__gmon_start__@plt>
  40076d:	48 83 c4 08          	add    rsp,0x8
  400771:	c3                   	ret    

Disassembly of section .plt:

0000000000400780 <__asan_init_v3@plt-0x10>:
  400780:	ff 35 ba 0a 20 00    	push   QWORD PTR [rip+0x200aba]        # 601240 <_GLOBAL_OFFSET_TABLE_+0x8>
  400786:	ff 25 bc 0a 20 00    	jmp    QWORD PTR [rip+0x200abc]        # 601248 <_GLOBAL_OFFSET_TABLE_+0x10>
  40078c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]

0000000000400790 <__asan_init_v3@plt>:
  400790:	ff 25 ba 0a 20 00    	jmp    QWORD PTR [rip+0x200aba]        # 601250 <_GLOBAL_OFFSET_TABLE_+0x18>
  400796:	68 00 00 00 00       	push   0x0
  40079b:	e9 e0 ff ff ff       	jmp    400780 <_init+0x28>

00000000004007a0 <memcpy@plt>:
  4007a0:	ff 25 b2 0a 20 00    	jmp    QWORD PTR [rip+0x200ab2]        # 601258 <_GLOBAL_OFFSET_TABLE_+0x20>
  4007a6:	68 01 00 00 00       	push   0x1
  4007ab:	e9 d0 ff ff ff       	jmp    400780 <_init+0x28>

00000000004007b0 <__gmon_start__@plt>:
  4007b0:	ff 25 aa 0a 20 00    	jmp    QWORD PTR [rip+0x200aaa]        # 601260 <_GLOBAL_OFFSET_TABLE_+0x28>
  4007b6:	68 02 00 00 00       	push   0x2
  4007bb:	e9 c0 ff ff ff       	jmp    400780 <_init+0x28>

00000000004007c0 <puts@plt>:
  4007c0:	ff 25 a2 0a 20 00    	jmp    QWORD PTR [rip+0x200aa2]        # 601268 <_GLOBAL_OFFSET_TABLE_+0x30>
  4007c6:	68 03 00 00 00       	push   0x3
  4007cb:	e9 b0 ff ff ff       	jmp    400780 <_init+0x28>

00000000004007d0 <__asan_stack_malloc_1@plt>:
  4007d0:	ff 25 9a 0a 20 00    	jmp    QWORD PTR [rip+0x200a9a]        # 601270 <_GLOBAL_OFFSET_TABLE_+0x38>
  4007d6:	68 04 00 00 00       	push   0x4
  4007db:	e9 a0 ff ff ff       	jmp    400780 <_init+0x28>

00000000004007e0 <__asan_report_load_n@plt>:
  4007e0:	ff 25 92 0a 20 00    	jmp    QWORD PTR [rip+0x200a92]        # 601278 <_GLOBAL_OFFSET_TABLE_+0x40>
  4007e6:	68 05 00 00 00       	push   0x5
  4007eb:	e9 90 ff ff ff       	jmp    400780 <_init+0x28>

00000000004007f0 <__asan_report_store_n@plt>:
  4007f0:	ff 25 8a 0a 20 00    	jmp    QWORD PTR [rip+0x200a8a]        # 601280 <_GLOBAL_OFFSET_TABLE_+0x48>
  4007f6:	68 06 00 00 00       	push   0x6
  4007fb:	e9 80 ff ff ff       	jmp    400780 <_init+0x28>

0000000000400800 <__libc_start_main@plt>:
  400800:	ff 25 82 0a 20 00    	jmp    QWORD PTR [rip+0x200a82]        # 601288 <_GLOBAL_OFFSET_TABLE_+0x50>
  400806:	68 07 00 00 00       	push   0x7
  40080b:	e9 70 ff ff ff       	jmp    400780 <_init+0x28>

0000000000400810 <__asan_unregister_globals@plt>:
  400810:	ff 25 7a 0a 20 00    	jmp    QWORD PTR [rip+0x200a7a]        # 601290 <_GLOBAL_OFFSET_TABLE_+0x58>
  400816:	68 08 00 00 00       	push   0x8
  40081b:	e9 60 ff ff ff       	jmp    400780 <_init+0x28>

0000000000400820 <__asan_register_globals@plt>:
  400820:	ff 25 72 0a 20 00    	jmp    QWORD PTR [rip+0x200a72]        # 601298 <_GLOBAL_OFFSET_TABLE_+0x60>
  400826:	68 09 00 00 00       	push   0x9
  40082b:	e9 50 ff ff ff       	jmp    400780 <_init+0x28>

Disassembly of section .text:

0000000000400830 <_start>:
_start():
  400830:	31 ed                	xor    ebp,ebp
  400832:	49 89 d1             	mov    r9,rdx
  400835:	5e                   	pop    rsi
  400836:	48 89 e2             	mov    rdx,rsp
  400839:	48 83 e4 f0          	and    rsp,0xfffffffffffffff0
  40083d:	50                   	push   rax
  40083e:	54                   	push   rsp
  40083f:	49 c7 c0 d0 0b 40 00 	mov    r8,0x400bd0
  400846:	48 c7 c1 60 0b 40 00 	mov    rcx,0x400b60
  40084d:	48 c7 c7 e1 0a 40 00 	mov    rdi,0x400ae1
  400854:	e8 a7 ff ff ff       	call   400800 <__libc_start_main@plt>
  400859:	f4                   	hlt    
  40085a:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]

0000000000400860 <deregister_tm_clones>:
deregister_tm_clones():
  400860:	b8 37 14 60 00       	mov    eax,0x601437
  400865:	55                   	push   rbp
  400866:	48 2d 30 14 60 00    	sub    rax,0x601430
  40086c:	48 83 f8 0e          	cmp    rax,0xe
  400870:	48 89 e5             	mov    rbp,rsp
  400873:	76 1b                	jbe    400890 <deregister_tm_clones+0x30>
  400875:	b8 00 00 00 00       	mov    eax,0x0
  40087a:	48 85 c0             	test   rax,rax
  40087d:	74 11                	je     400890 <deregister_tm_clones+0x30>
  40087f:	5d                   	pop    rbp
  400880:	bf 30 14 60 00       	mov    edi,0x601430
  400885:	ff e0                	jmp    rax
  400887:	66 0f 1f 84 00 00 00 	nop    WORD PTR [rax+rax*1+0x0]
  40088e:	00 00 
  400890:	5d                   	pop    rbp
  400891:	c3                   	ret    
  400892:	66 66 66 66 66 2e 0f 	data16 data16 data16 data16 nop WORD PTR cs:[rax+rax*1+0x0]
  400899:	1f 84 00 00 00 00 00 

00000000004008a0 <register_tm_clones>:
register_tm_clones():
  4008a0:	be 30 14 60 00       	mov    esi,0x601430
  4008a5:	55                   	push   rbp
  4008a6:	48 81 ee 30 14 60 00 	sub    rsi,0x601430
  4008ad:	48 c1 fe 03          	sar    rsi,0x3
  4008b1:	48 89 e5             	mov    rbp,rsp
  4008b4:	48 89 f0             	mov    rax,rsi
  4008b7:	48 c1 e8 3f          	shr    rax,0x3f
  4008bb:	48 01 c6             	add    rsi,rax
  4008be:	48 d1 fe             	sar    rsi,1
  4008c1:	74 15                	je     4008d8 <register_tm_clones+0x38>
  4008c3:	b8 00 00 00 00       	mov    eax,0x0
  4008c8:	48 85 c0             	test   rax,rax
  4008cb:	74 0b                	je     4008d8 <register_tm_clones+0x38>
  4008cd:	5d                   	pop    rbp
  4008ce:	bf 30 14 60 00       	mov    edi,0x601430
  4008d3:	ff e0                	jmp    rax
  4008d5:	0f 1f 00             	nop    DWORD PTR [rax]
  4008d8:	5d                   	pop    rbp
  4008d9:	c3                   	ret    
  4008da:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]

00000000004008e0 <__do_global_dtors_aux>:
__do_global_dtors_aux():
  4008e0:	80 3d 5d 0b 20 00 00 	cmp    BYTE PTR [rip+0x200b5d],0x0        # 601444 <completed.6661>
  4008e7:	75 11                	jne    4008fa <__do_global_dtors_aux+0x1a>
  4008e9:	55                   	push   rbp
  4008ea:	48 89 e5             	mov    rbp,rsp
  4008ed:	e8 6e ff ff ff       	call   400860 <deregister_tm_clones>
  4008f2:	5d                   	pop    rbp
  4008f3:	c6 05 4a 0b 20 00 01 	mov    BYTE PTR [rip+0x200b4a],0x1        # 601444 <completed.6661>
  4008fa:	f3 c3                	repz ret 
  4008fc:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]

0000000000400900 <frame_dummy>:
frame_dummy():
  400900:	bf 28 10 60 00       	mov    edi,0x601028
  400905:	48 83 3f 00          	cmp    QWORD PTR [rdi],0x0
  400909:	75 05                	jne    400910 <frame_dummy+0x10>
  40090b:	eb 93                	jmp    4008a0 <register_tm_clones>
  40090d:	0f 1f 00             	nop    DWORD PTR [rax]
  400910:	b8 00 00 00 00       	mov    eax,0x0
  400915:	48 85 c0             	test   rax,rax
  400918:	74 f1                	je     40090b <frame_dummy+0xb>
  40091a:	55                   	push   rbp
  40091b:	48 89 e5             	mov    rbp,rsp
  40091e:	ff d0                	call   rax
  400920:	5d                   	pop    rbp
  400921:	e9 7a ff ff ff       	jmp    4008a0 <register_tm_clones>

0000000000400926 <call_gbo>:
call_gbo():
/home/kyoupark/git/kb/asan/gbo.c:10
  400926:	55                   	push   rbp
  400927:	48 89 e5             	mov    rbp,rsp
  40092a:	41 55                	push   r13
  40092c:	41 54                	push   r12
  40092e:	53                   	push   rbx
  40092f:	48 83 ec 68          	sub    rsp,0x68
  400933:	48 8d 5d 80          	lea    rbx,[rbp-0x80]
  400937:	49 89 dd             	mov    r13,rbx
  40093a:	83 3d ff 0a 20 00 00 	cmp    DWORD PTR [rip+0x200aff],0x0        # 601440 <__asan_option_detect_stack_use_after_return>
  400941:	74 10                	je     400953 <call_gbo+0x2d>
  400943:	48 89 de             	mov    rsi,rbx
  400946:	bf 60 00 00 00       	mov    edi,0x60
  40094b:	e8 80 fe ff ff       	call   4007d0 <__asan_stack_malloc_1@plt>
  400950:	48 89 c3             	mov    rbx,rax
  400953:	48 8d 43 60          	lea    rax,[rbx+0x60]
  400957:	48 89 c7             	mov    rdi,rax
  40095a:	48 c7 03 b3 8a b5 41 	mov    QWORD PTR [rbx],0x41b58ab3
  400961:	48 c7 43 08 00 0c 40 	mov    QWORD PTR [rbx+0x8],0x400c00
  400968:	00 
  400969:	48 c7 43 10 26 09 40 	mov    QWORD PTR [rbx+0x10],0x400926
  400970:	00 
  400971:	49 89 dc             	mov    r12,rbx
  400974:	49 c1 ec 03          	shr    r12,0x3
  400978:	41 c7 84 24 00 80 ff 	mov    DWORD PTR [r12+0x7fff8000],0xf1f1f1f1
  40097f:	7f f1 f1 f1 f1 
  400984:	41 c7 84 24 04 80 ff 	mov    DWORD PTR [r12+0x7fff8004],0xf4f40200
  40098b:	7f 00 02 f4 f4 
  400990:	41 c7 84 24 08 80 ff 	mov    DWORD PTR [r12+0x7fff8008],0xf3f3f3f3
  400997:	7f f3 f3 f3 f3 
/home/kyoupark/git/kb/asan/gbo.c:12
  40099c:	b8 00 13 60 00       	mov    eax,0x601300
  4009a1:	b9 08 00 00 00       	mov    ecx,0x8
  4009a6:	48 89 c2             	mov    rdx,rax
  4009a9:	48 c1 ea 03          	shr    rdx,0x3
  4009ad:	48 81 c2 00 80 ff 7f 	add    rdx,0x7fff8000
  4009b4:	0f b6 12             	movzx  edx,BYTE PTR [rdx]
  4009b7:	84 d2                	test   dl,dl
  4009b9:	40 0f 95 c6          	setne  sil
  4009bd:	49 89 c0             	mov    r8,rax
  4009c0:	41 83 e0 07          	and    r8d,0x7
  4009c4:	41 38 d0             	cmp    r8b,dl
  4009c7:	0f 9d c2             	setge  dl
  4009ca:	21 d6                	and    esi,edx
  4009cc:	41 89 f0             	mov    r8d,esi
  4009cf:	48 8d 51 ff          	lea    rdx,[rcx-0x1]
  4009d3:	48 8d 34 10          	lea    rsi,[rax+rdx*1]
  4009d7:	48 89 f2             	mov    rdx,rsi
  4009da:	48 c1 ea 03          	shr    rdx,0x3
  4009de:	48 81 c2 00 80 ff 7f 	add    rdx,0x7fff8000
  4009e5:	0f b6 12             	movzx  edx,BYTE PTR [rdx]
  4009e8:	84 d2                	test   dl,dl
  4009ea:	41 0f 95 c1          	setne  r9b
  4009ee:	83 e6 07             	and    esi,0x7
  4009f1:	40 38 d6             	cmp    sil,dl
  4009f4:	0f 9d c2             	setge  dl
  4009f7:	44 21 ca             	and    edx,r9d
  4009fa:	44 09 c2             	or     edx,r8d
  4009fd:	84 d2                	test   dl,dl
  4009ff:	74 0b                	je     400a0c <call_gbo+0xe6>
  400a01:	48 89 ce             	mov    rsi,rcx
  400a04:	48 89 c7             	mov    rdi,rax
  400a07:	e8 d4 fd ff ff       	call   4007e0 <__asan_report_load_n@plt>
  400a0c:	48 8d 47 c0          	lea    rax,[rdi-0x40]
  400a10:	b9 08 00 00 00       	mov    ecx,0x8
  400a15:	48 89 c2             	mov    rdx,rax
  400a18:	48 c1 ea 03          	shr    rdx,0x3
  400a1c:	48 81 c2 00 80 ff 7f 	add    rdx,0x7fff8000
  400a23:	0f b6 12             	movzx  edx,BYTE PTR [rdx]
  400a26:	84 d2                	test   dl,dl
  400a28:	40 0f 95 c6          	setne  sil
  400a2c:	49 89 c0             	mov    r8,rax
  400a2f:	41 83 e0 07          	and    r8d,0x7
  400a33:	41 38 d0             	cmp    r8b,dl
  400a36:	0f 9d c2             	setge  dl
  400a39:	21 d6                	and    esi,edx
  400a3b:	41 89 f0             	mov    r8d,esi
  400a3e:	48 8d 51 ff          	lea    rdx,[rcx-0x1]
  400a42:	48 8d 34 10          	lea    rsi,[rax+rdx*1]
  400a46:	48 89 f2             	mov    rdx,rsi
  400a49:	48 c1 ea 03          	shr    rdx,0x3
  400a4d:	48 81 c2 00 80 ff 7f 	add    rdx,0x7fff8000
  400a54:	0f b6 12             	movzx  edx,BYTE PTR [rdx]
  400a57:	84 d2                	test   dl,dl
  400a59:	41 0f 95 c1          	setne  r9b
  400a5d:	83 e6 07             	and    esi,0x7
  400a60:	40 38 d6             	cmp    sil,dl
  400a63:	0f 9d c2             	setge  dl
  400a66:	44 21 ca             	and    edx,r9d
  400a69:	44 09 c2             	or     edx,r8d
  400a6c:	84 d2                	test   dl,dl
  400a6e:	74 0b                	je     400a7b <call_gbo+0x155>
  400a70:	48 89 ce             	mov    rsi,rcx
  400a73:	48 89 c7             	mov    rdi,rax
  400a76:	e8 75 fd ff ff       	call   4007f0 <__asan_report_store_n@plt>
  400a7b:	48 8d 47 c0          	lea    rax,[rdi-0x40]
  400a7f:	ba 08 00 00 00       	mov    edx,0x8
  400a84:	be 00 13 60 00       	mov    esi,0x601300
  400a89:	48 89 c7             	mov    rdi,rax
  400a8c:	e8 0f fd ff ff       	call   4007a0 <memcpy@plt>
/home/kyoupark/git/kb/asan/gbo.c:13
  400a91:	90                   	nop
/home/kyoupark/git/kb/asan/gbo.c:10
  400a92:	49 39 dd             	cmp    r13,rbx
  400a95:	74 27                	je     400abe <call_gbo+0x198>
  400a97:	48 c7 03 0e 36 e0 45 	mov    QWORD PTR [rbx],0x45e0360e
  400a9e:	48 b8 f5 f5 f5 f5 f5 	movabs rax,0xf5f5f5f5f5f5f5f5
  400aa5:	f5 f5 f5 
  400aa8:	49 89 84 24 00 80 ff 	mov    QWORD PTR [r12+0x7fff8000],rax
  400aaf:	7f 
  400ab0:	41 c7 84 24 08 80 ff 	mov    DWORD PTR [r12+0x7fff8008],0xf5f5f5f5
  400ab7:	7f f5 f5 f5 f5 
  400abc:	eb 18                	jmp    400ad6 <call_gbo+0x1b0>
  400abe:	49 c7 84 24 00 80 ff 	mov    QWORD PTR [r12+0x7fff8000],0x0
  400ac5:	7f 00 00 00 00 
  400aca:	41 c7 84 24 08 80 ff 	mov    DWORD PTR [r12+0x7fff8008],0x0
  400ad1:	7f 00 00 00 00 
/home/kyoupark/git/kb/asan/gbo.c:14
  400ad6:	48 83 c4 68          	add    rsp,0x68
  400ada:	5b                   	pop    rbx
  400adb:	41 5c                	pop    r12
  400add:	41 5d                	pop    r13
  400adf:	5d                   	pop    rbp
  400ae0:	c3                   	ret    

0000000000400ae1 <main>:
main():
/home/kyoupark/git/kb/asan/gbo.c:17
  400ae1:	55                   	push   rbp
  400ae2:	48 89 e5             	mov    rbp,rsp
  400ae5:	48 83 ec 10          	sub    rsp,0x10
  400ae9:	89 7d fc             	mov    DWORD PTR [rbp-0x4],edi
  400aec:	48 89 75 f0          	mov    QWORD PTR [rbp-0x10],rsi
/home/kyoupark/git/kb/asan/gbo.c:18
  400af0:	bf 20 0c 40 00       	mov    edi,0x400c20
  400af5:	e8 c6 fc ff ff       	call   4007c0 <puts@plt>
/home/kyoupark/git/kb/asan/gbo.c:19
  400afa:	bf 60 0c 40 00       	mov    edi,0x400c60
  400aff:	e8 bc fc ff ff       	call   4007c0 <puts@plt>
/home/kyoupark/git/kb/asan/gbo.c:20
  400b04:	bf c0 0c 40 00       	mov    edi,0x400cc0
  400b09:	e8 b2 fc ff ff       	call   4007c0 <puts@plt>
/home/kyoupark/git/kb/asan/gbo.c:22
  400b0e:	b8 00 00 00 00       	mov    eax,0x0
  400b13:	e8 0e fe ff ff       	call   400926 <call_gbo>
/home/kyoupark/git/kb/asan/gbo.c:24
  400b18:	bf 00 0d 40 00       	mov    edi,0x400d00
  400b1d:	e8 9e fc ff ff       	call   4007c0 <puts@plt>
/home/kyoupark/git/kb/asan/gbo.c:26
  400b22:	b8 00 00 00 00       	mov    eax,0x0
/home/kyoupark/git/kb/asan/gbo.c:27
  400b27:	c9                   	leave  
  400b28:	c3                   	ret    

0000000000400b29 <_GLOBAL__sub_D_00099_0_call_gbo>:
_GLOBAL__sub_D_00099_0_call_gbo():
  400b29:	55                   	push   rbp
  400b2a:	48 89 e5             	mov    rbp,rsp
  400b2d:	be 05 00 00 00       	mov    esi,0x5
  400b32:	bf 40 13 60 00       	mov    edi,0x601340
  400b37:	e8 d4 fc ff ff       	call   400810 <__asan_unregister_globals@plt>
  400b3c:	5d                   	pop    rbp
  400b3d:	c3                   	ret    

0000000000400b3e <_GLOBAL__sub_I_00099_1_call_gbo>:
_GLOBAL__sub_I_00099_1_call_gbo():
  400b3e:	55                   	push   rbp
  400b3f:	48 89 e5             	mov    rbp,rsp
  400b42:	e8 49 fc ff ff       	call   400790 <__asan_init_v3@plt>
  400b47:	be 05 00 00 00       	mov    esi,0x5
  400b4c:	bf 40 13 60 00       	mov    edi,0x601340
  400b51:	e8 ca fc ff ff       	call   400820 <__asan_register_globals@plt>
  400b56:	5d                   	pop    rbp
  400b57:	c3                   	ret    
  400b58:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
  400b5f:	00 

0000000000400b60 <__libc_csu_init>:
__libc_csu_init():
  400b60:	41 57                	push   r15
  400b62:	41 89 ff             	mov    r15d,edi
  400b65:	41 56                	push   r14
  400b67:	49 89 f6             	mov    r14,rsi
  400b6a:	41 55                	push   r13
  400b6c:	49 89 d5             	mov    r13,rdx
  400b6f:	41 54                	push   r12
  400b71:	4c 8d 25 90 04 20 00 	lea    r12,[rip+0x200490]        # 601008 <__init_array_start>
  400b78:	55                   	push   rbp
  400b79:	48 8d 2d 98 04 20 00 	lea    rbp,[rip+0x200498]        # 601018 <__init_array_end>
  400b80:	53                   	push   rbx
  400b81:	4c 29 e5             	sub    rbp,r12
  400b84:	31 db                	xor    ebx,ebx
  400b86:	48 c1 fd 03          	sar    rbp,0x3
  400b8a:	48 83 ec 08          	sub    rsp,0x8
  400b8e:	e8 c5 fb ff ff       	call   400758 <_init>
  400b93:	48 85 ed             	test   rbp,rbp
  400b96:	74 1e                	je     400bb6 <__libc_csu_init+0x56>
  400b98:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
  400b9f:	00 
  400ba0:	4c 89 ea             	mov    rdx,r13
  400ba3:	4c 89 f6             	mov    rsi,r14
  400ba6:	44 89 ff             	mov    edi,r15d
  400ba9:	41 ff 14 dc          	call   QWORD PTR [r12+rbx*8]
  400bad:	48 83 c3 01          	add    rbx,0x1
  400bb1:	48 39 eb             	cmp    rbx,rbp
  400bb4:	75 ea                	jne    400ba0 <__libc_csu_init+0x40>
  400bb6:	48 83 c4 08          	add    rsp,0x8
  400bba:	5b                   	pop    rbx
  400bbb:	5d                   	pop    rbp
  400bbc:	41 5c                	pop    r12
  400bbe:	41 5d                	pop    r13
  400bc0:	41 5e                	pop    r14
  400bc2:	41 5f                	pop    r15
  400bc4:	c3                   	ret    
  400bc5:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
  400bcc:	00 00 00 00 

0000000000400bd0 <__libc_csu_fini>:
__libc_csu_fini():
  400bd0:	f3 c3                	repz ret 

Disassembly of section .fini:

0000000000400bd4 <_fini>:
_fini():
  400bd4:	48 83 ec 08          	sub    rsp,0x8
  400bd8:	48 83 c4 08          	add    rsp,0x8
  400bdc:	c3                   	ret    
