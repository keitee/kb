
gbo_host_no_asan_32:     file format elf32-i386


Disassembly of section .init:

00000444 <_init>:
_init():
 444:	53                   	push   ebx
 445:	83 ec 08             	sub    esp,0x8
 448:	e8 d3 00 00 00       	call   520 <__x86.get_pc_thunk.bx>
 44d:	81 c3 b3 1b 00 00    	add    ebx,0x1bb3
 453:	8b 83 f0 ff ff ff    	mov    eax,DWORD PTR [ebx-0x10]
 459:	85 c0                	test   eax,eax
 45b:	74 05                	je     462 <_init+0x1e>
 45d:	e8 76 00 00 00       	call   4d8 <.plt.got+0x8>
 462:	83 c4 08             	add    esp,0x8
 465:	5b                   	pop    ebx
 466:	c3                   	ret    

Disassembly of section .plt:

00000470 <.plt>:
 470:	ff b3 04 00 00 00    	push   DWORD PTR [ebx+0x4]
 476:	ff a3 08 00 00 00    	jmp    DWORD PTR [ebx+0x8]
 47c:	00 00                	add    BYTE PTR [eax],al
	...

00000480 <printf@plt>:
 480:	ff a3 0c 00 00 00    	jmp    DWORD PTR [ebx+0xc]
 486:	68 00 00 00 00       	push   0x0
 48b:	e9 e0 ff ff ff       	jmp    470 <.plt>

00000490 <puts@plt>:
 490:	ff a3 10 00 00 00    	jmp    DWORD PTR [ebx+0x10]
 496:	68 08 00 00 00       	push   0x8
 49b:	e9 d0 ff ff ff       	jmp    470 <.plt>

000004a0 <dl_iterate_phdr@plt>:
 4a0:	ff a3 14 00 00 00    	jmp    DWORD PTR [ebx+0x14]
 4a6:	68 10 00 00 00       	push   0x10
 4ab:	e9 c0 ff ff ff       	jmp    470 <.plt>

000004b0 <__libc_start_main@plt>:
 4b0:	ff a3 18 00 00 00    	jmp    DWORD PTR [ebx+0x18]
 4b6:	68 18 00 00 00       	push   0x18
 4bb:	e9 b0 ff ff ff       	jmp    470 <.plt>

000004c0 <putchar@plt>:
 4c0:	ff a3 1c 00 00 00    	jmp    DWORD PTR [ebx+0x1c]
 4c6:	68 20 00 00 00       	push   0x20
 4cb:	e9 a0 ff ff ff       	jmp    470 <.plt>

Disassembly of section .plt.got:

000004d0 <.plt.got>:
 4d0:	ff a3 ec ff ff ff    	jmp    DWORD PTR [ebx-0x14]
 4d6:	66 90                	xchg   ax,ax
 4d8:	ff a3 f0 ff ff ff    	jmp    DWORD PTR [ebx-0x10]
 4de:	66 90                	xchg   ax,ax

Disassembly of section .text:

000004e0 <_start>:
_start():
 4e0:	31 ed                	xor    ebp,ebp
 4e2:	5e                   	pop    esi
 4e3:	89 e1                	mov    ecx,esp
 4e5:	83 e4 f0             	and    esp,0xfffffff0
 4e8:	50                   	push   eax
 4e9:	54                   	push   esp
 4ea:	52                   	push   edx
 4eb:	e8 22 00 00 00       	call   512 <_start+0x32>
 4f0:	81 c3 10 1b 00 00    	add    ebx,0x1b10
 4f6:	8d 83 40 e8 ff ff    	lea    eax,[ebx-0x17c0]
 4fc:	50                   	push   eax
 4fd:	8d 83 e0 e7 ff ff    	lea    eax,[ebx-0x1820]
 503:	50                   	push   eax
 504:	51                   	push   ecx
 505:	56                   	push   esi
 506:	ff b3 f4 ff ff ff    	push   DWORD PTR [ebx-0xc]
 50c:	e8 9f ff ff ff       	call   4b0 <__libc_start_main@plt>
 511:	f4                   	hlt    
 512:	8b 1c 24             	mov    ebx,DWORD PTR [esp]
 515:	c3                   	ret    
 516:	66 90                	xchg   ax,ax
 518:	66 90                	xchg   ax,ax
 51a:	66 90                	xchg   ax,ax
 51c:	66 90                	xchg   ax,ax
 51e:	66 90                	xchg   ax,ax

00000520 <__x86.get_pc_thunk.bx>:
__x86.get_pc_thunk.bx():
 520:	8b 1c 24             	mov    ebx,DWORD PTR [esp]
 523:	c3                   	ret    
 524:	66 90                	xchg   ax,ax
 526:	66 90                	xchg   ax,ax
 528:	66 90                	xchg   ax,ax
 52a:	66 90                	xchg   ax,ax
 52c:	66 90                	xchg   ax,ax
 52e:	66 90                	xchg   ax,ax

00000530 <deregister_tm_clones>:
deregister_tm_clones():
 530:	e8 17 01 00 00       	call   64c <__x86.get_pc_thunk.dx>
 535:	81 c2 cb 1a 00 00    	add    edx,0x1acb
 53b:	8d 8a 30 00 00 00    	lea    ecx,[edx+0x30]
 541:	8d 82 33 00 00 00    	lea    eax,[edx+0x33]
 547:	29 c8                	sub    eax,ecx
 549:	83 f8 06             	cmp    eax,0x6
 54c:	76 17                	jbe    565 <deregister_tm_clones+0x35>
 54e:	8b 82 e8 ff ff ff    	mov    eax,DWORD PTR [edx-0x18]
 554:	85 c0                	test   eax,eax
 556:	74 0d                	je     565 <deregister_tm_clones+0x35>
 558:	55                   	push   ebp
 559:	89 e5                	mov    ebp,esp
 55b:	83 ec 14             	sub    esp,0x14
 55e:	51                   	push   ecx
 55f:	ff d0                	call   eax
 561:	83 c4 10             	add    esp,0x10
 564:	c9                   	leave  
 565:	f3 c3                	repz ret 
 567:	89 f6                	mov    esi,esi
 569:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00000570 <register_tm_clones>:
register_tm_clones():
 570:	e8 d7 00 00 00       	call   64c <__x86.get_pc_thunk.dx>
 575:	81 c2 8b 1a 00 00    	add    edx,0x1a8b
 57b:	55                   	push   ebp
 57c:	8d 8a 30 00 00 00    	lea    ecx,[edx+0x30]
 582:	8d 82 30 00 00 00    	lea    eax,[edx+0x30]
 588:	89 e5                	mov    ebp,esp
 58a:	53                   	push   ebx
 58b:	29 c8                	sub    eax,ecx
 58d:	c1 f8 02             	sar    eax,0x2
 590:	83 ec 04             	sub    esp,0x4
 593:	89 c3                	mov    ebx,eax
 595:	c1 eb 1f             	shr    ebx,0x1f
 598:	01 d8                	add    eax,ebx
 59a:	d1 f8                	sar    eax,1
 59c:	74 14                	je     5b2 <register_tm_clones+0x42>
 59e:	8b 92 fc ff ff ff    	mov    edx,DWORD PTR [edx-0x4]
 5a4:	85 d2                	test   edx,edx
 5a6:	74 0a                	je     5b2 <register_tm_clones+0x42>
 5a8:	83 ec 08             	sub    esp,0x8
 5ab:	50                   	push   eax
 5ac:	51                   	push   ecx
 5ad:	ff d2                	call   edx
 5af:	83 c4 10             	add    esp,0x10
 5b2:	8b 5d fc             	mov    ebx,DWORD PTR [ebp-0x4]
 5b5:	c9                   	leave  
 5b6:	c3                   	ret    
 5b7:	89 f6                	mov    esi,esi
 5b9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

000005c0 <__do_global_dtors_aux>:
__do_global_dtors_aux():
 5c0:	55                   	push   ebp
 5c1:	89 e5                	mov    ebp,esp
 5c3:	53                   	push   ebx
 5c4:	e8 57 ff ff ff       	call   520 <__x86.get_pc_thunk.bx>
 5c9:	81 c3 37 1a 00 00    	add    ebx,0x1a37
 5cf:	83 ec 04             	sub    esp,0x4
 5d2:	80 bb 2e 00 00 00 00 	cmp    BYTE PTR [ebx+0x2e],0x0
 5d9:	75 27                	jne    602 <__do_global_dtors_aux+0x42>
 5db:	8b 83 ec ff ff ff    	mov    eax,DWORD PTR [ebx-0x14]
 5e1:	85 c0                	test   eax,eax
 5e3:	74 11                	je     5f6 <__do_global_dtors_aux+0x36>
 5e5:	83 ec 0c             	sub    esp,0xc
 5e8:	ff b3 24 00 00 00    	push   DWORD PTR [ebx+0x24]
 5ee:	e8 dd fe ff ff       	call   4d0 <.plt.got>
 5f3:	83 c4 10             	add    esp,0x10
 5f6:	e8 35 ff ff ff       	call   530 <deregister_tm_clones>
 5fb:	c6 83 2e 00 00 00 01 	mov    BYTE PTR [ebx+0x2e],0x1
 602:	8b 5d fc             	mov    ebx,DWORD PTR [ebp-0x4]
 605:	c9                   	leave  
 606:	c3                   	ret    
 607:	89 f6                	mov    esi,esi
 609:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00000610 <frame_dummy>:
frame_dummy():
 610:	e8 37 00 00 00       	call   64c <__x86.get_pc_thunk.dx>
 615:	81 c2 eb 19 00 00    	add    edx,0x19eb
 61b:	8d 82 f4 fe ff ff    	lea    eax,[edx-0x10c]
 621:	8b 08                	mov    ecx,DWORD PTR [eax]
 623:	85 c9                	test   ecx,ecx
 625:	75 09                	jne    630 <frame_dummy+0x20>
 627:	e9 44 ff ff ff       	jmp    570 <register_tm_clones>
 62c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
 630:	8b 92 f8 ff ff ff    	mov    edx,DWORD PTR [edx-0x8]
 636:	85 d2                	test   edx,edx
 638:	74 ed                	je     627 <frame_dummy+0x17>
 63a:	55                   	push   ebp
 63b:	89 e5                	mov    ebp,esp
 63d:	83 ec 14             	sub    esp,0x14
 640:	50                   	push   eax
 641:	ff d2                	call   edx
 643:	83 c4 10             	add    esp,0x10
 646:	c9                   	leave  
 647:	e9 24 ff ff ff       	jmp    570 <register_tm_clones>

0000064c <__x86.get_pc_thunk.dx>:
__x86.get_pc_thunk.dx():
 64c:	8b 14 24             	mov    edx,DWORD PTR [esp]
 64f:	c3                   	ret    

00000650 <header_handler>:
header_handler():
/home/kyoupark/git/kb/asan/gbo.c:114
 650:	55                   	push   ebp
 651:	89 e5                	mov    ebp,esp
 653:	53                   	push   ebx
 654:	83 ec 14             	sub    esp,0x14
 657:	e8 c4 fe ff ff       	call   520 <__x86.get_pc_thunk.bx>
 65c:	81 c3 a4 19 00 00    	add    ebx,0x19a4
/home/kyoupark/git/kb/asan/gbo.c:115
 662:	c7 45 f4 00 00 00 00 	mov    DWORD PTR [ebp-0xc],0x0
/home/kyoupark/git/kb/asan/gbo.c:117
 669:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 66c:	8b 00                	mov    eax,DWORD PTR [eax]
/home/kyoupark/git/kb/asan/gbo.c:116
 66e:	89 c1                	mov    ecx,eax
/home/kyoupark/git/kb/asan/gbo.c:117
 670:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 673:	0f b7 40 0c          	movzx  eax,WORD PTR [eax+0xc]
/home/kyoupark/git/kb/asan/gbo.c:116
 677:	0f b7 d0             	movzx  edx,ax
 67a:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 67d:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
 680:	51                   	push   ecx
 681:	52                   	push   edx
 682:	50                   	push   eax
 683:	8d 83 60 e8 ff ff    	lea    eax,[ebx-0x17a0]
 689:	50                   	push   eax
 68a:	e8 f1 fd ff ff       	call   480 <printf@plt>
 68f:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo.c:118
 692:	c7 45 f4 00 00 00 00 	mov    DWORD PTR [ebp-0xc],0x0
 699:	eb 67                	jmp    702 <header_handler+0xb2>
/home/kyoupark/git/kb/asan/gbo.c:120 (discriminator 3)
 69b:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 69e:	8b 10                	mov    edx,DWORD PTR [eax]
 6a0:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 6a3:	8b 40 08             	mov    eax,DWORD PTR [eax+0x8]
 6a6:	8b 4d f4             	mov    ecx,DWORD PTR [ebp-0xc]
 6a9:	c1 e1 05             	shl    ecx,0x5
 6ac:	01 c8                	add    eax,ecx
 6ae:	8b 40 08             	mov    eax,DWORD PTR [eax+0x8]
 6b1:	01 d0                	add    eax,edx
/home/kyoupark/git/kb/asan/gbo.c:119 (discriminator 3)
 6b3:	83 ec 04             	sub    esp,0x4
 6b6:	50                   	push   eax
 6b7:	ff 75 f4             	push   DWORD PTR [ebp-0xc]
 6ba:	8d 83 82 e8 ff ff    	lea    eax,[ebx-0x177e]
 6c0:	50                   	push   eax
 6c1:	e8 ba fd ff ff       	call   480 <printf@plt>
 6c6:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo.c:122 (discriminator 3)
 6c9:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 6cc:	8b 40 08             	mov    eax,DWORD PTR [eax+0x8]
 6cf:	8b 55 f4             	mov    edx,DWORD PTR [ebp-0xc]
 6d2:	c1 e2 05             	shl    edx,0x5
 6d5:	01 d0                	add    eax,edx
/home/kyoupark/git/kb/asan/gbo.c:121 (discriminator 3)
 6d7:	8b 50 18             	mov    edx,DWORD PTR [eax+0x18]
/home/kyoupark/git/kb/asan/gbo.c:122 (discriminator 3)
 6da:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 6dd:	8b 40 08             	mov    eax,DWORD PTR [eax+0x8]
 6e0:	8b 4d f4             	mov    ecx,DWORD PTR [ebp-0xc]
 6e3:	c1 e1 05             	shl    ecx,0x5
 6e6:	01 c8                	add    eax,ecx
/home/kyoupark/git/kb/asan/gbo.c:121 (discriminator 3)
 6e8:	8b 00                	mov    eax,DWORD PTR [eax]
 6ea:	83 ec 04             	sub    esp,0x4
 6ed:	52                   	push   edx
 6ee:	50                   	push   eax
 6ef:	8d 83 9f e8 ff ff    	lea    eax,[ebx-0x1761]
 6f5:	50                   	push   eax
 6f6:	e8 85 fd ff ff       	call   480 <printf@plt>
 6fb:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo.c:118 (discriminator 3)
 6fe:	83 45 f4 01          	add    DWORD PTR [ebp-0xc],0x1
/home/kyoupark/git/kb/asan/gbo.c:118 (discriminator 1)
 702:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 705:	0f b7 40 0c          	movzx  eax,WORD PTR [eax+0xc]
 709:	0f b7 c0             	movzx  eax,ax
 70c:	3b 45 f4             	cmp    eax,DWORD PTR [ebp-0xc]
 70f:	7f 8a                	jg     69b <header_handler+0x4b>
/home/kyoupark/git/kb/asan/gbo.c:124
 711:	83 ec 0c             	sub    esp,0xc
 714:	6a 0a                	push   0xa
 716:	e8 a5 fd ff ff       	call   4c0 <putchar@plt>
 71b:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo.c:125
 71e:	b8 00 00 00 00       	mov    eax,0x0
/home/kyoupark/git/kb/asan/gbo.c:126
 723:	8b 5d fc             	mov    ebx,DWORD PTR [ebp-0x4]
 726:	c9                   	leave  
 727:	c3                   	ret    

00000728 <call_gbo>:
call_gbo():
/home/kyoupark/git/kb/asan/gbo.c:131
 728:	55                   	push   ebp
 729:	89 e5                	mov    ebp,esp
 72b:	83 ec 10             	sub    esp,0x10
 72e:	e8 a4 00 00 00       	call   7d7 <__x86.get_pc_thunk.ax>
 733:	05 cd 18 00 00       	add    eax,0x18cd
/home/kyoupark/git/kb/asan/gbo.c:133
 738:	8b 90 2c 00 00 00    	mov    edx,DWORD PTR [eax+0x2c]
 73e:	8b 80 28 00 00 00    	mov    eax,DWORD PTR [eax+0x28]
 744:	89 45 f6             	mov    DWORD PTR [ebp-0xa],eax
 747:	89 55 fa             	mov    DWORD PTR [ebp-0x6],edx
/home/kyoupark/git/kb/asan/gbo.c:134
 74a:	90                   	nop
/home/kyoupark/git/kb/asan/gbo.c:135
 74b:	c9                   	leave  
 74c:	c3                   	ret    

0000074d <main>:
main():
/home/kyoupark/git/kb/asan/gbo.c:138
 74d:	8d 4c 24 04          	lea    ecx,[esp+0x4]
 751:	83 e4 f0             	and    esp,0xfffffff0
 754:	ff 71 fc             	push   DWORD PTR [ecx-0x4]
 757:	55                   	push   ebp
 758:	89 e5                	mov    ebp,esp
 75a:	53                   	push   ebx
 75b:	51                   	push   ecx
 75c:	e8 bf fd ff ff       	call   520 <__x86.get_pc_thunk.bx>
 761:	81 c3 9f 18 00 00    	add    ebx,0x189f
/home/kyoupark/git/kb/asan/gbo.c:139
 767:	83 ec 08             	sub    esp,0x8
 76a:	6a 00                	push   0x0
 76c:	8d 83 50 e6 ff ff    	lea    eax,[ebx-0x19b0]
 772:	50                   	push   eax
 773:	e8 28 fd ff ff       	call   4a0 <dl_iterate_phdr@plt>
 778:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo.c:141
 77b:	83 ec 0c             	sub    esp,0xc
 77e:	8d 83 b8 e8 ff ff    	lea    eax,[ebx-0x1748]
 784:	50                   	push   eax
 785:	e8 06 fd ff ff       	call   490 <puts@plt>
 78a:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo.c:142
 78d:	83 ec 0c             	sub    esp,0xc
 790:	8d 83 c8 e8 ff ff    	lea    eax,[ebx-0x1738]
 796:	50                   	push   eax
 797:	e8 f4 fc ff ff       	call   490 <puts@plt>
 79c:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo.c:143
 79f:	83 ec 0c             	sub    esp,0xc
 7a2:	8d 83 ec e8 ff ff    	lea    eax,[ebx-0x1714]
 7a8:	50                   	push   eax
 7a9:	e8 e2 fc ff ff       	call   490 <puts@plt>
 7ae:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo.c:145
 7b1:	e8 72 ff ff ff       	call   728 <call_gbo>
/home/kyoupark/git/kb/asan/gbo.c:147
 7b6:	83 ec 0c             	sub    esp,0xc
 7b9:	8d 83 f3 e8 ff ff    	lea    eax,[ebx-0x170d]
 7bf:	50                   	push   eax
 7c0:	e8 cb fc ff ff       	call   490 <puts@plt>
 7c5:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo.c:149
 7c8:	b8 00 00 00 00       	mov    eax,0x0
/home/kyoupark/git/kb/asan/gbo.c:150
 7cd:	8d 65 f8             	lea    esp,[ebp-0x8]
 7d0:	59                   	pop    ecx
 7d1:	5b                   	pop    ebx
 7d2:	5d                   	pop    ebp
 7d3:	8d 61 fc             	lea    esp,[ecx-0x4]
 7d6:	c3                   	ret    

000007d7 <__x86.get_pc_thunk.ax>:
__x86.get_pc_thunk.ax():
 7d7:	8b 04 24             	mov    eax,DWORD PTR [esp]
 7da:	c3                   	ret    
 7db:	66 90                	xchg   ax,ax
 7dd:	66 90                	xchg   ax,ax
 7df:	90                   	nop

000007e0 <__libc_csu_init>:
__libc_csu_init():
 7e0:	55                   	push   ebp
 7e1:	57                   	push   edi
 7e2:	56                   	push   esi
 7e3:	53                   	push   ebx
 7e4:	e8 37 fd ff ff       	call   520 <__x86.get_pc_thunk.bx>
 7e9:	81 c3 17 18 00 00    	add    ebx,0x1817
 7ef:	83 ec 0c             	sub    esp,0xc
 7f2:	8b 6c 24 20          	mov    ebp,DWORD PTR [esp+0x20]
 7f6:	8d b3 f0 fe ff ff    	lea    esi,[ebx-0x110]
 7fc:	e8 43 fc ff ff       	call   444 <_init>
 801:	8d 83 ec fe ff ff    	lea    eax,[ebx-0x114]
 807:	29 c6                	sub    esi,eax
 809:	c1 fe 02             	sar    esi,0x2
 80c:	85 f6                	test   esi,esi
 80e:	74 25                	je     835 <__libc_csu_init+0x55>
 810:	31 ff                	xor    edi,edi
 812:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
 818:	83 ec 04             	sub    esp,0x4
 81b:	ff 74 24 2c          	push   DWORD PTR [esp+0x2c]
 81f:	ff 74 24 2c          	push   DWORD PTR [esp+0x2c]
 823:	55                   	push   ebp
 824:	ff 94 bb ec fe ff ff 	call   DWORD PTR [ebx+edi*4-0x114]
 82b:	83 c7 01             	add    edi,0x1
 82e:	83 c4 10             	add    esp,0x10
 831:	39 fe                	cmp    esi,edi
 833:	75 e3                	jne    818 <__libc_csu_init+0x38>
 835:	83 c4 0c             	add    esp,0xc
 838:	5b                   	pop    ebx
 839:	5e                   	pop    esi
 83a:	5f                   	pop    edi
 83b:	5d                   	pop    ebp
 83c:	c3                   	ret    
 83d:	8d 76 00             	lea    esi,[esi+0x0]

00000840 <__libc_csu_fini>:
__libc_csu_fini():
 840:	f3 c3                	repz ret 

Disassembly of section .fini:

00000844 <_fini>:
_fini():
 844:	53                   	push   ebx
 845:	83 ec 08             	sub    esp,0x8
 848:	e8 d3 fc ff ff       	call   520 <__x86.get_pc_thunk.bx>
 84d:	81 c3 b3 17 00 00    	add    ebx,0x17b3
 853:	83 c4 08             	add    esp,0x8
 856:	5b                   	pop    ebx
 857:	c3                   	ret    
