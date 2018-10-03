
gbo_host_asan:     file format elf32-i386


Disassembly of section .init:

00000668 <_init>:
_init():
 668:	53                   	push   ebx
 669:	83 ec 08             	sub    esp,0x8
 66c:	e8 ff 00 00 00       	call   770 <__x86.get_pc_thunk.bx>
 671:	81 c3 8f 19 00 00    	add    ebx,0x198f
 677:	8b 83 f8 ff ff ff    	mov    eax,DWORD PTR [ebx-0x8]
 67d:	85 c0                	test   eax,eax
 67f:	74 05                	je     686 <_init+0x1e>
 681:	e8 a2 00 00 00       	call   728 <.plt.got+0x8>
 686:	83 c4 08             	add    esp,0x8
 689:	5b                   	pop    ebx
 68a:	c3                   	ret    

Disassembly of section .plt:

00000690 <.plt>:
 690:	ff b3 04 00 00 00    	push   DWORD PTR [ebx+0x4]
 696:	ff a3 08 00 00 00    	jmp    DWORD PTR [ebx+0x8]
 69c:	00 00                	add    BYTE PTR [eax],al
	...

000006a0 <__asan_register_globals@plt>:
 6a0:	ff a3 0c 00 00 00    	jmp    DWORD PTR [ebx+0xc]
 6a6:	68 00 00 00 00       	push   0x0
 6ab:	e9 e0 ff ff ff       	jmp    690 <.plt>

000006b0 <__asan_stack_malloc_1@plt>:
 6b0:	ff a3 10 00 00 00    	jmp    DWORD PTR [ebx+0x10]
 6b6:	68 08 00 00 00       	push   0x8
 6bb:	e9 d0 ff ff ff       	jmp    690 <.plt>

000006c0 <__libc_start_main@plt>:
 6c0:	ff a3 14 00 00 00    	jmp    DWORD PTR [ebx+0x14]
 6c6:	68 10 00 00 00       	push   0x10
 6cb:	e9 c0 ff ff ff       	jmp    690 <.plt>

000006d0 <__asan_unregister_globals@plt>:
 6d0:	ff a3 18 00 00 00    	jmp    DWORD PTR [ebx+0x18]
 6d6:	68 18 00 00 00       	push   0x18
 6db:	e9 b0 ff ff ff       	jmp    690 <.plt>

000006e0 <__asan_init@plt>:
 6e0:	ff a3 1c 00 00 00    	jmp    DWORD PTR [ebx+0x1c]
 6e6:	68 20 00 00 00       	push   0x20
 6eb:	e9 a0 ff ff ff       	jmp    690 <.plt>

000006f0 <__asan_version_mismatch_check_v6@plt>:
 6f0:	ff a3 20 00 00 00    	jmp    DWORD PTR [ebx+0x20]
 6f6:	68 28 00 00 00       	push   0x28
 6fb:	e9 90 ff ff ff       	jmp    690 <.plt>

00000700 <__asan_report_load8@plt>:
 700:	ff a3 24 00 00 00    	jmp    DWORD PTR [ebx+0x24]
 706:	68 30 00 00 00       	push   0x30
 70b:	e9 80 ff ff ff       	jmp    690 <.plt>

00000710 <puts@plt>:
 710:	ff a3 28 00 00 00    	jmp    DWORD PTR [ebx+0x28]
 716:	68 38 00 00 00       	push   0x38
 71b:	e9 70 ff ff ff       	jmp    690 <.plt>

Disassembly of section .plt.got:

00000720 <.plt.got>:
 720:	ff a3 e4 ff ff ff    	jmp    DWORD PTR [ebx-0x1c]
 726:	66 90                	xchg   ax,ax
 728:	ff a3 f8 ff ff ff    	jmp    DWORD PTR [ebx-0x8]
 72e:	66 90                	xchg   ax,ax

Disassembly of section .text:

00000730 <_start>:
_start():
 730:	31 ed                	xor    ebp,ebp
 732:	5e                   	pop    esi
 733:	89 e1                	mov    ecx,esp
 735:	83 e4 f0             	and    esp,0xfffffff0
 738:	50                   	push   eax
 739:	54                   	push   esp
 73a:	52                   	push   edx
 73b:	e8 22 00 00 00       	call   762 <_start+0x32>
 740:	81 c3 c0 18 00 00    	add    ebx,0x18c0
 746:	8d 83 e0 ea ff ff    	lea    eax,[ebx-0x1520]
 74c:	50                   	push   eax
 74d:	8d 83 80 ea ff ff    	lea    eax,[ebx-0x1580]
 753:	50                   	push   eax
 754:	51                   	push   ecx
 755:	56                   	push   esi
 756:	ff b3 e8 ff ff ff    	push   DWORD PTR [ebx-0x18]
 75c:	e8 5f ff ff ff       	call   6c0 <__libc_start_main@plt>
 761:	f4                   	hlt    
 762:	8b 1c 24             	mov    ebx,DWORD PTR [esp]
 765:	c3                   	ret    
 766:	66 90                	xchg   ax,ax
 768:	66 90                	xchg   ax,ax
 76a:	66 90                	xchg   ax,ax
 76c:	66 90                	xchg   ax,ax
 76e:	66 90                	xchg   ax,ax

00000770 <__x86.get_pc_thunk.bx>:
__x86.get_pc_thunk.bx():
 770:	8b 1c 24             	mov    ebx,DWORD PTR [esp]
 773:	c3                   	ret    
 774:	66 90                	xchg   ax,ax
 776:	66 90                	xchg   ax,ax
 778:	66 90                	xchg   ax,ax
 77a:	66 90                	xchg   ax,ax
 77c:	66 90                	xchg   ax,ax
 77e:	66 90                	xchg   ax,ax

00000780 <deregister_tm_clones>:
deregister_tm_clones():
 780:	e8 17 01 00 00       	call   89c <__x86.get_pc_thunk.dx>
 785:	81 c2 7b 18 00 00    	add    edx,0x187b
 78b:	8d 8a 4c 01 00 00    	lea    ecx,[edx+0x14c]
 791:	8d 82 4f 01 00 00    	lea    eax,[edx+0x14f]
 797:	29 c8                	sub    eax,ecx
 799:	83 f8 06             	cmp    eax,0x6
 79c:	76 17                	jbe    7b5 <deregister_tm_clones+0x35>
 79e:	8b 82 f4 ff ff ff    	mov    eax,DWORD PTR [edx-0xc]
 7a4:	85 c0                	test   eax,eax
 7a6:	74 0d                	je     7b5 <deregister_tm_clones+0x35>
 7a8:	55                   	push   ebp
 7a9:	89 e5                	mov    ebp,esp
 7ab:	83 ec 14             	sub    esp,0x14
 7ae:	51                   	push   ecx
 7af:	ff d0                	call   eax
 7b1:	83 c4 10             	add    esp,0x10
 7b4:	c9                   	leave  
 7b5:	f3 c3                	repz ret 
 7b7:	89 f6                	mov    esi,esi
 7b9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

000007c0 <register_tm_clones>:
register_tm_clones():
 7c0:	e8 d7 00 00 00       	call   89c <__x86.get_pc_thunk.dx>
 7c5:	81 c2 3b 18 00 00    	add    edx,0x183b
 7cb:	55                   	push   ebp
 7cc:	8d 8a 4c 01 00 00    	lea    ecx,[edx+0x14c]
 7d2:	8d 82 4c 01 00 00    	lea    eax,[edx+0x14c]
 7d8:	89 e5                	mov    ebp,esp
 7da:	53                   	push   ebx
 7db:	29 c8                	sub    eax,ecx
 7dd:	c1 f8 02             	sar    eax,0x2
 7e0:	83 ec 04             	sub    esp,0x4
 7e3:	89 c3                	mov    ebx,eax
 7e5:	c1 eb 1f             	shr    ebx,0x1f
 7e8:	01 d8                	add    eax,ebx
 7ea:	d1 f8                	sar    eax,1
 7ec:	74 14                	je     802 <register_tm_clones+0x42>
 7ee:	8b 92 fc ff ff ff    	mov    edx,DWORD PTR [edx-0x4]
 7f4:	85 d2                	test   edx,edx
 7f6:	74 0a                	je     802 <register_tm_clones+0x42>
 7f8:	83 ec 08             	sub    esp,0x8
 7fb:	50                   	push   eax
 7fc:	51                   	push   ecx
 7fd:	ff d2                	call   edx
 7ff:	83 c4 10             	add    esp,0x10
 802:	8b 5d fc             	mov    ebx,DWORD PTR [ebp-0x4]
 805:	c9                   	leave  
 806:	c3                   	ret    
 807:	89 f6                	mov    esi,esi
 809:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00000810 <__do_global_dtors_aux>:
__do_global_dtors_aux():
 810:	55                   	push   ebp
 811:	89 e5                	mov    ebp,esp
 813:	53                   	push   ebx
 814:	e8 57 ff ff ff       	call   770 <__x86.get_pc_thunk.bx>
 819:	81 c3 e7 17 00 00    	add    ebx,0x17e7
 81f:	83 ec 04             	sub    esp,0x4
 822:	80 bb 4c 01 00 00 00 	cmp    BYTE PTR [ebx+0x14c],0x0
 829:	75 27                	jne    852 <__do_global_dtors_aux+0x42>
 82b:	8b 83 e4 ff ff ff    	mov    eax,DWORD PTR [ebx-0x1c]
 831:	85 c0                	test   eax,eax
 833:	74 11                	je     846 <__do_global_dtors_aux+0x36>
 835:	83 ec 0c             	sub    esp,0xc
 838:	ff b3 44 00 00 00    	push   DWORD PTR [ebx+0x44]
 83e:	e8 dd fe ff ff       	call   720 <.plt.got>
 843:	83 c4 10             	add    esp,0x10
 846:	e8 35 ff ff ff       	call   780 <deregister_tm_clones>
 84b:	c6 83 4c 01 00 00 01 	mov    BYTE PTR [ebx+0x14c],0x1
 852:	8b 5d fc             	mov    ebx,DWORD PTR [ebp-0x4]
 855:	c9                   	leave  
 856:	c3                   	ret    
 857:	89 f6                	mov    esi,esi
 859:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00000860 <frame_dummy>:
frame_dummy():
 860:	e8 37 00 00 00       	call   89c <__x86.get_pc_thunk.dx>
 865:	81 c2 9b 17 00 00    	add    edx,0x179b
 86b:	8d 82 d8 fe ff ff    	lea    eax,[edx-0x128]
 871:	8b 08                	mov    ecx,DWORD PTR [eax]
 873:	85 c9                	test   ecx,ecx
 875:	75 09                	jne    880 <frame_dummy+0x20>
 877:	e9 44 ff ff ff       	jmp    7c0 <register_tm_clones>
 87c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
 880:	8b 92 ec ff ff ff    	mov    edx,DWORD PTR [edx-0x14]
 886:	85 d2                	test   edx,edx
 888:	74 ed                	je     877 <frame_dummy+0x17>
 88a:	55                   	push   ebp
 88b:	89 e5                	mov    ebp,esp
 88d:	83 ec 14             	sub    esp,0x14
 890:	50                   	push   eax
 891:	ff d2                	call   edx
 893:	83 c4 10             	add    esp,0x10
 896:	c9                   	leave  
 897:	e9 24 ff ff ff       	jmp    7c0 <register_tm_clones>

0000089c <__x86.get_pc_thunk.dx>:
__x86.get_pc_thunk.dx():
 89c:	8b 14 24             	mov    edx,DWORD PTR [esp]
 89f:	c3                   	ret    

000008a0 <call_gbo>:
call_gbo():
/home/kyoupark/git/kb/asan/gbo.c:114
 8a0:	55                   	push   ebp
 8a1:	89 e5                	mov    ebp,esp
 8a3:	57                   	push   edi
 8a4:	56                   	push   esi
 8a5:	53                   	push   ebx
 8a6:	83 ec 7c             	sub    esp,0x7c
 8a9:	e8 c2 fe ff ff       	call   770 <__x86.get_pc_thunk.bx>
 8ae:	81 c3 52 17 00 00    	add    ebx,0x1752
 8b4:	8d 75 88             	lea    esi,[ebp-0x78]
 8b7:	89 75 84             	mov    DWORD PTR [ebp-0x7c],esi
 8ba:	8b 83 f0 ff ff ff    	mov    eax,DWORD PTR [ebx-0x10]
 8c0:	83 38 00             	cmp    DWORD PTR [eax],0x0
 8c3:	74 13                	je     8d8 <call_gbo+0x38>
 8c5:	83 ec 0c             	sub    esp,0xc
 8c8:	6a 60                	push   0x60
 8ca:	e8 e1 fd ff ff       	call   6b0 <__asan_stack_malloc_1@plt>
 8cf:	83 c4 10             	add    esp,0x10
 8d2:	85 c0                	test   eax,eax
 8d4:	74 02                	je     8d8 <call_gbo+0x38>
 8d6:	89 c6                	mov    esi,eax
 8d8:	8d 46 60             	lea    eax,[esi+0x60]
 8db:	89 c7                	mov    edi,eax
 8dd:	c7 06 b3 8a b5 41    	mov    DWORD PTR [esi],0x41b58ab3
 8e3:	8d 83 20 eb ff ff    	lea    eax,[ebx-0x14e0]
 8e9:	89 46 04             	mov    DWORD PTR [esi+0x4],eax
 8ec:	8d 83 a0 e8 ff ff    	lea    eax,[ebx-0x1760]
 8f2:	89 46 08             	mov    DWORD PTR [esi+0x8],eax
 8f5:	89 f1                	mov    ecx,esi
 8f7:	c1 e9 03             	shr    ecx,0x3
 8fa:	c7 81 00 00 00 20 f1 	mov    DWORD PTR [ecx+0x20000000],0xf1f1f1f1
 901:	f1 f1 f1 
 904:	c7 81 04 00 00 20 00 	mov    DWORD PTR [ecx+0x20000004],0xf4f40200
 90b:	02 f4 f4 
 90e:	c7 81 08 00 00 20 f3 	mov    DWORD PTR [ecx+0x20000008],0xf3f3f3f3
 915:	f3 f3 f3 
/home/kyoupark/git/kb/asan/gbo.c:116
 918:	8d 83 60 00 00 00    	lea    eax,[ebx+0x60]
 91e:	89 c2                	mov    edx,eax
 920:	89 d0                	mov    eax,edx
 922:	c1 e8 03             	shr    eax,0x3
 925:	05 00 00 00 20       	add    eax,0x20000000
 92a:	0f b6 00             	movzx  eax,BYTE PTR [eax]
 92d:	84 c0                	test   al,al
 92f:	74 09                	je     93a <call_gbo+0x9a>
 931:	83 ec 0c             	sub    esp,0xc
 934:	52                   	push   edx
 935:	e8 c6 fd ff ff       	call   700 <__asan_report_load8@plt>
 93a:	8b 83 60 00 00 00    	mov    eax,DWORD PTR [ebx+0x60]
 940:	8b 93 64 00 00 00    	mov    edx,DWORD PTR [ebx+0x64]
 946:	89 47 c0             	mov    DWORD PTR [edi-0x40],eax
 949:	89 57 c4             	mov    DWORD PTR [edi-0x3c],edx
/home/kyoupark/git/kb/asan/gbo.c:117
 94c:	90                   	nop
/home/kyoupark/git/kb/asan/gbo.c:114
 94d:	39 75 84             	cmp    DWORD PTR [ebp-0x7c],esi
 950:	74 26                	je     978 <call_gbo+0xd8>
 952:	c7 06 0e 36 e0 45    	mov    DWORD PTR [esi],0x45e0360e
 958:	c7 81 00 00 00 20 f5 	mov    DWORD PTR [ecx+0x20000000],0xf5f5f5f5
 95f:	f5 f5 f5 
 962:	c7 81 04 00 00 20 f5 	mov    DWORD PTR [ecx+0x20000004],0xf5f5f5f5
 969:	f5 f5 f5 
 96c:	c7 81 08 00 00 20 f5 	mov    DWORD PTR [ecx+0x20000008],0xf5f5f5f5
 973:	f5 f5 f5 
 976:	eb 1e                	jmp    996 <call_gbo+0xf6>
 978:	c7 81 00 00 00 20 00 	mov    DWORD PTR [ecx+0x20000000],0x0
 97f:	00 00 00 
 982:	c7 81 04 00 00 20 00 	mov    DWORD PTR [ecx+0x20000004],0x0
 989:	00 00 00 
 98c:	c7 81 08 00 00 20 00 	mov    DWORD PTR [ecx+0x20000008],0x0
 993:	00 00 00 
/home/kyoupark/git/kb/asan/gbo.c:118
 996:	8d 65 f4             	lea    esp,[ebp-0xc]
 999:	5b                   	pop    ebx
 99a:	5e                   	pop    esi
 99b:	5f                   	pop    edi
 99c:	5d                   	pop    ebp
 99d:	c3                   	ret    

0000099e <main>:
main():
/home/kyoupark/git/kb/asan/gbo.c:121
 99e:	8d 4c 24 04          	lea    ecx,[esp+0x4]
 9a2:	83 e4 f0             	and    esp,0xfffffff0
 9a5:	ff 71 fc             	push   DWORD PTR [ecx-0x4]
 9a8:	55                   	push   ebp
 9a9:	89 e5                	mov    ebp,esp
 9ab:	53                   	push   ebx
 9ac:	51                   	push   ecx
 9ad:	e8 be fd ff ff       	call   770 <__x86.get_pc_thunk.bx>
 9b2:	81 c3 4e 16 00 00    	add    ebx,0x164e
/home/kyoupark/git/kb/asan/gbo.c:122
 9b8:	83 ec 0c             	sub    esp,0xc
 9bb:	8d 83 40 eb ff ff    	lea    eax,[ebx-0x14c0]
 9c1:	50                   	push   eax
 9c2:	e8 49 fd ff ff       	call   710 <puts@plt>
 9c7:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo.c:123
 9ca:	83 ec 0c             	sub    esp,0xc
 9cd:	8d 83 80 eb ff ff    	lea    eax,[ebx-0x1480]
 9d3:	50                   	push   eax
 9d4:	e8 37 fd ff ff       	call   710 <puts@plt>
 9d9:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo.c:124
 9dc:	83 ec 0c             	sub    esp,0xc
 9df:	8d 83 e0 eb ff ff    	lea    eax,[ebx-0x1420]
 9e5:	50                   	push   eax
 9e6:	e8 25 fd ff ff       	call   710 <puts@plt>
 9eb:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo.c:126
 9ee:	e8 ad fe ff ff       	call   8a0 <call_gbo>
/home/kyoupark/git/kb/asan/gbo.c:128
 9f3:	83 ec 0c             	sub    esp,0xc
 9f6:	8d 83 20 ec ff ff    	lea    eax,[ebx-0x13e0]
 9fc:	50                   	push   eax
 9fd:	e8 0e fd ff ff       	call   710 <puts@plt>
 a02:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo.c:130
 a05:	b8 00 00 00 00       	mov    eax,0x0
/home/kyoupark/git/kb/asan/gbo.c:131
 a0a:	8d 65 f8             	lea    esp,[ebp-0x8]
 a0d:	59                   	pop    ecx
 a0e:	5b                   	pop    ebx
 a0f:	5d                   	pop    ebp
 a10:	8d 61 fc             	lea    esp,[ecx-0x4]
 a13:	c3                   	ret    

00000a14 <_GLOBAL__sub_D_00099_0_call_gbo>:
_GLOBAL__sub_D_00099_0_call_gbo():
 a14:	55                   	push   ebp
 a15:	89 e5                	mov    ebp,esp
 a17:	53                   	push   ebx
 a18:	83 ec 04             	sub    esp,0x4
 a1b:	e8 55 00 00 00       	call   a75 <__x86.get_pc_thunk.ax>
 a20:	05 e0 15 00 00       	add    eax,0x15e0
 a25:	83 ec 08             	sub    esp,0x8
 a28:	6a 05                	push   0x5
 a2a:	8d 90 c0 00 00 00    	lea    edx,[eax+0xc0]
 a30:	52                   	push   edx
 a31:	89 c3                	mov    ebx,eax
 a33:	e8 98 fc ff ff       	call   6d0 <__asan_unregister_globals@plt>
 a38:	83 c4 10             	add    esp,0x10
 a3b:	8b 5d fc             	mov    ebx,DWORD PTR [ebp-0x4]
 a3e:	c9                   	leave  
 a3f:	c3                   	ret    

00000a40 <_GLOBAL__sub_I_00099_1_call_gbo>:
_GLOBAL__sub_I_00099_1_call_gbo():
 a40:	55                   	push   ebp
 a41:	89 e5                	mov    ebp,esp
 a43:	53                   	push   ebx
 a44:	83 ec 04             	sub    esp,0x4
 a47:	e8 24 fd ff ff       	call   770 <__x86.get_pc_thunk.bx>
 a4c:	81 c3 b4 15 00 00    	add    ebx,0x15b4
 a52:	e8 89 fc ff ff       	call   6e0 <__asan_init@plt>
 a57:	e8 94 fc ff ff       	call   6f0 <__asan_version_mismatch_check_v6@plt>
 a5c:	83 ec 08             	sub    esp,0x8
 a5f:	6a 05                	push   0x5
 a61:	8d 83 c0 00 00 00    	lea    eax,[ebx+0xc0]
 a67:	50                   	push   eax
 a68:	e8 33 fc ff ff       	call   6a0 <__asan_register_globals@plt>
 a6d:	83 c4 10             	add    esp,0x10
 a70:	8b 5d fc             	mov    ebx,DWORD PTR [ebp-0x4]
 a73:	c9                   	leave  
 a74:	c3                   	ret    

00000a75 <__x86.get_pc_thunk.ax>:
__x86.get_pc_thunk.ax():
 a75:	8b 04 24             	mov    eax,DWORD PTR [esp]
 a78:	c3                   	ret    
 a79:	66 90                	xchg   ax,ax
 a7b:	66 90                	xchg   ax,ax
 a7d:	66 90                	xchg   ax,ax
 a7f:	90                   	nop

00000a80 <__libc_csu_init>:
__libc_csu_init():
 a80:	55                   	push   ebp
 a81:	57                   	push   edi
 a82:	56                   	push   esi
 a83:	53                   	push   ebx
 a84:	e8 e7 fc ff ff       	call   770 <__x86.get_pc_thunk.bx>
 a89:	81 c3 77 15 00 00    	add    ebx,0x1577
 a8f:	83 ec 0c             	sub    esp,0xc
 a92:	8b 6c 24 20          	mov    ebp,DWORD PTR [esp+0x20]
 a96:	8d b3 d0 fe ff ff    	lea    esi,[ebx-0x130]
 a9c:	e8 c7 fb ff ff       	call   668 <_init>
 aa1:	8d 83 c8 fe ff ff    	lea    eax,[ebx-0x138]
 aa7:	29 c6                	sub    esi,eax
 aa9:	c1 fe 02             	sar    esi,0x2
 aac:	85 f6                	test   esi,esi
 aae:	74 25                	je     ad5 <__libc_csu_init+0x55>
 ab0:	31 ff                	xor    edi,edi
 ab2:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
 ab8:	83 ec 04             	sub    esp,0x4
 abb:	ff 74 24 2c          	push   DWORD PTR [esp+0x2c]
 abf:	ff 74 24 2c          	push   DWORD PTR [esp+0x2c]
 ac3:	55                   	push   ebp
 ac4:	ff 94 bb c8 fe ff ff 	call   DWORD PTR [ebx+edi*4-0x138]
 acb:	83 c7 01             	add    edi,0x1
 ace:	83 c4 10             	add    esp,0x10
 ad1:	39 fe                	cmp    esi,edi
 ad3:	75 e3                	jne    ab8 <__libc_csu_init+0x38>
 ad5:	83 c4 0c             	add    esp,0xc
 ad8:	5b                   	pop    ebx
 ad9:	5e                   	pop    esi
 ada:	5f                   	pop    edi
 adb:	5d                   	pop    ebp
 adc:	c3                   	ret    
 add:	8d 76 00             	lea    esi,[esi+0x0]

00000ae0 <__libc_csu_fini>:
__libc_csu_fini():
 ae0:	f3 c3                	repz ret 

Disassembly of section .fini:

00000ae4 <_fini>:
_fini():
 ae4:	53                   	push   ebx
 ae5:	83 ec 08             	sub    esp,0x8
 ae8:	e8 83 fc ff ff       	call   770 <__x86.get_pc_thunk.bx>
 aed:	81 c3 13 15 00 00    	add    ebx,0x1513
 af3:	83 c4 08             	add    esp,0x8
 af6:	5b                   	pop    ebx
 af7:	c3                   	ret    
