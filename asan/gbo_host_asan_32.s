
gbo_host_asan_32:     file format elf32-i386


Disassembly of section .init:

00000794 <_init>:
_init():
 794:	53                   	push   ebx
 795:	83 ec 08             	sub    esp,0x8
 798:	e8 53 01 00 00       	call   8f0 <__x86.get_pc_thunk.bx>
 79d:	81 c3 63 28 00 00    	add    ebx,0x2863
 7a3:	8b 83 f8 ff ff ff    	mov    eax,DWORD PTR [ebx-0x8]
 7a9:	85 c0                	test   eax,eax
 7ab:	74 05                	je     7b2 <_init+0x1e>
 7ad:	e8 f6 00 00 00       	call   8a8 <.plt.got+0x8>
 7b2:	83 c4 08             	add    esp,0x8
 7b5:	5b                   	pop    ebx
 7b6:	c3                   	ret    

Disassembly of section .plt:

000007c0 <.plt>:
 7c0:	ff b3 04 00 00 00    	push   DWORD PTR [ebx+0x4]
 7c6:	ff a3 08 00 00 00    	jmp    DWORD PTR [ebx+0x8]
 7cc:	00 00                	add    BYTE PTR [eax],al
	...

000007d0 <printf@plt>:
 7d0:	ff a3 0c 00 00 00    	jmp    DWORD PTR [ebx+0xc]
 7d6:	68 00 00 00 00       	push   0x0
 7db:	e9 e0 ff ff ff       	jmp    7c0 <.plt>

000007e0 <__asan_register_globals@plt>:
 7e0:	ff a3 10 00 00 00    	jmp    DWORD PTR [ebx+0x10]
 7e6:	68 08 00 00 00       	push   0x8
 7eb:	e9 d0 ff ff ff       	jmp    7c0 <.plt>

000007f0 <dl_iterate_phdr@plt>:
 7f0:	ff a3 14 00 00 00    	jmp    DWORD PTR [ebx+0x14]
 7f6:	68 10 00 00 00       	push   0x10
 7fb:	e9 c0 ff ff ff       	jmp    7c0 <.plt>

00000800 <__asan_stack_malloc_1@plt>:
 800:	ff a3 18 00 00 00    	jmp    DWORD PTR [ebx+0x18]
 806:	68 18 00 00 00       	push   0x18
 80b:	e9 b0 ff ff ff       	jmp    7c0 <.plt>

00000810 <__asan_report_load4@plt>:
 810:	ff a3 1c 00 00 00    	jmp    DWORD PTR [ebx+0x1c]
 816:	68 20 00 00 00       	push   0x20
 81b:	e9 a0 ff ff ff       	jmp    7c0 <.plt>

00000820 <__libc_start_main@plt>:
 820:	ff a3 20 00 00 00    	jmp    DWORD PTR [ebx+0x20]
 826:	68 28 00 00 00       	push   0x28
 82b:	e9 90 ff ff ff       	jmp    7c0 <.plt>

00000830 <__asan_report_load2@plt>:
 830:	ff a3 24 00 00 00    	jmp    DWORD PTR [ebx+0x24]
 836:	68 30 00 00 00       	push   0x30
 83b:	e9 80 ff ff ff       	jmp    7c0 <.plt>

00000840 <__asan_unregister_globals@plt>:
 840:	ff a3 28 00 00 00    	jmp    DWORD PTR [ebx+0x28]
 846:	68 38 00 00 00       	push   0x38
 84b:	e9 70 ff ff ff       	jmp    7c0 <.plt>

00000850 <__asan_init@plt>:
 850:	ff a3 2c 00 00 00    	jmp    DWORD PTR [ebx+0x2c]
 856:	68 40 00 00 00       	push   0x40
 85b:	e9 60 ff ff ff       	jmp    7c0 <.plt>

00000860 <__asan_version_mismatch_check_v6@plt>:
 860:	ff a3 30 00 00 00    	jmp    DWORD PTR [ebx+0x30]
 866:	68 48 00 00 00       	push   0x48
 86b:	e9 50 ff ff ff       	jmp    7c0 <.plt>

00000870 <__asan_report_load8@plt>:
 870:	ff a3 34 00 00 00    	jmp    DWORD PTR [ebx+0x34]
 876:	68 50 00 00 00       	push   0x50
 87b:	e9 40 ff ff ff       	jmp    7c0 <.plt>

00000880 <puts@plt>:
 880:	ff a3 38 00 00 00    	jmp    DWORD PTR [ebx+0x38]
 886:	68 58 00 00 00       	push   0x58
 88b:	e9 30 ff ff ff       	jmp    7c0 <.plt>

00000890 <putchar@plt>:
 890:	ff a3 3c 00 00 00    	jmp    DWORD PTR [ebx+0x3c]
 896:	68 60 00 00 00       	push   0x60
 89b:	e9 20 ff ff ff       	jmp    7c0 <.plt>

Disassembly of section .plt.got:

000008a0 <.plt.got>:
 8a0:	ff a3 e4 ff ff ff    	jmp    DWORD PTR [ebx-0x1c]
 8a6:	66 90                	xchg   ax,ax
 8a8:	ff a3 f8 ff ff ff    	jmp    DWORD PTR [ebx-0x8]
 8ae:	66 90                	xchg   ax,ax

Disassembly of section .text:

000008b0 <_start>:
_start():
 8b0:	31 ed                	xor    ebp,ebp
 8b2:	5e                   	pop    esi
 8b3:	89 e1                	mov    ecx,esp
 8b5:	83 e4 f0             	and    esp,0xfffffff0
 8b8:	50                   	push   eax
 8b9:	54                   	push   esp
 8ba:	52                   	push   edx
 8bb:	e8 22 00 00 00       	call   8e2 <_start+0x32>
 8c0:	81 c3 40 27 00 00    	add    ebx,0x2740
 8c6:	8d 83 80 df ff ff    	lea    eax,[ebx-0x2080]
 8cc:	50                   	push   eax
 8cd:	8d 83 20 df ff ff    	lea    eax,[ebx-0x20e0]
 8d3:	50                   	push   eax
 8d4:	51                   	push   ecx
 8d5:	56                   	push   esi
 8d6:	ff b3 e8 ff ff ff    	push   DWORD PTR [ebx-0x18]
 8dc:	e8 3f ff ff ff       	call   820 <__libc_start_main@plt>
 8e1:	f4                   	hlt    
 8e2:	8b 1c 24             	mov    ebx,DWORD PTR [esp]
 8e5:	c3                   	ret    
 8e6:	66 90                	xchg   ax,ax
 8e8:	66 90                	xchg   ax,ax
 8ea:	66 90                	xchg   ax,ax
 8ec:	66 90                	xchg   ax,ax
 8ee:	66 90                	xchg   ax,ax

000008f0 <__x86.get_pc_thunk.bx>:
__x86.get_pc_thunk.bx():
 8f0:	8b 1c 24             	mov    ebx,DWORD PTR [esp]
 8f3:	c3                   	ret    
 8f4:	66 90                	xchg   ax,ax
 8f6:	66 90                	xchg   ax,ax
 8f8:	66 90                	xchg   ax,ax
 8fa:	66 90                	xchg   ax,ax
 8fc:	66 90                	xchg   ax,ax
 8fe:	66 90                	xchg   ax,ax

00000900 <deregister_tm_clones>:
deregister_tm_clones():
 900:	e8 17 01 00 00       	call   a1c <__x86.get_pc_thunk.dx>
 905:	81 c2 fb 26 00 00    	add    edx,0x26fb
 90b:	8d 8a a0 01 00 00    	lea    ecx,[edx+0x1a0]
 911:	8d 82 a3 01 00 00    	lea    eax,[edx+0x1a3]
 917:	29 c8                	sub    eax,ecx
 919:	83 f8 06             	cmp    eax,0x6
 91c:	76 17                	jbe    935 <deregister_tm_clones+0x35>
 91e:	8b 82 f4 ff ff ff    	mov    eax,DWORD PTR [edx-0xc]
 924:	85 c0                	test   eax,eax
 926:	74 0d                	je     935 <deregister_tm_clones+0x35>
 928:	55                   	push   ebp
 929:	89 e5                	mov    ebp,esp
 92b:	83 ec 14             	sub    esp,0x14
 92e:	51                   	push   ecx
 92f:	ff d0                	call   eax
 931:	83 c4 10             	add    esp,0x10
 934:	c9                   	leave  
 935:	f3 c3                	repz ret 
 937:	89 f6                	mov    esi,esi
 939:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00000940 <register_tm_clones>:
register_tm_clones():
 940:	e8 d7 00 00 00       	call   a1c <__x86.get_pc_thunk.dx>
 945:	81 c2 bb 26 00 00    	add    edx,0x26bb
 94b:	55                   	push   ebp
 94c:	8d 8a a0 01 00 00    	lea    ecx,[edx+0x1a0]
 952:	8d 82 a0 01 00 00    	lea    eax,[edx+0x1a0]
 958:	89 e5                	mov    ebp,esp
 95a:	53                   	push   ebx
 95b:	29 c8                	sub    eax,ecx
 95d:	c1 f8 02             	sar    eax,0x2
 960:	83 ec 04             	sub    esp,0x4
 963:	89 c3                	mov    ebx,eax
 965:	c1 eb 1f             	shr    ebx,0x1f
 968:	01 d8                	add    eax,ebx
 96a:	d1 f8                	sar    eax,1
 96c:	74 14                	je     982 <register_tm_clones+0x42>
 96e:	8b 92 fc ff ff ff    	mov    edx,DWORD PTR [edx-0x4]
 974:	85 d2                	test   edx,edx
 976:	74 0a                	je     982 <register_tm_clones+0x42>
 978:	83 ec 08             	sub    esp,0x8
 97b:	50                   	push   eax
 97c:	51                   	push   ecx
 97d:	ff d2                	call   edx
 97f:	83 c4 10             	add    esp,0x10
 982:	8b 5d fc             	mov    ebx,DWORD PTR [ebp-0x4]
 985:	c9                   	leave  
 986:	c3                   	ret    
 987:	89 f6                	mov    esi,esi
 989:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00000990 <__do_global_dtors_aux>:
__do_global_dtors_aux():
 990:	55                   	push   ebp
 991:	89 e5                	mov    ebp,esp
 993:	53                   	push   ebx
 994:	e8 57 ff ff ff       	call   8f0 <__x86.get_pc_thunk.bx>
 999:	81 c3 67 26 00 00    	add    ebx,0x2667
 99f:	83 ec 04             	sub    esp,0x4
 9a2:	80 bb a0 01 00 00 00 	cmp    BYTE PTR [ebx+0x1a0],0x0
 9a9:	75 27                	jne    9d2 <__do_global_dtors_aux+0x42>
 9ab:	8b 83 e4 ff ff ff    	mov    eax,DWORD PTR [ebx-0x1c]
 9b1:	85 c0                	test   eax,eax
 9b3:	74 11                	je     9c6 <__do_global_dtors_aux+0x36>
 9b5:	83 ec 0c             	sub    esp,0xc
 9b8:	ff b3 44 00 00 00    	push   DWORD PTR [ebx+0x44]
 9be:	e8 dd fe ff ff       	call   8a0 <.plt.got>
 9c3:	83 c4 10             	add    esp,0x10
 9c6:	e8 35 ff ff ff       	call   900 <deregister_tm_clones>
 9cb:	c6 83 a0 01 00 00 01 	mov    BYTE PTR [ebx+0x1a0],0x1
 9d2:	8b 5d fc             	mov    ebx,DWORD PTR [ebp-0x4]
 9d5:	c9                   	leave  
 9d6:	c3                   	ret    
 9d7:	89 f6                	mov    esi,esi
 9d9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

000009e0 <frame_dummy>:
frame_dummy():
 9e0:	e8 37 00 00 00       	call   a1c <__x86.get_pc_thunk.dx>
 9e5:	81 c2 1b 26 00 00    	add    edx,0x261b
 9eb:	8d 82 d8 fe ff ff    	lea    eax,[edx-0x128]
 9f1:	8b 08                	mov    ecx,DWORD PTR [eax]
 9f3:	85 c9                	test   ecx,ecx
 9f5:	75 09                	jne    a00 <frame_dummy+0x20>
 9f7:	e9 44 ff ff ff       	jmp    940 <register_tm_clones>
 9fc:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
 a00:	8b 92 ec ff ff ff    	mov    edx,DWORD PTR [edx-0x14]
 a06:	85 d2                	test   edx,edx
 a08:	74 ed                	je     9f7 <frame_dummy+0x17>
 a0a:	55                   	push   ebp
 a0b:	89 e5                	mov    ebp,esp
 a0d:	83 ec 14             	sub    esp,0x14
 a10:	50                   	push   eax
 a11:	ff d2                	call   edx
 a13:	83 c4 10             	add    esp,0x10
 a16:	c9                   	leave  
 a17:	e9 24 ff ff ff       	jmp    940 <register_tm_clones>

00000a1c <__x86.get_pc_thunk.dx>:
__x86.get_pc_thunk.dx():
 a1c:	8b 14 24             	mov    edx,DWORD PTR [esp]
 a1f:	c3                   	ret    

00000a20 <header_handler>:
header_handler():
/home/kyoupark/git/kb/asan/gbo.c:114
 a20:	55                   	push   ebp
 a21:	89 e5                	mov    ebp,esp
 a23:	57                   	push   edi
 a24:	56                   	push   esi
 a25:	53                   	push   ebx
 a26:	83 ec 2c             	sub    esp,0x2c
 a29:	e8 c2 fe ff ff       	call   8f0 <__x86.get_pc_thunk.bx>
 a2e:	81 c3 d2 25 00 00    	add    ebx,0x25d2
/home/kyoupark/git/kb/asan/gbo.c:115
 a34:	c7 45 e4 00 00 00 00 	mov    DWORD PTR [ebp-0x1c],0x0
/home/kyoupark/git/kb/asan/gbo.c:117
 a3b:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 a3e:	89 c2                	mov    edx,eax
 a40:	c1 ea 03             	shr    edx,0x3
 a43:	81 c2 00 00 00 20    	add    edx,0x20000000
 a49:	0f b6 12             	movzx  edx,BYTE PTR [edx]
 a4c:	84 d2                	test   dl,dl
 a4e:	0f 95 45 d7          	setne  BYTE PTR [ebp-0x29]
 a52:	89 c6                	mov    esi,eax
 a54:	83 e6 07             	and    esi,0x7
 a57:	8d 4e 03             	lea    ecx,[esi+0x3]
 a5a:	38 d1                	cmp    cl,dl
 a5c:	0f 9d c2             	setge  dl
 a5f:	22 55 d7             	and    dl,BYTE PTR [ebp-0x29]
 a62:	84 d2                	test   dl,dl
 a64:	74 09                	je     a6f <header_handler+0x4f>
 a66:	83 ec 0c             	sub    esp,0xc
 a69:	50                   	push   eax
 a6a:	e8 a1 fd ff ff       	call   810 <__asan_report_load4@plt>
 a6f:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 a72:	8b 00                	mov    eax,DWORD PTR [eax]
/home/kyoupark/git/kb/asan/gbo.c:116
 a74:	89 c7                	mov    edi,eax
/home/kyoupark/git/kb/asan/gbo.c:117
 a76:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 a79:	83 c0 0c             	add    eax,0xc
 a7c:	89 c2                	mov    edx,eax
 a7e:	c1 ea 03             	shr    edx,0x3
 a81:	81 c2 00 00 00 20    	add    edx,0x20000000
 a87:	0f b6 12             	movzx  edx,BYTE PTR [edx]
 a8a:	84 d2                	test   dl,dl
 a8c:	0f 95 45 d7          	setne  BYTE PTR [ebp-0x29]
 a90:	89 c6                	mov    esi,eax
 a92:	83 e6 07             	and    esi,0x7
 a95:	8d 4e 01             	lea    ecx,[esi+0x1]
 a98:	38 d1                	cmp    cl,dl
 a9a:	0f 9d c2             	setge  dl
 a9d:	22 55 d7             	and    dl,BYTE PTR [ebp-0x29]
 aa0:	84 d2                	test   dl,dl
 aa2:	74 09                	je     aad <header_handler+0x8d>
 aa4:	83 ec 0c             	sub    esp,0xc
 aa7:	50                   	push   eax
 aa8:	e8 83 fd ff ff       	call   830 <__asan_report_load2@plt>
 aad:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 ab0:	0f b7 40 0c          	movzx  eax,WORD PTR [eax+0xc]
/home/kyoupark/git/kb/asan/gbo.c:116
 ab4:	0f b7 f0             	movzx  esi,ax
 ab7:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 aba:	83 c0 04             	add    eax,0x4
 abd:	89 c2                	mov    edx,eax
 abf:	89 d0                	mov    eax,edx
 ac1:	c1 e8 03             	shr    eax,0x3
 ac4:	05 00 00 00 20       	add    eax,0x20000000
 ac9:	0f b6 08             	movzx  ecx,BYTE PTR [eax]
 acc:	84 c9                	test   cl,cl
 ace:	0f 95 45 d7          	setne  BYTE PTR [ebp-0x29]
 ad2:	89 d0                	mov    eax,edx
 ad4:	83 e0 07             	and    eax,0x7
 ad7:	83 c0 03             	add    eax,0x3
 ada:	38 c8                	cmp    al,cl
 adc:	0f 9d c0             	setge  al
 adf:	22 45 d7             	and    al,BYTE PTR [ebp-0x29]
 ae2:	84 c0                	test   al,al
 ae4:	74 09                	je     aef <header_handler+0xcf>
 ae6:	83 ec 0c             	sub    esp,0xc
 ae9:	52                   	push   edx
 aea:	e8 21 fd ff ff       	call   810 <__asan_report_load4@plt>
 aef:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 af2:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
 af5:	57                   	push   edi
 af6:	56                   	push   esi
 af7:	50                   	push   eax
 af8:	8d 83 c0 df ff ff    	lea    eax,[ebx-0x2040]
 afe:	50                   	push   eax
 aff:	e8 cc fc ff ff       	call   7d0 <printf@plt>
 b04:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo.c:118
 b07:	c7 45 e4 00 00 00 00 	mov    DWORD PTR [ebp-0x1c],0x0
 b0e:	e9 af 01 00 00       	jmp    cc2 <header_handler+0x2a2>
/home/kyoupark/git/kb/asan/gbo.c:120 (discriminator 3)
 b13:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 b16:	89 c2                	mov    edx,eax
 b18:	c1 ea 03             	shr    edx,0x3
 b1b:	81 c2 00 00 00 20    	add    edx,0x20000000
 b21:	0f b6 12             	movzx  edx,BYTE PTR [edx]
 b24:	84 d2                	test   dl,dl
 b26:	0f 95 45 d7          	setne  BYTE PTR [ebp-0x29]
 b2a:	89 c6                	mov    esi,eax
 b2c:	83 e6 07             	and    esi,0x7
 b2f:	8d 4e 03             	lea    ecx,[esi+0x3]
 b32:	38 d1                	cmp    cl,dl
 b34:	0f 9d c2             	setge  dl
 b37:	22 55 d7             	and    dl,BYTE PTR [ebp-0x29]
 b3a:	84 d2                	test   dl,dl
 b3c:	74 09                	je     b47 <header_handler+0x127>
/home/kyoupark/git/kb/asan/gbo.c:120
 b3e:	83 ec 0c             	sub    esp,0xc
 b41:	50                   	push   eax
 b42:	e8 c9 fc ff ff       	call   810 <__asan_report_load4@plt>
/home/kyoupark/git/kb/asan/gbo.c:120 (discriminator 3)
 b47:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 b4a:	8b 38                	mov    edi,DWORD PTR [eax]
 b4c:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 b4f:	83 c0 08             	add    eax,0x8
 b52:	89 c2                	mov    edx,eax
 b54:	c1 ea 03             	shr    edx,0x3
 b57:	81 c2 00 00 00 20    	add    edx,0x20000000
 b5d:	0f b6 12             	movzx  edx,BYTE PTR [edx]
 b60:	84 d2                	test   dl,dl
 b62:	0f 95 45 d7          	setne  BYTE PTR [ebp-0x29]
 b66:	89 c6                	mov    esi,eax
 b68:	83 e6 07             	and    esi,0x7
 b6b:	8d 4e 03             	lea    ecx,[esi+0x3]
 b6e:	38 d1                	cmp    cl,dl
 b70:	0f 9d c2             	setge  dl
 b73:	22 55 d7             	and    dl,BYTE PTR [ebp-0x29]
 b76:	84 d2                	test   dl,dl
 b78:	74 09                	je     b83 <header_handler+0x163>
/home/kyoupark/git/kb/asan/gbo.c:120
 b7a:	83 ec 0c             	sub    esp,0xc
 b7d:	50                   	push   eax
 b7e:	e8 8d fc ff ff       	call   810 <__asan_report_load4@plt>
/home/kyoupark/git/kb/asan/gbo.c:120 (discriminator 3)
 b83:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 b86:	8b 40 08             	mov    eax,DWORD PTR [eax+0x8]
 b89:	8b 55 e4             	mov    edx,DWORD PTR [ebp-0x1c]
 b8c:	c1 e2 05             	shl    edx,0x5
 b8f:	8d 0c 10             	lea    ecx,[eax+edx*1]
 b92:	8d 41 08             	lea    eax,[ecx+0x8]
 b95:	89 c2                	mov    edx,eax
 b97:	89 d0                	mov    eax,edx
 b99:	c1 e8 03             	shr    eax,0x3
 b9c:	05 00 00 00 20       	add    eax,0x20000000
 ba1:	0f b6 00             	movzx  eax,BYTE PTR [eax]
 ba4:	88 45 d7             	mov    BYTE PTR [ebp-0x29],al
 ba7:	84 c0                	test   al,al
 ba9:	0f 95 45 d6          	setne  BYTE PTR [ebp-0x2a]
 bad:	89 d6                	mov    esi,edx
 baf:	83 e6 07             	and    esi,0x7
 bb2:	89 f0                	mov    eax,esi
 bb4:	83 c0 03             	add    eax,0x3
 bb7:	3a 45 d7             	cmp    al,BYTE PTR [ebp-0x29]
 bba:	0f 9d c0             	setge  al
 bbd:	22 45 d6             	and    al,BYTE PTR [ebp-0x2a]
 bc0:	84 c0                	test   al,al
 bc2:	74 09                	je     bcd <header_handler+0x1ad>
/home/kyoupark/git/kb/asan/gbo.c:120
 bc4:	83 ec 0c             	sub    esp,0xc
 bc7:	52                   	push   edx
 bc8:	e8 43 fc ff ff       	call   810 <__asan_report_load4@plt>
/home/kyoupark/git/kb/asan/gbo.c:120 (discriminator 3)
 bcd:	8b 41 08             	mov    eax,DWORD PTR [ecx+0x8]
 bd0:	01 f8                	add    eax,edi
/home/kyoupark/git/kb/asan/gbo.c:119 (discriminator 3)
 bd2:	83 ec 04             	sub    esp,0x4
 bd5:	50                   	push   eax
 bd6:	ff 75 e4             	push   DWORD PTR [ebp-0x1c]
 bd9:	8d 83 20 e0 ff ff    	lea    eax,[ebx-0x1fe0]
 bdf:	50                   	push   eax
 be0:	e8 eb fb ff ff       	call   7d0 <printf@plt>
 be5:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo.c:122 (discriminator 3)
 be8:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 beb:	83 c0 08             	add    eax,0x8
 bee:	89 c2                	mov    edx,eax
 bf0:	c1 ea 03             	shr    edx,0x3
 bf3:	81 c2 00 00 00 20    	add    edx,0x20000000
 bf9:	0f b6 12             	movzx  edx,BYTE PTR [edx]
 bfc:	84 d2                	test   dl,dl
 bfe:	0f 95 45 d7          	setne  BYTE PTR [ebp-0x29]
 c02:	89 c6                	mov    esi,eax
 c04:	83 e6 07             	and    esi,0x7
 c07:	8d 4e 03             	lea    ecx,[esi+0x3]
 c0a:	38 d1                	cmp    cl,dl
 c0c:	0f 9d c2             	setge  dl
 c0f:	22 55 d7             	and    dl,BYTE PTR [ebp-0x29]
 c12:	84 d2                	test   dl,dl
 c14:	74 09                	je     c1f <header_handler+0x1ff>
/home/kyoupark/git/kb/asan/gbo.c:122
 c16:	83 ec 0c             	sub    esp,0xc
 c19:	50                   	push   eax
 c1a:	e8 f1 fb ff ff       	call   810 <__asan_report_load4@plt>
/home/kyoupark/git/kb/asan/gbo.c:122 (discriminator 3)
 c1f:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 c22:	8b 40 08             	mov    eax,DWORD PTR [eax+0x8]
 c25:	8b 55 e4             	mov    edx,DWORD PTR [ebp-0x1c]
 c28:	c1 e2 05             	shl    edx,0x5
 c2b:	8d 3c 02             	lea    edi,[edx+eax*1]
/home/kyoupark/git/kb/asan/gbo.c:121 (discriminator 3)
 c2e:	8d 47 18             	lea    eax,[edi+0x18]
 c31:	89 c1                	mov    ecx,eax
 c33:	c1 e9 03             	shr    ecx,0x3
 c36:	81 c1 00 00 00 20    	add    ecx,0x20000000
 c3c:	0f b6 09             	movzx  ecx,BYTE PTR [ecx]
 c3f:	84 c9                	test   cl,cl
 c41:	0f 95 45 d7          	setne  BYTE PTR [ebp-0x29]
 c45:	89 c6                	mov    esi,eax
 c47:	83 e6 07             	and    esi,0x7
 c4a:	8d 56 03             	lea    edx,[esi+0x3]
 c4d:	38 ca                	cmp    dl,cl
 c4f:	0f 9d c1             	setge  cl
 c52:	22 4d d7             	and    cl,BYTE PTR [ebp-0x29]
 c55:	84 c9                	test   cl,cl
 c57:	74 09                	je     c62 <header_handler+0x242>
/home/kyoupark/git/kb/asan/gbo.c:121
 c59:	83 ec 0c             	sub    esp,0xc
 c5c:	50                   	push   eax
 c5d:	e8 ae fb ff ff       	call   810 <__asan_report_load4@plt>
/home/kyoupark/git/kb/asan/gbo.c:121 (discriminator 3)
 c62:	8b 7f 18             	mov    edi,DWORD PTR [edi+0x18]
/home/kyoupark/git/kb/asan/gbo.c:122 (discriminator 3)
 c65:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 c68:	8b 40 08             	mov    eax,DWORD PTR [eax+0x8]
 c6b:	8b 55 e4             	mov    edx,DWORD PTR [ebp-0x1c]
 c6e:	c1 e2 05             	shl    edx,0x5
 c71:	8d 34 10             	lea    esi,[eax+edx*1]
/home/kyoupark/git/kb/asan/gbo.c:121 (discriminator 3)
 c74:	89 f0                	mov    eax,esi
 c76:	89 c2                	mov    edx,eax
 c78:	89 d0                	mov    eax,edx
 c7a:	c1 e8 03             	shr    eax,0x3
 c7d:	05 00 00 00 20       	add    eax,0x20000000
 c82:	0f b6 08             	movzx  ecx,BYTE PTR [eax]
 c85:	84 c9                	test   cl,cl
 c87:	0f 95 45 d7          	setne  BYTE PTR [ebp-0x29]
 c8b:	89 d0                	mov    eax,edx
 c8d:	83 e0 07             	and    eax,0x7
 c90:	83 c0 03             	add    eax,0x3
 c93:	38 c8                	cmp    al,cl
 c95:	0f 9d c0             	setge  al
 c98:	22 45 d7             	and    al,BYTE PTR [ebp-0x29]
 c9b:	84 c0                	test   al,al
 c9d:	74 09                	je     ca8 <header_handler+0x288>
/home/kyoupark/git/kb/asan/gbo.c:121
 c9f:	83 ec 0c             	sub    esp,0xc
 ca2:	52                   	push   edx
 ca3:	e8 68 fb ff ff       	call   810 <__asan_report_load4@plt>
/home/kyoupark/git/kb/asan/gbo.c:121 (discriminator 3)
 ca8:	8b 06                	mov    eax,DWORD PTR [esi]
 caa:	83 ec 04             	sub    esp,0x4
 cad:	57                   	push   edi
 cae:	50                   	push   eax
 caf:	8d 83 60 e0 ff ff    	lea    eax,[ebx-0x1fa0]
 cb5:	50                   	push   eax
 cb6:	e8 15 fb ff ff       	call   7d0 <printf@plt>
 cbb:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo.c:118 (discriminator 3)
 cbe:	83 45 e4 01          	add    DWORD PTR [ebp-0x1c],0x1
/home/kyoupark/git/kb/asan/gbo.c:118 (discriminator 1)
 cc2:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 cc5:	83 c0 0c             	add    eax,0xc
 cc8:	89 c2                	mov    edx,eax
 cca:	c1 ea 03             	shr    edx,0x3
 ccd:	81 c2 00 00 00 20    	add    edx,0x20000000
 cd3:	0f b6 12             	movzx  edx,BYTE PTR [edx]
 cd6:	84 d2                	test   dl,dl
 cd8:	0f 95 45 d7          	setne  BYTE PTR [ebp-0x29]
 cdc:	89 c6                	mov    esi,eax
 cde:	83 e6 07             	and    esi,0x7
 ce1:	8d 4e 01             	lea    ecx,[esi+0x1]
 ce4:	38 d1                	cmp    cl,dl
 ce6:	0f 9d c2             	setge  dl
 ce9:	22 55 d7             	and    dl,BYTE PTR [ebp-0x29]
 cec:	84 d2                	test   dl,dl
 cee:	74 09                	je     cf9 <header_handler+0x2d9>
/home/kyoupark/git/kb/asan/gbo.c:118
 cf0:	83 ec 0c             	sub    esp,0xc
 cf3:	50                   	push   eax
 cf4:	e8 37 fb ff ff       	call   830 <__asan_report_load2@plt>
/home/kyoupark/git/kb/asan/gbo.c:118 (discriminator 1)
 cf9:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 cfc:	0f b7 40 0c          	movzx  eax,WORD PTR [eax+0xc]
 d00:	0f b7 c0             	movzx  eax,ax
 d03:	3b 45 e4             	cmp    eax,DWORD PTR [ebp-0x1c]
 d06:	0f 8f 07 fe ff ff    	jg     b13 <header_handler+0xf3>
/home/kyoupark/git/kb/asan/gbo.c:124
 d0c:	83 ec 0c             	sub    esp,0xc
 d0f:	6a 0a                	push   0xa
 d11:	e8 7a fb ff ff       	call   890 <putchar@plt>
 d16:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo.c:125
 d19:	b8 00 00 00 00       	mov    eax,0x0
/home/kyoupark/git/kb/asan/gbo.c:126
 d1e:	8d 65 f4             	lea    esp,[ebp-0xc]
 d21:	5b                   	pop    ebx
 d22:	5e                   	pop    esi
 d23:	5f                   	pop    edi
 d24:	5d                   	pop    ebp
 d25:	c3                   	ret    

00000d26 <call_gbo>:
call_gbo():
/home/kyoupark/git/kb/asan/gbo.c:131
 d26:	55                   	push   ebp
 d27:	89 e5                	mov    ebp,esp
 d29:	57                   	push   edi
 d2a:	56                   	push   esi
 d2b:	53                   	push   ebx
 d2c:	83 ec 7c             	sub    esp,0x7c
 d2f:	e8 bc fb ff ff       	call   8f0 <__x86.get_pc_thunk.bx>
 d34:	81 c3 cc 22 00 00    	add    ebx,0x22cc
 d3a:	8d 75 88             	lea    esi,[ebp-0x78]
 d3d:	89 75 84             	mov    DWORD PTR [ebp-0x7c],esi
 d40:	8b 83 f0 ff ff ff    	mov    eax,DWORD PTR [ebx-0x10]
 d46:	83 38 00             	cmp    DWORD PTR [eax],0x0
 d49:	74 13                	je     d5e <call_gbo+0x38>
 d4b:	83 ec 0c             	sub    esp,0xc
 d4e:	6a 60                	push   0x60
 d50:	e8 ab fa ff ff       	call   800 <__asan_stack_malloc_1@plt>
 d55:	83 c4 10             	add    esp,0x10
 d58:	85 c0                	test   eax,eax
 d5a:	74 02                	je     d5e <call_gbo+0x38>
 d5c:	89 c6                	mov    esi,eax
 d5e:	8d 46 60             	lea    eax,[esi+0x60]
 d61:	89 c7                	mov    edi,eax
 d63:	c7 06 b3 8a b5 41    	mov    DWORD PTR [esi],0x41b58ab3
 d69:	8d 83 a0 e0 ff ff    	lea    eax,[ebx-0x1f60]
 d6f:	89 46 04             	mov    DWORD PTR [esi+0x4],eax
 d72:	8d 83 26 dd ff ff    	lea    eax,[ebx-0x22da]
 d78:	89 46 08             	mov    DWORD PTR [esi+0x8],eax
 d7b:	89 f1                	mov    ecx,esi
 d7d:	c1 e9 03             	shr    ecx,0x3
 d80:	c7 81 00 00 00 20 f1 	mov    DWORD PTR [ecx+0x20000000],0xf1f1f1f1
 d87:	f1 f1 f1 
 d8a:	c7 81 04 00 00 20 00 	mov    DWORD PTR [ecx+0x20000004],0xf4f40200
 d91:	02 f4 f4 
 d94:	c7 81 08 00 00 20 f3 	mov    DWORD PTR [ecx+0x20000008],0xf3f3f3f3
 d9b:	f3 f3 f3 
/home/kyoupark/git/kb/asan/gbo.c:133
 d9e:	8d 83 60 00 00 00    	lea    eax,[ebx+0x60]
 da4:	89 c2                	mov    edx,eax
 da6:	89 d0                	mov    eax,edx
 da8:	c1 e8 03             	shr    eax,0x3
 dab:	05 00 00 00 20       	add    eax,0x20000000
 db0:	0f b6 00             	movzx  eax,BYTE PTR [eax]
 db3:	84 c0                	test   al,al
 db5:	74 09                	je     dc0 <call_gbo+0x9a>
 db7:	83 ec 0c             	sub    esp,0xc
 dba:	52                   	push   edx
 dbb:	e8 b0 fa ff ff       	call   870 <__asan_report_load8@plt>
 dc0:	8b 83 60 00 00 00    	mov    eax,DWORD PTR [ebx+0x60]
 dc6:	8b 93 64 00 00 00    	mov    edx,DWORD PTR [ebx+0x64]
 dcc:	89 47 c0             	mov    DWORD PTR [edi-0x40],eax
 dcf:	89 57 c4             	mov    DWORD PTR [edi-0x3c],edx
/home/kyoupark/git/kb/asan/gbo.c:134
 dd2:	90                   	nop
/home/kyoupark/git/kb/asan/gbo.c:131
 dd3:	39 75 84             	cmp    DWORD PTR [ebp-0x7c],esi
 dd6:	74 26                	je     dfe <call_gbo+0xd8>
 dd8:	c7 06 0e 36 e0 45    	mov    DWORD PTR [esi],0x45e0360e
 dde:	c7 81 00 00 00 20 f5 	mov    DWORD PTR [ecx+0x20000000],0xf5f5f5f5
 de5:	f5 f5 f5 
 de8:	c7 81 04 00 00 20 f5 	mov    DWORD PTR [ecx+0x20000004],0xf5f5f5f5
 def:	f5 f5 f5 
 df2:	c7 81 08 00 00 20 f5 	mov    DWORD PTR [ecx+0x20000008],0xf5f5f5f5
 df9:	f5 f5 f5 
 dfc:	eb 1e                	jmp    e1c <call_gbo+0xf6>
 dfe:	c7 81 00 00 00 20 00 	mov    DWORD PTR [ecx+0x20000000],0x0
 e05:	00 00 00 
 e08:	c7 81 04 00 00 20 00 	mov    DWORD PTR [ecx+0x20000004],0x0
 e0f:	00 00 00 
 e12:	c7 81 08 00 00 20 00 	mov    DWORD PTR [ecx+0x20000008],0x0
 e19:	00 00 00 
/home/kyoupark/git/kb/asan/gbo.c:135
 e1c:	8d 65 f4             	lea    esp,[ebp-0xc]
 e1f:	5b                   	pop    ebx
 e20:	5e                   	pop    esi
 e21:	5f                   	pop    edi
 e22:	5d                   	pop    ebp
 e23:	c3                   	ret    

00000e24 <main>:
main():
/home/kyoupark/git/kb/asan/gbo.c:138
 e24:	8d 4c 24 04          	lea    ecx,[esp+0x4]
 e28:	83 e4 f0             	and    esp,0xfffffff0
 e2b:	ff 71 fc             	push   DWORD PTR [ecx-0x4]
 e2e:	55                   	push   ebp
 e2f:	89 e5                	mov    ebp,esp
 e31:	53                   	push   ebx
 e32:	51                   	push   ecx
 e33:	e8 b8 fa ff ff       	call   8f0 <__x86.get_pc_thunk.bx>
 e38:	81 c3 c8 21 00 00    	add    ebx,0x21c8
/home/kyoupark/git/kb/asan/gbo.c:139
 e3e:	83 ec 08             	sub    esp,0x8
 e41:	6a 00                	push   0x0
 e43:	8d 83 20 da ff ff    	lea    eax,[ebx-0x25e0]
 e49:	50                   	push   eax
 e4a:	e8 a1 f9 ff ff       	call   7f0 <dl_iterate_phdr@plt>
 e4f:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo.c:141
 e52:	83 ec 0c             	sub    esp,0xc
 e55:	8d 83 c0 e0 ff ff    	lea    eax,[ebx-0x1f40]
 e5b:	50                   	push   eax
 e5c:	e8 1f fa ff ff       	call   880 <puts@plt>
 e61:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo.c:142
 e64:	83 ec 0c             	sub    esp,0xc
 e67:	8d 83 00 e1 ff ff    	lea    eax,[ebx-0x1f00]
 e6d:	50                   	push   eax
 e6e:	e8 0d fa ff ff       	call   880 <puts@plt>
 e73:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo.c:143
 e76:	83 ec 0c             	sub    esp,0xc
 e79:	8d 83 60 e1 ff ff    	lea    eax,[ebx-0x1ea0]
 e7f:	50                   	push   eax
 e80:	e8 fb f9 ff ff       	call   880 <puts@plt>
 e85:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo.c:145
 e88:	e8 99 fe ff ff       	call   d26 <call_gbo>
/home/kyoupark/git/kb/asan/gbo.c:147
 e8d:	83 ec 0c             	sub    esp,0xc
 e90:	8d 83 a0 e1 ff ff    	lea    eax,[ebx-0x1e60]
 e96:	50                   	push   eax
 e97:	e8 e4 f9 ff ff       	call   880 <puts@plt>
 e9c:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo.c:149
 e9f:	b8 00 00 00 00       	mov    eax,0x0
/home/kyoupark/git/kb/asan/gbo.c:150
 ea4:	8d 65 f8             	lea    esp,[ebp-0x8]
 ea7:	59                   	pop    ecx
 ea8:	5b                   	pop    ebx
 ea9:	5d                   	pop    ebp
 eaa:	8d 61 fc             	lea    esp,[ecx-0x4]
 ead:	c3                   	ret    

00000eae <_GLOBAL__sub_D_00099_0_call_gbo>:
_GLOBAL__sub_D_00099_0_call_gbo():
 eae:	55                   	push   ebp
 eaf:	89 e5                	mov    ebp,esp
 eb1:	53                   	push   ebx
 eb2:	83 ec 04             	sub    esp,0x4
 eb5:	e8 55 00 00 00       	call   f0f <__x86.get_pc_thunk.ax>
 eba:	05 46 21 00 00       	add    eax,0x2146
 ebf:	83 ec 08             	sub    esp,0x8
 ec2:	6a 08                	push   0x8
 ec4:	8d 90 c0 00 00 00    	lea    edx,[eax+0xc0]
 eca:	52                   	push   edx
 ecb:	89 c3                	mov    ebx,eax
 ecd:	e8 6e f9 ff ff       	call   840 <__asan_unregister_globals@plt>
 ed2:	83 c4 10             	add    esp,0x10
 ed5:	8b 5d fc             	mov    ebx,DWORD PTR [ebp-0x4]
 ed8:	c9                   	leave  
 ed9:	c3                   	ret    

00000eda <_GLOBAL__sub_I_00099_1_call_gbo>:
_GLOBAL__sub_I_00099_1_call_gbo():
 eda:	55                   	push   ebp
 edb:	89 e5                	mov    ebp,esp
 edd:	53                   	push   ebx
 ede:	83 ec 04             	sub    esp,0x4
 ee1:	e8 0a fa ff ff       	call   8f0 <__x86.get_pc_thunk.bx>
 ee6:	81 c3 1a 21 00 00    	add    ebx,0x211a
 eec:	e8 5f f9 ff ff       	call   850 <__asan_init@plt>
 ef1:	e8 6a f9 ff ff       	call   860 <__asan_version_mismatch_check_v6@plt>
 ef6:	83 ec 08             	sub    esp,0x8
 ef9:	6a 08                	push   0x8
 efb:	8d 83 c0 00 00 00    	lea    eax,[ebx+0xc0]
 f01:	50                   	push   eax
 f02:	e8 d9 f8 ff ff       	call   7e0 <__asan_register_globals@plt>
 f07:	83 c4 10             	add    esp,0x10
 f0a:	8b 5d fc             	mov    ebx,DWORD PTR [ebp-0x4]
 f0d:	c9                   	leave  
 f0e:	c3                   	ret    

00000f0f <__x86.get_pc_thunk.ax>:
__x86.get_pc_thunk.ax():
 f0f:	8b 04 24             	mov    eax,DWORD PTR [esp]
 f12:	c3                   	ret    
 f13:	66 90                	xchg   ax,ax
 f15:	66 90                	xchg   ax,ax
 f17:	66 90                	xchg   ax,ax
 f19:	66 90                	xchg   ax,ax
 f1b:	66 90                	xchg   ax,ax
 f1d:	66 90                	xchg   ax,ax
 f1f:	90                   	nop

00000f20 <__libc_csu_init>:
__libc_csu_init():
 f20:	55                   	push   ebp
 f21:	57                   	push   edi
 f22:	56                   	push   esi
 f23:	53                   	push   ebx
 f24:	e8 c7 f9 ff ff       	call   8f0 <__x86.get_pc_thunk.bx>
 f29:	81 c3 d7 20 00 00    	add    ebx,0x20d7
 f2f:	83 ec 0c             	sub    esp,0xc
 f32:	8b 6c 24 20          	mov    ebp,DWORD PTR [esp+0x20]
 f36:	8d b3 d0 fe ff ff    	lea    esi,[ebx-0x130]
 f3c:	e8 53 f8 ff ff       	call   794 <_init>
 f41:	8d 83 c8 fe ff ff    	lea    eax,[ebx-0x138]
 f47:	29 c6                	sub    esi,eax
 f49:	c1 fe 02             	sar    esi,0x2
 f4c:	85 f6                	test   esi,esi
 f4e:	74 25                	je     f75 <__libc_csu_init+0x55>
 f50:	31 ff                	xor    edi,edi
 f52:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
 f58:	83 ec 04             	sub    esp,0x4
 f5b:	ff 74 24 2c          	push   DWORD PTR [esp+0x2c]
 f5f:	ff 74 24 2c          	push   DWORD PTR [esp+0x2c]
 f63:	55                   	push   ebp
 f64:	ff 94 bb c8 fe ff ff 	call   DWORD PTR [ebx+edi*4-0x138]
 f6b:	83 c7 01             	add    edi,0x1
 f6e:	83 c4 10             	add    esp,0x10
 f71:	39 fe                	cmp    esi,edi
 f73:	75 e3                	jne    f58 <__libc_csu_init+0x38>
 f75:	83 c4 0c             	add    esp,0xc
 f78:	5b                   	pop    ebx
 f79:	5e                   	pop    esi
 f7a:	5f                   	pop    edi
 f7b:	5d                   	pop    ebp
 f7c:	c3                   	ret    
 f7d:	8d 76 00             	lea    esi,[esi+0x0]

00000f80 <__libc_csu_fini>:
__libc_csu_fini():
 f80:	f3 c3                	repz ret 

Disassembly of section .fini:

00000f84 <_fini>:
_fini():
 f84:	53                   	push   ebx
 f85:	83 ec 08             	sub    esp,0x8
 f88:	e8 63 f9 ff ff       	call   8f0 <__x86.get_pc_thunk.bx>
 f8d:	81 c3 73 20 00 00    	add    ebx,0x2073
 f93:	83 c4 08             	add    esp,0x8
 f96:	5b                   	pop    ebx
 f97:	c3                   	ret    
