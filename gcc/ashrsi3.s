# GCC language support routines
# 32-bit integer arithmetic
# long __ashrsi3 (long a, int bits)

	.code16

	.text

	.global	__ashrsi3

__ashrsi3:
	mov %sp,%bx
	mov 2(%bx),%ax
	mov 4(%bx),%dx
	mov 6(%bx),%cx
    jcxz 2f
1:
	sar $1,%dx
	rcr $1,%ax
	loop 1b
2:
	ret
