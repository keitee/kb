
gbo_host_asan_32:     file format elf64-x86-64


Disassembly of section .init:

00000000004008e0 <_init>:
_init():
  4008e0:	48 83 ec 08          	sub    rsp,0x8
  4008e4:	48 8b 05 55 0e 20 00 	mov    rax,QWORD PTR [rip+0x200e55]        # 601740 <_DYNAMIC+0x200>
  4008eb:	48 85 c0             	test   rax,rax
  4008ee:	74 05                	je     4008f5 <_init+0x15>
  4008f0:	e8 5b 00 00 00       	call   400950 <__gmon_start__@plt>
  4008f5:	48 83 c4 08          	add    rsp,0x8
  4008f9:	c3                   	ret    

Disassembly of section .plt:

0000000000400900 <printf@plt-0x10>:
  400900:	ff 35 4a 0e 20 00    	push   QWORD PTR [rip+0x200e4a]        # 601750 <_GLOBAL_OFFSET_TABLE_+0x8>
  400906:	ff 25 4c 0e 20 00    	jmp    QWORD PTR [rip+0x200e4c]        # 601758 <_GLOBAL_OFFSET_TABLE_+0x10>
  40090c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]

0000000000400910 <printf@plt>:
  400910:	ff 25 4a 0e 20 00    	jmp    QWORD PTR [rip+0x200e4a]        # 601760 <_GLOBAL_OFFSET_TABLE_+0x18>
  400916:	68 00 00 00 00       	push   0x0
  40091b:	e9 e0 ff ff ff       	jmp    400900 <_init+0x20>

0000000000400920 <__asan_init_v3@plt>:
  400920:	ff 25 42 0e 20 00    	jmp    QWORD PTR [rip+0x200e42]        # 601768 <_GLOBAL_OFFSET_TABLE_+0x20>
  400926:	68 01 00 00 00       	push   0x1
  40092b:	e9 d0 ff ff ff       	jmp    400900 <_init+0x20>

0000000000400930 <memcpy@plt>:
  400930:	ff 25 3a 0e 20 00    	jmp    QWORD PTR [rip+0x200e3a]        # 601770 <_GLOBAL_OFFSET_TABLE_+0x28>
  400936:	68 02 00 00 00       	push   0x2
  40093b:	e9 c0 ff ff ff       	jmp    400900 <_init+0x20>

0000000000400940 <__asan_report_load8@plt>:
  400940:	ff 25 32 0e 20 00    	jmp    QWORD PTR [rip+0x200e32]        # 601778 <_GLOBAL_OFFSET_TABLE_+0x30>
  400946:	68 03 00 00 00       	push   0x3
  40094b:	e9 b0 ff ff ff       	jmp    400900 <_init+0x20>

0000000000400950 <__gmon_start__@plt>:
  400950:	ff 25 2a 0e 20 00    	jmp    QWORD PTR [rip+0x200e2a]        # 601780 <_GLOBAL_OFFSET_TABLE_+0x38>
  400956:	68 04 00 00 00       	push   0x4
  40095b:	e9 a0 ff ff ff       	jmp    400900 <_init+0x20>

0000000000400960 <puts@plt>:
  400960:	ff 25 22 0e 20 00    	jmp    QWORD PTR [rip+0x200e22]        # 601788 <_GLOBAL_OFFSET_TABLE_+0x40>
  400966:	68 05 00 00 00       	push   0x5
  40096b:	e9 90 ff ff ff       	jmp    400900 <_init+0x20>

0000000000400970 <__asan_stack_malloc_1@plt>:
  400970:	ff 25 1a 0e 20 00    	jmp    QWORD PTR [rip+0x200e1a]        # 601790 <_GLOBAL_OFFSET_TABLE_+0x48>
  400976:	68 06 00 00 00       	push   0x6
  40097b:	e9 80 ff ff ff       	jmp    400900 <_init+0x20>

0000000000400980 <putchar@plt>:
  400980:	ff 25 12 0e 20 00    	jmp    QWORD PTR [rip+0x200e12]        # 601798 <_GLOBAL_OFFSET_TABLE_+0x50>
  400986:	68 07 00 00 00       	push   0x7
  40098b:	e9 70 ff ff ff       	jmp    400900 <_init+0x20>

0000000000400990 <__asan_report_load4@plt>:
  400990:	ff 25 0a 0e 20 00    	jmp    QWORD PTR [rip+0x200e0a]        # 6017a0 <_GLOBAL_OFFSET_TABLE_+0x58>
  400996:	68 08 00 00 00       	push   0x8
  40099b:	e9 60 ff ff ff       	jmp    400900 <_init+0x20>

00000000004009a0 <__asan_report_load_n@plt>:
  4009a0:	ff 25 02 0e 20 00    	jmp    QWORD PTR [rip+0x200e02]        # 6017a8 <_GLOBAL_OFFSET_TABLE_+0x60>
  4009a6:	68 09 00 00 00       	push   0x9
  4009ab:	e9 50 ff ff ff       	jmp    400900 <_init+0x20>

00000000004009b0 <__asan_report_store_n@plt>:
  4009b0:	ff 25 fa 0d 20 00    	jmp    QWORD PTR [rip+0x200dfa]        # 6017b0 <_GLOBAL_OFFSET_TABLE_+0x68>
  4009b6:	68 0a 00 00 00       	push   0xa
  4009bb:	e9 40 ff ff ff       	jmp    400900 <_init+0x20>

00000000004009c0 <__libc_start_main@plt>:
  4009c0:	ff 25 f2 0d 20 00    	jmp    QWORD PTR [rip+0x200df2]        # 6017b8 <_GLOBAL_OFFSET_TABLE_+0x70>
  4009c6:	68 0b 00 00 00       	push   0xb
  4009cb:	e9 30 ff ff ff       	jmp    400900 <_init+0x20>

00000000004009d0 <dl_iterate_phdr@plt>:
  4009d0:	ff 25 ea 0d 20 00    	jmp    QWORD PTR [rip+0x200dea]        # 6017c0 <_GLOBAL_OFFSET_TABLE_+0x78>
  4009d6:	68 0c 00 00 00       	push   0xc
  4009db:	e9 20 ff ff ff       	jmp    400900 <_init+0x20>

00000000004009e0 <__asan_report_load2@plt>:
  4009e0:	ff 25 e2 0d 20 00    	jmp    QWORD PTR [rip+0x200de2]        # 6017c8 <_GLOBAL_OFFSET_TABLE_+0x80>
  4009e6:	68 0d 00 00 00       	push   0xd
  4009eb:	e9 10 ff ff ff       	jmp    400900 <_init+0x20>

00000000004009f0 <__asan_unregister_globals@plt>:
  4009f0:	ff 25 da 0d 20 00    	jmp    QWORD PTR [rip+0x200dda]        # 6017d0 <_GLOBAL_OFFSET_TABLE_+0x88>
  4009f6:	68 0e 00 00 00       	push   0xe
  4009fb:	e9 00 ff ff ff       	jmp    400900 <_init+0x20>

0000000000400a00 <__asan_register_globals@plt>:
  400a00:	ff 25 d2 0d 20 00    	jmp    QWORD PTR [rip+0x200dd2]        # 6017d8 <_GLOBAL_OFFSET_TABLE_+0x90>
  400a06:	68 0f 00 00 00       	push   0xf
  400a0b:	e9 f0 fe ff ff       	jmp    400900 <_init+0x20>

Disassembly of section .text:

0000000000400a10 <_start>:
_start():
  400a10:	31 ed                	xor    ebp,ebp
  400a12:	49 89 d1             	mov    r9,rdx
  400a15:	5e                   	pop    rsi
  400a16:	48 89 e2             	mov    rdx,rsp
  400a19:	48 83 e4 f0          	and    rsp,0xfffffffffffffff0
  400a1d:	50                   	push   rax
  400a1e:	54                   	push   rsp
  400a1f:	49 c7 c0 a0 10 40 00 	mov    r8,0x4010a0
  400a26:	48 c7 c1 30 10 40 00 	mov    rcx,0x401030
  400a2d:	48 c7 c7 a0 0f 40 00 	mov    rdi,0x400fa0
  400a34:	e8 87 ff ff ff       	call   4009c0 <__libc_start_main@plt>
  400a39:	f4                   	hlt    
  400a3a:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]

0000000000400a40 <deregister_tm_clones>:
deregister_tm_clones():
  400a40:	b8 07 1a 60 00       	mov    eax,0x601a07
  400a45:	55                   	push   rbp
  400a46:	48 2d 00 1a 60 00    	sub    rax,0x601a00
  400a4c:	48 83 f8 0e          	cmp    rax,0xe
  400a50:	48 89 e5             	mov    rbp,rsp
  400a53:	76 1b                	jbe    400a70 <deregister_tm_clones+0x30>
  400a55:	b8 00 00 00 00       	mov    eax,0x0
  400a5a:	48 85 c0             	test   rax,rax
  400a5d:	74 11                	je     400a70 <deregister_tm_clones+0x30>
  400a5f:	5d                   	pop    rbp
  400a60:	bf 00 1a 60 00       	mov    edi,0x601a00
  400a65:	ff e0                	jmp    rax
  400a67:	66 0f 1f 84 00 00 00 	nop    WORD PTR [rax+rax*1+0x0]
  400a6e:	00 00 
  400a70:	5d                   	pop    rbp
  400a71:	c3                   	ret    
  400a72:	66 66 66 66 66 2e 0f 	data16 data16 data16 data16 nop WORD PTR cs:[rax+rax*1+0x0]
  400a79:	1f 84 00 00 00 00 00 

0000000000400a80 <register_tm_clones>:
register_tm_clones():
  400a80:	be 00 1a 60 00       	mov    esi,0x601a00
  400a85:	55                   	push   rbp
  400a86:	48 81 ee 00 1a 60 00 	sub    rsi,0x601a00
  400a8d:	48 c1 fe 03          	sar    rsi,0x3
  400a91:	48 89 e5             	mov    rbp,rsp
  400a94:	48 89 f0             	mov    rax,rsi
  400a97:	48 c1 e8 3f          	shr    rax,0x3f
  400a9b:	48 01 c6             	add    rsi,rax
  400a9e:	48 d1 fe             	sar    rsi,1
  400aa1:	74 15                	je     400ab8 <register_tm_clones+0x38>
  400aa3:	b8 00 00 00 00       	mov    eax,0x0
  400aa8:	48 85 c0             	test   rax,rax
  400aab:	74 0b                	je     400ab8 <register_tm_clones+0x38>
  400aad:	5d                   	pop    rbp
  400aae:	bf 00 1a 60 00       	mov    edi,0x601a00
  400ab3:	ff e0                	jmp    rax
  400ab5:	0f 1f 00             	nop    DWORD PTR [rax]
  400ab8:	5d                   	pop    rbp
  400ab9:	c3                   	ret    
  400aba:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]

0000000000400ac0 <__do_global_dtors_aux>:
__do_global_dtors_aux():
  400ac0:	80 3d 3d 0f 20 00 00 	cmp    BYTE PTR [rip+0x200f3d],0x0        # 601a04 <completed.6661>
  400ac7:	75 11                	jne    400ada <__do_global_dtors_aux+0x1a>
  400ac9:	55                   	push   rbp
  400aca:	48 89 e5             	mov    rbp,rsp
  400acd:	e8 6e ff ff ff       	call   400a40 <deregister_tm_clones>
  400ad2:	5d                   	pop    rbp
  400ad3:	c6 05 2a 0f 20 00 01 	mov    BYTE PTR [rip+0x200f2a],0x1        # 601a04 <completed.6661>
  400ada:	f3 c3                	repz ret 
  400adc:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]

0000000000400ae0 <frame_dummy>:
frame_dummy():
  400ae0:	bf 38 15 60 00       	mov    edi,0x601538
  400ae5:	48 83 3f 00          	cmp    QWORD PTR [rdi],0x0
  400ae9:	75 05                	jne    400af0 <frame_dummy+0x10>
  400aeb:	eb 93                	jmp    400a80 <register_tm_clones>
  400aed:	0f 1f 00             	nop    DWORD PTR [rax]
  400af0:	b8 00 00 00 00       	mov    eax,0x0
  400af5:	48 85 c0             	test   rax,rax
  400af8:	74 f1                	je     400aeb <frame_dummy+0xb>
  400afa:	55                   	push   rbp
  400afb:	48 89 e5             	mov    rbp,rsp
  400afe:	ff d0                	call   rax
  400b00:	5d                   	pop    rbp
  400b01:	e9 7a ff ff ff       	jmp    400a80 <register_tm_clones>

0000000000400b06 <header_handler>:
header_handler():
/mnt/git/kb/asan/gbo.c:114
  400b06:	55                   	push   rbp
  400b07:	48 89 e5             	mov    rbp,rsp
  400b0a:	48 83 ec 30          	sub    rsp,0x30
  400b0e:	48 89 7d e8          	mov    QWORD PTR [rbp-0x18],rdi
  400b12:	48 89 75 e0          	mov    QWORD PTR [rbp-0x20],rsi
  400b16:	48 89 55 d8          	mov    QWORD PTR [rbp-0x28],rdx
/mnt/git/kb/asan/gbo.c:115
  400b1a:	c7 45 fc 00 00 00 00 	mov    DWORD PTR [rbp-0x4],0x0
/mnt/git/kb/asan/gbo.c:117
  400b21:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  400b25:	48 89 c2             	mov    rdx,rax
  400b28:	48 c1 ea 03          	shr    rdx,0x3
  400b2c:	48 81 c2 00 80 ff 7f 	add    rdx,0x7fff8000
  400b33:	0f b6 12             	movzx  edx,BYTE PTR [rdx]
  400b36:	84 d2                	test   dl,dl
  400b38:	74 08                	je     400b42 <header_handler+0x3c>
  400b3a:	48 89 c7             	mov    rdi,rax
  400b3d:	e8 fe fd ff ff       	call   400940 <__asan_report_load8@plt>
  400b42:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  400b46:	48 8b 00             	mov    rax,QWORD PTR [rax]
/mnt/git/kb/asan/gbo.c:116
  400b49:	48 89 c7             	mov    rdi,rax
/mnt/git/kb/asan/gbo.c:117
  400b4c:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  400b50:	48 83 c0 18          	add    rax,0x18
  400b54:	48 89 c2             	mov    rdx,rax
  400b57:	48 c1 ea 03          	shr    rdx,0x3
  400b5b:	48 81 c2 00 80 ff 7f 	add    rdx,0x7fff8000
  400b62:	0f b6 12             	movzx  edx,BYTE PTR [rdx]
  400b65:	84 d2                	test   dl,dl
  400b67:	0f 95 c1             	setne  cl
  400b6a:	48 89 c6             	mov    rsi,rax
  400b6d:	83 e6 07             	and    esi,0x7
  400b70:	83 c6 01             	add    esi,0x1
  400b73:	40 38 d6             	cmp    sil,dl
  400b76:	0f 9d c2             	setge  dl
  400b79:	21 ca                	and    edx,ecx
  400b7b:	84 d2                	test   dl,dl
  400b7d:	74 08                	je     400b87 <header_handler+0x81>
  400b7f:	48 89 c7             	mov    rdi,rax
  400b82:	e8 59 fe ff ff       	call   4009e0 <__asan_report_load2@plt>
  400b87:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  400b8b:	0f b7 40 18          	movzx  eax,WORD PTR [rax+0x18]
/mnt/git/kb/asan/gbo.c:116
  400b8f:	0f b7 d0             	movzx  edx,ax
  400b92:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  400b96:	48 83 c0 08          	add    rax,0x8
  400b9a:	48 89 c1             	mov    rcx,rax
  400b9d:	48 c1 e9 03          	shr    rcx,0x3
  400ba1:	48 81 c1 00 80 ff 7f 	add    rcx,0x7fff8000
  400ba8:	0f b6 09             	movzx  ecx,BYTE PTR [rcx]
  400bab:	84 c9                	test   cl,cl
  400bad:	74 08                	je     400bb7 <header_handler+0xb1>
  400baf:	48 89 c7             	mov    rdi,rax
  400bb2:	e8 89 fd ff ff       	call   400940 <__asan_report_load8@plt>
  400bb7:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  400bbb:	48 8b 40 08          	mov    rax,QWORD PTR [rax+0x8]
  400bbf:	48 89 f9             	mov    rcx,rdi
  400bc2:	48 89 c6             	mov    rsi,rax
  400bc5:	bf e0 10 40 00       	mov    edi,0x4010e0
  400bca:	b8 00 00 00 00       	mov    eax,0x0
  400bcf:	e8 3c fd ff ff       	call   400910 <printf@plt>
/mnt/git/kb/asan/gbo.c:118
  400bd4:	c7 45 fc 00 00 00 00 	mov    DWORD PTR [rbp-0x4],0x0
  400bdb:	e9 a5 01 00 00       	jmp    400d85 <header_handler+0x27f>
/mnt/git/kb/asan/gbo.c:120 (discriminator 3)
  400be0:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  400be4:	48 89 c2             	mov    rdx,rax
  400be7:	48 c1 ea 03          	shr    rdx,0x3
  400beb:	48 81 c2 00 80 ff 7f 	add    rdx,0x7fff8000
  400bf2:	0f b6 12             	movzx  edx,BYTE PTR [rdx]
  400bf5:	84 d2                	test   dl,dl
  400bf7:	74 08                	je     400c01 <header_handler+0xfb>
/mnt/git/kb/asan/gbo.c:120
  400bf9:	48 89 c7             	mov    rdi,rax
  400bfc:	e8 3f fd ff ff       	call   400940 <__asan_report_load8@plt>
/mnt/git/kb/asan/gbo.c:120 (discriminator 3)
  400c01:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  400c05:	48 8b 30             	mov    rsi,QWORD PTR [rax]
  400c08:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  400c0c:	48 83 c0 10          	add    rax,0x10
  400c10:	48 89 c2             	mov    rdx,rax
  400c13:	48 c1 ea 03          	shr    rdx,0x3
  400c17:	48 81 c2 00 80 ff 7f 	add    rdx,0x7fff8000
  400c1e:	0f b6 12             	movzx  edx,BYTE PTR [rdx]
  400c21:	84 d2                	test   dl,dl
  400c23:	74 08                	je     400c2d <header_handler+0x127>
/mnt/git/kb/asan/gbo.c:120
  400c25:	48 89 c7             	mov    rdi,rax
  400c28:	e8 13 fd ff ff       	call   400940 <__asan_report_load8@plt>
/mnt/git/kb/asan/gbo.c:120 (discriminator 3)
  400c2d:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  400c31:	48 8b 50 10          	mov    rdx,QWORD PTR [rax+0x10]
  400c35:	8b 45 fc             	mov    eax,DWORD PTR [rbp-0x4]
  400c38:	48 98                	cdqe   
  400c3a:	48 c1 e0 03          	shl    rax,0x3
  400c3e:	48 8d 0c c5 00 00 00 	lea    rcx,[rax*8+0x0]
  400c45:	00 
  400c46:	48 29 c1             	sub    rcx,rax
  400c49:	48 89 c8             	mov    rax,rcx
  400c4c:	48 01 d0             	add    rax,rdx
  400c4f:	48 8d 50 10          	lea    rdx,[rax+0x10]
  400c53:	48 89 d1             	mov    rcx,rdx
  400c56:	48 c1 e9 03          	shr    rcx,0x3
  400c5a:	48 81 c1 00 80 ff 7f 	add    rcx,0x7fff8000
  400c61:	0f b6 09             	movzx  ecx,BYTE PTR [rcx]
  400c64:	84 c9                	test   cl,cl
  400c66:	74 08                	je     400c70 <header_handler+0x16a>
/mnt/git/kb/asan/gbo.c:120
  400c68:	48 89 d7             	mov    rdi,rdx
  400c6b:	e8 d0 fc ff ff       	call   400940 <__asan_report_load8@plt>
/mnt/git/kb/asan/gbo.c:120 (discriminator 3)
  400c70:	48 8b 40 10          	mov    rax,QWORD PTR [rax+0x10]
  400c74:	48 01 f0             	add    rax,rsi
/mnt/git/kb/asan/gbo.c:119 (discriminator 3)
  400c77:	48 89 c2             	mov    rdx,rax
  400c7a:	8b 45 fc             	mov    eax,DWORD PTR [rbp-0x4]
  400c7d:	89 c6                	mov    esi,eax
  400c7f:	bf 40 11 40 00       	mov    edi,0x401140
  400c84:	b8 00 00 00 00       	mov    eax,0x0
  400c89:	e8 82 fc ff ff       	call   400910 <printf@plt>
/mnt/git/kb/asan/gbo.c:122 (discriminator 3)
  400c8e:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  400c92:	48 83 c0 10          	add    rax,0x10
  400c96:	48 89 c2             	mov    rdx,rax
  400c99:	48 c1 ea 03          	shr    rdx,0x3
  400c9d:	48 81 c2 00 80 ff 7f 	add    rdx,0x7fff8000
  400ca4:	0f b6 12             	movzx  edx,BYTE PTR [rdx]
  400ca7:	84 d2                	test   dl,dl
  400ca9:	74 08                	je     400cb3 <header_handler+0x1ad>
/mnt/git/kb/asan/gbo.c:122
  400cab:	48 89 c7             	mov    rdi,rax
  400cae:	e8 8d fc ff ff       	call   400940 <__asan_report_load8@plt>
/mnt/git/kb/asan/gbo.c:122 (discriminator 3)
  400cb3:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  400cb7:	48 8b 50 10          	mov    rdx,QWORD PTR [rax+0x10]
  400cbb:	8b 45 fc             	mov    eax,DWORD PTR [rbp-0x4]
  400cbe:	48 98                	cdqe   
  400cc0:	48 c1 e0 03          	shl    rax,0x3
  400cc4:	48 8d 0c c5 00 00 00 	lea    rcx,[rax*8+0x0]
  400ccb:	00 
  400ccc:	48 29 c1             	sub    rcx,rax
  400ccf:	48 89 c8             	mov    rax,rcx
  400cd2:	48 01 c2             	add    rdx,rax
/mnt/git/kb/asan/gbo.c:121 (discriminator 3)
  400cd5:	48 8d 42 04          	lea    rax,[rdx+0x4]
  400cd9:	48 89 c1             	mov    rcx,rax
  400cdc:	48 c1 e9 03          	shr    rcx,0x3
  400ce0:	48 81 c1 00 80 ff 7f 	add    rcx,0x7fff8000
  400ce7:	0f b6 09             	movzx  ecx,BYTE PTR [rcx]
  400cea:	84 c9                	test   cl,cl
  400cec:	40 0f 95 c6          	setne  sil
  400cf0:	48 89 c7             	mov    rdi,rax
  400cf3:	83 e7 07             	and    edi,0x7
  400cf6:	83 c7 03             	add    edi,0x3
  400cf9:	40 38 cf             	cmp    dil,cl
  400cfc:	0f 9d c1             	setge  cl
  400cff:	21 f1                	and    ecx,esi
  400d01:	84 c9                	test   cl,cl
  400d03:	74 08                	je     400d0d <header_handler+0x207>
/mnt/git/kb/asan/gbo.c:121
  400d05:	48 89 c7             	mov    rdi,rax
  400d08:	e8 83 fc ff ff       	call   400990 <__asan_report_load4@plt>
/mnt/git/kb/asan/gbo.c:121 (discriminator 3)
  400d0d:	8b 72 04             	mov    esi,DWORD PTR [rdx+0x4]
/mnt/git/kb/asan/gbo.c:122 (discriminator 3)
  400d10:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  400d14:	48 8b 50 10          	mov    rdx,QWORD PTR [rax+0x10]
  400d18:	8b 45 fc             	mov    eax,DWORD PTR [rbp-0x4]
  400d1b:	48 98                	cdqe   
  400d1d:	48 c1 e0 03          	shl    rax,0x3
  400d21:	48 8d 0c c5 00 00 00 	lea    rcx,[rax*8+0x0]
  400d28:	00 
  400d29:	48 29 c1             	sub    rcx,rax
  400d2c:	48 89 c8             	mov    rax,rcx
  400d2f:	48 8d 0c 02          	lea    rcx,[rdx+rax*1]
/mnt/git/kb/asan/gbo.c:121 (discriminator 3)
  400d33:	48 89 c8             	mov    rax,rcx
  400d36:	48 89 c2             	mov    rdx,rax
  400d39:	48 c1 ea 03          	shr    rdx,0x3
  400d3d:	48 81 c2 00 80 ff 7f 	add    rdx,0x7fff8000
  400d44:	0f b6 12             	movzx  edx,BYTE PTR [rdx]
  400d47:	84 d2                	test   dl,dl
  400d49:	40 0f 95 c7          	setne  dil
  400d4d:	49 89 c0             	mov    r8,rax
  400d50:	41 83 e0 07          	and    r8d,0x7
  400d54:	41 83 c0 03          	add    r8d,0x3
  400d58:	41 38 d0             	cmp    r8b,dl
  400d5b:	0f 9d c2             	setge  dl
  400d5e:	21 fa                	and    edx,edi
  400d60:	84 d2                	test   dl,dl
  400d62:	74 08                	je     400d6c <header_handler+0x266>
/mnt/git/kb/asan/gbo.c:121
  400d64:	48 89 c7             	mov    rdi,rax
  400d67:	e8 24 fc ff ff       	call   400990 <__asan_report_load4@plt>
/mnt/git/kb/asan/gbo.c:121 (discriminator 3)
  400d6c:	8b 01                	mov    eax,DWORD PTR [rcx]
  400d6e:	89 f2                	mov    edx,esi
  400d70:	89 c6                	mov    esi,eax
  400d72:	bf 80 11 40 00       	mov    edi,0x401180
  400d77:	b8 00 00 00 00       	mov    eax,0x0
  400d7c:	e8 8f fb ff ff       	call   400910 <printf@plt>
/mnt/git/kb/asan/gbo.c:118 (discriminator 3)
  400d81:	83 45 fc 01          	add    DWORD PTR [rbp-0x4],0x1
/mnt/git/kb/asan/gbo.c:118 (discriminator 1)
  400d85:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  400d89:	48 83 c0 18          	add    rax,0x18
  400d8d:	48 89 c2             	mov    rdx,rax
  400d90:	48 c1 ea 03          	shr    rdx,0x3
  400d94:	48 81 c2 00 80 ff 7f 	add    rdx,0x7fff8000
  400d9b:	0f b6 12             	movzx  edx,BYTE PTR [rdx]
  400d9e:	84 d2                	test   dl,dl
  400da0:	0f 95 c1             	setne  cl
  400da3:	48 89 c6             	mov    rsi,rax
  400da6:	83 e6 07             	and    esi,0x7
  400da9:	83 c6 01             	add    esi,0x1
  400dac:	40 38 d6             	cmp    sil,dl
  400daf:	0f 9d c2             	setge  dl
  400db2:	21 ca                	and    edx,ecx
  400db4:	84 d2                	test   dl,dl
  400db6:	74 08                	je     400dc0 <header_handler+0x2ba>
/mnt/git/kb/asan/gbo.c:118
  400db8:	48 89 c7             	mov    rdi,rax
  400dbb:	e8 20 fc ff ff       	call   4009e0 <__asan_report_load2@plt>
/mnt/git/kb/asan/gbo.c:118 (discriminator 1)
  400dc0:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  400dc4:	0f b7 40 18          	movzx  eax,WORD PTR [rax+0x18]
  400dc8:	0f b7 c0             	movzx  eax,ax
  400dcb:	3b 45 fc             	cmp    eax,DWORD PTR [rbp-0x4]
  400dce:	0f 8f 0c fe ff ff    	jg     400be0 <header_handler+0xda>
/mnt/git/kb/asan/gbo.c:124
  400dd4:	bf 0a 00 00 00       	mov    edi,0xa
  400dd9:	e8 a2 fb ff ff       	call   400980 <putchar@plt>
/mnt/git/kb/asan/gbo.c:125
  400dde:	b8 00 00 00 00       	mov    eax,0x0
/mnt/git/kb/asan/gbo.c:126
  400de3:	c9                   	leave  
  400de4:	c3                   	ret    

0000000000400de5 <call_gbo>:
call_gbo():
/mnt/git/kb/asan/gbo.c:131
  400de5:	55                   	push   rbp
  400de6:	48 89 e5             	mov    rbp,rsp
  400de9:	41 55                	push   r13
  400deb:	41 54                	push   r12
  400ded:	53                   	push   rbx
  400dee:	48 83 ec 68          	sub    rsp,0x68
  400df2:	48 8d 5d 80          	lea    rbx,[rbp-0x80]
  400df6:	49 89 dd             	mov    r13,rbx
  400df9:	83 3d 00 0c 20 00 00 	cmp    DWORD PTR [rip+0x200c00],0x0        # 601a00 <__TMC_END__>
  400e00:	74 10                	je     400e12 <call_gbo+0x2d>
  400e02:	48 89 de             	mov    rsi,rbx
  400e05:	bf 60 00 00 00       	mov    edi,0x60
  400e0a:	e8 61 fb ff ff       	call   400970 <__asan_stack_malloc_1@plt>
  400e0f:	48 89 c3             	mov    rbx,rax
  400e12:	48 8d 43 60          	lea    rax,[rbx+0x60]
  400e16:	48 89 c7             	mov    rdi,rax
  400e19:	48 c7 03 b3 8a b5 41 	mov    QWORD PTR [rbx],0x41b58ab3
  400e20:	48 c7 43 08 c0 11 40 	mov    QWORD PTR [rbx+0x8],0x4011c0
  400e27:	00 
  400e28:	48 c7 43 10 e5 0d 40 	mov    QWORD PTR [rbx+0x10],0x400de5
  400e2f:	00 
  400e30:	49 89 dc             	mov    r12,rbx
  400e33:	49 c1 ec 03          	shr    r12,0x3
  400e37:	41 c7 84 24 00 80 ff 	mov    DWORD PTR [r12+0x7fff8000],0xf1f1f1f1
  400e3e:	7f f1 f1 f1 f1 
  400e43:	41 c7 84 24 04 80 ff 	mov    DWORD PTR [r12+0x7fff8004],0xf4f40200
  400e4a:	7f 00 02 f4 f4 
  400e4f:	41 c7 84 24 08 80 ff 	mov    DWORD PTR [r12+0x7fff8008],0xf3f3f3f3
  400e56:	7f f3 f3 f3 f3 
/mnt/git/kb/asan/gbo.c:133
  400e5b:	b8 40 18 60 00       	mov    eax,0x601840
  400e60:	b9 08 00 00 00       	mov    ecx,0x8
  400e65:	48 89 c2             	mov    rdx,rax
  400e68:	48 c1 ea 03          	shr    rdx,0x3
  400e6c:	48 81 c2 00 80 ff 7f 	add    rdx,0x7fff8000
  400e73:	0f b6 12             	movzx  edx,BYTE PTR [rdx]
  400e76:	84 d2                	test   dl,dl
  400e78:	40 0f 95 c6          	setne  sil
  400e7c:	49 89 c0             	mov    r8,rax
  400e7f:	41 83 e0 07          	and    r8d,0x7
  400e83:	41 38 d0             	cmp    r8b,dl
  400e86:	0f 9d c2             	setge  dl
  400e89:	21 d6                	and    esi,edx
  400e8b:	41 89 f0             	mov    r8d,esi
  400e8e:	48 8d 51 ff          	lea    rdx,[rcx-0x1]
  400e92:	48 8d 34 10          	lea    rsi,[rax+rdx*1]
  400e96:	48 89 f2             	mov    rdx,rsi
  400e99:	48 c1 ea 03          	shr    rdx,0x3
  400e9d:	48 81 c2 00 80 ff 7f 	add    rdx,0x7fff8000
  400ea4:	0f b6 12             	movzx  edx,BYTE PTR [rdx]
  400ea7:	84 d2                	test   dl,dl
  400ea9:	41 0f 95 c1          	setne  r9b
  400ead:	83 e6 07             	and    esi,0x7
  400eb0:	40 38 d6             	cmp    sil,dl
  400eb3:	0f 9d c2             	setge  dl
  400eb6:	44 21 ca             	and    edx,r9d
  400eb9:	44 09 c2             	or     edx,r8d
  400ebc:	84 d2                	test   dl,dl
  400ebe:	74 0b                	je     400ecb <call_gbo+0xe6>
  400ec0:	48 89 ce             	mov    rsi,rcx
  400ec3:	48 89 c7             	mov    rdi,rax
  400ec6:	e8 d5 fa ff ff       	call   4009a0 <__asan_report_load_n@plt>
  400ecb:	48 8d 47 c0          	lea    rax,[rdi-0x40]
  400ecf:	b9 08 00 00 00       	mov    ecx,0x8
  400ed4:	48 89 c2             	mov    rdx,rax
  400ed7:	48 c1 ea 03          	shr    rdx,0x3
  400edb:	48 81 c2 00 80 ff 7f 	add    rdx,0x7fff8000
  400ee2:	0f b6 12             	movzx  edx,BYTE PTR [rdx]
  400ee5:	84 d2                	test   dl,dl
  400ee7:	40 0f 95 c6          	setne  sil
  400eeb:	49 89 c0             	mov    r8,rax
  400eee:	41 83 e0 07          	and    r8d,0x7
  400ef2:	41 38 d0             	cmp    r8b,dl
  400ef5:	0f 9d c2             	setge  dl
  400ef8:	21 d6                	and    esi,edx
  400efa:	41 89 f0             	mov    r8d,esi
  400efd:	48 8d 51 ff          	lea    rdx,[rcx-0x1]
  400f01:	48 8d 34 10          	lea    rsi,[rax+rdx*1]
  400f05:	48 89 f2             	mov    rdx,rsi
  400f08:	48 c1 ea 03          	shr    rdx,0x3
  400f0c:	48 81 c2 00 80 ff 7f 	add    rdx,0x7fff8000
  400f13:	0f b6 12             	movzx  edx,BYTE PTR [rdx]
  400f16:	84 d2                	test   dl,dl
  400f18:	41 0f 95 c1          	setne  r9b
  400f1c:	83 e6 07             	and    esi,0x7
  400f1f:	40 38 d6             	cmp    sil,dl
  400f22:	0f 9d c2             	setge  dl
  400f25:	44 21 ca             	and    edx,r9d
  400f28:	44 09 c2             	or     edx,r8d
  400f2b:	84 d2                	test   dl,dl
  400f2d:	74 0b                	je     400f3a <call_gbo+0x155>
  400f2f:	48 89 ce             	mov    rsi,rcx
  400f32:	48 89 c7             	mov    rdi,rax
  400f35:	e8 76 fa ff ff       	call   4009b0 <__asan_report_store_n@plt>
  400f3a:	48 8d 47 c0          	lea    rax,[rdi-0x40]
  400f3e:	ba 08 00 00 00       	mov    edx,0x8
  400f43:	be 40 18 60 00       	mov    esi,0x601840
  400f48:	48 89 c7             	mov    rdi,rax
  400f4b:	e8 e0 f9 ff ff       	call   400930 <memcpy@plt>
/mnt/git/kb/asan/gbo.c:134
  400f50:	90                   	nop
/mnt/git/kb/asan/gbo.c:131
  400f51:	49 39 dd             	cmp    r13,rbx
  400f54:	74 27                	je     400f7d <call_gbo+0x198>
  400f56:	48 c7 03 0e 36 e0 45 	mov    QWORD PTR [rbx],0x45e0360e
  400f5d:	48 b8 f5 f5 f5 f5 f5 	movabs rax,0xf5f5f5f5f5f5f5f5
  400f64:	f5 f5 f5 
  400f67:	49 89 84 24 00 80 ff 	mov    QWORD PTR [r12+0x7fff8000],rax
  400f6e:	7f 
  400f6f:	41 c7 84 24 08 80 ff 	mov    DWORD PTR [r12+0x7fff8008],0xf5f5f5f5
  400f76:	7f f5 f5 f5 f5 
  400f7b:	eb 18                	jmp    400f95 <call_gbo+0x1b0>
  400f7d:	49 c7 84 24 00 80 ff 	mov    QWORD PTR [r12+0x7fff8000],0x0
  400f84:	7f 00 00 00 00 
  400f89:	41 c7 84 24 08 80 ff 	mov    DWORD PTR [r12+0x7fff8008],0x0
  400f90:	7f 00 00 00 00 
/mnt/git/kb/asan/gbo.c:135
  400f95:	48 83 c4 68          	add    rsp,0x68
  400f99:	5b                   	pop    rbx
  400f9a:	41 5c                	pop    r12
  400f9c:	41 5d                	pop    r13
  400f9e:	5d                   	pop    rbp
  400f9f:	c3                   	ret    

0000000000400fa0 <main>:
main():
/mnt/git/kb/asan/gbo.c:138
  400fa0:	55                   	push   rbp
  400fa1:	48 89 e5             	mov    rbp,rsp
  400fa4:	48 83 ec 10          	sub    rsp,0x10
  400fa8:	89 7d fc             	mov    DWORD PTR [rbp-0x4],edi
  400fab:	48 89 75 f0          	mov    QWORD PTR [rbp-0x10],rsi
/mnt/git/kb/asan/gbo.c:139
  400faf:	be 00 00 00 00       	mov    esi,0x0
  400fb4:	bf 06 0b 40 00       	mov    edi,0x400b06
  400fb9:	e8 12 fa ff ff       	call   4009d0 <dl_iterate_phdr@plt>
/mnt/git/kb/asan/gbo.c:141
  400fbe:	bf e0 11 40 00       	mov    edi,0x4011e0
  400fc3:	e8 98 f9 ff ff       	call   400960 <puts@plt>
/mnt/git/kb/asan/gbo.c:142
  400fc8:	bf 20 12 40 00       	mov    edi,0x401220
  400fcd:	e8 8e f9 ff ff       	call   400960 <puts@plt>
/mnt/git/kb/asan/gbo.c:143
  400fd2:	bf 80 12 40 00       	mov    edi,0x401280
  400fd7:	e8 84 f9 ff ff       	call   400960 <puts@plt>
/mnt/git/kb/asan/gbo.c:145
  400fdc:	b8 00 00 00 00       	mov    eax,0x0
  400fe1:	e8 ff fd ff ff       	call   400de5 <call_gbo>
/mnt/git/kb/asan/gbo.c:147
  400fe6:	bf c0 12 40 00       	mov    edi,0x4012c0
  400feb:	e8 70 f9 ff ff       	call   400960 <puts@plt>
/mnt/git/kb/asan/gbo.c:149
  400ff0:	b8 00 00 00 00       	mov    eax,0x0
/mnt/git/kb/asan/gbo.c:150
  400ff5:	c9                   	leave  
  400ff6:	c3                   	ret    

0000000000400ff7 <_GLOBAL__sub_D_00099_0_call_gbo>:
_GLOBAL__sub_D_00099_0_call_gbo():
  400ff7:	55                   	push   rbp
  400ff8:	48 89 e5             	mov    rbp,rsp
  400ffb:	be 08 00 00 00       	mov    esi,0x8
  401000:	bf 80 18 60 00       	mov    edi,0x601880
  401005:	e8 e6 f9 ff ff       	call   4009f0 <__asan_unregister_globals@plt>
  40100a:	5d                   	pop    rbp
  40100b:	c3                   	ret    

000000000040100c <_GLOBAL__sub_I_00099_1_call_gbo>:
_GLOBAL__sub_I_00099_1_call_gbo():
  40100c:	55                   	push   rbp
  40100d:	48 89 e5             	mov    rbp,rsp
  401010:	e8 0b f9 ff ff       	call   400920 <__asan_init_v3@plt>
  401015:	be 08 00 00 00       	mov    esi,0x8
  40101a:	bf 80 18 60 00       	mov    edi,0x601880
  40101f:	e8 dc f9 ff ff       	call   400a00 <__asan_register_globals@plt>
  401024:	5d                   	pop    rbp
  401025:	c3                   	ret    
  401026:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
  40102d:	00 00 00 

0000000000401030 <__libc_csu_init>:
__libc_csu_init():
  401030:	41 57                	push   r15
  401032:	41 89 ff             	mov    r15d,edi
  401035:	41 56                	push   r14
  401037:	49 89 f6             	mov    r14,rsi
  40103a:	41 55                	push   r13
  40103c:	49 89 d5             	mov    r13,rdx
  40103f:	41 54                	push   r12
  401041:	4c 8d 25 d0 04 20 00 	lea    r12,[rip+0x2004d0]        # 601518 <__init_array_start>
  401048:	55                   	push   rbp
  401049:	48 8d 2d d8 04 20 00 	lea    rbp,[rip+0x2004d8]        # 601528 <__init_array_end>
  401050:	53                   	push   rbx
  401051:	4c 29 e5             	sub    rbp,r12
  401054:	31 db                	xor    ebx,ebx
  401056:	48 c1 fd 03          	sar    rbp,0x3
  40105a:	48 83 ec 08          	sub    rsp,0x8
  40105e:	e8 7d f8 ff ff       	call   4008e0 <_init>
  401063:	48 85 ed             	test   rbp,rbp
  401066:	74 1e                	je     401086 <__libc_csu_init+0x56>
  401068:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
  40106f:	00 
  401070:	4c 89 ea             	mov    rdx,r13
  401073:	4c 89 f6             	mov    rsi,r14
  401076:	44 89 ff             	mov    edi,r15d
  401079:	41 ff 14 dc          	call   QWORD PTR [r12+rbx*8]
  40107d:	48 83 c3 01          	add    rbx,0x1
  401081:	48 39 eb             	cmp    rbx,rbp
  401084:	75 ea                	jne    401070 <__libc_csu_init+0x40>
  401086:	48 83 c4 08          	add    rsp,0x8
  40108a:	5b                   	pop    rbx
  40108b:	5d                   	pop    rbp
  40108c:	41 5c                	pop    r12
  40108e:	41 5d                	pop    r13
  401090:	41 5e                	pop    r14
  401092:	41 5f                	pop    r15
  401094:	c3                   	ret    
  401095:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
  40109c:	00 00 00 00 

00000000004010a0 <__libc_csu_fini>:
__libc_csu_fini():
  4010a0:	f3 c3                	repz ret 

Disassembly of section .fini:

00000000004010a4 <_fini>:
_fini():
  4010a4:	48 83 ec 08          	sub    rsp,0x8
  4010a8:	48 83 c4 08          	add    rsp,0x8
  4010ac:	c3                   	ret    
