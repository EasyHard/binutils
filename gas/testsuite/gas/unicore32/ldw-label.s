        .data
        .global t
        .word 1

	.text
	.align	2
	.global	func
	.type	func,function
func:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	ldw	r15, .L3
	stw	r14, [r15+], #0
.L4:
	.align	2
.L3:
	.word	g
	.size	func, .-func
	.ident	"GCC: (UC4_1.0.4_20100811) 4.4.2"
