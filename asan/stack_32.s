
stack:     file format elf32-i386


Disassembly of section .init:

000003dc <_init>:
_init():
 3dc:	53                   	push   ebx
 3dd:	83 ec 08             	sub    esp,0x8
 3e0:	e8 ab 00 00 00       	call   490 <__x86.get_pc_thunk.bx>
 3e5:	81 c3 1b 1c 00 00    	add    ebx,0x1c1b
 3eb:	8b 83 f0 ff ff ff    	mov    eax,DWORD PTR [ebx-0x10]
 3f1:	85 c0                	test   eax,eax
 3f3:	74 05                	je     3fa <_init+0x1e>
 3f5:	e8 4e 00 00 00       	call   448 <.plt.got+0x8>
 3fa:	83 c4 08             	add    esp,0x8
 3fd:	5b                   	pop    ebx
 3fe:	c3                   	ret    

Disassembly of section .plt:

00000400 <.plt>:
 400:	ff b3 04 00 00 00    	push   DWORD PTR [ebx+0x4]
 406:	ff a3 08 00 00 00    	jmp    DWORD PTR [ebx+0x8]
 40c:	00 00                	add    BYTE PTR [eax],al
	...

00000410 <printf@plt>:
 410:	ff a3 0c 00 00 00    	jmp    DWORD PTR [ebx+0xc]
 416:	68 00 00 00 00       	push   0x0
 41b:	e9 e0 ff ff ff       	jmp    400 <.plt>

00000420 <puts@plt>:
 420:	ff a3 10 00 00 00    	jmp    DWORD PTR [ebx+0x10]
 426:	68 08 00 00 00       	push   0x8
 42b:	e9 d0 ff ff ff       	jmp    400 <.plt>

00000430 <__libc_start_main@plt>:
 430:	ff a3 14 00 00 00    	jmp    DWORD PTR [ebx+0x14]
 436:	68 10 00 00 00       	push   0x10
 43b:	e9 c0 ff ff ff       	jmp    400 <.plt>

Disassembly of section .plt.got:

00000440 <.plt.got>:
 440:	ff a3 ec ff ff ff    	jmp    DWORD PTR [ebx-0x14]
 446:	66 90                	xchg   ax,ax
 448:	ff a3 f0 ff ff ff    	jmp    DWORD PTR [ebx-0x10]
 44e:	66 90                	xchg   ax,ax

Disassembly of section .text:

00000450 <_start>:
_start():
 450:	31 ed                	xor    ebp,ebp
 452:	5e                   	pop    esi
 453:	89 e1                	mov    ecx,esp
 455:	83 e4 f0             	and    esp,0xfffffff0
 458:	50                   	push   eax
 459:	54                   	push   esp
 45a:	52                   	push   edx
 45b:	e8 22 00 00 00       	call   482 <_start+0x32>
 460:	81 c3 a0 1b 00 00    	add    ebx,0x1ba0
 466:	8d 83 f0 e7 ff ff    	lea    eax,[ebx-0x1810]
 46c:	50                   	push   eax
 46d:	8d 83 90 e7 ff ff    	lea    eax,[ebx-0x1870]
 473:	50                   	push   eax
 474:	51                   	push   ecx
 475:	56                   	push   esi
 476:	ff b3 f4 ff ff ff    	push   DWORD PTR [ebx-0xc]
 47c:	e8 af ff ff ff       	call   430 <__libc_start_main@plt>
 481:	f4                   	hlt    
 482:	8b 1c 24             	mov    ebx,DWORD PTR [esp]
 485:	c3                   	ret    
 486:	66 90                	xchg   ax,ax
 488:	66 90                	xchg   ax,ax
 48a:	66 90                	xchg   ax,ax
 48c:	66 90                	xchg   ax,ax
 48e:	66 90                	xchg   ax,ax

00000490 <__x86.get_pc_thunk.bx>:
__x86.get_pc_thunk.bx():
 490:	8b 1c 24             	mov    ebx,DWORD PTR [esp]
 493:	c3                   	ret    
 494:	66 90                	xchg   ax,ax
 496:	66 90                	xchg   ax,ax
 498:	66 90                	xchg   ax,ax
 49a:	66 90                	xchg   ax,ax
 49c:	66 90                	xchg   ax,ax
 49e:	66 90                	xchg   ax,ax

000004a0 <deregister_tm_clones>:
deregister_tm_clones():
 4a0:	e8 17 01 00 00       	call   5bc <__x86.get_pc_thunk.dx>
 4a5:	81 c2 5b 1b 00 00    	add    edx,0x1b5b
 4ab:	8d 8a 20 00 00 00    	lea    ecx,[edx+0x20]
 4b1:	8d 82 23 00 00 00    	lea    eax,[edx+0x23]
 4b7:	29 c8                	sub    eax,ecx
 4b9:	83 f8 06             	cmp    eax,0x6
 4bc:	76 17                	jbe    4d5 <deregister_tm_clones+0x35>
 4be:	8b 82 e8 ff ff ff    	mov    eax,DWORD PTR [edx-0x18]
 4c4:	85 c0                	test   eax,eax
 4c6:	74 0d                	je     4d5 <deregister_tm_clones+0x35>
 4c8:	55                   	push   ebp
 4c9:	89 e5                	mov    ebp,esp
 4cb:	83 ec 14             	sub    esp,0x14
 4ce:	51                   	push   ecx
 4cf:	ff d0                	call   eax
 4d1:	83 c4 10             	add    esp,0x10
 4d4:	c9                   	leave  
 4d5:	f3 c3                	repz ret 
 4d7:	89 f6                	mov    esi,esi
 4d9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

000004e0 <register_tm_clones>:
register_tm_clones():
 4e0:	e8 d7 00 00 00       	call   5bc <__x86.get_pc_thunk.dx>
 4e5:	81 c2 1b 1b 00 00    	add    edx,0x1b1b
 4eb:	55                   	push   ebp
 4ec:	8d 8a 20 00 00 00    	lea    ecx,[edx+0x20]
 4f2:	8d 82 20 00 00 00    	lea    eax,[edx+0x20]
 4f8:	89 e5                	mov    ebp,esp
 4fa:	53                   	push   ebx
 4fb:	29 c8                	sub    eax,ecx
 4fd:	c1 f8 02             	sar    eax,0x2
 500:	83 ec 04             	sub    esp,0x4
 503:	89 c3                	mov    ebx,eax
 505:	c1 eb 1f             	shr    ebx,0x1f
 508:	01 d8                	add    eax,ebx
 50a:	d1 f8                	sar    eax,1
 50c:	74 14                	je     522 <register_tm_clones+0x42>
 50e:	8b 92 fc ff ff ff    	mov    edx,DWORD PTR [edx-0x4]
 514:	85 d2                	test   edx,edx
 516:	74 0a                	je     522 <register_tm_clones+0x42>
 518:	83 ec 08             	sub    esp,0x8
 51b:	50                   	push   eax
 51c:	51                   	push   ecx
 51d:	ff d2                	call   edx
 51f:	83 c4 10             	add    esp,0x10
 522:	8b 5d fc             	mov    ebx,DWORD PTR [ebp-0x4]
 525:	c9                   	leave  
 526:	c3                   	ret    
 527:	89 f6                	mov    esi,esi
 529:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00000530 <__do_global_dtors_aux>:
__do_global_dtors_aux():
 530:	55                   	push   ebp
 531:	89 e5                	mov    ebp,esp
 533:	53                   	push   ebx
 534:	e8 57 ff ff ff       	call   490 <__x86.get_pc_thunk.bx>
 539:	81 c3 c7 1a 00 00    	add    ebx,0x1ac7
 53f:	83 ec 04             	sub    esp,0x4
 542:	80 bb 20 00 00 00 00 	cmp    BYTE PTR [ebx+0x20],0x0
 549:	75 27                	jne    572 <__do_global_dtors_aux+0x42>
 54b:	8b 83 ec ff ff ff    	mov    eax,DWORD PTR [ebx-0x14]
 551:	85 c0                	test   eax,eax
 553:	74 11                	je     566 <__do_global_dtors_aux+0x36>
 555:	83 ec 0c             	sub    esp,0xc
 558:	ff b3 1c 00 00 00    	push   DWORD PTR [ebx+0x1c]
 55e:	e8 dd fe ff ff       	call   440 <.plt.got>
 563:	83 c4 10             	add    esp,0x10
 566:	e8 35 ff ff ff       	call   4a0 <deregister_tm_clones>
 56b:	c6 83 20 00 00 00 01 	mov    BYTE PTR [ebx+0x20],0x1
 572:	8b 5d fc             	mov    ebx,DWORD PTR [ebp-0x4]
 575:	c9                   	leave  
 576:	c3                   	ret    
 577:	89 f6                	mov    esi,esi
 579:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00000580 <frame_dummy>:
frame_dummy():
 580:	e8 37 00 00 00       	call   5bc <__x86.get_pc_thunk.dx>
 585:	81 c2 7b 1a 00 00    	add    edx,0x1a7b
 58b:	8d 82 f4 fe ff ff    	lea    eax,[edx-0x10c]
 591:	8b 08                	mov    ecx,DWORD PTR [eax]
 593:	85 c9                	test   ecx,ecx
 595:	75 09                	jne    5a0 <frame_dummy+0x20>
 597:	e9 44 ff ff ff       	jmp    4e0 <register_tm_clones>
 59c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
 5a0:	8b 92 f8 ff ff ff    	mov    edx,DWORD PTR [edx-0x8]
 5a6:	85 d2                	test   edx,edx
 5a8:	74 ed                	je     597 <frame_dummy+0x17>
 5aa:	55                   	push   ebp
 5ab:	89 e5                	mov    ebp,esp
 5ad:	83 ec 14             	sub    esp,0x14
 5b0:	50                   	push   eax
 5b1:	ff d2                	call   edx
 5b3:	83 c4 10             	add    esp,0x10
 5b6:	c9                   	leave  
 5b7:	e9 24 ff ff ff       	jmp    4e0 <register_tm_clones>

000005bc <__x86.get_pc_thunk.dx>:
__x86.get_pc_thunk.dx():
 5bc:	8b 14 24             	mov    edx,DWORD PTR [esp]
 5bf:	c3                   	ret    

000005c0 <call_two>:
call_two():
/home/kyoupark/git/kb/asan/stack.c:7
 5c0:	55                   	push   ebp
 5c1:	89 e5                	mov    ebp,esp
 5c3:	53                   	push   ebx
 5c4:	83 ec 14             	sub    esp,0x14
 5c7:	e8 c4 fe ff ff       	call   490 <__x86.get_pc_thunk.bx>
 5cc:	81 c3 34 1a 00 00    	add    ebx,0x1a34
/home/kyoupark/git/kb/asan/stack.c:8
 5d2:	83 ec 0c             	sub    esp,0xc
 5d5:	8d 83 10 e8 ff ff    	lea    eax,[ebx-0x17f0]
 5db:	50                   	push   eax
 5dc:	e8 3f fe ff ff       	call   420 <puts@plt>
 5e1:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/stack.c:10
 5e4:	8b 45 10             	mov    eax,DWORD PTR [ebp+0x10]
 5e7:	89 45 f4             	mov    DWORD PTR [ebp-0xc],eax
/home/kyoupark/git/kb/asan/stack.c:11
 5ea:	8b 45 10             	mov    eax,DWORD PTR [ebp+0x10]
 5ed:	89 45 f0             	mov    DWORD PTR [ebp-0x10],eax
/home/kyoupark/git/kb/asan/stack.c:12
 5f0:	c7 45 e8 00 00 00 00 	mov    DWORD PTR [ebp-0x18],0x0
/home/kyoupark/git/kb/asan/stack.c:14
 5f7:	c7 45 ec 00 00 00 00 	mov    DWORD PTR [ebp-0x14],0x0
/home/kyoupark/git/kb/asan/stack.c:15
 5fe:	c7 45 ec 00 00 00 00 	mov    DWORD PTR [ebp-0x14],0x0
 605:	eb 0a                	jmp    611 <call_two+0x51>
/home/kyoupark/git/kb/asan/stack.c:16 (discriminator 3)
 607:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 60a:	01 45 f4             	add    DWORD PTR [ebp-0xc],eax
/home/kyoupark/git/kb/asan/stack.c:15 (discriminator 3)
 60d:	83 45 ec 01          	add    DWORD PTR [ebp-0x14],0x1
/home/kyoupark/git/kb/asan/stack.c:15 (discriminator 1)
 611:	83 7d ec 09          	cmp    DWORD PTR [ebp-0x14],0x9
 615:	7e f0                	jle    607 <call_two+0x47>
/home/kyoupark/git/kb/asan/stack.c:18
 617:	c7 45 ec 00 00 00 00 	mov    DWORD PTR [ebp-0x14],0x0
 61e:	eb 0a                	jmp    62a <call_two+0x6a>
/home/kyoupark/git/kb/asan/stack.c:19 (discriminator 3)
 620:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 623:	01 45 f0             	add    DWORD PTR [ebp-0x10],eax
/home/kyoupark/git/kb/asan/stack.c:18 (discriminator 3)
 626:	83 45 ec 01          	add    DWORD PTR [ebp-0x14],0x1
/home/kyoupark/git/kb/asan/stack.c:18 (discriminator 1)
 62a:	83 7d ec 13          	cmp    DWORD PTR [ebp-0x14],0x13
 62e:	7e f0                	jle    620 <call_two+0x60>
/home/kyoupark/git/kb/asan/stack.c:21
 630:	8b 55 f4             	mov    edx,DWORD PTR [ebp-0xc]
 633:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 636:	01 d0                	add    eax,edx
 638:	89 45 e8             	mov    DWORD PTR [ebp-0x18],eax
/home/kyoupark/git/kb/asan/stack.c:22
 63b:	83 ec 08             	sub    esp,0x8
 63e:	ff 75 e8             	push   DWORD PTR [ebp-0x18]
 641:	8d 83 21 e8 ff ff    	lea    eax,[ebx-0x17df]
 647:	50                   	push   eax
 648:	e8 c3 fd ff ff       	call   410 <printf@plt>
 64d:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/stack.c:23
 650:	8b 45 e8             	mov    eax,DWORD PTR [ebp-0x18]
/home/kyoupark/git/kb/asan/stack.c:24
 653:	8b 5d fc             	mov    ebx,DWORD PTR [ebp-0x4]
 656:	c9                   	leave  
 657:	c3                   	ret    


; when enters call_one
; (gdb) x $ebp
; 0xbffff198:     0xbffff1b8
; 
; (gdb) x/16 $ebp saved ebp       ra              arg1, 10        arg2, 20
; 0xbffff198:     0xbffff1b8      0x00400759      0x0000000a      0x00000014
; 
; 0xbffff188:     0xb7e6088b      0x00402000      0x00000001      0x00402000
; 0xbffff198:     0xbffff1b8      0x00400759      0x0000000a      0x00000014
;
; ebp + 12  : arg2
; ebp + 8   : arg1
; ebp + 4   : ra
; ebp       : 
; ebp - 4   : ebx
; ebp - 8   : 
; ebp - 12  : a1_
; ebp - 16  : s2_
; ebp - 20  : i
; ebp - 24  : result

00000658 <call_one>:
call_one():
/home/kyoupark/git/kb/asan/stack.c:27
 658:	55                   	push   ebp
 659:	89 e5                	mov    ebp,esp

; save ebx, -4
 65b:	53                   	push   ebx
 65c:	83 ec 14             	sub    esp,0x14   ; -20

 65f:	e8 2c fe ff ff       	call   490 <__x86.get_pc_thunk.bx> ; get ebx
 664:	81 c3 9c 19 00 00    	add    ebx,0x199c
/home/kyoupark/git/kb/asan/stack.c:28
 66a:	83 ec 0c             	sub    esp,0xc    ; -12

; to get global const string
; (gdb) x/20c ($ebx-0x17c9)
; 0x400837:       99 'c'  97 'a'  108 'l' 108 'l' 95 '_'  111 'o' 110 'n' 101 'e'
; 0x40083f:       58 ':'  32 ' '  98 'b'  101 'e' 103 'g' 105 'i' 110 'n' 46 '.'
; 0x400847:       0 '\000'        99 'c'  97 'a'  108 'l'
 66d:	8d 83 37 e8 ff ff    	lea    eax,[ebx-0x17c9]

 673:	50                   	push   eax
 674:	e8 a7 fd ff ff       	call   420 <puts@plt>

; int puts(const char *s);
; 16, why 16? may be because more args to make system call?
 679:	83 c4 10             	add    esp,0x10

; -12, a1_
/home/kyoupark/git/kb/asan/stack.c:30
 67c:	c7 45 f4 00 00 00 00 	mov    DWORD PTR [ebp-0xc],0x0    

; -16, a2_
/home/kyoupark/git/kb/asan/stack.c:31
 683:	c7 45 f0 00 00 00 00 	mov    DWORD PTR [ebp-0x10],0x0   

; -24, result
/home/kyoupark/git/kb/asan/stack.c:32
 68a:	c7 45 e8 00 00 00 00 	mov    DWORD PTR [ebp-0x18],0x0

; -20, i
/home/kyoupark/git/kb/asan/stack.c:34
 691:	c7 45 ec 00 00 00 00 	mov    DWORD PTR [ebp-0x14],0x0

; -20, i inside for()
/home/kyoupark/git/kb/asan/stack.c:35
 698:	c7 45 ec 00 00 00 00 	mov    DWORD PTR [ebp-0x14],0x0
 69f:	eb 0a                	jmp    6ab <call_one+0x53>

; a1_ += a1, ebp+8
/home/kyoupark/git/kb/asan/stack.c:36 (discriminator 3)
 6a1:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 6a4:	01 45 f4             	add    DWORD PTR [ebp-0xc],eax

; ++i
/home/kyoupark/git/kb/asan/stack.c:35 (discriminator 3)
 6a7:	83 45 ec 01          	add    DWORD PTR [ebp-0x14],0x1

; if i <= 9?
/home/kyoupark/git/kb/asan/stack.c:35 (discriminator 1)
 6ab:	83 7d ec 09          	cmp    DWORD PTR [ebp-0x14],0x9
 6af:	7e f0                	jle    6a1 <call_one+0x49>

/home/kyoupark/git/kb/asan/stack.c:38
 6b1:	c7 45 ec 00 00 00 00 	mov    DWORD PTR [ebp-0x14],0x0
 6b8:	eb 0a                	jmp    6c4 <call_one+0x6c>

/home/kyoupark/git/kb/asan/stack.c:39 (discriminator 3)
 6ba:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 6bd:	01 45 f0             	add    DWORD PTR [ebp-0x10],eax

/home/kyoupark/git/kb/asan/stack.c:38 (discriminator 3)
 6c0:	83 45 ec 01          	add    DWORD PTR [ebp-0x14],0x1

; if i <= 0x13, 19?
/home/kyoupark/git/kb/asan/stack.c:38 (discriminator 1)
 6c4:	83 7d ec 13          	cmp    DWORD PTR [ebp-0x14],0x13
 6c8:	7e f0                	jle    6ba <call_one+0x62>

/home/kyoupark/git/kb/asan/stack.c:41
 6ca:	83 ec 04             	sub    esp,0x4

; 3 args + ra and call
 6cd:	6a 64                	push   0x64
 6cf:	ff 75 f0             	push   DWORD PTR [ebp-0x10]
 6d2:	ff 75 f4             	push   DWORD PTR [ebp-0xc]
 6d5:	e8 e6 fe ff ff       	call   5c0 <call_two>
 6da:	83 c4 10             	add    esp,0x10

; save return to result var
 6dd:	89 45 e8             	mov    DWORD PTR [ebp-0x18],eax

/home/kyoupark/git/kb/asan/stack.c:42
 6e0:	83 ec 08             	sub    esp,0x8

; push result as arg
 6e3:	ff 75 e8             	push   DWORD PTR [ebp-0x18]

; ebx which has access to global still remains
 6e6:	8d 83 48 e8 ff ff    	lea    eax,[ebx-0x17b8]
 6ec:	50                   	push   eax
 6ed:	e8 1e fd ff ff       	call   410 <printf@plt>
 6f2:	83 c4 10             	add    esp,0x10

; save result
/home/kyoupark/git/kb/asan/stack.c:43
 6f5:	8b 45 e8             	mov    eax,DWORD PTR [ebp-0x18]
/home/kyoupark/git/kb/asan/stack.c:44
; pop saved ebx
 6f8:	8b 5d fc             	mov    ebx,DWORD PTR [ebp-0x4]

; epilog
 6fb:	c9                   	leave  
 6fc:	c3                   	ret    


000006fd <main>:
main():
/home/kyoupark/git/kb/asan/stack.c:47
 6fd:	8d 4c 24 04          	lea    ecx,[esp+0x4]
 701:	83 e4 f0             	and    esp,0xfffffff0
 704:	ff 71 fc             	push   DWORD PTR [ecx-0x4]
 707:	55                   	push   ebp
 708:	89 e5                	mov    ebp,esp
 70a:	53                   	push   ebx
 70b:	51                   	push   ecx
 70c:	e8 7f fd ff ff       	call   490 <__x86.get_pc_thunk.bx>
 711:	81 c3 ef 18 00 00    	add    ebx,0x18ef
/home/kyoupark/git/kb/asan/stack.c:48
 717:	83 ec 0c             	sub    esp,0xc
 71a:	8d 83 5e e8 ff ff    	lea    eax,[ebx-0x17a2]
 720:	50                   	push   eax
 721:	e8 fa fc ff ff       	call   420 <puts@plt>
 726:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/stack.c:49
 729:	83 ec 0c             	sub    esp,0xc
 72c:	8d 83 6c e8 ff ff    	lea    eax,[ebx-0x1794]
 732:	50                   	push   eax
 733:	e8 e8 fc ff ff       	call   420 <puts@plt>
 738:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/stack.c:50
 73b:	83 ec 0c             	sub    esp,0xc
 73e:	8d 83 90 e8 ff ff    	lea    eax,[ebx-0x1770]
 744:	50                   	push   eax
 745:	e8 d6 fc ff ff       	call   420 <puts@plt>
 74a:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/stack.c:52
 74d:	83 ec 08             	sub    esp,0x8
>750:	6a 14                	push   0x14
>752:	6a 0a                	push   0xa
>754:	e8 ff fe ff ff       	call   658 <call_one>
>759:	83 c4 10             	add    esp,0x10
 75c:	83 ec 08             	sub    esp,0x8
 75f:	50                   	push   eax
 760:	8d 83 98 e8 ff ff    	lea    eax,[ebx-0x1768]
 766:	50                   	push   eax
 767:	e8 a4 fc ff ff       	call   410 <printf@plt>
 76c:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/stack.c:53
 76f:	83 ec 0c             	sub    esp,0xc
 772:	8d 83 bd e8 ff ff    	lea    eax,[ebx-0x1743]
 778:	50                   	push   eax
 779:	e8 a2 fc ff ff       	call   420 <puts@plt>
 77e:	83 c4 10             	add    esp,0x10
/home/kyoupark/git/kb/asan/stack.c:55
 781:	b8 00 00 00 00       	mov    eax,0x0
/home/kyoupark/git/kb/asan/stack.c:56
 786:	8d 65 f8             	lea    esp,[ebp-0x8]
 789:	59                   	pop    ecx
 78a:	5b                   	pop    ebx
 78b:	5d                   	pop    ebp
 78c:	8d 61 fc             	lea    esp,[ecx-0x4]
 78f:	c3                   	ret    

00000790 <__libc_csu_init>:
__libc_csu_init():
 790:	55                   	push   ebp
 791:	57                   	push   edi
 792:	56                   	push   esi
 793:	53                   	push   ebx
 794:	e8 f7 fc ff ff       	call   490 <__x86.get_pc_thunk.bx>
 799:	81 c3 67 18 00 00    	add    ebx,0x1867
 79f:	83 ec 0c             	sub    esp,0xc
 7a2:	8b 6c 24 20          	mov    ebp,DWORD PTR [esp+0x20]
 7a6:	8d b3 f0 fe ff ff    	lea    esi,[ebx-0x110]
 7ac:	e8 2b fc ff ff       	call   3dc <_init>
 7b1:	8d 83 ec fe ff ff    	lea    eax,[ebx-0x114]
 7b7:	29 c6                	sub    esi,eax
 7b9:	c1 fe 02             	sar    esi,0x2
 7bc:	85 f6                	test   esi,esi
 7be:	74 25                	je     7e5 <__libc_csu_init+0x55>
 7c0:	31 ff                	xor    edi,edi
 7c2:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
 7c8:	83 ec 04             	sub    esp,0x4
 7cb:	ff 74 24 2c          	push   DWORD PTR [esp+0x2c]
 7cf:	ff 74 24 2c          	push   DWORD PTR [esp+0x2c]
 7d3:	55                   	push   ebp
 7d4:	ff 94 bb ec fe ff ff 	call   DWORD PTR [ebx+edi*4-0x114]
 7db:	83 c7 01             	add    edi,0x1
 7de:	83 c4 10             	add    esp,0x10
 7e1:	39 fe                	cmp    esi,edi
 7e3:	75 e3                	jne    7c8 <__libc_csu_init+0x38>
 7e5:	83 c4 0c             	add    esp,0xc
 7e8:	5b                   	pop    ebx
 7e9:	5e                   	pop    esi
 7ea:	5f                   	pop    edi
 7eb:	5d                   	pop    ebp
 7ec:	c3                   	ret    
 7ed:	8d 76 00             	lea    esi,[esi+0x0]

000007f0 <__libc_csu_fini>:
__libc_csu_fini():
 7f0:	f3 c3                	repz ret 

Disassembly of section .fini:

000007f4 <_fini>:
_fini():
 7f4:	53                   	push   ebx
 7f5:	83 ec 08             	sub    esp,0x8
 7f8:	e8 93 fc ff ff       	call   490 <__x86.get_pc_thunk.bx>
 7fd:	81 c3 03 18 00 00    	add    ebx,0x1803
 803:	83 c4 08             	add    esp,0x8
 806:	5b                   	pop    ebx
 807:	c3                   	ret    
