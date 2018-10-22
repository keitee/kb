
gbo_32_630_asan:     file format elf32-i386


Disassembly of section .init:

08048670 <_init>:
_init():
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crti.S:63
 8048670:	53                   	push   ebx
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crti.S:65
 8048671:	83 ec 08             	sub    esp,0x8
 8048674:	e8 67 01 00 00       	call   80487e0 <__x86.get_pc_thunk.bx>
 8048679:	81 c3 87 29 00 00    	add    ebx,0x2987
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crti.S:68
 804867f:	8b 83 fc ff ff ff    	mov    eax,DWORD PTR [ebx-0x4]
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crti.S:69
 8048685:	85 c0                	test   eax,eax
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crti.S:70
 8048687:	74 05                	je     804868e <_init+0x1e>
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crti.S:71
 8048689:	e8 12 01 00 00       	call   80487a0 <.plt.got>
 804868e:	e8 ef 01 00 00       	call   8048882 <frame_dummy>
 8048693:	e8 a9 09 00 00       	call   8049041 <__do_global_ctors_aux>
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crtn.S:40
 8048698:	83 c4 08             	add    esp,0x8
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crtn.S:41
 804869b:	5b                   	pop    ebx
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crtn.S:42
 804869c:	c3                   	ret    

Disassembly of section .plt:

080486a0 <.plt>:
 80486a0:	ff 35 04 b0 04 08    	push   DWORD PTR ds:0x804b004
 80486a6:	ff 25 08 b0 04 08    	jmp    DWORD PTR ds:0x804b008
 80486ac:	00 00                	add    BYTE PTR [eax],al
	...

080486b0 <printf@plt>:
 80486b0:	ff 25 0c b0 04 08    	jmp    DWORD PTR ds:0x804b00c
 80486b6:	68 00 00 00 00       	push   0x0
 80486bb:	e9 e0 ff ff ff       	jmp    80486a0 <.plt>

080486c0 <__asan_register_globals@plt>:
 80486c0:	ff 25 10 b0 04 08    	jmp    DWORD PTR ds:0x804b010
 80486c6:	68 08 00 00 00       	push   0x8
 80486cb:	e9 d0 ff ff ff       	jmp    80486a0 <.plt>

080486d0 <__register_frame_info_bases@plt>:
 80486d0:	ff 25 14 b0 04 08    	jmp    DWORD PTR ds:0x804b014
 80486d6:	68 10 00 00 00       	push   0x10
 80486db:	e9 c0 ff ff ff       	jmp    80486a0 <.plt>

080486e0 <dl_iterate_phdr@plt>:
 80486e0:	ff 25 18 b0 04 08    	jmp    DWORD PTR ds:0x804b018
 80486e6:	68 18 00 00 00       	push   0x18
 80486eb:	e9 b0 ff ff ff       	jmp    80486a0 <.plt>

080486f0 <__asan_stack_malloc_1@plt>:
 80486f0:	ff 25 1c b0 04 08    	jmp    DWORD PTR ds:0x804b01c
 80486f6:	68 20 00 00 00       	push   0x20
 80486fb:	e9 a0 ff ff ff       	jmp    80486a0 <.plt>

08048700 <__asan_report_load4@plt>:
 8048700:	ff 25 20 b0 04 08    	jmp    DWORD PTR ds:0x804b020
 8048706:	68 28 00 00 00       	push   0x28
 804870b:	e9 90 ff ff ff       	jmp    80486a0 <.plt>

08048710 <__libc_start_main@plt>:
 8048710:	ff 25 24 b0 04 08    	jmp    DWORD PTR ds:0x804b024
 8048716:	68 30 00 00 00       	push   0x30
 804871b:	e9 80 ff ff ff       	jmp    80486a0 <.plt>

08048720 <__asan_report_load2@plt>:
 8048720:	ff 25 28 b0 04 08    	jmp    DWORD PTR ds:0x804b028
 8048726:	68 38 00 00 00       	push   0x38
 804872b:	e9 70 ff ff ff       	jmp    80486a0 <.plt>

08048730 <__asan_unregister_globals@plt>:
 8048730:	ff 25 2c b0 04 08    	jmp    DWORD PTR ds:0x804b02c
 8048736:	68 40 00 00 00       	push   0x40
 804873b:	e9 60 ff ff ff       	jmp    80486a0 <.plt>

08048740 <__asan_init@plt>:
 8048740:	ff 25 30 b0 04 08    	jmp    DWORD PTR ds:0x804b030
 8048746:	68 48 00 00 00       	push   0x48
 804874b:	e9 50 ff ff ff       	jmp    80486a0 <.plt>

08048750 <__asan_version_mismatch_check_v6@plt>:
 8048750:	ff 25 34 b0 04 08    	jmp    DWORD PTR ds:0x804b034
 8048756:	68 50 00 00 00       	push   0x50
 804875b:	e9 40 ff ff ff       	jmp    80486a0 <.plt>

08048760 <__asan_report_load8@plt>:
 8048760:	ff 25 38 b0 04 08    	jmp    DWORD PTR ds:0x804b038
 8048766:	68 58 00 00 00       	push   0x58
 804876b:	e9 30 ff ff ff       	jmp    80486a0 <.plt>

08048770 <puts@plt>:
 8048770:	ff 25 3c b0 04 08    	jmp    DWORD PTR ds:0x804b03c
 8048776:	68 60 00 00 00       	push   0x60
 804877b:	e9 20 ff ff ff       	jmp    80486a0 <.plt>

08048780 <putchar@plt>:
 8048780:	ff 25 40 b0 04 08    	jmp    DWORD PTR ds:0x804b040
 8048786:	68 68 00 00 00       	push   0x68
 804878b:	e9 10 ff ff ff       	jmp    80486a0 <.plt>

08048790 <__deregister_frame_info_bases@plt>:
 8048790:	ff 25 44 b0 04 08    	jmp    DWORD PTR ds:0x804b044
 8048796:	68 70 00 00 00       	push   0x70
 804879b:	e9 00 ff ff ff       	jmp    80486a0 <.plt>

Disassembly of section .plt.got:

080487a0 <.plt.got>:
 80487a0:	ff 25 fc af 04 08    	jmp    DWORD PTR ds:0x804affc
 80487a6:	66 90                	xchg   ax,ax

Disassembly of section .text:

080487b0 <_start>:
_start():
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/start.S:61
 80487b0:	31 ed                	xor    ebp,ebp
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/start.S:66
 80487b2:	5e                   	pop    esi
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/start.S:67
 80487b3:	89 e1                	mov    ecx,esp
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/start.S:73
 80487b5:	83 e4 f0             	and    esp,0xfffffff0
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/start.S:74
 80487b8:	50                   	push   eax
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/start.S:79
 80487b9:	54                   	push   esp
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/start.S:81
 80487ba:	52                   	push   edx
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/start.S:105
 80487bb:	68 40 90 04 08       	push   0x8049040
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/start.S:106
 80487c0:	68 d0 8f 04 08       	push   0x8048fd0
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/start.S:108
 80487c5:	51                   	push   ecx
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/start.S:109
 80487c6:	56                   	push   esi
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/start.S:111
 80487c7:	68 c3 8e 04 08       	push   0x8048ec3
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/start.S:115
 80487cc:	e8 3f ff ff ff       	call   8048710 <__libc_start_main@plt>
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/start.S:118
 80487d1:	f4                   	hlt    
 80487d2:	66 90                	xchg   ax,ax
 80487d4:	66 90                	xchg   ax,ax
 80487d6:	66 90                	xchg   ax,ax
 80487d8:	66 90                	xchg   ax,ax
 80487da:	66 90                	xchg   ax,ax
 80487dc:	66 90                	xchg   ax,ax
 80487de:	66 90                	xchg   ax,ax

080487e0 <__x86.get_pc_thunk.bx>:
__x86.get_pc_thunk.bx():
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crti.S:66
 80487e0:	8b 1c 24             	mov    ebx,DWORD PTR [esp]
 80487e3:	c3                   	ret    

080487e4 <deregister_tm_clones>:
deregister_tm_clones():
 80487e4:	83 3d 68 b2 04 08 00 	cmp    DWORD PTR ds:0x804b268,0x0
 80487eb:	74 1a                	je     8048807 <deregister_tm_clones+0x23>
 80487ed:	b8 00 00 00 00       	mov    eax,0x0
 80487f2:	85 c0                	test   eax,eax
 80487f4:	74 11                	je     8048807 <deregister_tm_clones+0x23>
 80487f6:	55                   	push   ebp
 80487f7:	89 e5                	mov    ebp,esp
 80487f9:	83 ec 14             	sub    esp,0x14
 80487fc:	68 68 b2 04 08       	push   0x804b268
 8048801:	ff d0                	call   eax
 8048803:	83 c4 10             	add    esp,0x10
 8048806:	c9                   	leave  
 8048807:	c3                   	ret    

08048808 <register_tm_clones>:
register_tm_clones():
 8048808:	31 c0                	xor    eax,eax
 804880a:	83 3c c5 68 b2 04 08 	cmp    DWORD PTR [eax*8+0x804b268],0x0
 8048811:	00 
 8048812:	74 03                	je     8048817 <register_tm_clones+0xf>
 8048814:	40                   	inc    eax
 8048815:	eb f3                	jmp    804880a <register_tm_clones+0x2>
 8048817:	85 c0                	test   eax,eax
 8048819:	74 1b                	je     8048836 <register_tm_clones+0x2e>
 804881b:	ba 00 00 00 00       	mov    edx,0x0
 8048820:	85 d2                	test   edx,edx
 8048822:	74 12                	je     8048836 <register_tm_clones+0x2e>
 8048824:	55                   	push   ebp
 8048825:	89 e5                	mov    ebp,esp
 8048827:	83 ec 10             	sub    esp,0x10
 804882a:	50                   	push   eax
 804882b:	68 68 b2 04 08       	push   0x804b268
 8048830:	ff d2                	call   edx
 8048832:	83 c4 10             	add    esp,0x10
 8048835:	c9                   	leave  
 8048836:	c3                   	ret    

08048837 <__do_global_dtors_aux>:
__do_global_dtors_aux():
 8048837:	80 3d 74 b2 04 08 00 	cmp    BYTE PTR ds:0x804b274,0x0
 804883e:	74 01                	je     8048841 <__do_global_dtors_aux+0xa>
 8048840:	c3                   	ret    
 8048841:	55                   	push   ebp
 8048842:	89 e5                	mov    ebp,esp
 8048844:	83 ec 08             	sub    esp,0x8
 8048847:	a1 68 b0 04 08       	mov    eax,ds:0x804b068
 804884c:	8b 10                	mov    edx,DWORD PTR [eax]
 804884e:	85 d2                	test   edx,edx
 8048850:	74 0c                	je     804885e <__do_global_dtors_aux+0x27>
 8048852:	83 c0 04             	add    eax,0x4
 8048855:	a3 68 b0 04 08       	mov    ds:0x804b068,eax
 804885a:	ff d2                	call   edx
 804885c:	eb e9                	jmp    8048847 <__do_global_dtors_aux+0x10>
 804885e:	e8 81 ff ff ff       	call   80487e4 <deregister_tm_clones>
 8048863:	b8 90 87 04 08       	mov    eax,0x8048790
 8048868:	85 c0                	test   eax,eax
 804886a:	74 0d                	je     8048879 <__do_global_dtors_aux+0x42>
 804886c:	83 ec 0c             	sub    esp,0xc
 804886f:	68 a0 ac 04 08       	push   0x804aca0
 8048874:	ff d0                	call   eax
 8048876:	83 c4 10             	add    esp,0x10
 8048879:	c6 05 74 b2 04 08 01 	mov    BYTE PTR ds:0x804b274,0x1
 8048880:	c9                   	leave  
 8048881:	c3                   	ret    

08048882 <frame_dummy>:
frame_dummy():
 8048882:	55                   	push   ebp
 8048883:	b8 d0 86 04 08       	mov    eax,0x80486d0
 8048888:	89 e5                	mov    ebp,esp
 804888a:	83 ec 08             	sub    esp,0x8
 804888d:	e8 00 00 00 00       	call   8048892 <frame_dummy+0x10>
 8048892:	5a                   	pop    edx
 8048893:	81 c2 6e 27 00 00    	add    edx,0x276e
 8048899:	85 c0                	test   eax,eax
 804889b:	74 15                	je     80488b2 <frame_dummy+0x30>
 804889d:	52                   	push   edx
 804889e:	6a 00                	push   0x0
 80488a0:	68 78 b2 04 08       	push   0x804b278
 80488a5:	68 a0 ac 04 08       	push   0x804aca0
 80488aa:	e8 21 fe ff ff       	call   80486d0 <__register_frame_info_bases@plt>
 80488af:	83 c4 10             	add    esp,0x10
 80488b2:	ba 08 af 04 08       	mov    edx,0x804af08
 80488b7:	83 3a 00             	cmp    DWORD PTR [edx],0x0
 80488ba:	74 12                	je     80488ce <frame_dummy+0x4c>
 80488bc:	b8 00 00 00 00       	mov    eax,0x0
 80488c1:	85 c0                	test   eax,eax
 80488c3:	74 09                	je     80488ce <frame_dummy+0x4c>
 80488c5:	83 ec 0c             	sub    esp,0xc
 80488c8:	52                   	push   edx
 80488c9:	ff d0                	call   eax
 80488cb:	83 c4 10             	add    esp,0x10
 80488ce:	c9                   	leave  
 80488cf:	e9 34 ff ff ff       	jmp    8048808 <register_tm_clones>

080488d4 <header_handler>:
header_handler():
/home/kyoupark/git/kb/asan/gbo_recover.c:114
 80488d4:	55                   	push   ebp
 80488d5:	89 e5                	mov    ebp,esp
 80488d7:	57                   	push   edi
 80488d8:	56                   	push   esi
 80488d9:	53                   	push   ebx
 80488da:	83 ec 2c             	sub    esp,0x2c
/home/kyoupark/git/kb/asan/gbo_recover.c:115
 80488dd:	c7 45 e4 00 00 00 00 	mov    DWORD PTR [ebp-0x1c],0x0
/home/kyoupark/git/kb/asan/gbo_recover.c:117
 80488e4:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 80488e7:	89 c2                	mov    edx,eax
 80488e9:	c1 ea 03             	shr    edx,0x3
 80488ec:	81 c2 00 00 00 20    	add    edx,0x20000000
 80488f2:	0f b6 12             	movzx  edx,BYTE PTR [edx]
 80488f5:	84 d2                	test   dl,dl
 80488f7:	0f 95 c1             	setne  cl
 80488fa:	89 c3                	mov    ebx,eax
 80488fc:	83 e3 07             	and    ebx,0x7
 80488ff:	83 c3 03             	add    ebx,0x3
 8048902:	38 d3                	cmp    bl,dl
 8048904:	0f 9d c2             	setge  dl
 8048907:	21 ca                	and    edx,ecx
 8048909:	84 d2                	test   dl,dl
 804890b:	74 09                	je     8048916 <header_handler+0x42>
 804890d:	83 ec 0c             	sub    esp,0xc
 8048910:	50                   	push   eax
 8048911:	e8 ea fd ff ff       	call   8048700 <__asan_report_load4@plt>
 8048916:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 8048919:	8b 00                	mov    eax,DWORD PTR [eax]
/home/kyoupark/git/kb/asan/gbo_recover.c:116
 804891b:	89 c7                	mov    edi,eax
/home/kyoupark/git/kb/asan/gbo_recover.c:117
 804891d:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 8048920:	83 c0 0c             	add    eax,0xc
 8048923:	89 c2                	mov    edx,eax
 8048925:	c1 ea 03             	shr    edx,0x3
 8048928:	81 c2 00 00 00 20    	add    edx,0x20000000
 804892e:	0f b6 12             	movzx  edx,BYTE PTR [edx]
 8048931:	84 d2                	test   dl,dl
 8048933:	0f 95 c1             	setne  cl
 8048936:	89 c3                	mov    ebx,eax
 8048938:	83 e3 07             	and    ebx,0x7
 804893b:	83 c3 01             	add    ebx,0x1
 804893e:	38 d3                	cmp    bl,dl
 8048940:	0f 9d c2             	setge  dl
 8048943:	21 ca                	and    edx,ecx
 8048945:	84 d2                	test   dl,dl
 8048947:	74 09                	je     8048952 <header_handler+0x7e>
 8048949:	83 ec 0c             	sub    esp,0xc
 804894c:	50                   	push   eax
 804894d:	e8 ce fd ff ff       	call   8048720 <__asan_report_load2@plt>
 8048952:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 8048955:	0f b7 40 0c          	movzx  eax,WORD PTR [eax+0xc]
/home/kyoupark/git/kb/asan/gbo_recover.c:116
 8048959:	0f b7 c0             	movzx  eax,ax
 804895c:	89 45 d4             	mov    DWORD PTR [ebp-0x2c],eax
 804895f:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 8048962:	83 c0 04             	add    eax,0x4
 8048965:	89 c2                	mov    edx,eax
 8048967:	c1 ea 03             	shr    edx,0x3
 804896a:	81 c2 00 00 00 20    	add    edx,0x20000000
 8048970:	0f b6 12             	movzx  edx,BYTE PTR [edx]
 8048973:	84 d2                	test   dl,dl
 8048975:	0f 95 c3             	setne  bl
 8048978:	89 c6                	mov    esi,eax
 804897a:	83 e6 07             	and    esi,0x7
 804897d:	8d 4e 03             	lea    ecx,[esi+0x3]
 8048980:	38 d1                	cmp    cl,dl
 8048982:	0f 9d c2             	setge  dl
 8048985:	21 da                	and    edx,ebx
 8048987:	84 d2                	test   dl,dl
 8048989:	74 09                	je     8048994 <header_handler+0xc0>
 804898b:	83 ec 0c             	sub    esp,0xc
 804898e:	50                   	push   eax
 804898f:	e8 6c fd ff ff       	call   8048700 <__asan_report_load4@plt>
 8048994:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 8048997:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
 804899a:	57                   	push   edi
 804899b:	ff 75 d4             	push   DWORD PTR [ebp-0x2c]
 804899e:	50                   	push   eax
 804899f:	68 a0 90 04 08       	push   0x80490a0
 80489a4:	e8 07 fd ff ff       	call   80486b0 <printf@plt>
 80489a9:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:118
 80489ac:	c7 45 e4 00 00 00 00 	mov    DWORD PTR [ebp-0x1c],0x0
 80489b3:	e9 a3 01 00 00       	jmp    8048b5b <header_handler+0x287>
/home/kyoupark/git/kb/asan/gbo_recover.c:120 (discriminator 3)
 80489b8:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 80489bb:	89 c2                	mov    edx,eax
 80489bd:	c1 ea 03             	shr    edx,0x3
 80489c0:	81 c2 00 00 00 20    	add    edx,0x20000000
 80489c6:	0f b6 12             	movzx  edx,BYTE PTR [edx]
 80489c9:	84 d2                	test   dl,dl
 80489cb:	0f 95 c1             	setne  cl
 80489ce:	89 c3                	mov    ebx,eax
 80489d0:	83 e3 07             	and    ebx,0x7
 80489d3:	83 c3 03             	add    ebx,0x3
 80489d6:	38 d3                	cmp    bl,dl
 80489d8:	0f 9d c2             	setge  dl
 80489db:	21 ca                	and    edx,ecx
 80489dd:	84 d2                	test   dl,dl
 80489df:	74 09                	je     80489ea <header_handler+0x116>
 80489e1:	83 ec 0c             	sub    esp,0xc
 80489e4:	50                   	push   eax
 80489e5:	e8 16 fd ff ff       	call   8048700 <__asan_report_load4@plt>
 80489ea:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 80489ed:	8b 00                	mov    eax,DWORD PTR [eax]
 80489ef:	89 45 d4             	mov    DWORD PTR [ebp-0x2c],eax
 80489f2:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 80489f5:	83 c0 08             	add    eax,0x8
 80489f8:	89 c2                	mov    edx,eax
 80489fa:	c1 ea 03             	shr    edx,0x3
 80489fd:	81 c2 00 00 00 20    	add    edx,0x20000000
 8048a03:	0f b6 12             	movzx  edx,BYTE PTR [edx]
 8048a06:	84 d2                	test   dl,dl
 8048a08:	0f 95 c1             	setne  cl
 8048a0b:	89 c6                	mov    esi,eax
 8048a0d:	83 e6 07             	and    esi,0x7
 8048a10:	8d 5e 03             	lea    ebx,[esi+0x3]
 8048a13:	38 d3                	cmp    bl,dl
 8048a15:	0f 9d c2             	setge  dl
 8048a18:	21 ca                	and    edx,ecx
 8048a1a:	84 d2                	test   dl,dl
 8048a1c:	74 09                	je     8048a27 <header_handler+0x153>
 8048a1e:	83 ec 0c             	sub    esp,0xc
 8048a21:	50                   	push   eax
 8048a22:	e8 d9 fc ff ff       	call   8048700 <__asan_report_load4@plt>
 8048a27:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 8048a2a:	8b 40 08             	mov    eax,DWORD PTR [eax+0x8]
 8048a2d:	8b 55 e4             	mov    edx,DWORD PTR [ebp-0x1c]
 8048a30:	c1 e2 05             	shl    edx,0x5
 8048a33:	8d 3c 02             	lea    edi,[edx+eax*1]
 8048a36:	8d 47 08             	lea    eax,[edi+0x8]
 8048a39:	89 c1                	mov    ecx,eax
 8048a3b:	c1 e9 03             	shr    ecx,0x3
 8048a3e:	81 c1 00 00 00 20    	add    ecx,0x20000000
 8048a44:	0f b6 09             	movzx  ecx,BYTE PTR [ecx]
 8048a47:	84 c9                	test   cl,cl
 8048a49:	0f 95 45 d3          	setne  BYTE PTR [ebp-0x2d]
 8048a4d:	0f b6 5d d3          	movzx  ebx,BYTE PTR [ebp-0x2d]
 8048a51:	89 c6                	mov    esi,eax
 8048a53:	83 e6 07             	and    esi,0x7
 8048a56:	8d 56 03             	lea    edx,[esi+0x3]
 8048a59:	38 ca                	cmp    dl,cl
 8048a5b:	0f 9d c1             	setge  cl
 8048a5e:	21 d9                	and    ecx,ebx
 8048a60:	84 c9                	test   cl,cl
 8048a62:	74 09                	je     8048a6d <header_handler+0x199>
 8048a64:	83 ec 0c             	sub    esp,0xc
 8048a67:	50                   	push   eax
 8048a68:	e8 93 fc ff ff       	call   8048700 <__asan_report_load4@plt>
 8048a6d:	8b 47 08             	mov    eax,DWORD PTR [edi+0x8]
 8048a70:	03 45 d4             	add    eax,DWORD PTR [ebp-0x2c]
/home/kyoupark/git/kb/asan/gbo_recover.c:119
 8048a73:	83 ec 04             	sub    esp,0x4
 8048a76:	50                   	push   eax
 8048a77:	ff 75 e4             	push   DWORD PTR [ebp-0x1c]
 8048a7a:	68 00 91 04 08       	push   0x8049100
 8048a7f:	e8 2c fc ff ff       	call   80486b0 <printf@plt>
 8048a84:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:122
 8048a87:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 8048a8a:	83 c0 08             	add    eax,0x8
 8048a8d:	89 c2                	mov    edx,eax
 8048a8f:	c1 ea 03             	shr    edx,0x3
 8048a92:	81 c2 00 00 00 20    	add    edx,0x20000000
 8048a98:	0f b6 12             	movzx  edx,BYTE PTR [edx]
 8048a9b:	84 d2                	test   dl,dl
 8048a9d:	0f 95 c1             	setne  cl
 8048aa0:	89 c3                	mov    ebx,eax
 8048aa2:	83 e3 07             	and    ebx,0x7
 8048aa5:	83 c3 03             	add    ebx,0x3
 8048aa8:	38 d3                	cmp    bl,dl
 8048aaa:	0f 9d c2             	setge  dl
 8048aad:	21 ca                	and    edx,ecx
 8048aaf:	84 d2                	test   dl,dl
 8048ab1:	74 09                	je     8048abc <header_handler+0x1e8>
 8048ab3:	83 ec 0c             	sub    esp,0xc
 8048ab6:	50                   	push   eax
 8048ab7:	e8 44 fc ff ff       	call   8048700 <__asan_report_load4@plt>
 8048abc:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 8048abf:	8b 40 08             	mov    eax,DWORD PTR [eax+0x8]
 8048ac2:	8b 55 e4             	mov    edx,DWORD PTR [ebp-0x1c]
 8048ac5:	c1 e2 05             	shl    edx,0x5
 8048ac8:	01 c2                	add    edx,eax
/home/kyoupark/git/kb/asan/gbo_recover.c:121
 8048aca:	8d 42 18             	lea    eax,[edx+0x18]
 8048acd:	89 c1                	mov    ecx,eax
 8048acf:	c1 e9 03             	shr    ecx,0x3
 8048ad2:	81 c1 00 00 00 20    	add    ecx,0x20000000
 8048ad8:	0f b6 09             	movzx  ecx,BYTE PTR [ecx]
 8048adb:	84 c9                	test   cl,cl
 8048add:	0f 95 45 d4          	setne  BYTE PTR [ebp-0x2c]
 8048ae1:	89 c6                	mov    esi,eax
 8048ae3:	83 e6 07             	and    esi,0x7
 8048ae6:	8d 5e 03             	lea    ebx,[esi+0x3]
 8048ae9:	38 cb                	cmp    bl,cl
 8048aeb:	0f 9d c1             	setge  cl
 8048aee:	22 4d d4             	and    cl,BYTE PTR [ebp-0x2c]
 8048af1:	84 c9                	test   cl,cl
 8048af3:	74 09                	je     8048afe <header_handler+0x22a>
 8048af5:	83 ec 0c             	sub    esp,0xc
 8048af8:	50                   	push   eax
 8048af9:	e8 02 fc ff ff       	call   8048700 <__asan_report_load4@plt>
 8048afe:	8b 7a 18             	mov    edi,DWORD PTR [edx+0x18]
/home/kyoupark/git/kb/asan/gbo_recover.c:122
 8048b01:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 8048b04:	8b 40 08             	mov    eax,DWORD PTR [eax+0x8]
 8048b07:	8b 55 e4             	mov    edx,DWORD PTR [ebp-0x1c]
 8048b0a:	c1 e2 05             	shl    edx,0x5
 8048b0d:	8d 0c 10             	lea    ecx,[eax+edx*1]
/home/kyoupark/git/kb/asan/gbo_recover.c:121
 8048b10:	89 c8                	mov    eax,ecx
 8048b12:	89 c2                	mov    edx,eax
 8048b14:	c1 ea 03             	shr    edx,0x3
 8048b17:	81 c2 00 00 00 20    	add    edx,0x20000000
 8048b1d:	0f b6 12             	movzx  edx,BYTE PTR [edx]
 8048b20:	84 d2                	test   dl,dl
 8048b22:	0f 95 45 d4          	setne  BYTE PTR [ebp-0x2c]
 8048b26:	89 c6                	mov    esi,eax
 8048b28:	83 e6 07             	and    esi,0x7
 8048b2b:	8d 5e 03             	lea    ebx,[esi+0x3]
 8048b2e:	38 d3                	cmp    bl,dl
 8048b30:	0f 9d c2             	setge  dl
 8048b33:	22 55 d4             	and    dl,BYTE PTR [ebp-0x2c]
 8048b36:	84 d2                	test   dl,dl
 8048b38:	74 09                	je     8048b43 <header_handler+0x26f>
 8048b3a:	83 ec 0c             	sub    esp,0xc
 8048b3d:	50                   	push   eax
 8048b3e:	e8 bd fb ff ff       	call   8048700 <__asan_report_load4@plt>
 8048b43:	8b 01                	mov    eax,DWORD PTR [ecx]
 8048b45:	83 ec 04             	sub    esp,0x4
 8048b48:	57                   	push   edi
 8048b49:	50                   	push   eax
 8048b4a:	68 40 91 04 08       	push   0x8049140
 8048b4f:	e8 5c fb ff ff       	call   80486b0 <printf@plt>
 8048b54:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:118
 8048b57:	83 45 e4 01          	add    DWORD PTR [ebp-0x1c],0x1
 8048b5b:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 8048b5e:	83 c0 0c             	add    eax,0xc
 8048b61:	89 c2                	mov    edx,eax
 8048b63:	c1 ea 03             	shr    edx,0x3
 8048b66:	81 c2 00 00 00 20    	add    edx,0x20000000
 8048b6c:	0f b6 12             	movzx  edx,BYTE PTR [edx]
 8048b6f:	84 d2                	test   dl,dl
 8048b71:	0f 95 c1             	setne  cl
 8048b74:	89 c3                	mov    ebx,eax
 8048b76:	83 e3 07             	and    ebx,0x7
 8048b79:	83 c3 01             	add    ebx,0x1
 8048b7c:	38 d3                	cmp    bl,dl
 8048b7e:	0f 9d c2             	setge  dl
 8048b81:	21 ca                	and    edx,ecx
 8048b83:	84 d2                	test   dl,dl
 8048b85:	74 09                	je     8048b90 <header_handler+0x2bc>
 8048b87:	83 ec 0c             	sub    esp,0xc
 8048b8a:	50                   	push   eax
 8048b8b:	e8 90 fb ff ff       	call   8048720 <__asan_report_load2@plt>
 8048b90:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 8048b93:	0f b7 40 0c          	movzx  eax,WORD PTR [eax+0xc]
 8048b97:	0f b7 c0             	movzx  eax,ax
 8048b9a:	3b 45 e4             	cmp    eax,DWORD PTR [ebp-0x1c]
 8048b9d:	0f 8f 15 fe ff ff    	jg     80489b8 <header_handler+0xe4>
/home/kyoupark/git/kb/asan/gbo_recover.c:124
 8048ba3:	83 ec 0c             	sub    esp,0xc
 8048ba6:	6a 0a                	push   0xa
 8048ba8:	e8 d3 fb ff ff       	call   8048780 <putchar@plt>
 8048bad:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:125
 8048bb0:	b8 00 00 00 00       	mov    eax,0x0
/home/kyoupark/git/kb/asan/gbo_recover.c:126
 8048bb5:	8d 65 f4             	lea    esp,[ebp-0xc]
 8048bb8:	5b                   	pop    ebx
 8048bb9:	5e                   	pop    esi
 8048bba:	5f                   	pop    edi
 8048bbb:	5d                   	pop    ebp
 8048bbc:	c3                   	ret    

08048bbd <call_normal>:
call_normal():
/home/kyoupark/git/kb/asan/gbo_recover.c:131
 8048bbd:	55                   	push   ebp
 8048bbe:	89 e5                	mov    ebp,esp
 8048bc0:	83 ec 18             	sub    esp,0x18
/home/kyoupark/git/kb/asan/gbo_recover.c:132
 8048bc3:	83 ec 0c             	sub    esp,0xc
 8048bc6:	68 80 91 04 08       	push   0x8049180
 8048bcb:	e8 a0 fb ff ff       	call   8048770 <puts@plt>
 8048bd0:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:133
 8048bd3:	c7 45 f0 00 00 00 00 	mov    DWORD PTR [ebp-0x10],0x0
 8048bda:	c7 45 f4 00 00 00 00 	mov    DWORD PTR [ebp-0xc],0x0
/home/kyoupark/git/kb/asan/gbo_recover.c:135
 8048be1:	eb 0a                	jmp    8048bed <call_normal+0x30>
/home/kyoupark/git/kb/asan/gbo_recover.c:136 (discriminator 2)
 8048be3:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
 8048be6:	01 45 f0             	add    DWORD PTR [ebp-0x10],eax
/home/kyoupark/git/kb/asan/gbo_recover.c:135
 8048be9:	83 45 f4 01          	add    DWORD PTR [ebp-0xc],0x1
 8048bed:	83 7d f4 09          	cmp    DWORD PTR [ebp-0xc],0x9
 8048bf1:	7e f0                	jle    8048be3 <call_normal+0x26>
/home/kyoupark/git/kb/asan/gbo_recover.c:138
 8048bf3:	83 ec 0c             	sub    esp,0xc
 8048bf6:	68 c0 91 04 08       	push   0x80491c0
 8048bfb:	e8 70 fb ff ff       	call   8048770 <puts@plt>
 8048c00:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:139
 8048c03:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
/home/kyoupark/git/kb/asan/gbo_recover.c:140
 8048c06:	c9                   	leave  
 8048c07:	c3                   	ret    

08048c08 <call_gbo_1>:
call_gbo_1():
/home/kyoupark/git/kb/asan/gbo_recover.c:143
 8048c08:	55                   	push   ebp
 8048c09:	89 e5                	mov    ebp,esp
 8048c0b:	57                   	push   edi
 8048c0c:	56                   	push   esi
 8048c0d:	53                   	push   ebx
 8048c0e:	83 ec 6c             	sub    esp,0x6c
 8048c11:	8d 5d 88             	lea    ebx,[ebp-0x78]
 8048c14:	89 df                	mov    edi,ebx
 8048c16:	83 3d 70 b2 04 08 00 	cmp    DWORD PTR ds:0x804b270,0x0
 8048c1d:	74 13                	je     8048c32 <call_gbo_1+0x2a>
 8048c1f:	83 ec 0c             	sub    esp,0xc
 8048c22:	6a 60                	push   0x60
 8048c24:	e8 c7 fa ff ff       	call   80486f0 <__asan_stack_malloc_1@plt>
 8048c29:	83 c4 10             	add    esp,0x10
 8048c2c:	85 c0                	test   eax,eax
 8048c2e:	74 02                	je     8048c32 <call_gbo_1+0x2a>
 8048c30:	89 c3                	mov    ebx,eax
 8048c32:	8d 43 60             	lea    eax,[ebx+0x60]
 8048c35:	89 c6                	mov    esi,eax
 8048c37:	c7 03 b3 8a b5 41    	mov    DWORD PTR [ebx],0x41b58ab3
 8048c3d:	c7 43 04 00 92 04 08 	mov    DWORD PTR [ebx+0x4],0x8049200
 8048c44:	c7 43 08 08 8c 04 08 	mov    DWORD PTR [ebx+0x8],0x8048c08
 8048c4b:	89 d8                	mov    eax,ebx
 8048c4d:	c1 e8 03             	shr    eax,0x3
 8048c50:	c7 80 00 00 00 20 f1 	mov    DWORD PTR [eax+0x20000000],0xf1f1f1f1
 8048c57:	f1 f1 f1 
 8048c5a:	c7 80 04 00 00 20 00 	mov    DWORD PTR [eax+0x20000004],0xf4f40200
 8048c61:	02 f4 f4 
 8048c64:	c7 80 08 00 00 20 f3 	mov    DWORD PTR [eax+0x20000008],0xf3f3f3f3
 8048c6b:	f3 f3 f3 
/home/kyoupark/git/kb/asan/gbo_recover.c:145
 8048c6e:	ba 80 b0 04 08       	mov    edx,0x804b080
 8048c73:	89 d1                	mov    ecx,edx
 8048c75:	c1 e9 03             	shr    ecx,0x3
 8048c78:	81 c1 00 00 00 20    	add    ecx,0x20000000
 8048c7e:	0f b6 09             	movzx  ecx,BYTE PTR [ecx]
 8048c81:	84 c9                	test   cl,cl
 8048c83:	74 09                	je     8048c8e <call_gbo_1+0x86>
 8048c85:	83 ec 0c             	sub    esp,0xc
 8048c88:	52                   	push   edx
 8048c89:	e8 d2 fa ff ff       	call   8048760 <__asan_report_load8@plt>
 8048c8e:	8b 15 80 b0 04 08    	mov    edx,DWORD PTR ds:0x804b080
 8048c94:	8b 0d 84 b0 04 08    	mov    ecx,DWORD PTR ds:0x804b084
 8048c9a:	89 56 c0             	mov    DWORD PTR [esi-0x40],edx
 8048c9d:	89 4e c4             	mov    DWORD PTR [esi-0x3c],ecx
/home/kyoupark/git/kb/asan/gbo_recover.c:146
 8048ca0:	90                   	nop
/home/kyoupark/git/kb/asan/gbo_recover.c:143
 8048ca1:	39 df                	cmp    edi,ebx
 8048ca3:	74 26                	je     8048ccb <call_gbo_1+0xc3>
 8048ca5:	c7 03 0e 36 e0 45    	mov    DWORD PTR [ebx],0x45e0360e
 8048cab:	c7 80 00 00 00 20 f5 	mov    DWORD PTR [eax+0x20000000],0xf5f5f5f5
 8048cb2:	f5 f5 f5 
 8048cb5:	c7 80 04 00 00 20 f5 	mov    DWORD PTR [eax+0x20000004],0xf5f5f5f5
 8048cbc:	f5 f5 f5 
 8048cbf:	c7 80 08 00 00 20 f5 	mov    DWORD PTR [eax+0x20000008],0xf5f5f5f5
 8048cc6:	f5 f5 f5 
 8048cc9:	eb 1e                	jmp    8048ce9 <call_gbo_1+0xe1>
 8048ccb:	c7 80 00 00 00 20 00 	mov    DWORD PTR [eax+0x20000000],0x0
 8048cd2:	00 00 00 
 8048cd5:	c7 80 04 00 00 20 00 	mov    DWORD PTR [eax+0x20000004],0x0
 8048cdc:	00 00 00 
 8048cdf:	c7 80 08 00 00 20 00 	mov    DWORD PTR [eax+0x20000008],0x0
 8048ce6:	00 00 00 
/home/kyoupark/git/kb/asan/gbo_recover.c:147
 8048ce9:	8d 65 f4             	lea    esp,[ebp-0xc]
 8048cec:	5b                   	pop    ebx
 8048ced:	5e                   	pop    esi
 8048cee:	5f                   	pop    edi
 8048cef:	5d                   	pop    ebp
 8048cf0:	c3                   	ret    

08048cf1 <call_gbo_2>:
call_gbo_2():
/home/kyoupark/git/kb/asan/gbo_recover.c:150
 8048cf1:	55                   	push   ebp
 8048cf2:	89 e5                	mov    ebp,esp
 8048cf4:	57                   	push   edi
 8048cf5:	56                   	push   esi
 8048cf6:	53                   	push   ebx
 8048cf7:	83 ec 6c             	sub    esp,0x6c
 8048cfa:	8d 5d 88             	lea    ebx,[ebp-0x78]
 8048cfd:	89 df                	mov    edi,ebx
 8048cff:	83 3d 70 b2 04 08 00 	cmp    DWORD PTR ds:0x804b270,0x0
 8048d06:	74 13                	je     8048d1b <call_gbo_2+0x2a>
 8048d08:	83 ec 0c             	sub    esp,0xc
 8048d0b:	6a 60                	push   0x60
 8048d0d:	e8 de f9 ff ff       	call   80486f0 <__asan_stack_malloc_1@plt>
 8048d12:	83 c4 10             	add    esp,0x10
 8048d15:	85 c0                	test   eax,eax
 8048d17:	74 02                	je     8048d1b <call_gbo_2+0x2a>
 8048d19:	89 c3                	mov    ebx,eax
 8048d1b:	8d 43 60             	lea    eax,[ebx+0x60]
 8048d1e:	89 c6                	mov    esi,eax
 8048d20:	c7 03 b3 8a b5 41    	mov    DWORD PTR [ebx],0x41b58ab3
 8048d26:	c7 43 04 00 92 04 08 	mov    DWORD PTR [ebx+0x4],0x8049200
 8048d2d:	c7 43 08 f1 8c 04 08 	mov    DWORD PTR [ebx+0x8],0x8048cf1
 8048d34:	89 d8                	mov    eax,ebx
 8048d36:	c1 e8 03             	shr    eax,0x3
 8048d39:	c7 80 00 00 00 20 f1 	mov    DWORD PTR [eax+0x20000000],0xf1f1f1f1
 8048d40:	f1 f1 f1 
 8048d43:	c7 80 04 00 00 20 00 	mov    DWORD PTR [eax+0x20000004],0xf4f40200
 8048d4a:	02 f4 f4 
 8048d4d:	c7 80 08 00 00 20 f3 	mov    DWORD PTR [eax+0x20000008],0xf3f3f3f3
 8048d54:	f3 f3 f3 
/home/kyoupark/git/kb/asan/gbo_recover.c:152
 8048d57:	ba 80 b0 04 08       	mov    edx,0x804b080
 8048d5c:	89 d1                	mov    ecx,edx
 8048d5e:	c1 e9 03             	shr    ecx,0x3
 8048d61:	81 c1 00 00 00 20    	add    ecx,0x20000000
 8048d67:	0f b6 09             	movzx  ecx,BYTE PTR [ecx]
 8048d6a:	84 c9                	test   cl,cl
 8048d6c:	74 09                	je     8048d77 <call_gbo_2+0x86>
 8048d6e:	83 ec 0c             	sub    esp,0xc
 8048d71:	52                   	push   edx
 8048d72:	e8 e9 f9 ff ff       	call   8048760 <__asan_report_load8@plt>
 8048d77:	8b 15 80 b0 04 08    	mov    edx,DWORD PTR ds:0x804b080
 8048d7d:	8b 0d 84 b0 04 08    	mov    ecx,DWORD PTR ds:0x804b084
 8048d83:	89 56 c0             	mov    DWORD PTR [esi-0x40],edx
 8048d86:	89 4e c4             	mov    DWORD PTR [esi-0x3c],ecx
/home/kyoupark/git/kb/asan/gbo_recover.c:153
 8048d89:	90                   	nop
/home/kyoupark/git/kb/asan/gbo_recover.c:150
 8048d8a:	39 df                	cmp    edi,ebx
 8048d8c:	74 26                	je     8048db4 <call_gbo_2+0xc3>
 8048d8e:	c7 03 0e 36 e0 45    	mov    DWORD PTR [ebx],0x45e0360e
 8048d94:	c7 80 00 00 00 20 f5 	mov    DWORD PTR [eax+0x20000000],0xf5f5f5f5
 8048d9b:	f5 f5 f5 
 8048d9e:	c7 80 04 00 00 20 f5 	mov    DWORD PTR [eax+0x20000004],0xf5f5f5f5
 8048da5:	f5 f5 f5 
 8048da8:	c7 80 08 00 00 20 f5 	mov    DWORD PTR [eax+0x20000008],0xf5f5f5f5
 8048daf:	f5 f5 f5 
 8048db2:	eb 1e                	jmp    8048dd2 <call_gbo_2+0xe1>
 8048db4:	c7 80 00 00 00 20 00 	mov    DWORD PTR [eax+0x20000000],0x0
 8048dbb:	00 00 00 
 8048dbe:	c7 80 04 00 00 20 00 	mov    DWORD PTR [eax+0x20000004],0x0
 8048dc5:	00 00 00 
 8048dc8:	c7 80 08 00 00 20 00 	mov    DWORD PTR [eax+0x20000008],0x0
 8048dcf:	00 00 00 
/home/kyoupark/git/kb/asan/gbo_recover.c:154
 8048dd2:	8d 65 f4             	lea    esp,[ebp-0xc]
 8048dd5:	5b                   	pop    ebx
 8048dd6:	5e                   	pop    esi
 8048dd7:	5f                   	pop    edi
 8048dd8:	5d                   	pop    ebp
 8048dd9:	c3                   	ret    

08048dda <call_gbo_3>:
call_gbo_3():
/home/kyoupark/git/kb/asan/gbo_recover.c:157
 8048dda:	55                   	push   ebp
 8048ddb:	89 e5                	mov    ebp,esp
 8048ddd:	57                   	push   edi
 8048dde:	56                   	push   esi
 8048ddf:	53                   	push   ebx
 8048de0:	83 ec 6c             	sub    esp,0x6c
 8048de3:	8d 5d 88             	lea    ebx,[ebp-0x78]
 8048de6:	89 df                	mov    edi,ebx
 8048de8:	83 3d 70 b2 04 08 00 	cmp    DWORD PTR ds:0x804b270,0x0
 8048def:	74 13                	je     8048e04 <call_gbo_3+0x2a>
 8048df1:	83 ec 0c             	sub    esp,0xc
 8048df4:	6a 60                	push   0x60
 8048df6:	e8 f5 f8 ff ff       	call   80486f0 <__asan_stack_malloc_1@plt>
 8048dfb:	83 c4 10             	add    esp,0x10
 8048dfe:	85 c0                	test   eax,eax
 8048e00:	74 02                	je     8048e04 <call_gbo_3+0x2a>
 8048e02:	89 c3                	mov    ebx,eax
 8048e04:	8d 43 60             	lea    eax,[ebx+0x60]
 8048e07:	89 c6                	mov    esi,eax
 8048e09:	c7 03 b3 8a b5 41    	mov    DWORD PTR [ebx],0x41b58ab3
 8048e0f:	c7 43 04 00 92 04 08 	mov    DWORD PTR [ebx+0x4],0x8049200
 8048e16:	c7 43 08 da 8d 04 08 	mov    DWORD PTR [ebx+0x8],0x8048dda
 8048e1d:	89 d8                	mov    eax,ebx
 8048e1f:	c1 e8 03             	shr    eax,0x3
 8048e22:	c7 80 00 00 00 20 f1 	mov    DWORD PTR [eax+0x20000000],0xf1f1f1f1
 8048e29:	f1 f1 f1 
 8048e2c:	c7 80 04 00 00 20 00 	mov    DWORD PTR [eax+0x20000004],0xf4f40200
 8048e33:	02 f4 f4 
 8048e36:	c7 80 08 00 00 20 f3 	mov    DWORD PTR [eax+0x20000008],0xf3f3f3f3
 8048e3d:	f3 f3 f3 
/home/kyoupark/git/kb/asan/gbo_recover.c:159
 8048e40:	ba 80 b0 04 08       	mov    edx,0x804b080
 8048e45:	89 d1                	mov    ecx,edx
 8048e47:	c1 e9 03             	shr    ecx,0x3
 8048e4a:	81 c1 00 00 00 20    	add    ecx,0x20000000
 8048e50:	0f b6 09             	movzx  ecx,BYTE PTR [ecx]
 8048e53:	84 c9                	test   cl,cl
 8048e55:	74 09                	je     8048e60 <call_gbo_3+0x86>
 8048e57:	83 ec 0c             	sub    esp,0xc
 8048e5a:	52                   	push   edx
 8048e5b:	e8 00 f9 ff ff       	call   8048760 <__asan_report_load8@plt>
 8048e60:	8b 15 80 b0 04 08    	mov    edx,DWORD PTR ds:0x804b080
 8048e66:	8b 0d 84 b0 04 08    	mov    ecx,DWORD PTR ds:0x804b084
 8048e6c:	89 56 c0             	mov    DWORD PTR [esi-0x40],edx
 8048e6f:	89 4e c4             	mov    DWORD PTR [esi-0x3c],ecx
/home/kyoupark/git/kb/asan/gbo_recover.c:160
 8048e72:	90                   	nop
/home/kyoupark/git/kb/asan/gbo_recover.c:157
 8048e73:	39 df                	cmp    edi,ebx
 8048e75:	74 26                	je     8048e9d <call_gbo_3+0xc3>
 8048e77:	c7 03 0e 36 e0 45    	mov    DWORD PTR [ebx],0x45e0360e
 8048e7d:	c7 80 00 00 00 20 f5 	mov    DWORD PTR [eax+0x20000000],0xf5f5f5f5
 8048e84:	f5 f5 f5 
 8048e87:	c7 80 04 00 00 20 f5 	mov    DWORD PTR [eax+0x20000004],0xf5f5f5f5
 8048e8e:	f5 f5 f5 
 8048e91:	c7 80 08 00 00 20 f5 	mov    DWORD PTR [eax+0x20000008],0xf5f5f5f5
 8048e98:	f5 f5 f5 
 8048e9b:	eb 1e                	jmp    8048ebb <call_gbo_3+0xe1>
 8048e9d:	c7 80 00 00 00 20 00 	mov    DWORD PTR [eax+0x20000000],0x0
 8048ea4:	00 00 00 
 8048ea7:	c7 80 04 00 00 20 00 	mov    DWORD PTR [eax+0x20000004],0x0
 8048eae:	00 00 00 
 8048eb1:	c7 80 08 00 00 20 00 	mov    DWORD PTR [eax+0x20000008],0x0
 8048eb8:	00 00 00 
/home/kyoupark/git/kb/asan/gbo_recover.c:161
 8048ebb:	8d 65 f4             	lea    esp,[ebp-0xc]
 8048ebe:	5b                   	pop    ebx
 8048ebf:	5e                   	pop    esi
 8048ec0:	5f                   	pop    edi
 8048ec1:	5d                   	pop    ebp
 8048ec2:	c3                   	ret    

08048ec3 <main>:
main():
/home/kyoupark/git/kb/asan/gbo_recover.c:164
 8048ec3:	8d 4c 24 04          	lea    ecx,[esp+0x4]
 8048ec7:	83 e4 f0             	and    esp,0xfffffff0
 8048eca:	ff 71 fc             	push   DWORD PTR [ecx-0x4]
 8048ecd:	55                   	push   ebp
 8048ece:	89 e5                	mov    ebp,esp
 8048ed0:	51                   	push   ecx
 8048ed1:	83 ec 04             	sub    esp,0x4
/home/kyoupark/git/kb/asan/gbo_recover.c:165
 8048ed4:	83 ec 08             	sub    esp,0x8
 8048ed7:	6a 00                	push   0x0
 8048ed9:	68 d4 88 04 08       	push   0x80488d4
 8048ede:	e8 fd f7 ff ff       	call   80486e0 <dl_iterate_phdr@plt>
 8048ee3:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:167
 8048ee6:	83 ec 0c             	sub    esp,0xc
 8048ee9:	68 20 92 04 08       	push   0x8049220
 8048eee:	e8 7d f8 ff ff       	call   8048770 <puts@plt>
 8048ef3:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:168
 8048ef6:	83 ec 0c             	sub    esp,0xc
 8048ef9:	68 60 92 04 08       	push   0x8049260
 8048efe:	e8 6d f8 ff ff       	call   8048770 <puts@plt>
 8048f03:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:169
 8048f06:	83 ec 0c             	sub    esp,0xc
 8048f09:	68 c0 92 04 08       	push   0x80492c0
 8048f0e:	e8 5d f8 ff ff       	call   8048770 <puts@plt>
 8048f13:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:171
 8048f16:	83 ec 0c             	sub    esp,0xc
 8048f19:	68 00 93 04 08       	push   0x8049300
 8048f1e:	e8 4d f8 ff ff       	call   8048770 <puts@plt>
 8048f23:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:172
 8048f26:	e8 dd fc ff ff       	call   8048c08 <call_gbo_1>
/home/kyoupark/git/kb/asan/gbo_recover.c:174
 8048f2b:	83 ec 0c             	sub    esp,0xc
 8048f2e:	68 40 93 04 08       	push   0x8049340
 8048f33:	e8 38 f8 ff ff       	call   8048770 <puts@plt>
 8048f38:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:175
 8048f3b:	e8 b1 fd ff ff       	call   8048cf1 <call_gbo_2>
/home/kyoupark/git/kb/asan/gbo_recover.c:177
 8048f40:	83 ec 0c             	sub    esp,0xc
 8048f43:	68 80 93 04 08       	push   0x8049380
 8048f48:	e8 23 f8 ff ff       	call   8048770 <puts@plt>
 8048f4d:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:178
 8048f50:	e8 85 fe ff ff       	call   8048dda <call_gbo_3>
/home/kyoupark/git/kb/asan/gbo_recover.c:180
 8048f55:	83 ec 0c             	sub    esp,0xc
 8048f58:	68 c0 93 04 08       	push   0x80493c0
 8048f5d:	e8 0e f8 ff ff       	call   8048770 <puts@plt>
 8048f62:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:181
 8048f65:	e8 53 fc ff ff       	call   8048bbd <call_normal>
/home/kyoupark/git/kb/asan/gbo_recover.c:183
 8048f6a:	83 ec 0c             	sub    esp,0xc
 8048f6d:	68 00 94 04 08       	push   0x8049400
 8048f72:	e8 f9 f7 ff ff       	call   8048770 <puts@plt>
 8048f77:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:184
 8048f7a:	b8 00 00 00 00       	mov    eax,0x0
/home/kyoupark/git/kb/asan/gbo_recover.c:185
 8048f7f:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
 8048f82:	c9                   	leave  
 8048f83:	8d 61 fc             	lea    esp,[ecx-0x4]
 8048f86:	c3                   	ret    

08048f87 <_GLOBAL__sub_D_00099_0_call_normal>:
_GLOBAL__sub_D_00099_0_call_normal():
 8048f87:	55                   	push   ebp
 8048f88:	89 e5                	mov    ebp,esp
 8048f8a:	83 ec 08             	sub    esp,0x8
 8048f8d:	83 ec 08             	sub    esp,0x8
 8048f90:	6a 0e                	push   0xe
 8048f92:	68 e0 b0 04 08       	push   0x804b0e0
 8048f97:	e8 94 f7 ff ff       	call   8048730 <__asan_unregister_globals@plt>
 8048f9c:	83 c4 10             	add    esp,0x10
 8048f9f:	c9                   	leave  
 8048fa0:	c3                   	ret    

08048fa1 <_GLOBAL__sub_I_00099_1_call_normal>:
_GLOBAL__sub_I_00099_1_call_normal():
 8048fa1:	55                   	push   ebp
 8048fa2:	89 e5                	mov    ebp,esp
 8048fa4:	83 ec 08             	sub    esp,0x8
 8048fa7:	e8 94 f7 ff ff       	call   8048740 <__asan_init@plt>
 8048fac:	e8 9f f7 ff ff       	call   8048750 <__asan_version_mismatch_check_v6@plt>
 8048fb1:	83 ec 08             	sub    esp,0x8
 8048fb4:	6a 0e                	push   0xe
 8048fb6:	68 e0 b0 04 08       	push   0x804b0e0
 8048fbb:	e8 00 f7 ff ff       	call   80486c0 <__asan_register_globals@plt>
 8048fc0:	83 c4 10             	add    esp,0x10
 8048fc3:	c9                   	leave  
 8048fc4:	c3                   	ret    
 8048fc5:	66 90                	xchg   ax,ax
 8048fc7:	66 90                	xchg   ax,ax
 8048fc9:	66 90                	xchg   ax,ax
 8048fcb:	66 90                	xchg   ax,ax
 8048fcd:	66 90                	xchg   ax,ax
 8048fcf:	90                   	nop

08048fd0 <__libc_csu_init>:
__libc_csu_init():
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/elf-init.c:68
 8048fd0:	55                   	push   ebp
 8048fd1:	57                   	push   edi
 8048fd2:	56                   	push   esi
 8048fd3:	53                   	push   ebx
 8048fd4:	e8 07 f8 ff ff       	call   80487e0 <__x86.get_pc_thunk.bx>
 8048fd9:	81 c3 27 20 00 00    	add    ebx,0x2027
 8048fdf:	83 ec 2c             	sub    esp,0x2c
 8048fe2:	8b 6c 24 40          	mov    ebp,DWORD PTR [esp+0x40]
 8048fe6:	8b 7c 24 44          	mov    edi,DWORD PTR [esp+0x44]
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/elf-init.c:83
 8048fea:	e8 81 f6 ff ff       	call   8048670 <_init>
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/elf-init.c:86
 8048fef:	8d 83 f0 fe ff ff    	lea    eax,[ebx-0x110]
 8048ff5:	8d 93 f4 fe ff ff    	lea    edx,[ebx-0x10c]
 8048ffb:	29 c2                	sub    edx,eax
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/elf-init.c:87
 8048ffd:	c1 fa 02             	sar    edx,0x2
 8049000:	89 54 24 1c          	mov    DWORD PTR [esp+0x1c],edx
 8049004:	74 27                	je     804902d <__libc_csu_init+0x5d>
 8049006:	31 f6                	xor    esi,esi
 8049008:	90                   	nop
 8049009:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/elf-init.c:88 (discriminator 3)
 8049010:	8b 44 24 48          	mov    eax,DWORD PTR [esp+0x48]
 8049014:	89 7c 24 04          	mov    DWORD PTR [esp+0x4],edi
 8049018:	89 2c 24             	mov    DWORD PTR [esp],ebp
 804901b:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
 804901f:	ff 94 b3 f0 fe ff ff 	call   DWORD PTR [ebx+esi*4-0x110]
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/elf-init.c:87 (discriminator 3)
 8049026:	46                   	inc    esi
 8049027:	39 74 24 1c          	cmp    DWORD PTR [esp+0x1c],esi
 804902b:	75 e3                	jne    8049010 <__libc_csu_init+0x40>
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/elf-init.c:89
 804902d:	83 c4 2c             	add    esp,0x2c
 8049030:	5b                   	pop    ebx
 8049031:	5e                   	pop    esi
 8049032:	5f                   	pop    edi
 8049033:	5d                   	pop    ebp
 8049034:	c3                   	ret    
 8049035:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
 8049039:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

08049040 <__libc_csu_fini>:
__libc_csu_fini():
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/elf-init.c:96
 8049040:	c3                   	ret    

08049041 <__do_global_ctors_aux>:
__do_global_ctors_aux():
 8049041:	55                   	push   ebp
 8049042:	89 e5                	mov    ebp,esp
 8049044:	53                   	push   ebx
 8049045:	bb f8 ae 04 08       	mov    ebx,0x804aef8
 804904a:	52                   	push   edx
 804904b:	8b 03                	mov    eax,DWORD PTR [ebx]
 804904d:	83 f8 ff             	cmp    eax,0xffffffff
 8049050:	74 07                	je     8049059 <__do_global_ctors_aux+0x18>
 8049052:	ff d0                	call   eax
 8049054:	83 eb 04             	sub    ebx,0x4
 8049057:	eb f2                	jmp    804904b <__do_global_ctors_aux+0xa>
 8049059:	58                   	pop    eax
 804905a:	5b                   	pop    ebx
 804905b:	5d                   	pop    ebp
 804905c:	c3                   	ret    

Disassembly of section .fini:

08049060 <_fini>:
_fini():
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crti.S:82
 8049060:	53                   	push   ebx
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crti.S:83
 8049061:	83 ec 08             	sub    esp,0x8
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crti.S:84
 8049064:	e8 77 f7 ff ff       	call   80487e0 <__x86.get_pc_thunk.bx>
 8049069:	81 c3 97 1f 00 00    	add    ebx,0x1f97
 804906f:	e8 c3 f7 ff ff       	call   8048837 <__do_global_dtors_aux>
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crtn.S:45
 8049074:	83 c4 08             	add    esp,0x8
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crtn.S:46
 8049077:	5b                   	pop    ebx
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crtn.S:47
 8049078:	c3                   	ret    
