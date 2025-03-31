	.cpu cortex-m0plus
	.arch armv6s-m
	.fpu softvfp
	.eabi_attribute 23, 1
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 2
	.eabi_attribute 34, 0
	.eabi_attribute 18, 4
	.file	"reverse_int.c"
	.text
	.align	1
	.p2align 2,,3
	.global	reverse_int
	.syntax unified
	.code	16
	.thumb_func
	.type	reverse_int, %function
reverse_int:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	movs	r3, r0
	push	{r4, lr}
	movs	r2, #32
	movs	r0, #0
	movs	r4, #1
.L2:
	movs	r1, r4
	lsls	r0, r0, #1
	ands	r1, r3
	subs	r2, r2, #1
	orrs	r0, r1
	lsrs	r3, r3, #1
	cmp	r2, #0
	bne	.L2
	@ sp needed
	pop	{r4, pc}
	.size	reverse_int, .-reverse_int
	.ident	"GCC: (Arm GNU Toolchain 13.3.Rel1 (Build arm-13.24)) 13.3.1 20240614"