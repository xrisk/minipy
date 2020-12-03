	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 11, 0
	.globl	_pow                            ## -- Begin function pow
	.p2align	4, 0x90
_pow:                                   ## @pow
	.cfi_startproc
## %bb.0:                               ## %entry
	movl	%edi, -8(%rsp)
	movl	%esi, -4(%rsp)
	movl	$1, -16(%rsp)
	movl	$1, -12(%rsp)
	testl	%esi, %esi
	jle	LBB0_2
	.p2align	4, 0x90
LBB0_1:                                 ## %loop
                                        ## =>This Inner Loop Header: Depth=1
	movl	-16(%rsp), %eax
	imull	-8(%rsp), %eax
	movl	%eax, -16(%rsp)
	movl	-12(%rsp), %eax
	incl	%eax
	movl	%eax, -12(%rsp)
	cmpl	-4(%rsp), %eax
	jle	LBB0_1
LBB0_2:                                 ## %after
	movl	-16(%rsp), %eax
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_readInt32                      ## -- Begin function readInt32
	.p2align	4, 0x90
_readInt32:                             ## @readInt32
	.cfi_startproc
## %bb.0:                               ## %entry
	movl	$5, %eax
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_printd                         ## -- Begin function printd
	.p2align	4, 0x90
_printd:                                ## @printd
	.cfi_startproc
## %bb.0:                               ## %entry
	movl	%edi, -4(%rsp)
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_func                           ## -- Begin function func
	.p2align	4, 0x90
_func:                                  ## @func
	.cfi_startproc
## %bb.0:                               ## %entry
	pushq	%rbp
	.cfi_def_cfa_offset 16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	subq	$24, %rsp
	.cfi_def_cfa_offset 48
	.cfi_offset %rbx, -24
	.cfi_offset %rbp, -16
	movl	%edi, 16(%rsp)
	movl	%esi, 20(%rsp)
	movl	$0, 12(%rsp)
	movl	$1, 8(%rsp)
	testl	%edi, %edi
	jle	LBB3_2
	.p2align	4, 0x90
LBB3_1:                                 ## %loop
                                        ## =>This Inner Loop Header: Depth=1
	movl	12(%rsp), %ebp
	movl	8(%rsp), %ebx
	movl	20(%rsp), %esi
	movl	%ebx, %edi
	callq	_pow
	imull	%ebx, %eax
	addl	%ebp, %eax
	movl	%eax, 12(%rsp)
	movl	8(%rsp), %eax
	incl	%eax
	movl	%eax, 8(%rsp)
	cmpl	16(%rsp), %eax
	jle	LBB3_1
LBB3_2:                                 ## %after
	movl	12(%rsp), %eax
	addq	$24, %rsp
	popq	%rbx
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_main                           ## -- Begin function main
	.p2align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## %bb.0:                               ## %entry
	pushq	%rax
	.cfi_def_cfa_offset 16
	callq	_readInt32
	movl	%eax, (%rsp)
	callq	_readInt32
	movl	%eax, 4(%rsp)
	movl	(%rsp), %edi
	movl	%eax, %esi
	callq	_func
	movl	%eax, %edi
	callq	_printd
	popq	%rax
	retq
	.cfi_endproc
                                        ## -- End function
.subsections_via_symbols
