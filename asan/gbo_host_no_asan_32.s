
gbo_host_no_asan:     file format elf32-i386


Disassembly of section .init:

000003bc <_init>:
_init():
 3bc:	53                   	push   ebx
 3bd:	83 ec 08             	sub    esp,0x8
 3c0:	e8 9b 00 00 00       	call   460 <__x86.get_pc_thunk.bx>
 3c5:	81 c3 3b 1c 00 00    	add    ebx,0x1c3b
 3cb:	8b 83 f0 ff ff ff    	mov    eax,DWORD PTR [ebx-0x10]
 3d1:	85 c0                	test   eax,eax
 3d3:	74 05                	je     3da <_init+0x1e>
 3d5:	e8 3e 00 00 00       	call   418 <.plt.got+0x8>
 3da:	83 c4 08             	add    esp,0x8
 3dd:	5b                   	pop    ebx
 3de:	c3                   	ret    

Disassembly of section .plt:

000003e0 <.plt>:
 3e0:	ff b3 04 00 00 00    	push   DWORD PTR [ebx+0x4]
 3e6:	ff a3 08 00 00 00    	jmp    DWORD PTR [ebx+0x8]
 3ec:	00 00                	add    BYTE PTR [eax],al
	...

000003f0 <puts@plt>:
 3f0:	ff a3 0c 00 00 00    	jmp    DWORD PTR [ebx+0xc]
 3f6:	68 00 00 00 00       	push   0x0
 3fb:	e9 e0 ff ff ff       	jmp    3e0 <.plt>

00000400 <__libc_start_main@plt>:
 400:	ff a3 10 00 00 00    	jmp    DWORD PTR [ebx+0x10]
 406:	68 08 00 00 00       	push   0x8
 40b:	e9 d0 ff ff ff       	jmp    3e0 <.plt>

Disassembly of section .plt.got:

00000410 <.plt.got>:
 410:	ff a3 ec ff ff ff    	jmp    DWORD PTR [ebx-0x14]
 416:	66 90                	xchg   ax,ax
 418:	ff a3 f0 ff ff ff    	jmp    DWORD PTR [ebx-0x10]
 41e:	66 90                	xchg   ax,ax

Disassembly of section .text:

00000420 <_start>:
_start():
 420:	31 ed                	xor    ebp,ebp
 422:	5e                   	pop    esi
 423:	89 e1                	mov    ecx,esp
 425:	83 e4 f0             	and    esp,0xfffffff0
 428:	50                   	push   eax
 429:	54                   	push   esp
 42a:	52                   	push   edx
 42b:	e8 22 00 00 00       	call   452 <_start+0x32>
 430:	81 c3 d0 1b 00 00    	add    ebx,0x1bd0
 436:	8d 83 90 e6 ff ff    	lea    eax,[ebx-0x1970]
 43c:	50                   	push   eax
 43d:	8d 83 30 e6 ff ff    	lea    eax,[ebx-0x19d0]
 443:	50                   	push   eax
 444:	51                   	push   ecx
 445:	56                   	push   esi
 446:	ff b3 f4 ff ff ff    	push   DWORD PTR [ebx-0xc]
 44c:	e8 af ff ff ff       	call   400 <__libc_start_main@plt>
 451:	f4                   	hlt    
 452:	8b 1c 24             	mov    ebx,DWORD PTR [esp]
 455:	c3                   	ret    
 456:	66 90                	xchg   ax,ax
 458:	66 90                	xchg   ax,ax
 45a:	66 90                	xchg   ax,ax
 45c:	66 90                	xchg   ax,ax
 45e:	66 90                	xchg   ax,ax

00000460 <__x86.get_pc_thunk.bx>:
__x86.get_pc_thunk.bx():
 460:	8b 1c 24             	mov    ebx,DWORD PTR [esp]
 463:	c3                   	ret    
 464:	66 90                	xchg   ax,ax
 466:	66 90                	xchg   ax,ax
 468:	66 90                	xchg   ax,ax
 46a:	66 90                	xchg   ax,ax
 46c:	66 90                	xchg   ax,ax
 46e:	66 90                	xchg   ax,ax

00000470 <deregister_tm_clones>:
deregister_tm_clones():
 470:	e8 17 01 00 00       	call   58c <__x86.get_pc_thunk.dx>
 475:	81 c2 8b 1b 00 00    	add    edx,0x1b8b
 47b:	8d 8a 24 00 00 00    	lea    ecx,[edx+0x24]
 481:	8d 82 27 00 00 00    	lea    eax,[edx+0x27]
 487:	29 c8                	sub    eax,ecx
 489:	83 f8 06             	cmp    eax,0x6
 48c:	76 17                	jbe    4a5 <deregister_tm_clones+0x35>
 48e:	8b 82 e8 ff ff ff    	mov    eax,DWORD PTR [edx-0x18]
 494:	85 c0                	test   eax,eax
 496:	74 0d                	je     4a5 <deregister_tm_clones+0x35>
 498:	55                   	push   ebp
 499:	89 e5                	mov    ebp,esp
 49b:	83 ec 14             	sub    esp,0x14
 49e:	51                   	push   ecx
 49f:	ff d0                	call   eax
 4a1:	83 c4 10             	add    esp,0x10
 4a4:	c9                   	leave  
 4a5:	f3 c3                	repz ret 
 4a7:	89 f6                	mov    esi,esi
 4a9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

000004b0 <register_tm_clones>:
register_tm_clones():
 4b0:	e8 d7 00 00 00       	call   58c <__x86.get_pc_thunk.dx>
 4b5:	81 c2 4b 1b 00 00    	add    edx,0x1b4b
 4bb:	55                   	push   ebp
 4bc:	8d 8a 24 00 00 00    	lea    ecx,[edx+0x24]
 4c2:	8d 82 24 00 00 00    	lea    eax,[edx+0x24]
 4c8:	89 e5                	mov    ebp,esp
 4ca:	53                   	push   ebx
 4cb:	29 c8                	sub    eax,ecx
 4cd:	c1 f8 02             	sar    eax,0x2
 4d0:	83 ec 04             	sub    esp,0x4
 4d3:	89 c3                	mov    ebx,eax
 4d5:	c1 eb 1f             	shr    ebx,0x1f
 4d8:	01 d8                	add    eax,ebx
 4da:	d1 f8                	sar    eax,1
 4dc:	74 14                	je     4f2 <register_tm_clones+0x42>
 4de:	8b 92 fc ff ff ff    	mov    edx,DWORD PTR [edx-0x4]
 4e4:	85 d2                	test   edx,edx
 4e6:	74 0a                	je     4f2 <register_tm_clones+0x42>
 4e8:	83 ec 08             	sub    esp,0x8
 4eb:	50                   	push   eax
 4ec:	51                   	push   ecx
 4ed:	ff d2                	call   edx
 4ef:	83 c4 10             	add    esp,0x10
 4f2:	8b 5d fc             	mov    ebx,DWORD PTR [ebp-0x4]
 4f5:	c9                   	leave  
 4f6:	c3                   	ret    
 4f7:	89 f6                	mov    esi,esi
 4f9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00000500 <__do_global_dtors_aux>:
__do_global_dtors_aux():
 500:	55                   	push   ebp
 501:	89 e5                	mov    ebp,esp
 503:	53                   	push   ebx
 504:	e8 57 ff ff ff       	call   460 <__x86.get_pc_thunk.bx>
 509:	81 c3 f7 1a 00 00    	add    ebx,0x1af7
 50f:	83 ec 04             	sub    esp,0x4
 512:	80 bb 22 00 00 00 00 	cmp    BYTE PTR [ebx+0x22],0x0
 519:	75 27                	jne    542 <__do_global_dtors_aux+0x42>
 51b:	8b 83 ec ff ff ff    	mov    eax,DWORD PTR [ebx-0x14]
 521:	85 c0                	test   eax,eax
 523:	74 11                	je     536 <__do_global_dtors_aux+0x36>
 525:	83 ec 0c             	sub    esp,0xc
 528:	ff b3 18 00 00 00    	push   DWORD PTR [ebx+0x18]
 52e:	e8 dd fe ff ff       	call   410 <.plt.got>
 533:	83 c4 10             	add    esp,0x10
 536:	e8 35 ff ff ff       	call   470 <deregister_tm_clones>
 53b:	c6 83 22 00 00 00 01 	mov    BYTE PTR [ebx+0x22],0x1
 542:	8b 5d fc             	mov    ebx,DWORD PTR [ebp-0x4]
 545:	c9                   	leave  
 546:	c3                   	ret    
 547:	89 f6                	mov    esi,esi
 549:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00000550 <frame_dummy>:
frame_dummy():
 550:	e8 37 00 00 00       	call   58c <__x86.get_pc_thunk.dx>
 555:	81 c2 ab 1a 00 00    	add    edx,0x1aab
 55b:	8d 82 f4 fe ff ff    	lea    eax,[edx-0x10c]
 561:	8b 08                	mov    ecx,DWORD PTR [eax]
 563:	85 c9                	test   ecx,ecx
 565:	75 09                	jne    570 <frame_dummy+0x20>
 567:	e9 44 ff ff ff       	jmp    4b0 <register_tm_clones>
 56c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
 570:	8b 92 f8 ff ff ff    	mov    edx,DWORD PTR [edx-0x8]
 576:	85 d2                	test   edx,edx
 578:	74 ed                	je     567 <frame_dummy+0x17>
 57a:	55                   	push   ebp
 57b:	89 e5                	mov    ebp,esp
 57d:	83 ec 14             	sub    esp,0x14
 580:	50                   	push   eax
 581:	ff d2                	call   edx
 583:	83 c4 10             	add    esp,0x10
 586:	c9                   	leave  
 587:	e9 24 ff ff ff       	jmp    4b0 <register_tm_clones>

0000058c <__x86.get_pc_thunk.dx>:
__x86.get_pc_thunk.dx():
 58c:	8b 14 24             	mov    edx,DWORD PTR [esp]
 58f:	c3                   	ret    

00000590 <call_gbo>:
call_gbo():
/home/kyoupark/git/kb/asan/gbo.c:114
 590:	55                   	push   ebp
 591:	89 e5                	mov    ebp,esp
 593:	83 ec 10             	sub    esp,0x10
 596:	e8 90 00 00 00       	call   62b <__x86.get_pc_thunk.ax>
 59b:	05 65 1a 00 00       	add    eax,0x1a65
/home/kyoupark/git/kb/asan/gbo.c:116
 5a0:	8b 90 20 00 00 00    	mov    edx,DWORD PTR [eax+0x20]
 5a6:	8b 80 1c 00 00 00    	mov    eax,DWORD PTR [eax+0x1c]
 5ac:	89 45 f6             	mov    DWORD PTR [ebp-0xa],eax
 5af:	89 55 fa             	mov    DWORD PTR [ebp-0x6],edx
/home/kyoupark/git/kb/asan/gbo.c:117
 5b2:	90                   	nop
/home/kyoupark/git/kb/asan/gbo.c:118
 5b3:	c9                   	leave  
 5b4:	c3                   	ret    

000005b5 <main>:
main():
/home/kyoupark/git/kb/asan/gbo.c:121
 5b5:	8d 4c 24 04          	lea    ecx,[esp+0x4]
 5b9:	83 e4 f0             	and    esp,0xfffffff0
 5bc:	ff 71 fc             	push   DWORD PTR [ecx-0x4]
 5bf:	55                   	push   ebp
 5c0:	89 e5                	mov    ebp,esp
 5c2:	53                   	push   ebx
 5c3:	51                   	push   ecx
 5c4:	e8 97 fe ff ff       	call   460 <__x86.get_pc_thunk.bx>
 5c9:	81 c3 37 1a 00 00    	add    ebx,0x1a37
/home/kyoupark/git/kb/asan/gbo.c:122
 5cf:	83 ec 0c             	sub    esp,0xc
 5d2:	8d 83 b0 e6 ff ff    	lea    eax,[ebx-0x1950]
 5d8:	50                   	push   eax
 5d9:	e8 12 fe ff ff       	call   3f0 <puts@plt>
 5de:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo.c:123
 5e1:	83 ec 0c             	sub    esp,0xc
 5e4:	8d 83 c0 e6 ff ff    	lea    eax,[ebx-0x1940]
 5ea:	50                   	push   eax
 5eb:	e8 00 fe ff ff       	call   3f0 <puts@plt>
 5f0:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo.c:124
 5f3:	83 ec 0c             	sub    esp,0xc
 5f6:	8d 83 e4 e6 ff ff    	lea    eax,[ebx-0x191c]
 5fc:	50                   	push   eax
 5fd:	e8 ee fd ff ff       	call   3f0 <puts@plt>
 602:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo.c:126
 605:	e8 86 ff ff ff       	call   590 <call_gbo>
/home/kyoupark/git/kb/asan/gbo.c:128
 60a:	83 ec 0c             	sub    esp,0xc
 60d:	8d 83 eb e6 ff ff    	lea    eax,[ebx-0x1915]
 613:	50                   	push   eax
 614:	e8 d7 fd ff ff       	call   3f0 <puts@plt>
 619:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/gbo.c:130
 61c:	b8 00 00 00 00       	mov    eax,0x0
/home/kyoupark/git/kb/asan/gbo.c:131
 621:	8d 65 f8             	lea    esp,[ebp-0x8]
 624:	59                   	pop    ecx
 625:	5b                   	pop    ebx
 626:	5d                   	pop    ebp
 627:	8d 61 fc             	lea    esp,[ecx-0x4]
 62a:	c3                   	ret    

0000062b <__x86.get_pc_thunk.ax>:
__x86.get_pc_thunk.ax():
 62b:	8b 04 24             	mov    eax,DWORD PTR [esp]
 62e:	c3                   	ret    
 62f:	90                   	nop

00000630 <__libc_csu_init>:
__libc_csu_init():
 630:	55                   	push   ebp
 631:	57                   	push   edi
 632:	56                   	push   esi
 633:	53                   	push   ebx
 634:	e8 27 fe ff ff       	call   460 <__x86.get_pc_thunk.bx>
 639:	81 c3 c7 19 00 00    	add    ebx,0x19c7
 63f:	83 ec 0c             	sub    esp,0xc
 642:	8b 6c 24 20          	mov    ebp,DWORD PTR [esp+0x20]
 646:	8d b3 f0 fe ff ff    	lea    esi,[ebx-0x110]
 64c:	e8 6b fd ff ff       	call   3bc <_init>
 651:	8d 83 ec fe ff ff    	lea    eax,[ebx-0x114]
 657:	29 c6                	sub    esi,eax
 659:	c1 fe 02             	sar    esi,0x2
 65c:	85 f6                	test   esi,esi
 65e:	74 25                	je     685 <__libc_csu_init+0x55>
 660:	31 ff                	xor    edi,edi
 662:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
 668:	83 ec 04             	sub    esp,0x4
 66b:	ff 74 24 2c          	push   DWORD PTR [esp+0x2c]
 66f:	ff 74 24 2c          	push   DWORD PTR [esp+0x2c]
 673:	55                   	push   ebp
 674:	ff 94 bb ec fe ff ff 	call   DWORD PTR [ebx+edi*4-0x114]
 67b:	83 c7 01             	add    edi,0x1
 67e:	83 c4 10             	add    esp,0x10
 681:	39 fe                	cmp    esi,edi
 683:	75 e3                	jne    668 <__libc_csu_init+0x38>
 685:	83 c4 0c             	add    esp,0xc
 688:	5b                   	pop    ebx
 689:	5e                   	pop    esi
 68a:	5f                   	pop    edi
 68b:	5d                   	pop    ebp
 68c:	c3                   	ret    
 68d:	8d 76 00             	lea    esi,[esi+0x0]

00000690 <__libc_csu_fini>:
__libc_csu_fini():
 690:	f3 c3                	repz ret 

Disassembly of section .fini:

00000694 <_fini>:
_fini():
 694:	53                   	push   ebx
 695:	83 ec 08             	sub    esp,0x8
 698:	e8 c3 fd ff ff       	call   460 <__x86.get_pc_thunk.bx>
 69d:	81 c3 63 19 00 00    	add    ebx,0x1963
 6a3:	83 c4 08             	add    esp,0x8
 6a6:	5b                   	pop    ebx
 6a7:	c3                   	ret    
