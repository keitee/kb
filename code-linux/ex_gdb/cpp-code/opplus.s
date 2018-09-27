
opplus.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <main>:
   0:	55                   	push   rbp
   1:	48 89 e5             	mov    rbp,rsp
   4:	53                   	push   rbx
   5:	48 83 ec 48          	sub    rsp,0x48
   9:	48 8d 45 e0          	lea    rax,[rbp-0x20]
   d:	be 0a 00 00 00       	mov    esi,0xa
  12:	48 89 c7             	mov    rdi,rax
  15:	e8 00 00 00 00       	call   1a <main+0x1a>
  1a:	48 8d 45 d0          	lea    rax,[rbp-0x30]
  1e:	be 14 00 00 00       	mov    esi,0x14
  23:	48 89 c7             	mov    rdi,rax
  26:	e8 00 00 00 00       	call   2b <main+0x2b>
  2b:	48 8d 45 c0          	lea    rax,[rbp-0x40]
  2f:	48 8d 4d d0          	lea    rcx,[rbp-0x30]
  33:	48 8d 55 e0          	lea    rdx,[rbp-0x20]
  37:	48 8d 75 e0          	lea    rsi,[rbp-0x20]
  3b:	48 89 c7             	mov    rdi,rax
  3e:	e8 00 00 00 00       	call   43 <main+0x43>
  43:	48 8d 45 c0          	lea    rax,[rbp-0x40]
  47:	48 89 c7             	mov    rdi,rax
  4a:	e8 00 00 00 00       	call   4f <main+0x4f>
  4f:	48 8d 45 b0          	lea    rax,[rbp-0x50]
  53:	48 8d 4d d0          	lea    rcx,[rbp-0x30]
  57:	48 8d 55 e0          	lea    rdx,[rbp-0x20]
  5b:	48 8d 75 e0          	lea    rsi,[rbp-0x20]
  5f:	48 89 c7             	mov    rdi,rax
  62:	e8 00 00 00 00       	call   67 <main+0x67>
  67:	48 8d 45 b0          	lea    rax,[rbp-0x50]
  6b:	48 89 c7             	mov    rdi,rax
  6e:	e8 00 00 00 00       	call   73 <main+0x73>
  73:	bb 00 00 00 00       	mov    ebx,0x0
  78:	48 8d 45 b0          	lea    rax,[rbp-0x50]
  7c:	48 89 c7             	mov    rdi,rax
  7f:	e8 00 00 00 00       	call   84 <main+0x84>
  84:	48 8d 45 c0          	lea    rax,[rbp-0x40]
  88:	48 89 c7             	mov    rdi,rax
  8b:	e8 00 00 00 00       	call   90 <main+0x90>
  90:	48 8d 45 d0          	lea    rax,[rbp-0x30]
  94:	48 89 c7             	mov    rdi,rax
  97:	e8 00 00 00 00       	call   9c <main+0x9c>
  9c:	48 8d 45 e0          	lea    rax,[rbp-0x20]
  a0:	48 89 c7             	mov    rdi,rax
  a3:	e8 00 00 00 00       	call   a8 <main+0xa8>
  a8:	89 d8                	mov    eax,ebx
  aa:	eb 43                	jmp    ef <main+0xef>
  ac:	48 89 c3             	mov    rbx,rax
  af:	48 8d 45 b0          	lea    rax,[rbp-0x50]
  b3:	48 89 c7             	mov    rdi,rax
  b6:	e8 00 00 00 00       	call   bb <main+0xbb>
  bb:	eb 03                	jmp    c0 <main+0xc0>
  bd:	48 89 c3             	mov    rbx,rax
  c0:	48 8d 45 c0          	lea    rax,[rbp-0x40]
  c4:	48 89 c7             	mov    rdi,rax
  c7:	e8 00 00 00 00       	call   cc <main+0xcc>
  cc:	48 8d 45 d0          	lea    rax,[rbp-0x30]
  d0:	48 89 c7             	mov    rdi,rax
  d3:	e8 00 00 00 00       	call   d8 <main+0xd8>
  d8:	48 8d 45 e0          	lea    rax,[rbp-0x20]
  dc:	48 89 c7             	mov    rdi,rax
  df:	e8 00 00 00 00       	call   e4 <main+0xe4>
  e4:	48 89 d8             	mov    rax,rbx
  e7:	48 89 c7             	mov    rdi,rax
  ea:	e8 00 00 00 00       	call   ef <main+0xef>
  ef:	48 83 c4 48          	add    rsp,0x48
  f3:	5b                   	pop    rbx
  f4:	5d                   	pop    rbp
  f5:	c3                   	ret    

00000000000000f6 <__static_initialization_and_destruction_0(int, int)>:
  f6:	55                   	push   rbp
  f7:	48 89 e5             	mov    rbp,rsp
  fa:	48 83 ec 10          	sub    rsp,0x10
  fe:	89 7d fc             	mov    DWORD PTR [rbp-0x4],edi
 101:	89 75 f8             	mov    DWORD PTR [rbp-0x8],esi
 104:	83 7d fc 01          	cmp    DWORD PTR [rbp-0x4],0x1
 108:	75 27                	jne    131 <__static_initialization_and_destruction_0(int, int)+0x3b>
 10a:	81 7d f8 ff ff 00 00 	cmp    DWORD PTR [rbp-0x8],0xffff
 111:	75 1e                	jne    131 <__static_initialization_and_destruction_0(int, int)+0x3b>
 113:	bf 00 00 00 00       	mov    edi,0x0
 118:	e8 00 00 00 00       	call   11d <__static_initialization_and_destruction_0(int, int)+0x27>
 11d:	ba 00 00 00 00       	mov    edx,0x0
 122:	be 00 00 00 00       	mov    esi,0x0
 127:	bf 00 00 00 00       	mov    edi,0x0
 12c:	e8 00 00 00 00       	call   131 <__static_initialization_and_destruction_0(int, int)+0x3b>
 131:	c9                   	leave  
 132:	c3                   	ret    

0000000000000133 <_GLOBAL__sub_I_main>:
 133:	55                   	push   rbp
 134:	48 89 e5             	mov    rbp,rsp
 137:	be ff ff 00 00       	mov    esi,0xffff
 13c:	bf 01 00 00 00       	mov    edi,0x1
 141:	e8 b0 ff ff ff       	call   f6 <__static_initialization_and_destruction_0(int, int)>
 146:	5d                   	pop    rbp
 147:	c3                   	ret    

Disassembly of section .text._ZN3FooC2Ei:

0000000000000000 <Foo::Foo(int)>:
   0:	55                   	push   rbp
   1:	48 89 e5             	mov    rbp,rsp
   4:	48 89 7d f8          	mov    QWORD PTR [rbp-0x8],rdi
   8:	89 75 f4             	mov    DWORD PTR [rbp-0xc],esi
   b:	48 8b 45 f8          	mov    rax,QWORD PTR [rbp-0x8]
   f:	8b 55 f4             	mov    edx,DWORD PTR [rbp-0xc]
  12:	89 10                	mov    DWORD PTR [rax],edx
  14:	5d                   	pop    rbp
  15:	c3                   	ret    

Disassembly of section .text._ZN3FooD2Ev:

0000000000000000 <Foo::~Foo()>:
   0:	55                   	push   rbp
   1:	48 89 e5             	mov    rbp,rsp
   4:	48 89 7d f8          	mov    QWORD PTR [rbp-0x8],rdi
   8:	5d                   	pop    rbp
   9:	c3                   	ret    

Disassembly of section .text._ZN3FoopLERKS_:

0000000000000000 <Foo::operator+=(Foo const&)>:
   0:	55                   	push   rbp
   1:	48 89 e5             	mov    rbp,rsp
   4:	48 89 7d f8          	mov    QWORD PTR [rbp-0x8],rdi
   8:	48 89 75 f0          	mov    QWORD PTR [rbp-0x10],rsi
   c:	48 8b 45 f8          	mov    rax,QWORD PTR [rbp-0x8]
  10:	8b 10                	mov    edx,DWORD PTR [rax]
  12:	48 8b 45 f0          	mov    rax,QWORD PTR [rbp-0x10]
  16:	8b 00                	mov    eax,DWORD PTR [rax]
  18:	01 c2                	add    edx,eax
  1a:	48 8b 45 f8          	mov    rax,QWORD PTR [rbp-0x8]
  1e:	89 10                	mov    DWORD PTR [rax],edx
  20:	48 8b 45 f8          	mov    rax,QWORD PTR [rbp-0x8]
  24:	5d                   	pop    rbp
  25:	c3                   	ret    

Disassembly of section .text._ZN3Foo10PrintValueEv:

0000000000000000 <Foo::PrintValue()>:
   0:	55                   	push   rbp
   1:	48 89 e5             	mov    rbp,rsp
   4:	53                   	push   rbx
   5:	48 83 ec 18          	sub    rsp,0x18
   9:	48 89 7d e8          	mov    QWORD PTR [rbp-0x18],rdi
   d:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  11:	8b 18                	mov    ebx,DWORD PTR [rax]
  13:	be 00 00 00 00       	mov    esi,0x0
  18:	bf 00 00 00 00       	mov    edi,0x0
  1d:	e8 00 00 00 00       	call   22 <Foo::PrintValue()+0x22>
  22:	89 de                	mov    esi,ebx
  24:	48 89 c7             	mov    rdi,rax
  27:	e8 00 00 00 00       	call   2c <Foo::PrintValue()+0x2c>
  2c:	be 00 00 00 00       	mov    esi,0x0
  31:	48 89 c7             	mov    rdi,rax
  34:	e8 00 00 00 00       	call   39 <Foo::PrintValue()+0x39>
  39:	48 83 c4 18          	add    rsp,0x18
  3d:	5b                   	pop    rbx
  3e:	5d                   	pop    rbp
  3f:	c3                   	ret    

Disassembly of section .text._ZN3Foo8op_plus1ERKS_S1_:

0000000000000000 <Foo::op_plus1(Foo const&, Foo const&)>:
   0:	55                   	push   rbp
   1:	48 89 e5             	mov    rbp,rsp
   4:	48 83 ec 20          	sub    rsp,0x20
   8:	48 89 7d f8          	mov    QWORD PTR [rbp-0x8],rdi
   c:	48 89 75 f0          	mov    QWORD PTR [rbp-0x10],rsi
  10:	48 89 55 e8          	mov    QWORD PTR [rbp-0x18],rdx
  14:	48 89 4d e0          	mov    QWORD PTR [rbp-0x20],rcx
  18:	48 8b 45 f8          	mov    rax,QWORD PTR [rbp-0x8]
  1c:	48 8b 55 e8          	mov    rdx,QWORD PTR [rbp-0x18]
  20:	8b 12                	mov    edx,DWORD PTR [rdx]
  22:	89 10                	mov    DWORD PTR [rax],edx
  24:	48 8b 55 e0          	mov    rdx,QWORD PTR [rbp-0x20]
  28:	48 8b 45 f8          	mov    rax,QWORD PTR [rbp-0x8]
  2c:	48 89 d6             	mov    rsi,rdx
  2f:	48 89 c7             	mov    rdi,rax
  32:	e8 00 00 00 00       	call   37 <Foo::op_plus1(Foo const&, Foo const&)+0x37>
  37:	90                   	nop
  38:	48 8b 45 f8          	mov    rax,QWORD PTR [rbp-0x8]
  3c:	c9                   	leave  
  3d:	c3                   	ret    

Disassembly of section .text._ZN3Foo8op_plus2ERKS_S1_:

0000000000000000 <Foo::op_plus2(Foo const&, Foo const&)>:
   0:	55                   	push   rbp
   1:	48 89 e5             	mov    rbp,rsp
   4:	48 83 ec 30          	sub    rsp,0x30
   8:	48 89 7d e8          	mov    QWORD PTR [rbp-0x18],rdi
   c:	48 89 75 e0          	mov    QWORD PTR [rbp-0x20],rsi
  10:	48 89 55 d8          	mov    QWORD PTR [rbp-0x28],rdx
  14:	48 89 4d d0          	mov    QWORD PTR [rbp-0x30],rcx
  18:	48 8b 45 d8          	mov    rax,QWORD PTR [rbp-0x28]
  1c:	8b 00                	mov    eax,DWORD PTR [rax]
  1e:	89 45 f0             	mov    DWORD PTR [rbp-0x10],eax
  21:	48 8b 55 d0          	mov    rdx,QWORD PTR [rbp-0x30]
  25:	48 8d 45 f0          	lea    rax,[rbp-0x10]
  29:	48 89 d6             	mov    rsi,rdx
  2c:	48 89 c7             	mov    rdi,rax
  2f:	e8 00 00 00 00       	call   34 <Foo::op_plus2(Foo const&, Foo const&)+0x34>
  34:	48 89 c2             	mov    rdx,rax
  37:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  3b:	8b 12                	mov    edx,DWORD PTR [rdx]
  3d:	89 10                	mov    DWORD PTR [rax],edx
  3f:	48 8d 45 f0          	lea    rax,[rbp-0x10]
  43:	48 89 c7             	mov    rdi,rax
  46:	e8 00 00 00 00       	call   4b <Foo::op_plus2(Foo const&, Foo const&)+0x4b>
  4b:	90                   	nop
  4c:	48 8b 45 e8          	mov    rax,QWORD PTR [rbp-0x18]
  50:	c9                   	leave  
  51:	c3                   	ret    
