
gbo_32_630_asan_recover:     file format elf32-i386


Disassembly of section .init:

08048618 <_init>:
_init():
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crti.S:63
 8048618:	53                   	push   ebx
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crti.S:65
 8048619:	83 ec 08             	sub    esp,0x8
 804861c:	e8 4f 01 00 00       	call   8048770 <__x86.get_pc_thunk.bx>
 8048621:	81 c3 df 29 00 00    	add    ebx,0x29df
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crti.S:68
 8048627:	8b 83 fc ff ff ff    	mov    eax,DWORD PTR [ebx-0x4]
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crti.S:69
 804862d:	85 c0                	test   eax,eax
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crti.S:70
 804862f:	74 05                	je     8048636 <_init+0x1e>
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crti.S:71
 8048631:	e8 fa 00 00 00       	call   8048730 <.plt.got>
 8048636:	e8 c1 01 00 00       	call   80487fc <frame_dummy>
 804863b:	e8 71 09 00 00       	call   8048fb1 <__do_global_ctors_aux>
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crtn.S:40
 8048640:	83 c4 08             	add    esp,0x8
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crtn.S:41
 8048643:	5b                   	pop    ebx
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crtn.S:42
 8048644:	c3                   	ret    

Disassembly of section .plt:

08048650 <.plt>:
 8048650:	ff 35 04 b0 04 08    	push   DWORD PTR ds:0x804b004
 8048656:	ff 25 08 b0 04 08    	jmp    DWORD PTR ds:0x804b008
 804865c:	00 00                	add    BYTE PTR [eax],al
	...

08048660 <printf@plt>:
 8048660:	ff 25 0c b0 04 08    	jmp    DWORD PTR ds:0x804b00c
 8048666:	68 00 00 00 00       	push   0x0
 804866b:	e9 e0 ff ff ff       	jmp    8048650 <.plt>

08048670 <__asan_register_globals@plt>:
 8048670:	ff 25 10 b0 04 08    	jmp    DWORD PTR ds:0x804b010
 8048676:	68 08 00 00 00       	push   0x8
 804867b:	e9 d0 ff ff ff       	jmp    8048650 <.plt>

08048680 <dl_iterate_phdr@plt>:
 8048680:	ff 25 14 b0 04 08    	jmp    DWORD PTR ds:0x804b014
 8048686:	68 10 00 00 00       	push   0x10
 804868b:	e9 c0 ff ff ff       	jmp    8048650 <.plt>

08048690 <__asan_stack_malloc_1@plt>:
 8048690:	ff 25 18 b0 04 08    	jmp    DWORD PTR ds:0x804b018
 8048696:	68 18 00 00 00       	push   0x18
 804869b:	e9 b0 ff ff ff       	jmp    8048650 <.plt>

080486a0 <__asan_report_load4_noabort@plt>:
 80486a0:	ff 25 1c b0 04 08    	jmp    DWORD PTR ds:0x804b01c
 80486a6:	68 20 00 00 00       	push   0x20
 80486ab:	e9 a0 ff ff ff       	jmp    8048650 <.plt>

080486b0 <__libc_start_main@plt>:
 80486b0:	ff 25 20 b0 04 08    	jmp    DWORD PTR ds:0x804b020
 80486b6:	68 28 00 00 00       	push   0x28
 80486bb:	e9 90 ff ff ff       	jmp    8048650 <.plt>

080486c0 <__asan_unregister_globals@plt>:
 80486c0:	ff 25 24 b0 04 08    	jmp    DWORD PTR ds:0x804b024
 80486c6:	68 30 00 00 00       	push   0x30
 80486cb:	e9 80 ff ff ff       	jmp    8048650 <.plt>

080486d0 <__asan_init@plt>:
 80486d0:	ff 25 28 b0 04 08    	jmp    DWORD PTR ds:0x804b028
 80486d6:	68 38 00 00 00       	push   0x38
 80486db:	e9 70 ff ff ff       	jmp    8048650 <.plt>

080486e0 <__asan_version_mismatch_check_v6@plt>:
 80486e0:	ff 25 2c b0 04 08    	jmp    DWORD PTR ds:0x804b02c
 80486e6:	68 40 00 00 00       	push   0x40
 80486eb:	e9 60 ff ff ff       	jmp    8048650 <.plt>

080486f0 <puts@plt>:
 80486f0:	ff 25 30 b0 04 08    	jmp    DWORD PTR ds:0x804b030
 80486f6:	68 48 00 00 00       	push   0x48
 80486fb:	e9 50 ff ff ff       	jmp    8048650 <.plt>

08048700 <__asan_report_load8_noabort@plt>:
 8048700:	ff 25 34 b0 04 08    	jmp    DWORD PTR ds:0x804b034
 8048706:	68 50 00 00 00       	push   0x50
 804870b:	e9 40 ff ff ff       	jmp    8048650 <.plt>

08048710 <__asan_report_load2_noabort@plt>:
 8048710:	ff 25 38 b0 04 08    	jmp    DWORD PTR ds:0x804b038
 8048716:	68 58 00 00 00       	push   0x58
 804871b:	e9 30 ff ff ff       	jmp    8048650 <.plt>

08048720 <putchar@plt>:
 8048720:	ff 25 3c b0 04 08    	jmp    DWORD PTR ds:0x804b03c
 8048726:	68 60 00 00 00       	push   0x60
 804872b:	e9 20 ff ff ff       	jmp    8048650 <.plt>

Disassembly of section .plt.got:

08048730 <.plt.got>:
 8048730:	ff 25 fc af 04 08    	jmp    DWORD PTR ds:0x804affc
 8048736:	66 90                	xchg   ax,ax

Disassembly of section .text:

08048740 <_start>:
_start():
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/start.S:61
 8048740:	31 ed                	xor    ebp,ebp
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/start.S:66
 8048742:	5e                   	pop    esi
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/start.S:67
 8048743:	89 e1                	mov    ecx,esp
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/start.S:73
 8048745:	83 e4 f0             	and    esp,0xfffffff0
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/start.S:74
 8048748:	50                   	push   eax
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/start.S:79
 8048749:	54                   	push   esp
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/start.S:81
 804874a:	52                   	push   edx
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/start.S:105
 804874b:	68 b0 8f 04 08       	push   0x8048fb0
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/start.S:106
 8048750:	68 40 8f 04 08       	push   0x8048f40
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/start.S:108
 8048755:	51                   	push   ecx
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/start.S:109
 8048756:	56                   	push   esi
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/start.S:111
 8048757:	68 31 8e 04 08       	push   0x8048e31
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/start.S:115
 804875c:	e8 4f ff ff ff       	call   80486b0 <__libc_start_main@plt>
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/start.S:118
 8048761:	f4                   	hlt    
 8048762:	66 90                	xchg   ax,ax
 8048764:	66 90                	xchg   ax,ax
 8048766:	66 90                	xchg   ax,ax
 8048768:	66 90                	xchg   ax,ax
 804876a:	66 90                	xchg   ax,ax
 804876c:	66 90                	xchg   ax,ax
 804876e:	66 90                	xchg   ax,ax

08048770 <__x86.get_pc_thunk.bx>:
__x86.get_pc_thunk.bx():
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crti.S:66
 8048770:	8b 1c 24             	mov    ebx,DWORD PTR [esp]
 8048773:	c3                   	ret    

08048774 <deregister_tm_clones>:
deregister_tm_clones():
 8048774:	83 3d 48 b2 04 08 00 	cmp    DWORD PTR ds:0x804b248,0x0
 804877b:	74 1a                	je     8048797 <deregister_tm_clones+0x23>
 804877d:	b8 00 00 00 00       	mov    eax,0x0
 8048782:	85 c0                	test   eax,eax
 8048784:	74 11                	je     8048797 <deregister_tm_clones+0x23>
 8048786:	55                   	push   ebp
 8048787:	89 e5                	mov    ebp,esp
 8048789:	83 ec 14             	sub    esp,0x14
 804878c:	68 48 b2 04 08       	push   0x804b248
 8048791:	ff d0                	call   eax
 8048793:	83 c4 10             	add    esp,0x10
 8048796:	c9                   	leave  
 8048797:	c3                   	ret    

08048798 <register_tm_clones>:
register_tm_clones():
 8048798:	31 c0                	xor    eax,eax
 804879a:	83 3c c5 48 b2 04 08 	cmp    DWORD PTR [eax*8+0x804b248],0x0
 80487a1:	00 
 80487a2:	74 03                	je     80487a7 <register_tm_clones+0xf>
 80487a4:	40                   	inc    eax
 80487a5:	eb f3                	jmp    804879a <register_tm_clones+0x2>
 80487a7:	85 c0                	test   eax,eax
 80487a9:	74 1b                	je     80487c6 <register_tm_clones+0x2e>
 80487ab:	ba 00 00 00 00       	mov    edx,0x0
 80487b0:	85 d2                	test   edx,edx
 80487b2:	74 12                	je     80487c6 <register_tm_clones+0x2e>
 80487b4:	55                   	push   ebp
 80487b5:	89 e5                	mov    ebp,esp
 80487b7:	83 ec 10             	sub    esp,0x10
 80487ba:	50                   	push   eax
 80487bb:	68 48 b2 04 08       	push   0x804b248
 80487c0:	ff d2                	call   edx
 80487c2:	83 c4 10             	add    esp,0x10
 80487c5:	c9                   	leave  
 80487c6:	c3                   	ret    

080487c7 <__do_global_dtors_aux>:
__do_global_dtors_aux():
 80487c7:	80 3d 54 b2 04 08 00 	cmp    BYTE PTR ds:0x804b254,0x0
 80487ce:	74 01                	je     80487d1 <__do_global_dtors_aux+0xa>
 80487d0:	c3                   	ret    
 80487d1:	55                   	push   ebp
 80487d2:	89 e5                	mov    ebp,esp
 80487d4:	83 ec 08             	sub    esp,0x8
 80487d7:	a1 48 b0 04 08       	mov    eax,ds:0x804b048
 80487dc:	8b 10                	mov    edx,DWORD PTR [eax]
 80487de:	85 d2                	test   edx,edx
 80487e0:	74 0c                	je     80487ee <__do_global_dtors_aux+0x27>
 80487e2:	83 c0 04             	add    eax,0x4
 80487e5:	a3 48 b0 04 08       	mov    ds:0x804b048,eax
 80487ea:	ff d2                	call   edx
 80487ec:	eb e9                	jmp    80487d7 <__do_global_dtors_aux+0x10>
 80487ee:	e8 81 ff ff ff       	call   8048774 <deregister_tm_clones>
 80487f3:	c6 05 54 b2 04 08 01 	mov    BYTE PTR ds:0x804b254,0x1
 80487fa:	c9                   	leave  
 80487fb:	c3                   	ret    

080487fc <frame_dummy>:
frame_dummy():
 80487fc:	ba f8 ae 04 08       	mov    edx,0x804aef8
 8048801:	83 3a 00             	cmp    DWORD PTR [edx],0x0
 8048804:	74 16                	je     804881c <frame_dummy+0x20>
 8048806:	b8 00 00 00 00       	mov    eax,0x0
 804880b:	85 c0                	test   eax,eax
 804880d:	74 0d                	je     804881c <frame_dummy+0x20>
 804880f:	55                   	push   ebp
 8048810:	89 e5                	mov    ebp,esp
 8048812:	83 ec 14             	sub    esp,0x14
 8048815:	52                   	push   edx
 8048816:	ff d0                	call   eax
 8048818:	83 c4 10             	add    esp,0x10
 804881b:	c9                   	leave  
 804881c:	e9 77 ff ff ff       	jmp    8048798 <register_tm_clones>

08048821 <header_handler>:
header_handler():
/home/kyoupark/git/kb/asan/gbo_recover.c:114
 8048821:	55                   	push   ebp
 8048822:	89 e5                	mov    ebp,esp
 8048824:	57                   	push   edi
 8048825:	56                   	push   esi
 8048826:	53                   	push   ebx
 8048827:	83 ec 2c             	sub    esp,0x2c
/home/kyoupark/git/kb/asan/gbo_recover.c:115
 804882a:	c7 45 e4 00 00 00 00 	mov    DWORD PTR [ebp-0x1c],0x0
/home/kyoupark/git/kb/asan/gbo_recover.c:117
 8048831:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 8048834:	89 c2                	mov    edx,eax
 8048836:	c1 ea 03             	shr    edx,0x3
 8048839:	81 c2 00 00 00 20    	add    edx,0x20000000
 804883f:	0f b6 12             	movzx  edx,BYTE PTR [edx]
 8048842:	84 d2                	test   dl,dl
 8048844:	0f 95 c1             	setne  cl
 8048847:	89 c3                	mov    ebx,eax
 8048849:	83 e3 07             	and    ebx,0x7
 804884c:	83 c3 03             	add    ebx,0x3
 804884f:	38 d3                	cmp    bl,dl
 8048851:	0f 9d c2             	setge  dl
 8048854:	21 ca                	and    edx,ecx
 8048856:	84 d2                	test   dl,dl
 8048858:	74 0c                	je     8048866 <header_handler+0x45>
 804885a:	83 ec 0c             	sub    esp,0xc
 804885d:	50                   	push   eax
 804885e:	e8 3d fe ff ff       	call   80486a0 <__asan_report_load4_noabort@plt>
 8048863:	83 c4 10             	add    esp,0x10
 8048866:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 8048869:	8b 00                	mov    eax,DWORD PTR [eax]
/home/kyoupark/git/kb/asan/gbo_recover.c:116
 804886b:	89 c7                	mov    edi,eax
/home/kyoupark/git/kb/asan/gbo_recover.c:117
 804886d:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 8048870:	83 c0 0c             	add    eax,0xc
 8048873:	89 c2                	mov    edx,eax
 8048875:	c1 ea 03             	shr    edx,0x3
 8048878:	81 c2 00 00 00 20    	add    edx,0x20000000
 804887e:	0f b6 12             	movzx  edx,BYTE PTR [edx]
 8048881:	84 d2                	test   dl,dl
 8048883:	0f 95 c1             	setne  cl
 8048886:	89 c3                	mov    ebx,eax
 8048888:	83 e3 07             	and    ebx,0x7
 804888b:	83 c3 01             	add    ebx,0x1
 804888e:	38 d3                	cmp    bl,dl
 8048890:	0f 9d c2             	setge  dl
 8048893:	21 ca                	and    edx,ecx
 8048895:	84 d2                	test   dl,dl
 8048897:	74 0c                	je     80488a5 <header_handler+0x84>
 8048899:	83 ec 0c             	sub    esp,0xc
 804889c:	50                   	push   eax
 804889d:	e8 6e fe ff ff       	call   8048710 <__asan_report_load2_noabort@plt>
 80488a2:	83 c4 10             	add    esp,0x10
 80488a5:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 80488a8:	0f b7 40 0c          	movzx  eax,WORD PTR [eax+0xc]
/home/kyoupark/git/kb/asan/gbo_recover.c:116
 80488ac:	0f b7 d8             	movzx  ebx,ax
 80488af:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 80488b2:	83 c0 04             	add    eax,0x4
 80488b5:	89 c2                	mov    edx,eax
 80488b7:	c1 ea 03             	shr    edx,0x3
 80488ba:	81 c2 00 00 00 20    	add    edx,0x20000000
 80488c0:	0f b6 12             	movzx  edx,BYTE PTR [edx]
 80488c3:	84 d2                	test   dl,dl
 80488c5:	0f 95 45 d7          	setne  BYTE PTR [ebp-0x29]
 80488c9:	89 c6                	mov    esi,eax
 80488cb:	83 e6 07             	and    esi,0x7
 80488ce:	8d 4e 03             	lea    ecx,[esi+0x3]
 80488d1:	38 d1                	cmp    cl,dl
 80488d3:	0f 9d c2             	setge  dl
 80488d6:	22 55 d7             	and    dl,BYTE PTR [ebp-0x29]
 80488d9:	84 d2                	test   dl,dl
 80488db:	74 0c                	je     80488e9 <header_handler+0xc8>
 80488dd:	83 ec 0c             	sub    esp,0xc
 80488e0:	50                   	push   eax
 80488e1:	e8 ba fd ff ff       	call   80486a0 <__asan_report_load4_noabort@plt>
 80488e6:	83 c4 10             	add    esp,0x10
 80488e9:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 80488ec:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
 80488ef:	57                   	push   edi
 80488f0:	53                   	push   ebx
 80488f1:	50                   	push   eax
 80488f2:	68 20 90 04 08       	push   0x8049020
 80488f7:	e8 64 fd ff ff       	call   8048660 <printf@plt>
 80488fc:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:118
 80488ff:	c7 45 e4 00 00 00 00 	mov    DWORD PTR [ebp-0x1c],0x0
 8048906:	e9 af 01 00 00       	jmp    8048aba <header_handler+0x299>
/home/kyoupark/git/kb/asan/gbo_recover.c:120 (discriminator 3)
 804890b:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 804890e:	89 c2                	mov    edx,eax
 8048910:	c1 ea 03             	shr    edx,0x3
 8048913:	81 c2 00 00 00 20    	add    edx,0x20000000
 8048919:	0f b6 12             	movzx  edx,BYTE PTR [edx]
 804891c:	84 d2                	test   dl,dl
 804891e:	0f 95 c1             	setne  cl
 8048921:	89 c3                	mov    ebx,eax
 8048923:	83 e3 07             	and    ebx,0x7
 8048926:	83 c3 03             	add    ebx,0x3
 8048929:	38 d3                	cmp    bl,dl
 804892b:	0f 9d c2             	setge  dl
 804892e:	21 ca                	and    edx,ecx
 8048930:	84 d2                	test   dl,dl
 8048932:	74 0c                	je     8048940 <header_handler+0x11f>
 8048934:	83 ec 0c             	sub    esp,0xc
 8048937:	50                   	push   eax
 8048938:	e8 63 fd ff ff       	call   80486a0 <__asan_report_load4_noabort@plt>
 804893d:	83 c4 10             	add    esp,0x10
 8048940:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 8048943:	8b 30                	mov    esi,DWORD PTR [eax]
 8048945:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 8048948:	83 c0 08             	add    eax,0x8
 804894b:	89 c2                	mov    edx,eax
 804894d:	c1 ea 03             	shr    edx,0x3
 8048950:	81 c2 00 00 00 20    	add    edx,0x20000000
 8048956:	0f b6 12             	movzx  edx,BYTE PTR [edx]
 8048959:	84 d2                	test   dl,dl
 804895b:	0f 95 c1             	setne  cl
 804895e:	89 c3                	mov    ebx,eax
 8048960:	83 e3 07             	and    ebx,0x7
 8048963:	83 c3 03             	add    ebx,0x3
 8048966:	38 d3                	cmp    bl,dl
 8048968:	0f 9d c2             	setge  dl
 804896b:	21 ca                	and    edx,ecx
 804896d:	84 d2                	test   dl,dl
 804896f:	74 0c                	je     804897d <header_handler+0x15c>
 8048971:	83 ec 0c             	sub    esp,0xc
 8048974:	50                   	push   eax
 8048975:	e8 26 fd ff ff       	call   80486a0 <__asan_report_load4_noabort@plt>
 804897a:	83 c4 10             	add    esp,0x10
 804897d:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 8048980:	8b 40 08             	mov    eax,DWORD PTR [eax+0x8]
 8048983:	8b 55 e4             	mov    edx,DWORD PTR [ebp-0x1c]
 8048986:	c1 e2 05             	shl    edx,0x5
 8048989:	8d 1c 10             	lea    ebx,[eax+edx*1]
 804898c:	8d 43 08             	lea    eax,[ebx+0x8]
 804898f:	89 c2                	mov    edx,eax
 8048991:	c1 ea 03             	shr    edx,0x3
 8048994:	81 c2 00 00 00 20    	add    edx,0x20000000
 804899a:	0f b6 12             	movzx  edx,BYTE PTR [edx]
 804899d:	84 d2                	test   dl,dl
 804899f:	0f 95 45 d7          	setne  BYTE PTR [ebp-0x29]
 80489a3:	89 c7                	mov    edi,eax
 80489a5:	83 e7 07             	and    edi,0x7
 80489a8:	8d 4f 03             	lea    ecx,[edi+0x3]
 80489ab:	38 d1                	cmp    cl,dl
 80489ad:	0f 9d c2             	setge  dl
 80489b0:	22 55 d7             	and    dl,BYTE PTR [ebp-0x29]
 80489b3:	84 d2                	test   dl,dl
 80489b5:	74 0c                	je     80489c3 <header_handler+0x1a2>
 80489b7:	83 ec 0c             	sub    esp,0xc
 80489ba:	50                   	push   eax
 80489bb:	e8 e0 fc ff ff       	call   80486a0 <__asan_report_load4_noabort@plt>
 80489c0:	83 c4 10             	add    esp,0x10
 80489c3:	8b 43 08             	mov    eax,DWORD PTR [ebx+0x8]
 80489c6:	01 f0                	add    eax,esi
/home/kyoupark/git/kb/asan/gbo_recover.c:119
 80489c8:	83 ec 04             	sub    esp,0x4
 80489cb:	50                   	push   eax
 80489cc:	ff 75 e4             	push   DWORD PTR [ebp-0x1c]
 80489cf:	68 80 90 04 08       	push   0x8049080
 80489d4:	e8 87 fc ff ff       	call   8048660 <printf@plt>
 80489d9:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:122
 80489dc:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 80489df:	83 c0 08             	add    eax,0x8
 80489e2:	89 c2                	mov    edx,eax
 80489e4:	c1 ea 03             	shr    edx,0x3
 80489e7:	81 c2 00 00 00 20    	add    edx,0x20000000
 80489ed:	0f b6 12             	movzx  edx,BYTE PTR [edx]
 80489f0:	84 d2                	test   dl,dl
 80489f2:	0f 95 c1             	setne  cl
 80489f5:	89 c3                	mov    ebx,eax
 80489f7:	83 e3 07             	and    ebx,0x7
 80489fa:	83 c3 03             	add    ebx,0x3
 80489fd:	38 d3                	cmp    bl,dl
 80489ff:	0f 9d c2             	setge  dl
 8048a02:	21 ca                	and    edx,ecx
 8048a04:	84 d2                	test   dl,dl
 8048a06:	74 0c                	je     8048a14 <header_handler+0x1f3>
 8048a08:	83 ec 0c             	sub    esp,0xc
 8048a0b:	50                   	push   eax
 8048a0c:	e8 8f fc ff ff       	call   80486a0 <__asan_report_load4_noabort@plt>
 8048a11:	83 c4 10             	add    esp,0x10
 8048a14:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 8048a17:	8b 40 08             	mov    eax,DWORD PTR [eax+0x8]
 8048a1a:	8b 55 e4             	mov    edx,DWORD PTR [ebp-0x1c]
 8048a1d:	c1 e2 05             	shl    edx,0x5
 8048a20:	8d 1c 10             	lea    ebx,[eax+edx*1]
/home/kyoupark/git/kb/asan/gbo_recover.c:121
 8048a23:	8d 43 18             	lea    eax,[ebx+0x18]
 8048a26:	89 c2                	mov    edx,eax
 8048a28:	c1 ea 03             	shr    edx,0x3
 8048a2b:	81 c2 00 00 00 20    	add    edx,0x20000000
 8048a31:	0f b6 12             	movzx  edx,BYTE PTR [edx]
 8048a34:	84 d2                	test   dl,dl
 8048a36:	0f 95 45 d7          	setne  BYTE PTR [ebp-0x29]
 8048a3a:	89 c6                	mov    esi,eax
 8048a3c:	83 e6 07             	and    esi,0x7
 8048a3f:	8d 4e 03             	lea    ecx,[esi+0x3]
 8048a42:	38 d1                	cmp    cl,dl
 8048a44:	0f 9d c2             	setge  dl
 8048a47:	22 55 d7             	and    dl,BYTE PTR [ebp-0x29]
 8048a4a:	84 d2                	test   dl,dl
 8048a4c:	74 0c                	je     8048a5a <header_handler+0x239>
 8048a4e:	83 ec 0c             	sub    esp,0xc
 8048a51:	50                   	push   eax
 8048a52:	e8 49 fc ff ff       	call   80486a0 <__asan_report_load4_noabort@plt>
 8048a57:	83 c4 10             	add    esp,0x10
 8048a5a:	8b 73 18             	mov    esi,DWORD PTR [ebx+0x18]
/home/kyoupark/git/kb/asan/gbo_recover.c:122
 8048a5d:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 8048a60:	8b 40 08             	mov    eax,DWORD PTR [eax+0x8]
 8048a63:	8b 55 e4             	mov    edx,DWORD PTR [ebp-0x1c]
 8048a66:	c1 e2 05             	shl    edx,0x5
 8048a69:	8d 1c 10             	lea    ebx,[eax+edx*1]
/home/kyoupark/git/kb/asan/gbo_recover.c:121
 8048a6c:	89 d8                	mov    eax,ebx
 8048a6e:	89 c2                	mov    edx,eax
 8048a70:	c1 ea 03             	shr    edx,0x3
 8048a73:	81 c2 00 00 00 20    	add    edx,0x20000000
 8048a79:	0f b6 12             	movzx  edx,BYTE PTR [edx]
 8048a7c:	84 d2                	test   dl,dl
 8048a7e:	0f 95 45 d7          	setne  BYTE PTR [ebp-0x29]
 8048a82:	89 c7                	mov    edi,eax
 8048a84:	83 e7 07             	and    edi,0x7
 8048a87:	8d 4f 03             	lea    ecx,[edi+0x3]
 8048a8a:	38 d1                	cmp    cl,dl
 8048a8c:	0f 9d c2             	setge  dl
 8048a8f:	22 55 d7             	and    dl,BYTE PTR [ebp-0x29]
 8048a92:	84 d2                	test   dl,dl
 8048a94:	74 0c                	je     8048aa2 <header_handler+0x281>
 8048a96:	83 ec 0c             	sub    esp,0xc
 8048a99:	50                   	push   eax
 8048a9a:	e8 01 fc ff ff       	call   80486a0 <__asan_report_load4_noabort@plt>
 8048a9f:	83 c4 10             	add    esp,0x10
 8048aa2:	8b 03                	mov    eax,DWORD PTR [ebx]
 8048aa4:	83 ec 04             	sub    esp,0x4
 8048aa7:	56                   	push   esi
 8048aa8:	50                   	push   eax
 8048aa9:	68 c0 90 04 08       	push   0x80490c0
 8048aae:	e8 ad fb ff ff       	call   8048660 <printf@plt>
 8048ab3:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:118
 8048ab6:	83 45 e4 01          	add    DWORD PTR [ebp-0x1c],0x1
 8048aba:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 8048abd:	83 c0 0c             	add    eax,0xc
 8048ac0:	89 c2                	mov    edx,eax
 8048ac2:	c1 ea 03             	shr    edx,0x3
 8048ac5:	81 c2 00 00 00 20    	add    edx,0x20000000
 8048acb:	0f b6 12             	movzx  edx,BYTE PTR [edx]
 8048ace:	84 d2                	test   dl,dl
 8048ad0:	0f 95 c1             	setne  cl
 8048ad3:	89 c3                	mov    ebx,eax
 8048ad5:	83 e3 07             	and    ebx,0x7
 8048ad8:	83 c3 01             	add    ebx,0x1
 8048adb:	38 d3                	cmp    bl,dl
 8048add:	0f 9d c2             	setge  dl
 8048ae0:	21 ca                	and    edx,ecx
 8048ae2:	84 d2                	test   dl,dl
 8048ae4:	74 0c                	je     8048af2 <header_handler+0x2d1>
 8048ae6:	83 ec 0c             	sub    esp,0xc
 8048ae9:	50                   	push   eax
 8048aea:	e8 21 fc ff ff       	call   8048710 <__asan_report_load2_noabort@plt>
 8048aef:	83 c4 10             	add    esp,0x10
 8048af2:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 8048af5:	0f b7 40 0c          	movzx  eax,WORD PTR [eax+0xc]
 8048af9:	0f b7 c0             	movzx  eax,ax
 8048afc:	3b 45 e4             	cmp    eax,DWORD PTR [ebp-0x1c]
 8048aff:	0f 8f 06 fe ff ff    	jg     804890b <header_handler+0xea>
/home/kyoupark/git/kb/asan/gbo_recover.c:124
 8048b05:	83 ec 0c             	sub    esp,0xc
 8048b08:	6a 0a                	push   0xa
 8048b0a:	e8 11 fc ff ff       	call   8048720 <putchar@plt>
 8048b0f:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:125
 8048b12:	b8 00 00 00 00       	mov    eax,0x0
/home/kyoupark/git/kb/asan/gbo_recover.c:126
 8048b17:	8d 65 f4             	lea    esp,[ebp-0xc]
 8048b1a:	5b                   	pop    ebx
 8048b1b:	5e                   	pop    esi
 8048b1c:	5f                   	pop    edi
 8048b1d:	5d                   	pop    ebp
 8048b1e:	c3                   	ret    

08048b1f <call_normal>:
call_normal():
/home/kyoupark/git/kb/asan/gbo_recover.c:131
 8048b1f:	55                   	push   ebp
 8048b20:	89 e5                	mov    ebp,esp
 8048b22:	83 ec 18             	sub    esp,0x18
/home/kyoupark/git/kb/asan/gbo_recover.c:132
 8048b25:	83 ec 0c             	sub    esp,0xc
 8048b28:	68 00 91 04 08       	push   0x8049100
 8048b2d:	e8 be fb ff ff       	call   80486f0 <puts@plt>
 8048b32:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:133
 8048b35:	c7 45 f0 00 00 00 00 	mov    DWORD PTR [ebp-0x10],0x0
 8048b3c:	c7 45 f4 00 00 00 00 	mov    DWORD PTR [ebp-0xc],0x0
/home/kyoupark/git/kb/asan/gbo_recover.c:135
 8048b43:	eb 0a                	jmp    8048b4f <call_normal+0x30>
/home/kyoupark/git/kb/asan/gbo_recover.c:136 (discriminator 2)
 8048b45:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
 8048b48:	01 45 f0             	add    DWORD PTR [ebp-0x10],eax
/home/kyoupark/git/kb/asan/gbo_recover.c:135
 8048b4b:	83 45 f4 01          	add    DWORD PTR [ebp-0xc],0x1
 8048b4f:	83 7d f4 09          	cmp    DWORD PTR [ebp-0xc],0x9
 8048b53:	7e f0                	jle    8048b45 <call_normal+0x26>
/home/kyoupark/git/kb/asan/gbo_recover.c:138
 8048b55:	83 ec 0c             	sub    esp,0xc
 8048b58:	68 40 91 04 08       	push   0x8049140
 8048b5d:	e8 8e fb ff ff       	call   80486f0 <puts@plt>
 8048b62:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:139
 8048b65:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
/home/kyoupark/git/kb/asan/gbo_recover.c:140
 8048b68:	c9                   	leave  
 8048b69:	c3                   	ret    

08048b6a <call_gbo_1>:
call_gbo_1():
/home/kyoupark/git/kb/asan/gbo_recover.c:143
 8048b6a:	55                   	push   ebp
 8048b6b:	89 e5                	mov    ebp,esp
 8048b6d:	57                   	push   edi
 8048b6e:	56                   	push   esi
 8048b6f:	53                   	push   ebx
 8048b70:	83 ec 7c             	sub    esp,0x7c
 8048b73:	8d 75 88             	lea    esi,[ebp-0x78]
 8048b76:	89 75 84             	mov    DWORD PTR [ebp-0x7c],esi
 8048b79:	83 3d 50 b2 04 08 00 	cmp    DWORD PTR ds:0x804b250,0x0
 8048b80:	74 13                	je     8048b95 <call_gbo_1+0x2b>
 8048b82:	83 ec 0c             	sub    esp,0xc
 8048b85:	6a 60                	push   0x60
 8048b87:	e8 04 fb ff ff       	call   8048690 <__asan_stack_malloc_1@plt>
 8048b8c:	83 c4 10             	add    esp,0x10
 8048b8f:	85 c0                	test   eax,eax
 8048b91:	74 02                	je     8048b95 <call_gbo_1+0x2b>
 8048b93:	89 c6                	mov    esi,eax
 8048b95:	8d 46 60             	lea    eax,[esi+0x60]
 8048b98:	89 c7                	mov    edi,eax
 8048b9a:	c7 06 b3 8a b5 41    	mov    DWORD PTR [esi],0x41b58ab3
 8048ba0:	c7 46 04 80 91 04 08 	mov    DWORD PTR [esi+0x4],0x8049180
 8048ba7:	c7 46 08 6a 8b 04 08 	mov    DWORD PTR [esi+0x8],0x8048b6a
 8048bae:	89 f3                	mov    ebx,esi
 8048bb0:	c1 eb 03             	shr    ebx,0x3
 8048bb3:	c7 83 00 00 00 20 f1 	mov    DWORD PTR [ebx+0x20000000],0xf1f1f1f1
 8048bba:	f1 f1 f1 
 8048bbd:	c7 83 04 00 00 20 00 	mov    DWORD PTR [ebx+0x20000004],0xf4f40200
 8048bc4:	02 f4 f4 
 8048bc7:	c7 83 08 00 00 20 f3 	mov    DWORD PTR [ebx+0x20000008],0xf3f3f3f3
 8048bce:	f3 f3 f3 
/home/kyoupark/git/kb/asan/gbo_recover.c:145
 8048bd1:	b8 60 b0 04 08       	mov    eax,0x804b060
 8048bd6:	89 c2                	mov    edx,eax
 8048bd8:	c1 ea 03             	shr    edx,0x3
 8048bdb:	81 c2 00 00 00 20    	add    edx,0x20000000
 8048be1:	0f b6 12             	movzx  edx,BYTE PTR [edx]
 8048be4:	84 d2                	test   dl,dl
 8048be6:	74 0c                	je     8048bf4 <call_gbo_1+0x8a>
 8048be8:	83 ec 0c             	sub    esp,0xc
 8048beb:	50                   	push   eax
 8048bec:	e8 0f fb ff ff       	call   8048700 <__asan_report_load8_noabort@plt>
 8048bf1:	83 c4 10             	add    esp,0x10
 8048bf4:	a1 60 b0 04 08       	mov    eax,ds:0x804b060
 8048bf9:	8b 15 64 b0 04 08    	mov    edx,DWORD PTR ds:0x804b064
 8048bff:	89 47 c0             	mov    DWORD PTR [edi-0x40],eax
 8048c02:	89 57 c4             	mov    DWORD PTR [edi-0x3c],edx
/home/kyoupark/git/kb/asan/gbo_recover.c:146
 8048c05:	90                   	nop
/home/kyoupark/git/kb/asan/gbo_recover.c:143
 8048c06:	39 75 84             	cmp    DWORD PTR [ebp-0x7c],esi
 8048c09:	74 26                	je     8048c31 <call_gbo_1+0xc7>
 8048c0b:	c7 06 0e 36 e0 45    	mov    DWORD PTR [esi],0x45e0360e
 8048c11:	c7 83 00 00 00 20 f5 	mov    DWORD PTR [ebx+0x20000000],0xf5f5f5f5
 8048c18:	f5 f5 f5 
 8048c1b:	c7 83 04 00 00 20 f5 	mov    DWORD PTR [ebx+0x20000004],0xf5f5f5f5
 8048c22:	f5 f5 f5 
 8048c25:	c7 83 08 00 00 20 f5 	mov    DWORD PTR [ebx+0x20000008],0xf5f5f5f5
 8048c2c:	f5 f5 f5 
 8048c2f:	eb 1e                	jmp    8048c4f <call_gbo_1+0xe5>
 8048c31:	c7 83 00 00 00 20 00 	mov    DWORD PTR [ebx+0x20000000],0x0
 8048c38:	00 00 00 
 8048c3b:	c7 83 04 00 00 20 00 	mov    DWORD PTR [ebx+0x20000004],0x0
 8048c42:	00 00 00 
 8048c45:	c7 83 08 00 00 20 00 	mov    DWORD PTR [ebx+0x20000008],0x0
 8048c4c:	00 00 00 
/home/kyoupark/git/kb/asan/gbo_recover.c:147
 8048c4f:	8d 65 f4             	lea    esp,[ebp-0xc]
 8048c52:	5b                   	pop    ebx
 8048c53:	5e                   	pop    esi
 8048c54:	5f                   	pop    edi
 8048c55:	5d                   	pop    ebp
 8048c56:	c3                   	ret    

08048c57 <call_gbo_2>:
call_gbo_2():
/home/kyoupark/git/kb/asan/gbo_recover.c:150
 8048c57:	55                   	push   ebp
 8048c58:	89 e5                	mov    ebp,esp
 8048c5a:	57                   	push   edi
 8048c5b:	56                   	push   esi
 8048c5c:	53                   	push   ebx
 8048c5d:	83 ec 7c             	sub    esp,0x7c
 8048c60:	8d 75 88             	lea    esi,[ebp-0x78]
 8048c63:	89 75 84             	mov    DWORD PTR [ebp-0x7c],esi
 8048c66:	83 3d 50 b2 04 08 00 	cmp    DWORD PTR ds:0x804b250,0x0
 8048c6d:	74 13                	je     8048c82 <call_gbo_2+0x2b>
 8048c6f:	83 ec 0c             	sub    esp,0xc
 8048c72:	6a 60                	push   0x60
 8048c74:	e8 17 fa ff ff       	call   8048690 <__asan_stack_malloc_1@plt>
 8048c79:	83 c4 10             	add    esp,0x10
 8048c7c:	85 c0                	test   eax,eax
 8048c7e:	74 02                	je     8048c82 <call_gbo_2+0x2b>
 8048c80:	89 c6                	mov    esi,eax
 8048c82:	8d 46 60             	lea    eax,[esi+0x60]
 8048c85:	89 c7                	mov    edi,eax
 8048c87:	c7 06 b3 8a b5 41    	mov    DWORD PTR [esi],0x41b58ab3
 8048c8d:	c7 46 04 80 91 04 08 	mov    DWORD PTR [esi+0x4],0x8049180
 8048c94:	c7 46 08 57 8c 04 08 	mov    DWORD PTR [esi+0x8],0x8048c57
 8048c9b:	89 f3                	mov    ebx,esi
 8048c9d:	c1 eb 03             	shr    ebx,0x3
 8048ca0:	c7 83 00 00 00 20 f1 	mov    DWORD PTR [ebx+0x20000000],0xf1f1f1f1
 8048ca7:	f1 f1 f1 
 8048caa:	c7 83 04 00 00 20 00 	mov    DWORD PTR [ebx+0x20000004],0xf4f40200
 8048cb1:	02 f4 f4 
 8048cb4:	c7 83 08 00 00 20 f3 	mov    DWORD PTR [ebx+0x20000008],0xf3f3f3f3
 8048cbb:	f3 f3 f3 
/home/kyoupark/git/kb/asan/gbo_recover.c:152
 8048cbe:	b8 60 b0 04 08       	mov    eax,0x804b060
 8048cc3:	89 c2                	mov    edx,eax
 8048cc5:	c1 ea 03             	shr    edx,0x3
 8048cc8:	81 c2 00 00 00 20    	add    edx,0x20000000
 8048cce:	0f b6 12             	movzx  edx,BYTE PTR [edx]
 8048cd1:	84 d2                	test   dl,dl
 8048cd3:	74 0c                	je     8048ce1 <call_gbo_2+0x8a>
 8048cd5:	83 ec 0c             	sub    esp,0xc
 8048cd8:	50                   	push   eax
 8048cd9:	e8 22 fa ff ff       	call   8048700 <__asan_report_load8_noabort@plt>
 8048cde:	83 c4 10             	add    esp,0x10
 8048ce1:	a1 60 b0 04 08       	mov    eax,ds:0x804b060
 8048ce6:	8b 15 64 b0 04 08    	mov    edx,DWORD PTR ds:0x804b064
 8048cec:	89 47 c0             	mov    DWORD PTR [edi-0x40],eax
 8048cef:	89 57 c4             	mov    DWORD PTR [edi-0x3c],edx
/home/kyoupark/git/kb/asan/gbo_recover.c:153
 8048cf2:	90                   	nop
/home/kyoupark/git/kb/asan/gbo_recover.c:150
 8048cf3:	39 75 84             	cmp    DWORD PTR [ebp-0x7c],esi
 8048cf6:	74 26                	je     8048d1e <call_gbo_2+0xc7>
 8048cf8:	c7 06 0e 36 e0 45    	mov    DWORD PTR [esi],0x45e0360e
 8048cfe:	c7 83 00 00 00 20 f5 	mov    DWORD PTR [ebx+0x20000000],0xf5f5f5f5
 8048d05:	f5 f5 f5 
 8048d08:	c7 83 04 00 00 20 f5 	mov    DWORD PTR [ebx+0x20000004],0xf5f5f5f5
 8048d0f:	f5 f5 f5 
 8048d12:	c7 83 08 00 00 20 f5 	mov    DWORD PTR [ebx+0x20000008],0xf5f5f5f5
 8048d19:	f5 f5 f5 
 8048d1c:	eb 1e                	jmp    8048d3c <call_gbo_2+0xe5>
 8048d1e:	c7 83 00 00 00 20 00 	mov    DWORD PTR [ebx+0x20000000],0x0
 8048d25:	00 00 00 
 8048d28:	c7 83 04 00 00 20 00 	mov    DWORD PTR [ebx+0x20000004],0x0
 8048d2f:	00 00 00 
 8048d32:	c7 83 08 00 00 20 00 	mov    DWORD PTR [ebx+0x20000008],0x0
 8048d39:	00 00 00 
/home/kyoupark/git/kb/asan/gbo_recover.c:154
 8048d3c:	8d 65 f4             	lea    esp,[ebp-0xc]
 8048d3f:	5b                   	pop    ebx
 8048d40:	5e                   	pop    esi
 8048d41:	5f                   	pop    edi
 8048d42:	5d                   	pop    ebp
 8048d43:	c3                   	ret    

08048d44 <call_gbo_3>:
call_gbo_3():
/home/kyoupark/git/kb/asan/gbo_recover.c:157
 8048d44:	55                   	push   ebp
 8048d45:	89 e5                	mov    ebp,esp
 8048d47:	57                   	push   edi
 8048d48:	56                   	push   esi
 8048d49:	53                   	push   ebx
 8048d4a:	83 ec 7c             	sub    esp,0x7c
 8048d4d:	8d 75 88             	lea    esi,[ebp-0x78]
 8048d50:	89 75 84             	mov    DWORD PTR [ebp-0x7c],esi
 8048d53:	83 3d 50 b2 04 08 00 	cmp    DWORD PTR ds:0x804b250,0x0
 8048d5a:	74 13                	je     8048d6f <call_gbo_3+0x2b>
 8048d5c:	83 ec 0c             	sub    esp,0xc
 8048d5f:	6a 60                	push   0x60
 8048d61:	e8 2a f9 ff ff       	call   8048690 <__asan_stack_malloc_1@plt>
 8048d66:	83 c4 10             	add    esp,0x10
 8048d69:	85 c0                	test   eax,eax
 8048d6b:	74 02                	je     8048d6f <call_gbo_3+0x2b>
 8048d6d:	89 c6                	mov    esi,eax
 8048d6f:	8d 46 60             	lea    eax,[esi+0x60]
 8048d72:	89 c7                	mov    edi,eax
 8048d74:	c7 06 b3 8a b5 41    	mov    DWORD PTR [esi],0x41b58ab3
 8048d7a:	c7 46 04 80 91 04 08 	mov    DWORD PTR [esi+0x4],0x8049180
 8048d81:	c7 46 08 44 8d 04 08 	mov    DWORD PTR [esi+0x8],0x8048d44
 8048d88:	89 f3                	mov    ebx,esi
 8048d8a:	c1 eb 03             	shr    ebx,0x3
 8048d8d:	c7 83 00 00 00 20 f1 	mov    DWORD PTR [ebx+0x20000000],0xf1f1f1f1
 8048d94:	f1 f1 f1 
 8048d97:	c7 83 04 00 00 20 00 	mov    DWORD PTR [ebx+0x20000004],0xf4f40200
 8048d9e:	02 f4 f4 
 8048da1:	c7 83 08 00 00 20 f3 	mov    DWORD PTR [ebx+0x20000008],0xf3f3f3f3
 8048da8:	f3 f3 f3 
/home/kyoupark/git/kb/asan/gbo_recover.c:159
 8048dab:	b8 60 b0 04 08       	mov    eax,0x804b060
 8048db0:	89 c2                	mov    edx,eax
 8048db2:	c1 ea 03             	shr    edx,0x3
 8048db5:	81 c2 00 00 00 20    	add    edx,0x20000000
 8048dbb:	0f b6 12             	movzx  edx,BYTE PTR [edx]
 8048dbe:	84 d2                	test   dl,dl
 8048dc0:	74 0c                	je     8048dce <call_gbo_3+0x8a>
 8048dc2:	83 ec 0c             	sub    esp,0xc
 8048dc5:	50                   	push   eax
 8048dc6:	e8 35 f9 ff ff       	call   8048700 <__asan_report_load8_noabort@plt>
 8048dcb:	83 c4 10             	add    esp,0x10
 8048dce:	a1 60 b0 04 08       	mov    eax,ds:0x804b060
 8048dd3:	8b 15 64 b0 04 08    	mov    edx,DWORD PTR ds:0x804b064
 8048dd9:	89 47 c0             	mov    DWORD PTR [edi-0x40],eax
 8048ddc:	89 57 c4             	mov    DWORD PTR [edi-0x3c],edx
/home/kyoupark/git/kb/asan/gbo_recover.c:160
 8048ddf:	90                   	nop
/home/kyoupark/git/kb/asan/gbo_recover.c:157
 8048de0:	39 75 84             	cmp    DWORD PTR [ebp-0x7c],esi
 8048de3:	74 26                	je     8048e0b <call_gbo_3+0xc7>
 8048de5:	c7 06 0e 36 e0 45    	mov    DWORD PTR [esi],0x45e0360e
 8048deb:	c7 83 00 00 00 20 f5 	mov    DWORD PTR [ebx+0x20000000],0xf5f5f5f5
 8048df2:	f5 f5 f5 
 8048df5:	c7 83 04 00 00 20 f5 	mov    DWORD PTR [ebx+0x20000004],0xf5f5f5f5
 8048dfc:	f5 f5 f5 
 8048dff:	c7 83 08 00 00 20 f5 	mov    DWORD PTR [ebx+0x20000008],0xf5f5f5f5
 8048e06:	f5 f5 f5 
 8048e09:	eb 1e                	jmp    8048e29 <call_gbo_3+0xe5>
 8048e0b:	c7 83 00 00 00 20 00 	mov    DWORD PTR [ebx+0x20000000],0x0
 8048e12:	00 00 00 
 8048e15:	c7 83 04 00 00 20 00 	mov    DWORD PTR [ebx+0x20000004],0x0
 8048e1c:	00 00 00 
 8048e1f:	c7 83 08 00 00 20 00 	mov    DWORD PTR [ebx+0x20000008],0x0
 8048e26:	00 00 00 
/home/kyoupark/git/kb/asan/gbo_recover.c:161
 8048e29:	8d 65 f4             	lea    esp,[ebp-0xc]
 8048e2c:	5b                   	pop    ebx
 8048e2d:	5e                   	pop    esi
 8048e2e:	5f                   	pop    edi
 8048e2f:	5d                   	pop    ebp
 8048e30:	c3                   	ret    

08048e31 <main>:
main():
/home/kyoupark/git/kb/asan/gbo_recover.c:164
 8048e31:	8d 4c 24 04          	lea    ecx,[esp+0x4]
 8048e35:	83 e4 f0             	and    esp,0xfffffff0
 8048e38:	ff 71 fc             	push   DWORD PTR [ecx-0x4]
 8048e3b:	55                   	push   ebp
 8048e3c:	89 e5                	mov    ebp,esp
 8048e3e:	51                   	push   ecx
 8048e3f:	83 ec 04             	sub    esp,0x4
/home/kyoupark/git/kb/asan/gbo_recover.c:165
 8048e42:	83 ec 08             	sub    esp,0x8
 8048e45:	6a 00                	push   0x0
 8048e47:	68 21 88 04 08       	push   0x8048821
 8048e4c:	e8 2f f8 ff ff       	call   8048680 <dl_iterate_phdr@plt>
 8048e51:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:167
 8048e54:	83 ec 0c             	sub    esp,0xc
 8048e57:	68 a0 91 04 08       	push   0x80491a0
 8048e5c:	e8 8f f8 ff ff       	call   80486f0 <puts@plt>
 8048e61:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:168
 8048e64:	83 ec 0c             	sub    esp,0xc
 8048e67:	68 e0 91 04 08       	push   0x80491e0
 8048e6c:	e8 7f f8 ff ff       	call   80486f0 <puts@plt>
 8048e71:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:169
 8048e74:	83 ec 0c             	sub    esp,0xc
 8048e77:	68 40 92 04 08       	push   0x8049240
 8048e7c:	e8 6f f8 ff ff       	call   80486f0 <puts@plt>
 8048e81:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:171
 8048e84:	83 ec 0c             	sub    esp,0xc
 8048e87:	68 80 92 04 08       	push   0x8049280
 8048e8c:	e8 5f f8 ff ff       	call   80486f0 <puts@plt>
 8048e91:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:172
 8048e94:	e8 d1 fc ff ff       	call   8048b6a <call_gbo_1>
/home/kyoupark/git/kb/asan/gbo_recover.c:174
 8048e99:	83 ec 0c             	sub    esp,0xc
 8048e9c:	68 c0 92 04 08       	push   0x80492c0
 8048ea1:	e8 4a f8 ff ff       	call   80486f0 <puts@plt>
 8048ea6:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:175
 8048ea9:	e8 a9 fd ff ff       	call   8048c57 <call_gbo_2>
/home/kyoupark/git/kb/asan/gbo_recover.c:177
 8048eae:	83 ec 0c             	sub    esp,0xc
 8048eb1:	68 00 93 04 08       	push   0x8049300
 8048eb6:	e8 35 f8 ff ff       	call   80486f0 <puts@plt>
 8048ebb:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:178
 8048ebe:	e8 81 fe ff ff       	call   8048d44 <call_gbo_3>
/home/kyoupark/git/kb/asan/gbo_recover.c:180
 8048ec3:	83 ec 0c             	sub    esp,0xc
 8048ec6:	68 40 93 04 08       	push   0x8049340
 8048ecb:	e8 20 f8 ff ff       	call   80486f0 <puts@plt>
 8048ed0:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:181
 8048ed3:	e8 47 fc ff ff       	call   8048b1f <call_normal>
/home/kyoupark/git/kb/asan/gbo_recover.c:183
 8048ed8:	83 ec 0c             	sub    esp,0xc
 8048edb:	68 80 93 04 08       	push   0x8049380
 8048ee0:	e8 0b f8 ff ff       	call   80486f0 <puts@plt>
 8048ee5:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo_recover.c:184
 8048ee8:	b8 00 00 00 00       	mov    eax,0x0
/home/kyoupark/git/kb/asan/gbo_recover.c:185
 8048eed:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
 8048ef0:	c9                   	leave  
 8048ef1:	8d 61 fc             	lea    esp,[ecx-0x4]
 8048ef4:	c3                   	ret    

08048ef5 <_GLOBAL__sub_D_00099_0_call_normal>:
_GLOBAL__sub_D_00099_0_call_normal():
 8048ef5:	55                   	push   ebp
 8048ef6:	89 e5                	mov    ebp,esp
 8048ef8:	83 ec 08             	sub    esp,0x8
 8048efb:	83 ec 08             	sub    esp,0x8
 8048efe:	6a 0e                	push   0xe
 8048f00:	68 c0 b0 04 08       	push   0x804b0c0
 8048f05:	e8 b6 f7 ff ff       	call   80486c0 <__asan_unregister_globals@plt>
 8048f0a:	83 c4 10             	add    esp,0x10
 8048f0d:	c9                   	leave  
 8048f0e:	c3                   	ret    

08048f0f <_GLOBAL__sub_I_00099_1_call_normal>:
_GLOBAL__sub_I_00099_1_call_normal():
 8048f0f:	55                   	push   ebp
 8048f10:	89 e5                	mov    ebp,esp
 8048f12:	83 ec 08             	sub    esp,0x8
 8048f15:	e8 b6 f7 ff ff       	call   80486d0 <__asan_init@plt>
 8048f1a:	e8 c1 f7 ff ff       	call   80486e0 <__asan_version_mismatch_check_v6@plt>
 8048f1f:	83 ec 08             	sub    esp,0x8
 8048f22:	6a 0e                	push   0xe
 8048f24:	68 c0 b0 04 08       	push   0x804b0c0
 8048f29:	e8 42 f7 ff ff       	call   8048670 <__asan_register_globals@plt>
 8048f2e:	83 c4 10             	add    esp,0x10
 8048f31:	c9                   	leave  
 8048f32:	c3                   	ret    
 8048f33:	66 90                	xchg   ax,ax
 8048f35:	66 90                	xchg   ax,ax
 8048f37:	66 90                	xchg   ax,ax
 8048f39:	66 90                	xchg   ax,ax
 8048f3b:	66 90                	xchg   ax,ax
 8048f3d:	66 90                	xchg   ax,ax
 8048f3f:	90                   	nop

08048f40 <__libc_csu_init>:
__libc_csu_init():
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/elf-init.c:68
 8048f40:	55                   	push   ebp
 8048f41:	57                   	push   edi
 8048f42:	56                   	push   esi
 8048f43:	53                   	push   ebx
 8048f44:	e8 27 f8 ff ff       	call   8048770 <__x86.get_pc_thunk.bx>
 8048f49:	81 c3 b7 20 00 00    	add    ebx,0x20b7
 8048f4f:	83 ec 2c             	sub    esp,0x2c
 8048f52:	8b 6c 24 40          	mov    ebp,DWORD PTR [esp+0x40]
 8048f56:	8b 7c 24 44          	mov    edi,DWORD PTR [esp+0x44]
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/elf-init.c:83
 8048f5a:	e8 b9 f6 ff ff       	call   8048618 <_init>
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/elf-init.c:86
 8048f5f:	8d 83 e0 fe ff ff    	lea    eax,[ebx-0x120]
 8048f65:	8d 93 e4 fe ff ff    	lea    edx,[ebx-0x11c]
 8048f6b:	29 c2                	sub    edx,eax
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/elf-init.c:87
 8048f6d:	c1 fa 02             	sar    edx,0x2
 8048f70:	89 54 24 1c          	mov    DWORD PTR [esp+0x1c],edx
 8048f74:	74 27                	je     8048f9d <__libc_csu_init+0x5d>
 8048f76:	31 f6                	xor    esi,esi
 8048f78:	90                   	nop
 8048f79:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/elf-init.c:88 (discriminator 3)
 8048f80:	8b 44 24 48          	mov    eax,DWORD PTR [esp+0x48]
 8048f84:	89 7c 24 04          	mov    DWORD PTR [esp+0x4],edi
 8048f88:	89 2c 24             	mov    DWORD PTR [esp],ebp
 8048f8b:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
 8048f8f:	ff 94 b3 e0 fe ff ff 	call   DWORD PTR [ebx+esi*4-0x120]
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/elf-init.c:87 (discriminator 3)
 8048f96:	46                   	inc    esi
 8048f97:	39 74 24 1c          	cmp    DWORD PTR [esp+0x1c],esi
 8048f9b:	75 e3                	jne    8048f80 <__libc_csu_init+0x40>
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/elf-init.c:89
 8048f9d:	83 c4 2c             	add    esp,0x2c
 8048fa0:	5b                   	pop    ebx
 8048fa1:	5e                   	pop    esi
 8048fa2:	5f                   	pop    edi
 8048fa3:	5d                   	pop    ebp
 8048fa4:	c3                   	ret    
 8048fa5:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
 8048fa9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

08048fb0 <__libc_csu_fini>:
__libc_csu_fini():
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/elf-init.c:96
 8048fb0:	c3                   	ret    

08048fb1 <__do_global_ctors_aux>:
__do_global_ctors_aux():
 8048fb1:	55                   	push   ebp
 8048fb2:	89 e5                	mov    ebp,esp
 8048fb4:	53                   	push   ebx
 8048fb5:	bb e8 ae 04 08       	mov    ebx,0x804aee8
 8048fba:	52                   	push   edx
 8048fbb:	8b 03                	mov    eax,DWORD PTR [ebx]
 8048fbd:	83 f8 ff             	cmp    eax,0xffffffff
 8048fc0:	74 07                	je     8048fc9 <__do_global_ctors_aux+0x18>
 8048fc2:	ff d0                	call   eax
 8048fc4:	83 eb 04             	sub    ebx,0x4
 8048fc7:	eb f2                	jmp    8048fbb <__do_global_ctors_aux+0xa>
 8048fc9:	58                   	pop    eax
 8048fca:	5b                   	pop    ebx
 8048fcb:	5d                   	pop    ebp
 8048fcc:	c3                   	ret    

Disassembly of section .fini:

08048fd0 <_fini>:
_fini():
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crti.S:82
 8048fd0:	53                   	push   ebx
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crti.S:83
 8048fd1:	83 ec 08             	sub    esp,0x8
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crti.S:84
 8048fd4:	e8 97 f7 ff ff       	call   8048770 <__x86.get_pc_thunk.bx>
 8048fd9:	81 c3 27 20 00 00    	add    ebx,0x2027
 8048fdf:	e8 e3 f7 ff ff       	call   80487c7 <__do_global_dtors_aux>
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crtn.S:45
 8048fe4:	83 c4 08             	add    esp,0x8
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crtn.S:46
 8048fe7:	5b                   	pop    ebx
/mnt/bcb7778e-e87e-4609-857b-cada2826a487/cross-build-for-host/.build/src/glibc-2.24/csu/../sysdeps/i386/crtn.S:47
 8048fe8:	c3                   	ret    
