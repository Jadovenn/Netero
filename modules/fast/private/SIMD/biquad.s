section .text
bits 64

%include ./common.s

global biquad
biquad:
	alloc_framestack
	free_framestack
	ret
