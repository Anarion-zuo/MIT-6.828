	.file	"init.c"
	.stabs	"kern/init.c",100,0,2,.Ltext0
	.text
.Ltext0:
	.stabs	"gcc2_compiled.",60,0,0,0
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"entering test_backtrace %d\n"
.LC1:
	.string	"leaving test_backtrace %d\n"
	.text
	.p2align 4
	.stabs	"test_backtrace:F(0,1)=(0,1)",36,0,0,test_backtrace
	.stabs	"void:t(0,1)",128,0,0,0
	.stabs	"x:P(0,2)=r(0,2);-2147483648;2147483647;",64,0,0,6
	.stabs	"int:t(0,2)",128,0,0,0
	.globl	test_backtrace
	.type	test_backtrace, @function
test_backtrace:
	.stabn	68,0,13,.LM0-.LFBB1
.LM0:
.LFBB1:
.LFB0:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	.stabn	68,0,14,.LM1-.LFBB1
.LM1:
	movl	%edi, %esi
	xorl	%eax, %eax
	.stabn	68,0,13,.LM2-.LFBB1
.LM2:
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	.stabn	68,0,13,.LM3-.LFBB1
.LM3:
	movl	%edi, %ebp
	.stabn	68,0,14,.LM4-.LFBB1
.LM4:
	leaq	.LC0(%rip), %rdi
	call	cprintf@PLT
	.stabn	68,0,15,.LM5-.LFBB1
.LM5:
	testl	%ebp, %ebp
	jle	.L2
	.stabn	68,0,16,.LM6-.LFBB1
.LM6:
	leal	-1(%rbp), %r12d
.LBB10:
.LBB11:
	.stabn	68,0,14,.LM7-.LFBB1
.LM7:
	xorl	%eax, %eax
	leaq	.LC0(%rip), %rdi
	movl	%r12d, %esi
	call	cprintf@PLT
	.stabn	68,0,15,.LM8-.LFBB1
.LM8:
	testl	%r12d, %r12d
	je	.L3
	.stabn	68,0,16,.LM9-.LFBB1
.LM9:
	leal	-2(%rbp), %r13d
.LBB12:
.LBB13:
	.stabn	68,0,14,.LM10-.LFBB1
.LM10:
	xorl	%eax, %eax
	leaq	.LC0(%rip), %rdi
	movl	%r13d, %esi
	call	cprintf@PLT
	.stabn	68,0,15,.LM11-.LFBB1
.LM11:
	testl	%r13d, %r13d
	je	.L4
	.stabn	68,0,16,.LM12-.LFBB1
.LM12:
	leal	-3(%rbp), %r14d
.LBB14:
.LBB15:
	.stabn	68,0,14,.LM13-.LFBB1
.LM13:
	xorl	%eax, %eax
	leaq	.LC0(%rip), %rdi
	movl	%r14d, %esi
	call	cprintf@PLT
	.stabn	68,0,15,.LM14-.LFBB1
.LM14:
	testl	%r14d, %r14d
	je	.L5
	.stabn	68,0,16,.LM15-.LFBB1
.LM15:
	leal	-4(%rbp), %r15d
.LBB16:
.LBB17:
	.stabn	68,0,14,.LM16-.LFBB1
.LM16:
	xorl	%eax, %eax
	leaq	.LC0(%rip), %rdi
	movl	%r15d, %esi
	call	cprintf@PLT
	.stabn	68,0,15,.LM17-.LFBB1
.LM17:
	testl	%r15d, %r15d
	je	.L6
	.stabn	68,0,16,.LM18-.LFBB1
.LM18:
	leal	-5(%rbp), %edi
	call	test_backtrace
.L7:
	.stabn	68,0,19,.LM19-.LFBB1
.LM19:
	movl	%r15d, %esi
	leaq	.LC1(%rip), %rdi
	xorl	%eax, %eax
	call	cprintf@PLT
.L8:
.LBE17:
.LBE16:
	movl	%r14d, %esi
	leaq	.LC1(%rip), %rdi
	xorl	%eax, %eax
	call	cprintf@PLT
.L9:
.LBE15:
.LBE14:
	movl	%r13d, %esi
	leaq	.LC1(%rip), %rdi
	xorl	%eax, %eax
	call	cprintf@PLT
	.stabn	68,0,20,.LM20-.LFBB1
.LM20:
	jmp	.L10
	.p2align 4,,10
	.p2align 3
.L3:
.LBE13:
.LBE12:
	.stabn	68,0,18,.LM21-.LFBB1
.LM21:
	xorl	%edx, %edx
	xorl	%esi, %esi
	xorl	%edi, %edi
	call	mon_backtrace@PLT
.L10:
	.stabn	68,0,19,.LM22-.LFBB1
.LM22:
	movl	%r12d, %esi
	leaq	.LC1(%rip), %rdi
	xorl	%eax, %eax
	call	cprintf@PLT
.L11:
.LBE11:
.LBE10:
	movl	%ebp, %esi
	leaq	.LC1(%rip), %rdi
	.stabn	68,0,20,.LM23-.LFBB1
.LM23:
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	.stabn	68,0,19,.LM24-.LFBB1
.LM24:
	xorl	%eax, %eax
	.stabn	68,0,20,.LM25-.LFBB1
.LM25:
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	.stabn	68,0,19,.LM26-.LFBB1
.LM26:
	jmp	cprintf@PLT
	.p2align 4,,10
	.p2align 3
.L2:
	.cfi_restore_state
	.stabn	68,0,18,.LM27-.LFBB1
.LM27:
	xorl	%edx, %edx
	xorl	%esi, %esi
	xorl	%edi, %edi
	call	mon_backtrace@PLT
	jmp	.L11
	.p2align 4,,10
	.p2align 3
.L5:
.LBB27:
.LBB26:
.LBB25:
.LBB24:
.LBB22:
.LBB20:
	xorl	%edx, %edx
	xorl	%esi, %esi
	xorl	%edi, %edi
	call	mon_backtrace@PLT
	jmp	.L8
	.p2align 4,,10
	.p2align 3
.L4:
.LBE20:
.LBE22:
	xorl	%edx, %edx
	xorl	%esi, %esi
	xorl	%edi, %edi
	call	mon_backtrace@PLT
	jmp	.L9
	.p2align 4,,10
	.p2align 3
.L6:
.LBB23:
.LBB21:
.LBB19:
.LBB18:
	xorl	%edx, %edx
	xorl	%esi, %esi
	xorl	%edi, %edi
	call	mon_backtrace@PLT
	jmp	.L7
.LBE18:
.LBE19:
.LBE21:
.LBE23:
.LBE24:
.LBE25:
.LBE26:
.LBE27:
	.cfi_endproc
.LFE0:
	.size	test_backtrace, .-test_backtrace
.Lscope1:
	.section	.rodata.str1.1
.LC2:
	.string	"6828 decimal is %o octal!\n"
	.text
	.p2align 4
	.stabs	"i386_init:F(0,1)",36,0,0,i386_init
	.globl	i386_init
	.type	i386_init, @function
i386_init:
	.stabn	68,0,24,.LM28-.LFBB2
.LM28:
.LFBB2:
.LFB1:
	.cfi_startproc
	.stabn	68,0,30,.LM29-.LFBB2
.LM29:
	leaq	edata(%rip), %rdi
	leaq	end(%rip), %rdx
	.stabn	68,0,24,.LM30-.LFBB2
.LM30:
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	.stabn	68,0,30,.LM31-.LFBB2
.LM31:
	xorl	%esi, %esi
	.stabn	68,0,30,.LM32-.LFBB2
.LM32:
	subq	%rdi, %rdx
	.stabn	68,0,30,.LM33-.LFBB2
.LM33:
	call	memset@PLT
	.stabn	68,0,34,.LM34-.LFBB2
.LM34:
	call	cons_init@PLT
	.stabn	68,0,36,.LM35-.LFBB2
.LM35:
	leaq	.LC2(%rip), %rdi
	movl	$6828, %esi
	xorl	%eax, %eax
	call	cprintf@PLT
	.stabn	68,0,42,.LM36-.LFBB2
.LM36:
	movl	$3, %edi
	call	test_backtrace
	.p2align 4,,10
	.p2align 3
.L15:
	.stabn	68,0,46,.LM37-.LFBB2
.LM37:
	xorl	%edi, %edi
	call	monitor@PLT
	jmp	.L15
	.cfi_endproc
.LFE1:
	.size	i386_init, .-i386_init
.Lscope2:
	.section	.rodata.str1.1
.LC3:
	.string	"kernel panic at %s:%d: "
.LC4:
	.string	"\n"
	.text
	.p2align 4
	.stabs	"_panic:F(0,1)",36,0,0,_panic
	.stabs	"file:P(0,3)=*(0,4)=r(0,4);0;127;",64,0,0,5
	.stabs	"line:P(0,2)",64,0,0,4
	.stabs	"fmt:P(0,3)",64,0,0,6
	.stabs	"char:t(0,4)",128,0,0,0
	.globl	_panic
	.type	_panic, @function
_panic:
	.stabn	68,0,62,.LM38-.LFBB3
.LM38:
.LFBB3:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rdx, %rbp
	subq	$208, %rsp
	.cfi_def_cfa_offset 224
	movq	%rcx, 56(%rsp)
	movq	%r8, 64(%rsp)
	movq	%r9, 72(%rsp)
	testb	%al, %al
	je	.L18
	movaps	%xmm0, 80(%rsp)
	movaps	%xmm1, 96(%rsp)
	movaps	%xmm2, 112(%rsp)
	movaps	%xmm3, 128(%rsp)
	movaps	%xmm4, 144(%rsp)
	movaps	%xmm5, 160(%rsp)
	movaps	%xmm6, 176(%rsp)
	movaps	%xmm7, 192(%rsp)
.L18:
	.stabn	68,0,62,.LM39-.LFBB3
.LM39:
	movq	%fs:40, %rax
	movq	%rax, 24(%rsp)
	xorl	%eax, %eax
	.stabn	68,0,65,.LM40-.LFBB3
.LM40:
	cmpq	$0, panicstr(%rip)
	je	.L23
	.p2align 4,,10
	.p2align 3
.L20:
	.stabn	68,0,81,.LM41-.LFBB3
.LM41:
	xorl	%edi, %edi
	call	monitor@PLT
	jmp	.L20
.L23:
	.stabn	68,0,67,.LM42-.LFBB3
.LM42:
	movq	%rbp, panicstr(%rip)
	.stabn	68,0,70,.LM43-.LFBB3
.LM43:
#APP
# 70 "kern/init.c" 1
	cli; cld
# 0 "" 2
	.stabn	68,0,72,.LM44-.LFBB3
.LM44:
#NO_APP
	leaq	224(%rsp), %rax
	.stabn	68,0,73,.LM45-.LFBB3
.LM45:
	movl	%esi, %edx
	movq	%rdi, %rsi
	.stabn	68,0,72,.LM46-.LFBB3
.LM46:
	movl	$24, (%rsp)
	movq	%rax, 8(%rsp)
	leaq	32(%rsp), %rax
	.stabn	68,0,73,.LM47-.LFBB3
.LM47:
	leaq	.LC3(%rip), %rdi
	.stabn	68,0,72,.LM48-.LFBB3
.LM48:
	movq	%rax, 16(%rsp)
	.stabn	68,0,73,.LM49-.LFBB3
.LM49:
	xorl	%eax, %eax
	.stabn	68,0,72,.LM50-.LFBB3
.LM50:
	movl	$48, 4(%rsp)
	.stabn	68,0,73,.LM51-.LFBB3
.LM51:
	call	cprintf@PLT
	.stabn	68,0,74,.LM52-.LFBB3
.LM52:
	movq	%rbp, %rdi
	movq	%rsp, %rsi
	call	vcprintf@PLT
	.stabn	68,0,75,.LM53-.LFBB3
.LM53:
	leaq	.LC4(%rip), %rdi
	xorl	%eax, %eax
	call	cprintf@PLT
	jmp	.L20
	.cfi_endproc
.LFE2:
	.size	_panic, .-_panic
	.stabs	"ap:(0,5)=(0,6)=(0,7)=ar(0,8)=r(0,8);0;-1;;0;0;(0,9)=xs__va_list_tag:",128,0,0,0
	.stabs	"va_list:t(0,5)",128,0,0,0
	.stabs	"__builtin_va_list:t(0,6)",128,0,0,0
	.stabs	"__va_list_tag:t(0,9)=s24gp_offset:(0,10)=r(0,10);0;4294967295;,0,32;fp_offset:(0,10),32,32;overflow_arg_area:(0,11)=*(0,1),64,64;reg_save_area:(0,11),128,64;;",128,0,0,0
	.stabs	"unsigned int:t(0,10)",128,0,0,0
	.stabn	192,0,0,.LFBB3-.LFBB3
	.stabn	224,0,0,.Lscope3-.LFBB3
.Lscope3:
	.section	.rodata.str1.1
.LC5:
	.string	"kernel warning at %s:%d: "
	.text
	.p2align 4
	.stabs	"_warn:F(0,1)",36,0,0,_warn
	.stabs	"file:P(0,3)",64,0,0,10
	.stabs	"line:P(0,2)",64,0,0,11
	.stabs	"fmt:P(0,3)",64,0,0,6
	.globl	_warn
	.type	_warn, @function
_warn:
	.stabn	68,0,87,.LM54-.LFBB4
.LM54:
.LFBB4:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rdi, %r10
	movl	%esi, %r11d
	movq	%rdx, %rbp
	subq	$208, %rsp
	.cfi_def_cfa_offset 224
	movq	%rcx, 56(%rsp)
	movq	%r8, 64(%rsp)
	movq	%r9, 72(%rsp)
	testb	%al, %al
	je	.L25
	movaps	%xmm0, 80(%rsp)
	movaps	%xmm1, 96(%rsp)
	movaps	%xmm2, 112(%rsp)
	movaps	%xmm3, 128(%rsp)
	movaps	%xmm4, 144(%rsp)
	movaps	%xmm5, 160(%rsp)
	movaps	%xmm6, 176(%rsp)
	movaps	%xmm7, 192(%rsp)
.L25:
	.stabn	68,0,87,.LM55-.LFBB4
.LM55:
	movq	%fs:40, %rax
	movq	%rax, 24(%rsp)
	xorl	%eax, %eax
	.stabn	68,0,90,.LM56-.LFBB4
.LM56:
	leaq	224(%rsp), %rax
	.stabn	68,0,91,.LM57-.LFBB4
.LM57:
	movq	%r10, %rsi
	movl	%r11d, %edx
	.stabn	68,0,90,.LM58-.LFBB4
.LM58:
	movq	%rax, 8(%rsp)
	leaq	32(%rsp), %rax
	.stabn	68,0,91,.LM59-.LFBB4
.LM59:
	leaq	.LC5(%rip), %rdi
	.stabn	68,0,90,.LM60-.LFBB4
.LM60:
	movq	%rax, 16(%rsp)
	.stabn	68,0,91,.LM61-.LFBB4
.LM61:
	xorl	%eax, %eax
	.stabn	68,0,90,.LM62-.LFBB4
.LM62:
	movl	$24, (%rsp)
	movl	$48, 4(%rsp)
	.stabn	68,0,91,.LM63-.LFBB4
.LM63:
	call	cprintf@PLT
	.stabn	68,0,92,.LM64-.LFBB4
.LM64:
	movq	%rbp, %rdi
	movq	%rsp, %rsi
	call	vcprintf@PLT
	.stabn	68,0,93,.LM65-.LFBB4
.LM65:
	xorl	%eax, %eax
	leaq	.LC4(%rip), %rdi
	call	cprintf@PLT
	.stabn	68,0,95,.LM66-.LFBB4
.LM66:
	movq	24(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L28
	addq	$208, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
.L28:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE3:
	.size	_warn, .-_warn
	.stabs	"ap:(0,5)",128,0,0,0
	.stabn	192,0,0,.LFBB4-.LFBB4
	.stabn	224,0,0,.Lscope4-.LFBB4
.Lscope4:
	.globl	panicstr
	.bss
	.align 8
	.type	panicstr, @object
	.size	panicstr, 8
panicstr:
	.zero	8
	.stabs	"panicstr:G(0,3)",32,0,0,0
	.text
	.stabs	"",100,0,0,.Letext0
.Letext0:
	.ident	"GCC: (GNU) 10.1.0"
	.section	.note.GNU-stack,"",@progbits
